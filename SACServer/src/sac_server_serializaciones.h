/*
 * sac_server_serializaciones.h
 *
 *  Created on: 15 dic. 2019
 *      Author: utnso
 */

#ifndef SAC_SERVER_SERIALIZACIONES_H_
#define SAC_SERVER_SERIALIZACIONES_H_

#include <string.h>
#include <biblioNOC/paquetes.h>

typedef struct fuse_utimens_payload{
	char* path;
	time_t modified_time;
} t_utimens_request;

typedef struct fuse_truncate_payload{
	char* path;
	off_t size;
} t_truncate_request;

typedef struct fuse_read_payload{
	char* path;
	size_t size;
	off_t offset;
} t_read_request;

typedef struct fuse_write_payload{
	char* path;
	size_t size;
	off_t offset;
	void* data;
} t_write_request;

typedef struct fuse_rename_payload{
	char* old_path;
	char* new_path;
} t_rename_request;

// Serializaciones
void agregarPathBufferReaddir( t_paquete* paquete, char* path );
t_paquete* armarPaqueteReturnErrnoConOperacion( int return_value, int errno_code, int codigo_op );
t_paquete* armarPaqueteGetattr( int type, time_t last_mod_time, size_t size, int codigo_op );
t_paquete* armarPaqueteReaddir( int errno, int codigo_op );
t_paquete* armarPaqueteRead( int errno, size_t size, void* buffer, int codigo_op );

// Deserializaciones
t_utimens_request* deserializarUtimensReq( t_stream* buffer );
t_truncate_request* deserializarTruncateReq( t_stream* buffer );
t_read_request* deserializarReadReq( t_stream* buffer );
t_write_request* deserializarWriteReq( t_stream* buffer );
t_rename_request* deserializarRenameReq( t_stream* buffer );

// Destruir structs
void destruirUtimensReq( t_utimens_request* utimens_req );
void destruirTruncateReq( t_truncate_request* truncate_req );
void destruirReadReq( t_read_request* read_req );
void destruirWriteReq( t_write_request* write_req );
void destruirRenameReq( t_rename_request* rename_req );

#endif /* SAC_SERVER_SERIALIZACIONES_H_ */
