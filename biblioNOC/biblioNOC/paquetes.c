#include "paquetes.h"

/*------------------------------Paquetes------------------------------*/


int prueba(void) {
	puts("!!!prueba biblite si lo camibas de nuevo !!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}


void enviarPaquetes(int socketfd, t_paquete * unPaquete) {

	int desplazamiento = 0;

	int tamPaquete = sizeof(int);
	int tamCodOp = sizeof(int);
	int tamSize = sizeof(int);
	int tamData = unPaquete->buffer->size;

	int tamTotal = tamCodOp + tamSize + tamData;

	void * buffer = malloc(tamPaquete + tamTotal);

	//Tamaño del paquete
	memcpy(buffer + desplazamiento, &tamTotal, tamPaquete);
	desplazamiento += tamPaquete;

	//Codigo de operacion
	memcpy(buffer + desplazamiento, &unPaquete->codigoOperacion, tamCodOp);
	desplazamiento += tamCodOp;

	//Buffer -- size
	memcpy(buffer + desplazamiento, &unPaquete->buffer->size, tamSize);
	desplazamiento += tamSize;

	//Buffer -- data
	memcpy(buffer + desplazamiento, unPaquete->buffer->data, tamData);

	//Envio el paquete y compruebo errores
	int resultado = send(socketfd, buffer, tamPaquete + tamTotal, MSG_NOSIGNAL);

	if (resultado == -1)
		perror("send");

	//Libero memoria
	destruirPaquete(unPaquete);
	free(buffer);
}


t_paquete* recibirArmarPaquete(int socketCliente) {
	int size = recibirTamPaquete(socketCliente);
	if (size < 0) return NULL;
	return recibirPaquete(socketCliente, size);
}


int recibirTamPaquete(int client_socket) {
	int tamBuffer;

	//Creo el buffer
	void * buffer = malloc(sizeof(int));

	//Recibo el buffer
	int recvd = recv(client_socket, buffer, sizeof(int), MSG_WAITALL);

	//Verifico error o conexión cerrada por el cliente
	if (recvd <= 0) {
		tamBuffer = recvd;
		if (recvd == -1) {
			perror("recv");
		}

		perror( "Cierro conexion" );
		//Cierro el socket
		close(client_socket);

		//Libero memoria
		free(buffer);

		return -1;
	} else {

		//Copio el buffer en una variable asi despues lo libero
		memcpy(&tamBuffer, buffer, sizeof(int));
	}

	//Libero memoria
	free(buffer);

	return tamBuffer;
}

t_paquete * recibirPaquete(int client_socket, int tamPaquete) {
	//Reservo memoria para el buffer
	void * buffer = malloc(tamPaquete);
	//memset(buffer, '\0', tamPaquete); // Lo limpio al buffer para que no tenga basura

	//Recibo el buffer
	int recvd = recv(client_socket, buffer, tamPaquete, MSG_WAITALL);

	//Verifico error o conexión cerrada por el cliente
	if (recvd <= 0) {
		if (recvd == -1) {
			perror("recv");
		}
		printf("El socket %d ha producido un error "
				"y ha sido desconectado.\n", client_socket);

		//Cierro el socket
		close(client_socket);

	}

	//Armo el paquete a partir del buffer
	t_paquete * unPaquete = crearPaquete(buffer);

	return unPaquete;
}

t_paquete * crearPaquete(void * buffer) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	int desplazamiento = 0;

	int tamCodOp = sizeof(int);
	int tamSize = sizeof(size_t);

	//Codigo de operacion
	memcpy(&unPaquete->codigoOperacion, buffer + desplazamiento, tamCodOp);
	desplazamiento += tamCodOp;

	//Buffer -- size
	unPaquete->buffer = malloc(sizeof(t_stream));
	memcpy(&unPaquete->buffer->size, buffer + desplazamiento, tamSize);
	desplazamiento += tamSize;

	//Buffer -- data
	int tamData = unPaquete->buffer->size;
	unPaquete->buffer->data = malloc(tamData);
	memcpy(unPaquete->buffer->data, buffer + desplazamiento, tamData);

	//Libero memoria
	free(buffer);

	return unPaquete;
}

t_paquete * crearPaqueteError(int client_socket) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigoOperacion = ENVIAR_ERROR;
	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = sizeof(int);
	unPaquete->buffer->data = malloc(unPaquete->buffer->size);
	memcpy(unPaquete->buffer->data, &client_socket, unPaquete->buffer->size);
	return unPaquete;
}

