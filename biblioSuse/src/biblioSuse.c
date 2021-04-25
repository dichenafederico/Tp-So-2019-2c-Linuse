/* Lib implementation: It'll only schedule the last thread that was created */
#include "biblioSuse.h"

#define CONFIG_PATH "/home/utnso/workspace/tp-2019-2c-No-C-Nada/configs/biblioSuse/biblioSuse.cfg"
int g_max_multiprog; // Esta es la respuesta de grado de multiprogramacion en el server SUSE
int g_server_socket; // Esta es la referencia global al socket conectado al server SUSE

int suse_create(int tid) {
	/*
	 *  No chequeo contra max nivel de multiprog porque eso solo indica la cantidad de threads
	 *  que pueden estar en runnign/waiting, en NEW puede haber muchos mas que el max nivel
	 */
	printf("Se creo un nuevo hilo: %d...\n", tid);
	enviarThreadCreate( g_server_socket, tid );
	return 0;
}

int suse_schedule_next(void) {
	log_debug( g_logger, "Suse schedule next devuelvo siempre 0");
	enviarThreadScheduleNext( g_server_socket );
	return esperarRespuestaThreadScheduleNext( g_server_socket );
}

int suse_join(int tid) {
	printf("Esperando se cierra el hilo:%d \n", tid);
	enviarThreadJoin( g_server_socket, tid );
	return esperarRespuestaThreadJoin( g_server_socket );
}

int suse_close(int tid){
	printf("Haciendo close del hilo: %i\n", tid);
	enviarThreadClose( g_server_socket, tid );
	return esperarRespuestaThreadClose( g_server_socket );
}

int suse_wait(int tid, char *sem_name){
	log_info( g_logger, "Sem wait de %s, para tid %d", sem_name, tid );
	enviarSemWait( g_server_socket, tid, sem_name );
	return esperarRespuestaSemWait( g_server_socket );
}

int suse_signal(int tid, char *sem_name){
	log_info( g_logger, "Sem post de %s, para tid %d", sem_name, tid );
	enviarSemPost( g_server_socket, tid, sem_name );
	return esperarRespuestaSemPost( g_server_socket );
}

struct hilolay_operations operaciones = {
		.suse_close = &suse_close,
		.suse_create = &suse_create,
		.suse_join = &suse_join,
		.suse_schedule_next = &suse_schedule_next,
		.suse_signal = &suse_signal,
		.suse_wait = &suse_wait
};

void hilolay_init(void) {
	init_config( CONFIG_PATH );
	iniciar_log();

	log_info( g_logger, "Me conecto a SUSE server en %s:%s", g_config->ip, g_config->puerto );
	g_server_socket = conectarCliente( g_config->ip, atoi( g_config->puerto ), BIBLIO_SUSE_CLIENT_ID);
	esperarRespuestaConfig( g_server_socket );
	init_internal(&operaciones);
}

void init_config(char *path){
	g_config_commons = config_create(path);
	g_config = malloc( sizeof( t_config_lib_suse ) );

	g_config->ip = config_get_string_value( g_config_commons, "IP" );
	g_config->puerto = config_get_string_value( g_config_commons, "PUERTO" );
}

void enviarThreadCreate(int socket_dst, int tid) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigoOperacion = SUSE_CREATE;
	serializarNumero(unPaquete, tid);
	enviarPaquetes(socket_dst, unPaquete);
}

void enviarThreadJoin(int socket, int tid) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigoOperacion = SUSE_JOIN;
	serializarNumero(unPaquete, tid);
	enviarPaquetes(socket, unPaquete);
}

void enviarThreadClose(int socket, int tid) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigoOperacion = SUSE_CLOSE;
	serializarNumero(unPaquete, tid);
	enviarPaquetes(socket, unPaquete);
}

void enviarThreadScheduleNext( int socket ) {  // No le envío TID al SUSE Server, pero el Server Si envía el próximo Thread a ejecutar en la rta.
	t_paquete * unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigoOperacion = SUSE_SCHEDULE_NEXT;
	serializarNumero(unPaquete, 0);
	enviarPaquetes( socket, unPaquete );
}


