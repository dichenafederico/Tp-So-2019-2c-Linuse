/*
 * sac_defs.h
 *
 *  Created on: 16 dic. 2019
 *      Author: utnso
 */

#ifndef SAC_DEFS_H_
#define SAC_DEFS_H_

#include <commons/log.h>


typedef struct fuse_client_config{
	char* puerto;
	char* disc_file_path;
} t_fuse_config;

t_fuse_config* g_fuse_config;

t_log* g_logger;

#endif /* SAC_DEFS_H_ */
