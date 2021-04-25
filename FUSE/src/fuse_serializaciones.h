/*
 * fuse_serializaciones.h
 *
 *  Created on: 15 dic. 2019
 *      Author: utnso
 */

#ifndef FUSE_SERIALIZACIONES_H_
#define FUSE_SERIALIZACIONES_H_

#include <string.h>
#include <biblioNOC/paquetes.h>

typedef struct fuse_getattr_response{
	int type; // 0 no existe, 1 es archivo, 2 es directorio
	time_t last_mod_time;
	size_t size;
} t_getattr_response;

typedef struct fuse_readdir_response{
	int errno_value;
	int count;
	char** found;
} t_readdir_response;

typedef struct fuse_read_response{
	int errno_value;
	size_t size;
	void* data;
} t_read_response;

typedef struct fuse_return_errno_response{
	int return_value;
	int errno_value;
} t_return_errno_response;

// Serializaciones
t_paquete* armarPaquetePathConOperacion( char* path, int codigo_op );
t_paquete* armarPaqueteUtimens( char* path, time_t new_time, int codigo_op );
t_paquete* armarPaqueteTruncate( char* path, off_t size, int codigo_op );
t_paquete* armarPaqueteRead( char* path, size_t size, off_t offset, int codigo_op );
t_paquete* armarPaqueteWrite( char* path, size_t size, off_t offset, void* buffer, int codigo_op );
t_paquete* armarPaqueteRename( char* old_path, char* new_path, int codigo_op );

// Deserializaciones
t_return_errno_response * deserializarReturnErrno(t_stream * buffer);
t_getattr_response * deserializarGetattr(t_stream * buffer);
t_readdir_response* deserializarReaddir(t_stream * buffer);
t_read_response* deserializarRead(t_stream * buffer);

// Destruir structs
void destruirReaddirResponse( t_readdir_response* readdir_response );
void destruirReadResponse( t_read_response* read_response );

#endif /* FUSE_SERIALIZACIONES_H_ */