void enviarSemWait( int socket_dst, int tid, char* nombre ){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = SUSE_WAIT;

	serializarSemaforoRequest(unPaquete, tid, nombre);

	enviarPaquetes(socket_dst, unPaquete);
}

void enviarSemPost( int socket_dst, int tid, char* nombre ){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = SUSE_SIGNAL;

	serializarSemaforoRequest(unPaquete, tid, nombre);

	enviarPaquetes(socket_dst, unPaquete);
}



void esperarRespuestaConfig( int socket ){
	t_paquete *respuestaMultiprog = recibirArmarPaquete( socket );
	if( respuestaMultiprog->codigoOperacion == SUSE_GRADO_MULTIPROG ){
		g_max_multiprog = deserializarNumero( respuestaMultiprog->buffer );
		log_info( g_logger, "Recibi este grado de multiprog: %d", g_max_multiprog );
	} else {
		log_error( g_logger, "Recibi algo que no es el grado del multiprog");
	}
	destruirPaquete( respuestaMultiprog );
}

int esperarRespuestaThreadClose( int socket ){
	t_paquete *respuestaMultiprog = recibirArmarPaquete( socket );
	if( respuestaMultiprog->codigoOperacion != SUSE_CLOSE ){
		log_error( g_logger, "Recibi algo que no es respuesta de close");
	}
	int respuesta = deserializarNumero( respuestaMultiprog->buffer );
	log_info( g_logger, "Suse_Close recibio esta respuesta %d", respuesta );
	destruirPaquete( respuestaMultiprog );
	return respuesta;
}

int esperarRespuestaThreadJoin( int socket ){
	t_paquete *respuestaMultiprog = recibirArmarPaquete( socket );
	if( respuestaMultiprog->codigoOperacion != SUSE_JOIN ){
		log_error( g_logger, "Recibi algo que no es respuesta de join");
	}
	int respuesta = deserializarNumero( respuestaMultiprog->buffer );
	log_info( g_logger, "Suse_Join recibio esta respuesta %d", respuesta );
	destruirPaquete( respuestaMultiprog );
	return respuesta;
}

int  esperarRespuestaThreadScheduleNext( int socket ){
	t_paquete *respuestaMultiprog = recibirArmarPaquete( socket );
	if( respuestaMultiprog->codigoOperacion != SUSE_SCHEDULE_NEXT ){
		log_error( g_logger, "Recibi algo que no es respuesta de schedule_next");
	}
	int tid_to_exec = deserializarNumero( respuestaMultiprog->buffer );
	log_info( g_logger, "Suse_Schedule_Next:entra en ejecucion el TID %d", tid_to_exec );
	destruirPaquete( respuestaMultiprog );
	return tid_to_exec;
}


int esperarRespuestaSemWait( int socket ){
	t_paquete *respuestaMultiprog = recibirArmarPaquete( socket );
	if( respuestaMultiprog->codigoOperacion != SUSE_WAIT ){
		log_error( g_logger, "Recibi algo que no es respuesta de sem wait");
	}
	int respuesta = deserializarNumero( respuestaMultiprog->buffer );
	log_info( g_logger, "Sem wait recibio esta respuesta %d", respuesta );
	destruirPaquete( respuestaMultiprog );
	return respuesta;
}

int esperarRespuestaSemPost( int socket ){
	t_paquete *respuetaMultiprog = recibirArmarPaquete( socket );
	if( respuetaMultiprog->codigoOperacion != SUSE_SIGNAL ){
		log_error( g_logger, "Recibi algo que no es respuesta de sem signal");
	}
	int respuesta = deserializarNumero( respuetaMultiprog->buffer );
	log_info( g_logger, "Sem signal recibio esta respuesta %d", respuesta );
	destruirPaquete( respuetaMultiprog );
	return respuesta;
}

void iniciar_log(void) {
	g_logger = log_create("/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioSuse/logFiles/biblioSuse.log", "biblioSuse", 1, LOG_LEVEL_TRACE);
}
