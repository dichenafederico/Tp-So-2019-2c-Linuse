/*
 * suseDefs.c
 *
 *  Created on: 1 nov. 2019
 *      Author: utnso
 */

#include"suseDefs.h"

/*
int getSocketServidor(t_config* config)
{
	int socket_servidor;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
	char* addr = getAddress(config);
    char* port = getPort(config);
    getaddrinfo(addr, port, &hints, &servinfo);
    for (p=servinfo; p != NULL; p = p->ai_next) {  						// Se mantiene en loop hta obtener el socket
        if ((socket_servidor = socket(p->ai_family, 
				p->ai_socktype, p->ai_protocol)) == -1) continue; 		// No pudo obtener un socketServidor válido y vuelve a probar
        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {									
            close(socket_servidor);										// Bind no pudo reservar un puerto de esucha vuelve a probar
            continue;
        }
        break;  														// Se generó el socketServidor correctamente y pudo reservar un port. Sale del loop.
    }
    config_destroy(config);
	free(addr);
	free(port);
    freeaddrinfo(servinfo);
    return socket_servidor;
}
*/

void esperarClientes(int socket_servidor)
{
	int j = 20;
	while( j > 0 ) {
	if((listen(socket_servidor, SOMAXCONN)) == 0) {
	break;
	}
	j--;
	continue;
	}
}

int getSocketCliente(int socket_servidor)
{
	int socket_cliente;
	struct sockaddr dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr);
	if ((socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion)) != -1) { ;
	}
	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void* buffer;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	return buffer;
}

char* recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	return buffer;
}

int tamanio_array(char** array) {
	int tamanio;
	for (tamanio = 0; array[tamanio] != NULL; tamanio++)
		;
	return tamanio;
}

t_semaforo_request_suse* deserializarSemaforoRequest( t_stream* buffer ){
	t_semaforo_request_suse* sem_req_info = malloc( sizeof( t_semaforo_request_suse ) );

	sem_req_info->tid = *( (int *) buffer->data );
	sem_req_info->name = buffer->data + sizeof( int );

	return sem_req_info;
}
