/*
 * libSuseUtils.c
 *
 *  Created on: 2 nov. 2019
 *      Author: utnso
 */

#include "libSuseUtils.h"

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(ip, puerto, &hints, &server_info);
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		printf("Error de conexion. Servidor no disponible.");
	}
	freeaddrinfo(server_info);
	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = 0; // TODO revisar esto y borrarlo biewn
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, mensaje, paquete->buffer->size);
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);  // es un dato de Tipo Stream.
	send(socket_cliente, a_enviar, bytes, 0);
	free(a_enviar);
	free(paquete->buffer->data);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void serializarSemaforoRequest(t_paquete* paquete, int tid, char* nombre) {
	int tamNombre = strlen(nombre) + 1;
	int tamBuffer = tamNombre + sizeof( int );

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->data = calloc(1, tamBuffer);
	paquete->buffer->size = tamBuffer;

	memcpy(paquete->buffer->data, &tid, sizeof( int ));
	memcpy(paquete->buffer->data + sizeof( int ), nombre, tamNombre);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(paquete->codigoOperacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->data, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;
	return magic;
}

t_config* leer_config(char* configFile) {
	return config_create(configFile);
}

char* getAddress(t_config* config) {
	return config_get_string_value(config, "IP");
}

char* getPort(t_config* config) {
	return config_get_string_value(config, "PUERTO");
}
