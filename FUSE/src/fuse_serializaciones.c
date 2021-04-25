/*
 * fuse_serializaciones.c
 *
 *  Created on: 15 dic. 2019
 *      Author: utnso
 */
#include "fuse_serializaciones.h"

void serializarWrite( t_paquete* paquete, char* path, size_t size, off_t offset, void* buffer ){
	int largo_path = strlen( path ) + 1;
	int tamTotal = largo_path + sizeof( size_t ) + sizeof( off_t ) + size;

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	int data_offset = 0;

	strcpy( paquete->buffer->data, path );
	data_offset += largo_path;

	memcpy( paquete->buffer->data + data_offset, &size, sizeof( size_t ) );
	data_offset += sizeof( size_t );

	memcpy( paquete->buffer->data + data_offset, &offset, sizeof( off_t ) );
	data_offset += sizeof( off_t );

	memcpy( paquete->buffer->data + data_offset, buffer, size );
}

void serializarUtimens( t_paquete* paquete, char* path, time_t mod_time ){
	int largo_path = strlen( path ) + 1;
	int tamTotal = largo_path + sizeof( time_t );

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	int data_offset = 0;

	strcpy( paquete->buffer->data, path );
	data_offset += largo_path;

	memcpy( paquete->buffer->data + data_offset, &mod_time, sizeof( time_t ) );
}

void serializarTruncate( t_paquete* paquete, char* path, off_t size ){
	int largo_path = strlen( path ) + 1;
	int tamTotal = largo_path + sizeof( off_t );

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	strcpy( paquete->buffer->data, path );

	memcpy( paquete->buffer->data + largo_path, &size, sizeof( off_t ) );
}

void serializarRead( t_paquete* paquete, char* path, size_t size, off_t offset ){
	int largo_path = strlen( path ) + 1;
	int tamTotal = largo_path + sizeof( size_t ) + sizeof( off_t );

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	strcpy( paquete->buffer->data, path );
	desplazamiento+= largo_path;

	memcpy( paquete->buffer->data + desplazamiento, &size, sizeof( size_t ) );
	desplazamiento += sizeof( size_t );

	memcpy( paquete->buffer->data + desplazamiento, &offset, sizeof( off_t ) );
}

void serializarRename( t_paquete* paquete, char* old_path, char* new_path ){
	int largo_old_path = strlen( old_path ) + 1;
	int largo_new_path = strlen( new_path ) + 1;

	int tamTotal = largo_old_path + largo_new_path;

	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = tamTotal;
	paquete->buffer->data = malloc(tamTotal);

	strcpy( paquete->buffer->data, old_path );

	strcpy( paquete->buffer->data + largo_old_path, new_path );
}

t_return_errno_response * deserializarReturnErrno(t_stream * buffer){
	t_return_errno_response* response = malloc( sizeof( t_return_errno_response ) );

	response->return_value = *(int*) (buffer->data);
	response->errno_value = *(int*) (buffer->data + sizeof( int ));

	return response;
}

t_getattr_response * deserializarGetattr(t_stream * buffer){
	t_getattr_response* response = malloc( sizeof( t_getattr_response ) );
	int desplazamiento = 0;

	response->type = *(int*) (buffer->data);
	desplazamiento += sizeof( int );

	response->last_mod_time = *(time_t*) (buffer->data + desplazamiento);
	desplazamiento += sizeof( time_t );

	response->size = *(size_t*) (buffer->data + desplazamiento);

	return response;
}

t_readdir_response* deserializarReaddir(t_stream * buffer){
	t_readdir_response* response = malloc( sizeof( t_readdir_response ) );
	int desplazamiento = 0;

	response->errno_value = *(int*) (buffer->data);
	desplazamiento += sizeof( int );

	response->count = *(int*) (buffer->data + desplazamiento);
	desplazamiento += sizeof( int );

	response->found = malloc( sizeof( char* ) * response->count );

	for( int i = 0; i < response->count; i++ ){
		response->found[ i ] = strdup( buffer->data + desplazamiento );
		desplazamiento += ( strlen( buffer->data + desplazamiento ) + 1 );
	}

	return response;
}

t_read_response* deserializarRead(t_stream * buffer){
	t_read_response* response = malloc( sizeof( t_read_response ) );
	int desplazamiento = 0;

	// int errno - size_t size - void* buffer
	response->errno_value = *(int*) (buffer->data);
	desplazamiento += sizeof( int );

	response->size = *(size_t*) (buffer->data + desplazamiento);
	desplazamiento += sizeof( size_t );

	response->data = malloc( response->size );
	memcpy( response->data, buffer->data + desplazamiento, response->size );

	return response;
}

t_paquete* armarPaquetePathConOperacion( char* path, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarMensaje( paquete, path );
	return paquete;
}

t_paquete* armarPaqueteUtimens( char* path, time_t new_mod_time, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarUtimens( paquete, path, new_mod_time );
	return paquete;
}

t_paquete* armarPaqueteTruncate( char* path, off_t size, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarTruncate( paquete, path, size );
	return paquete;
}

t_paquete* armarPaqueteRead( char* path, size_t size, off_t offset, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarRead( paquete, path, size, offset );
	return paquete;
}

t_paquete* armarPaqueteWrite( char* path, size_t size, off_t offset, void* buffer, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarWrite( paquete, path, size, offset, buffer );
	return paquete;
}

t_paquete* armarPaqueteRename( char* old_path, char* new_path, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarRename( paquete, old_path, new_path );
	return paquete;
}

void destruirReaddirResponse( t_readdir_response* readdir_response ){
	for( int i = 0; i < readdir_response->count; i++ ){
		free( readdir_response->found[ i ] );
	}

	free( readdir_response->found );
	free( readdir_response );
}

void destruirReadResponse( t_read_response* read_response ){
	free( read_response->data );
	free( read_response );
}
