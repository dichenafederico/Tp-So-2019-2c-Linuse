#include "conexiones.h"

/*------------------------------Clientes------------------------------*/

int conectarCliente(const char * ip, int puerto, int cliente) {
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	//hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP


	char * puertoString = string_itoa(puerto);
	getaddrinfo(ip, puertoString, &hints, &serverInfo); // Carga en serverInfo los datos de la conexion
	free(puertoString);

	//Creo el socket
	int socketfd = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (connect(socketfd, serverInfo->ai_addr, serverInfo->ai_addrlen)) {
		perror(NULL);

		freeaddrinfo(serverInfo);

		return -1;
	}

	freeaddrinfo(serverInfo);

	enviarHandshake(socketfd, cliente);

	return socketfd;
}

void gestionarSolicitudes(int server_socket, void (*procesarPaquete)(void*, int*), t_log * logger) {

	int tamPaquete = recibirTamPaquete(server_socket);

	if (tamPaquete > 0) {
		t_paquete * unPaquete = recibirPaquete(server_socket, tamPaquete);

		int socketAux = server_socket;

		procesarPaquete(unPaquete, &server_socket);

		if (server_socket == -1) {
			log_error(logger, "El socket %d no a pasado el handshake "
					"y ha sido desconectado.\n", socketAux);

			//El server hace lo que tiene que hacer cuando se desconecta el socket
			t_paquete * unPaqueteError = crearPaqueteError(server_socket);
			procesarPaquete(unPaqueteError, &server_socket);

		}
	} else {
		//El server hace lo que tiene que hacer cuando se desconecta el socket
		t_paquete * unPaqueteError = crearPaqueteError(server_socket);
		procesarPaquete(unPaqueteError, &server_socket);

	}
}

/*------------------------------Servidor------------------------------*/

void iniciarServer(int puerto, void (*procesarPaquete)(void*, int*),
		t_log * logger) {
	fd_set set_master;
	fd_set set_copia;

	//Creo el socket server
	char * puertoString = string_itoa(puerto);
	int server_socket = crearSocketServer(puertoString);
	free(puertoString);

	//Borro el conjunto de descriptores de fichero
	FD_ZERO(&set_master);

	//Añado fd al conjunto
	FD_SET(server_socket, &set_master);

	//Setteo el socket mas alto
	int descriptor_mas_alto = server_socket;

	log_trace(logger, "Servidor listo para escuchar conexiones\n");

	while (true) {
		set_copia = set_master;

		if (select(descriptor_mas_alto + 1, &set_copia, NULL, NULL, NULL)
				== -1) {
			perror("select");
			break;
		}

		//Exploro conexiones existentes en busca de datos que leer
		int n_descriptor = 0;

		while (descriptor_mas_alto >= n_descriptor) {

			if (FD_ISSET(n_descriptor, &set_copia)) {

				//Si el descriptor es igual al server_socket quiere decir que un nuevo cliente se quiere conectar
				if (n_descriptor == server_socket) {
					gestionarNuevasConexiones(server_socket, &set_master,
							&descriptor_mas_alto, logger);
				} else {
					//Si el decriptor pertenece a un socket cliente ya aceptado
					gestionarDatosCliente(n_descriptor, &set_master,
							(void *) procesarPaquete, logger);
				}
			}
			n_descriptor++;
		}
	}

}


