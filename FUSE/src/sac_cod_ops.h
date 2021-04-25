/*
 * sac_cod_ops.h
 *
 *  Created on: 15 dic. 2019
 *      Author: utnso
 */

#ifndef SAC_COD_OPS_H_
#define SAC_COD_OPS_H_


#define SAC_CLI 1010

typedef enum  {
	SAC_getattr = 750,
	SAC_readdir,
	SAC_mknod,
	SAC_mkdir,
	SAC_unlink,
	SAC_rmdir,
	SAC_write,
	SAC_read,
	SAC_rename,
	SAC_truncate,
	SAC_utimens,
} t_cod_operaciones_SAC;

#endif /* SAC_COD_OPS_H_ */
