#include "util.h"

void serializarUINT32(t_paquete* unPaquete, uint32_t numero) {
	int tamNumero = sizeof(uint32_t);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamNumero;
	unPaquete->buffer->data = malloc(tamNumero);

	memcpy(unPaquete->buffer->data, &numero, tamNumero);
}

int deserializarUINT32(t_stream* buffer) {
	return *(uint32_t*) (buffer->data);
}

void serializarRespuestaGet( t_paquete* unPaquete, int operacionSatisfactoria, size_t size, void* buffer ){
	if( operacionSatisfactoria == -1 ){
		size = 0;
	}

	int tam_total = sizeof( int ) + sizeof( size_t ) + size;
	int desplazamiento = 0;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tam_total;
	unPaquete->buffer->data = malloc( tam_total );

	memcpy(unPaquete->buffer->data, &operacionSatisfactoria, sizeof( int ) );
	desplazamiento += sizeof( int );

	memcpy(unPaquete->buffer->data + desplazamiento, &size, sizeof( size_t ) );
	desplazamiento += sizeof( size_t );

	memcpy(unPaquete->buffer->data + desplazamiento, buffer, size);

}

//TODO verificar serializacion de void*

t_registromget* deserializarGet(t_stream * buffer){
	t_registromget * registro = malloc(sizeof(t_registromget));
	int desplazamiento = 0;

	memcpy(&registro->src,buffer->data + desplazamiento,sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&registro->n,buffer->data + desplazamiento, sizeof(size_t));
	desplazamiento += sizeof(size_t);

	return registro;
}
t_registromcopy* deserializarCopy(t_stream * buffer){
	t_registromcopy * registro = malloc(sizeof(t_registromcopy));

	int desplazamiento = 0;

	memcpy(&registro->n,buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&registro->dst,buffer->data + desplazamiento,sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	registro->src = malloc(registro->n);
	memcpy( registro->src ,buffer->data + desplazamiento, registro->n );

	return registro;
}

t_registromap* deserealizarMap(t_stream * buffer){
	t_registromap* registro = malloc(sizeof(t_registromap));

	int largo_path = strlen( (char*) buffer->data ) + 1;
	int desplazamiento = 0;

	registro->path = strdup( buffer->data );
	desplazamiento += largo_path;

	registro->length = *(size_t*) ( buffer->data + desplazamiento );
	desplazamiento += sizeof(size_t);

	registro->flags = *(int*) ( buffer->data + desplazamiento );

	return registro;
}

t_registrosync* deserealizarMsync(t_stream * buffer){
	t_registrosync* registro = malloc(sizeof(t_registrosync));
	int desplazamiento = 0;

	registro->addr = *(uint32_t*) buffer->data;
	registro->len = *(size_t*) ( buffer->data + sizeof( uint32_t ) );

	return registro;
}

t_registrounmap* deserealizarUnmap(t_stream * buffer){
	t_registrounmap* registro = malloc(sizeof(t_registrounmap));

	memcpy(registro->dir,buffer->data,sizeof(uint));

	return registro;
}

//-----------------------------------Respuestas--------------------------------------

void enviarRespuestaAlloc(int server_socket, uint32_t tamanio) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = MUSE_ALLOC;

	serializarUINT32(unPaquete, tamanio);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarRespuestaGet(int server_socket, int operacionSatisfactoria, size_t size, void* buffer){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = MUSE_GET;

	serializarRespuestaGet( unPaquete, operacionSatisfactoria, size, buffer );

	enviarPaquetes(server_socket,unPaquete);
}

void enviarRespuestaCopy(int server_socket, int operacionSatisfactoria){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = MUSE_COPY;

	serializarNumero(unPaquete, operacionSatisfactoria);

	enviarPaquetes(server_socket,unPaquete);
}

void enviarRespuestaMap(int server_socket, uint32_t posicionMemoriaMapeada){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = MUSE_MAP;

	serializarUINT32(unPaquete, posicionMemoriaMapeada);

	enviarPaquetes(server_socket,unPaquete);
}

void enviarRespuestaMsync(int server_socket, int operacionSatisfactoria){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = MUSE_SYNC;

	serializarNumero(unPaquete, operacionSatisfactoria);

	enviarPaquetes(server_socket,unPaquete);
}

void enviarRespuestaUnmap(int server_socket,int operacionSatisfactoria){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = MUSE_UNMAP;

	serializarUINT32(unPaquete, operacionSatisfactoria);

	enviarPaquetes(server_socket,unPaquete);
}

void destruirRequestCopy( t_registromcopy* registroCopy ){
	free( registroCopy->src );
	free( registroCopy );
}