int iniciarServidor(char* puerto, t_log* g_loggerDebug, void (*attendConnection)(int*)) {

	struct sockaddr_storage their_addr;
	struct addrinfo hints, *res;
	int status, sockfd;
	socklen_t addr_size;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((status = getaddrinfo(NULL, puerto, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	int yes = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	bind(sockfd, res->ai_addr, res->ai_addrlen);

	//TODO cerrar o free adrrinfo

	if (-1 == listen(sockfd, 10))
		perror("listen");

	log_debug( g_loggerDebug, "Esperando conexiones en puerto %s", puerto);

	while (1) {
		addr_size = sizeof(their_addr);
		int cliente_fd = accept(sockfd, (struct sockaddr*) &their_addr, &addr_size);

		pthread_t pid;

		log_debug( g_loggerDebug, "Me llego conexion con este socket %d", cliente_fd );
		int thread_status = pthread_create(&pid, NULL, (void*) attendConnection, (void*) cliente_fd);
		if( thread_status != 0 ){
			log_error( g_loggerDebug, "Thread create returno %d", thread_status );
			log_error( g_loggerDebug, "Thread create returno %s", strerror( thread_status ) );
		} else {
			pthread_detach( pid );
		}
	}
}

int crearSocketServer(char * puerto) {
	//Creo las estructuras
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	//hints.ai_family = AF_UNSPEC; // No importa si uso IPv4 o IPv6
	//hints.ai_flags = AI_PASSIVE; // Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, puerto, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE

	//Creo el socket
	int server_socket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);

	if (server_socket == -1)
		perror("socket");

	//No dejo el puerto ocupado
	int yes = 1;

	int resultado = setsockopt(server_socket,SOL_SOCKET , SO_REUSEADDR, &yes,
			sizeof(yes));

	if (resultado == -1)
		perror("setsockopt");

	//Conecto el puerto con el IP
	if (bind(server_socket, serverInfo->ai_addr, serverInfo->ai_addrlen))
		perror("bind");

	//Libero memoria
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	//Escuchar conexiones de entrada
	if (listen(server_socket, SOMAXCONN)) {
		perror("listen");
	}

	return server_socket;
}

void gestionarNuevasConexiones(int server_socket, fd_set * set_master,
		int * descriptor_mas_alto, t_log * logger) {
	//Socket del nuevo cliente
	int client_socket;

	//Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	//Acepto la nueva condicion
	if ((client_socket = accept(server_socket, (struct sockaddr *) &addr,
			&addrlen)) == -1) {
		log_error(logger, "El socket %d ha producido un error"
				"y ha sido desconectado.\n", client_socket);
		perror("accept");
		return;
	}

	log_trace(logger, "El socket %d se ha conectado al servidor.\n",
			client_socket);

	//Añado al conjunto maestro
	FD_SET(client_socket, set_master);

	//Actualizo el máximo
	if (client_socket > *descriptor_mas_alto)
		*descriptor_mas_alto = client_socket;
}

void gestionarDatosCliente(int client_socket, fd_set * set_master,
		void (*procesarPaquete)(void*, int*), t_log * logger) {

	int tamPaquete = recibirTamPaquete(client_socket);

	if (tamPaquete > 0) {
		t_paquete * unPaquete = recibirPaquete(client_socket, tamPaquete);

		int socketAux = client_socket;

		procesarPaquete(unPaquete, &client_socket);

		if (client_socket == -1) {
			log_error(logger, "El socket %d no a pasado el handshake "
					"y ha sido desconectado.\n", socketAux);

			//Cierro el socket
			close(socketAux);

			//Elimino el socket del conjunto maestro
			FD_CLR(socketAux, set_master);

			//El server hace lo que tiene que hacer cuando se desconecta el socket
			t_paquete * unPaqueteError = crearPaqueteError(client_socket);
			procesarPaquete(unPaqueteError, &client_socket);

		}
	} else {
		//El server hace lo que tiene que hacer cuando se desconecta el socket
		t_paquete * unPaqueteError = crearPaqueteError(client_socket);
		procesarPaquete(unPaqueteError, &client_socket);

		//Elimino el socket del conjunto maestro
		FD_CLR(client_socket, set_master);

	}
}

t_conexion* buscarConexion(t_list * diccionario, char * nombre, int socket, sem_t semaforo) {

	bool igualNombre = true;
	bool igualSocket = true;
	bool conexionCumpleCon(void* conexion){
		t_conexion* conexionBuscar = (t_conexion*) conexion;

		if (nombre != NULL)
			igualNombre = string_equals_ignore_case(conexionBuscar->nombre, nombre);

		if (socket != 0)
			igualSocket = (conexionBuscar->socket == socket );

			return (igualNombre && igualSocket);

	}
	sem_wait(&semaforo);
	t_conexion* conexionBuscada = list_find(diccionario,conexionCumpleCon);
	sem_post(&semaforo);
	return conexionBuscada;
}


void sacarConexion(t_list* diccionario, t_conexion* conexion){
	int index;
	for (index = 0;index<list_size(diccionario);index++){
		t_conexion* aux = list_get(diccionario,index);
		if(string_equals_ignore_case(aux->nombre,conexion->nombre)){
			t_conexion* auxABorrar = list_remove(diccionario,index);
			cerrarConexion(auxABorrar);
		}
	}
}

void cerrarConexion(void* conexion) {
	t_conexion* conexionACerrar = (t_conexion*)conexion;
	printf("cerrando %s\n",conexionACerrar->nombre);
	close(conexionACerrar->socket);
}

void destruirConexion(void* conexion){
		t_conexion* conexionACerrar = (t_conexion*)conexion;
		free(conexionACerrar->nombre);
		conexionACerrar->nombre=NULL;
		free(conexionACerrar);
}

void cerrarTodasLasConexiones(t_list * diccionario,sem_t semaforo) {
	sem_wait(&semaforo);
	list_iterate(diccionario, cerrarConexion);
	sem_post(&semaforo);
	destruirDiccionario(diccionario, semaforo);
}

void destruirDiccionario(t_list* diccionario,sem_t semaforo){
	sem_wait(&semaforo);
	list_clean_and_destroy_elements(diccionario,destruirConexion);
	sem_post(&semaforo);
}

int getTimeStamp(){

	struct timeval t;

	int tiempoMicroSegundos = gettimeofday(&t, NULL);

	int tiempoMiliSegundos = 0.001 * tiempoMicroSegundos;

	return tiempoMiliSegundos;

}


