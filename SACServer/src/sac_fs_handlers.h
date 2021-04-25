/*
 * sac_fs_handlers.h
 *
 *  Created on: 16 dic. 2019
 *      Author: utnso
 */

#ifndef SAC_FS_HANDLERS_H_
#define SAC_FS_HANDLERS_H_

t_paquete* procesar_getattr( t_paquete* request );
t_paquete* procesar_mknod( t_paquete* request );
t_paquete* procesar_mkdir( t_paquete* request );
t_paquete* procesar_utimens( t_paquete* request );
t_paquete* procesar_unlink( t_paquete* request );
t_paquete* procesar_rmdir( t_paquete* request );
t_paquete* procesar_readdir( t_paquete* request );
t_paquete* procesar_truncate( t_paquete* request );
t_paquete* procesar_read( t_paquete* request );
t_paquete* procesar_write( t_paquete* request );
t_paquete* procesar_rename( t_paquete* request );

void init_fs(char* disc_path);

#endif /* SAC_FS_HANDLERS_H_ */
