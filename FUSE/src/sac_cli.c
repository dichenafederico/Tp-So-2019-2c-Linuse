// -- FUSE import y boilerplate
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>

// -- SAC-CLI imports
#include <commons/log.h>
#include <biblioNOC/paquetes.h>
#include <biblioNOC/conexiones.h>
#include "sac.h"
#include "fuse_utils.h"
#include "sac_cod_ops.h"
#include "fuse_serializaciones.h"

t_log* g_logger;

/*
 * Esta es una estructura auxiliar utilizada para almacenar parametros
 * que nosotros le pasemos por linea de comando a la funcion principal
 * de FUSE
 */
struct t_runtime_options {
} runtime_options;

/*
 * Esta Macro sirve para definir nuestros propios parametros que queremos que
 * FUSE interprete. Esta va a ser utilizada mas abajo para completar el campos
 * welcome_msg de la variable runtime_options
 */
#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }


static int do_centralized_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	log_info( g_logger, "[readdir]:%s", path );

	t_paquete* request = armarPaquetePathConOperacion( path, SAC_readdir );
	t_paquete* response = send_request( request, g_fuse_config->ip, g_fuse_config->puerto );

	t_readdir_response* readdir_response = deserializarReaddir( response->buffer );
	destruirPaquete( response );

	if( readdir_response->errno_value != 0 ){
		int errno_value = readdir_response->errno_value;
		destruirReaddirResponse( readdir_response );

		errno = errno_value;
		return -errno_value;
	}

	filler(buffer, ".", NULL, 0); // Current Directory
	filler(buffer, "..", NULL, 0); // Parent Directory

	for( int i = 0; i < readdir_response->count; i++ ){
		filler(buffer, readdir_response->found[ i ], NULL, 0);
	}

	destruirReaddirResponse( readdir_response );
	return 0;
}

static int do_centralized_truncate(const char* path, off_t size){
	log_info( g_logger, "[truncate]:%s a size %ld", path, size );

	t_paquete* paquete = armarPaqueteTruncate( path, size, SAC_truncate );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static int do_centralized_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	log_info( g_logger, "[read]:%s", path );

	t_paquete* paquete = armarPaqueteRead( path, size, offset, SAC_read );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_read_response* read_response = deserializarRead( response->buffer );

	if( read_response->errno_value != 0 ){
		int errno_value = read_response->errno_value;
		errno = errno_value;
		destruirPaquete( response );
		destruirReadResponse( read_response );
		return -errno_value;
	}

	int copy_size = read_response->size;
	memcpy( buffer, read_response->data, copy_size );

	destruirPaquete( response );
	destruirReadResponse( read_response );
	return copy_size;
}

static int do_centralized_getattr(const char *path, struct stat *st) {
	log_info( g_logger, "[getattr]:%s", path );

	t_paquete* request = armarPaquetePathConOperacion( path, SAC_getattr );
	t_paquete* response = send_request( request, g_fuse_config->ip, g_fuse_config->puerto );

	t_getattr_response* getattr_response = deserializarGetattr( response->buffer );
	destruirPaquete( response ); // este se puede free de una porque solo saca valores

	if( getattr_response->type == 0 ){
		free( getattr_response );
		errno = ENOENT;
		return -ENOENT;
	}

	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	st->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now

	if( getattr_response->type == 1 )
		st->st_mode = S_IFREG | 0644;
	else
		st->st_mode = S_IFDIR | 0755;

	st->st_nlink = 1;
	st->st_mtime = getattr_response->last_mod_time; // The last "m"odification of the file/directory is right now
	st->st_size = getattr_response->size;

	free( getattr_response );
	return 0;
}

