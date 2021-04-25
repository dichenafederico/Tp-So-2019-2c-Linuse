/*
 * fuse_utils.h
 *
 *  Created on: 14 nov. 2019
 *      Author: utnso
 */

#ifndef FUSE_UTILS_H_
#define FUSE_UTILS_H_

#include <commons/config.h>

int get_datablock_index(off_t);
int get_indirect_block_index(off_t);
int get_datablock_index_inside_indirect_block(off_t);

int find_by_path(const char*);

size_t min(size_t, size_t);

void copy_file_contents(GFile*, const char*, size_t, off_t, int);

int liberarBloques(GFile* file, int cantidadDatablosActuales, int cantidadDatablocksFinales);
int reservarBloques( GFile* fileNode, int cantBlocksActuales, int cantBlocksFinales );
int get_occupied_datablocks_qty(size_t size);
int get_parent_node(const char *path);
int get_free_blocks();

void armar_config( char* path );

#endif /* FUSE_UTILS_H_ */
