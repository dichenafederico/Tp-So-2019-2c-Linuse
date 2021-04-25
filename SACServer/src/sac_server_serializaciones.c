/*
 * sac_server_serializaciones.c
 *
 *  Created on: 15 dic. 2019
 *      Author: utnso
 */

#include "sac_server_serializaciones.h"

void serializarDosNumeros(t_paquete* unPaquete, int numero_a, int numero_b){
	int tamTotal = sizeof(int) * 2;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	memcpy(unPaquete->buffer->data, &numero_a, sizeof( int ));
	memcpy(unPaquete->buffer->data + sizeof( int ), &numero_b, sizeof( int ));
}

void serializarGetattr( t_paquete* paquete, int type, time_t last_mod_time, size_t size ){
	int tamTotal = sizeof( int ) + sizeof( time_t ) + sizeof( size_t );
	int desplazamiento = 0;

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	memcpy(paquete->buffer->data, &type, sizeof( int ));
	desplazamiento += sizeof( int );

	memcpy(paquete->buffer->data + desplazamiento, &last_mod_time, sizeof( time_t ));
	desplazamiento += sizeof( time_t );

	memcpy(paquete->buffer->data + desplazamiento, &size, sizeof( size_t ));
}

void inicializarBufferReaddir( t_paquete* paquete, int errno_value ){
	int tamTotal = sizeof( int ) * 2;
	int countInicial = 0;

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	memcpy(paquete->buffer->data, &errno_value, sizeof( int ));

	memcpy(paquete->buffer->data + sizeof( int ), &countInicial, sizeof( int ));
}

void serializarRead( t_paquete* paquete, int errno_value, size_t size, void* buffer ){
	int tamTotal = sizeof( int ) + sizeof( size_t ) + size;
	int desplazamiento = 0;

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	memcpy(paquete->buffer->data, &errno_value, sizeof( int ));
	desplazamiento += sizeof( int );

	memcpy(paquete->buffer->data + desplazamiento, &size, sizeof( size_t ));
	desplazamiento += sizeof( size_t );

	memcpy(paquete->buffer->data + desplazamiento, buffer, size);
}

void agregarPathBufferReaddir( t_paquete* paquete, char* path ){
	int largo_path = strlen( path ) + 1;
	paquete->buffer->data = realloc( paquete->buffer->data, paquete->buffer->size + largo_path );
	strcpy( paquete->buffer->data + paquete->buffer->size, path );
	paquete->buffer->size += largo_path;

	int nuevo_count = *(int*) ( paquete->buffer->data + sizeof( int ) );
	nuevo_count++;
	memcpy(paquete->buffer->data + sizeof( int ), &nuevo_count, sizeof( int ));
}

t_paquete* armarPaqueteReturnErrnoConOperacion( int return_value, int errno_code, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarDosNumeros( paquete, return_value, errno_code );
	return paquete;
}

t_paquete* armarPaqueteGetattr( int type, time_t last_mod_time, size_t size, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarGetattr( paquete, type, last_mod_time, size );
	return paquete;
}

t_paquete* armarPaqueteReaddir( int errno_value, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	inicializarBufferReaddir( paquete, errno_value );
	return paquete;
}

t_paquete* armarPaqueteRead( int errno_value, size_t size, void* buffer, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarRead( paquete, errno_value, size, buffer );
	return paquete;
}

t_utimens_request* deserializarUtimensReq( t_stream* buffer ){
	t_utimens_request* utimens_req = malloc( sizeof( t_utimens_request ) );

	// path - time_t mod tim
	utimens_req->path = strdup( (char*) buffer->data );
	utimens_req->modified_time = *(time_t*) ( buffer->data + strlen( utimens_req->path ) + 1 );

	return utimens_req;
}

t_truncate_request* deserializarTruncateReq( t_stream* buffer ){
	t_truncate_request* truncate_req = malloc( sizeof( t_truncate_request ) );

	// path - off_t size
	truncate_req->path = strdup( (char*) buffer->data );
	truncate_req->size = *(off_t*) ( buffer->data + strlen( truncate_req->path ) + 1 );

	return truncate_req;
}

t_read_request* deserializarReadReq( t_stream* buffer ){
	t_read_request* read_req = malloc( sizeof( t_read_request ) );

	int desplazamiento = 0;
	// path - size_t size - off_t offset
	read_req->path = strdup( (char*) buffer->data );
	desplazamiento += ( strlen( read_req->path ) + 1 );

	read_req->size = *(size_t*) ( buffer->data + desplazamiento );
	desplazamiento += sizeof( size_t );

	read_req->offset = *(off_t*) ( buffer->data + desplazamiento );

	return read_req;
}

t_write_request* deserializarWriteReq( t_stream* buffer ){
	t_write_request* write_req = malloc( sizeof( t_write_request ) );

	int desplazamiento = 0;
	// path - size_t size - off_t offset - void* buffer
	write_req->path = strdup( (char*) buffer->data );
	desplazamiento += ( strlen( write_req->path ) + 1 );

	write_req->size = *(size_t*) ( buffer->data + desplazamiento );
	desplazamiento += sizeof( size_t );

	write_req->offset = *(off_t*) ( buffer->data + desplazamiento );
	desplazamiento += sizeof( off_t );

	write_req->data = malloc( write_req->size );
	memcpy( write_req->data, buffer->data + desplazamiento, write_req->size );

	return write_req;
}

t_rename_request* deserializarRenameReq( t_stream* buffer ){
	t_rename_request* rename_req = malloc( sizeof( t_rename_request ) );

	// old_path - new_path
	rename_req->old_path = strdup( (char*) buffer->data );
	rename_req->new_path = strdup( (char*) ( buffer->data + strlen( rename_req->old_path) + 1 ) );

	return rename_req;
}

void destruirUtimensReq( t_utimens_request* utimens_req ){
	free( utimens_req->path );
	free( utimens_req );
}

void destruirTruncateReq( t_truncate_request* truncate_req ){
	free( truncate_req->path );
	free( truncate_req );
}

void destruirReadReq( t_read_request* read_req ){
	free( read_req->path );
	free( read_req );
}

void destruirWriteReq( t_write_request* write_req ){
	free( write_req->path );
	free( write_req->data );
	free( write_req );
}

void destruirRenameReq( t_rename_request* rename_req ){
	free( rename_req->old_path );
	free( rename_req->new_path );
	free( rename_req );
}