static int do_centralized_mknod(const char *path, mode_t mode, dev_t device){
	log_info( g_logger, "[mknod]:%s", path );

	t_paquete* paquete = armarPaquetePathConOperacion( path, SAC_mknod );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static int do_centralized_utimens( const char *path, const struct timespec tv[2]){
	log_info( g_logger, "[utimens]:%s", path );

	// donde tv[ 0 ] es last access time y tv[ 1 ] es last modified time
	// solo existe last modified time en SAC asi que no tengo fecha de ultimo acceso que actualizar
	t_paquete* paquete = armarPaqueteUtimens( path, tv[0].tv_sec, SAC_utimens );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static int do_centralized_mkdir(const char *path, mode_t mode){
	log_info( g_logger, "[mkdir]:%s", path );
	t_paquete* paquete = armarPaquetePathConOperacion( path, SAC_mkdir );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static int do_centralized_unlink (const char *path){
	log_info( g_logger, "[unlink]:%s", path );

	t_paquete* paquete = armarPaquetePathConOperacion( path, SAC_unlink );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static int do_centralized_rmdir( const char *path ){
	log_info( g_logger, "[rmdir]:%s", path );

	t_paquete* paquete = armarPaquetePathConOperacion( path, SAC_rmdir );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static int do_centralized_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi){
	log_info( g_logger, "[write]:%s", path );

	t_paquete* paquete = armarPaqueteWrite( path, size, offset, buffer, SAC_write );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static int do_centralized_rename(const char *old_path, const char *new_path){
	log_info( g_logger, "[rename]:%s a %s", old_path, new_path );

	t_paquete* paquete = armarPaqueteRename( old_path, new_path, SAC_rename );
	t_paquete* response = send_request( paquete, g_fuse_config->ip, g_fuse_config->puerto );

	t_return_errno_response* errno_response = deserializarReturnErrno( response->buffer );

	if( errno_response->return_value < 0 )
		errno = errno_response->errno_value;

	int return_value = errno_response->return_value;

	destruirPaquete( response );
	free( errno_response );

	return return_value;
}

static struct fuse_operations operations = {
	.getattr  = do_centralized_getattr,
	.mknod    = do_centralized_mknod,
	.mkdir    = do_centralized_mkdir,
	.utimens  = do_centralized_utimens,
	.unlink   = do_centralized_unlink,
	.rmdir    = do_centralized_rmdir,
	.readdir  = do_centralized_readdir,
	.truncate = do_centralized_truncate,
	.read     = do_centralized_read,
	.write    = do_centralized_write,
	.rename   = do_centralized_rename,
};

/** keys for FUSE_OPT_ options */
enum {
	KEY_VERSION,
	KEY_HELP,
};

/*
 * Esta estructura es utilizada para decirle a la biblioteca de FUSE que
 * parametro puede recibir y donde tiene que guardar el valor de estos
 */
static struct fuse_opt fuse_options[] = {
		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};

// Dentro de los argumentos que recibe nuestro programa obligatoriamente
// debe estar el path al directorio donde vamos a montar nuestro FS
int main(int argc, char *argv[]) {

	g_logger = log_create( "/home/utnso/logs/FUSE/cli.log", "SACCLI", 1, LOG_LEVEL_TRACE );
	armar_config( "/home/utnso/workspace/tp-2019-2c-No-C-Nada/configs/FUSE/sacCli.cfg" );
	// TODO levantar config, crear struct y var global
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
		/** error parsing options */
		log_error( g_logger, "Invalid fuse arguments" );
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar todx
	// en varios threads
	return fuse_main(args.argc, args.argv, &operations, NULL);
}

// Funciones para conectar una sola ves por socket, request y, liberar

t_paquete* send_request( t_paquete* request, char* ip, char* puerto ){
	int server_socket = conectarCliente( ip, atoi( puerto ), SAC_CLI );

	enviarPaquetes( server_socket, request );
	t_paquete* respuesta = recibirArmarPaquete( server_socket );

	enviarAvisoDesconexion( server_socket );
	close( server_socket );
	return respuesta;
}

void armar_config( char* path ){
	t_config* config = config_create( path );
	g_fuse_config = malloc( sizeof( t_fuse_config ) );

	g_fuse_config->ip = strdup( (char*) config_get_string_value( config, "IP" ) );
	g_fuse_config->puerto = strdup( (char*) config_get_string_value( config, "PUERTO" ) );

	config_destroy( config );
}