void destruirPaquete(t_paquete * unPaquete) {
	free(unPaquete->buffer->data);
	free(unPaquete->buffer);
	free(unPaquete);
}

void mostrarPaquete(t_paquete * unPaquete) {
	printf("Muestro el paquete: \n");
	printf("Codigo de operacion: %d \n", unPaquete->codigoOperacion);
	printf("Tamanio del buffer: %d \n", unPaquete->buffer->size);
	printf("Buffer: %s \n", (char*) unPaquete->buffer->data);
}

/*-------------------------Enviar-------------------------*/
void enviarHandshake(int server_socket, int emisor) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = HANDSHAKE;

	serializarHandshake(unPaquete, emisor);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarMensaje(int server_socket, char * mensaje) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_MENSAJE;

	serializarMensaje(unPaquete, mensaje);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarMensajeConCodigo(int server_socket, char * mensaje, int codigoRespuesta) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = codigoRespuesta;

	serializarMensaje(unPaquete, mensaje);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarArchivo(int server_socket, char * rutaArchivo) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_ARCHIVO;

	serializarArchvivo(unPaquete, rutaArchivo);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarAvisoDesconexion(int server_socket){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_AVISO_DESCONEXION;

	serializarNumero(unPaquete, 0);

	enviarPaquetes(server_socket, unPaquete);
}


void enviarSolicitudTamanioValor(int server_socket) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = SOLICITAR_TAMANIO_VALOR;

	serializarNumero(unPaquete, 0);

	enviarPaquetes(server_socket, unPaquete);
}


void enviarRequestGossiping(int server_socket, int numeroMemoria, t_list* listaMemorias) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = GOSSIPING;

	serializarConexionMemoria( unPaquete, numeroMemoria, listaMemorias );

	enviarPaquetes(server_socket, unPaquete);
}


void enviarSolicitudClave(int server_socket, bool enviarQuery,char* nombreTabla, uint16_t clave){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = SELECT;

	if (enviarQuery) {
		serializarSelectQuery(unPaquete, nombreTabla, clave);
	} else {
		serializarSelect(unPaquete, nombreTabla, clave);
	}

	enviarPaquetes(server_socket, unPaquete);
}


void enviarInsert(int server_socket, bool enviarQuery,char* nombreTabla, uint16_t clave, char* value, uint64_t timestamp){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = INSERT;

	if (enviarQuery) {
		serializarInsertQuery(unPaquete, nombreTabla, clave, value, timestamp);
	} else {
		serializarInsert(unPaquete, nombreTabla, clave, value);
	}

	enviarPaquetes(server_socket, unPaquete);
}


void enviarDrop(int server_socket, char * nombreTabla) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = DROP;

	serializarMensaje(unPaquete, nombreTabla);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarDescribe(int server_socket, char * nombreTabla) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = DESCRIBE;

	serializarMensaje(unPaquete, nombreTabla);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarCreate(int server_socket, bool enviarQuery, char * nombreTabla, char * tipoConsistencia, int numeroParticiones, int tiempoCompactacion) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = CREATE;

	if (enviarQuery) {
		serializarCreateQuery(unPaquete, nombreTabla, tipoConsistencia, numeroParticiones, tiempoCompactacion);
	} else {
		serializarCreate(unPaquete, nombreTabla, tipoConsistencia, numeroParticiones, tiempoCompactacion);
	}

	enviarPaquetes(server_socket, unPaquete);
}


void enviarPool(int server_socket, int numeroMemoria, t_list* poolMemorias) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = RESPUESTA_SOLICITUD_POOL;

	serializarConexionMemoria(unPaquete, numeroMemoria, poolMemorias);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarOrdenJournal(int server_socket) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = JOURNAL;

	serializarNumero(unPaquete, 0);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarOrdenGossiping(int server_socket) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = REQUEST_GOSSIPING;

	serializarNumero(unPaquete, 0);

	enviarPaquetes(server_socket, unPaquete);
}


/*-------------------------Recibir-------------------------*/
int recibirHandshake(t_paquete * unPaquete) {
	return deserializarHandshake(unPaquete->buffer);
}

char * recibirMensaje(t_paquete * unPaquete) {
	return deserializarMensaje(unPaquete->buffer);
}

void* recibirArchivo(t_paquete * unPaquete) {
	return deserializarArchivo(unPaquete->buffer);
}

