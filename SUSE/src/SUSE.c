/*
 ============================================================================
 Name        : suseServer.c
 *
 *  Created on: 1 nov. 2019
 *      Author: utnso
 ============================================================================
 */

#include "SUSE.h"

/*--------------------------------definiciones-------------------------------*/
t_paquete* 				procesarPaqueteLibSuse				( t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente );
t_paquete* 				procesarThreadCreate				( t_paquete* paquete, t_client_suse* cliente_suse, int is_main_thread, int socket_cliente);
t_paquete* 				procesarThreadClose					( t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente);
t_paquete* 				procesarThreadJoin					( t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente);
t_paquete* 				procesarThreadScheduleNext			( t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente);
t_paquete* 				procesarSemSignal					( t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente);
t_paquete* 				procesarSemWait						( t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente);
t_paquete* 				armarPaqueteNumeroConOperacion		( int numero, int codigo_op );
void					esperarPaqueteCreateMain			( t_client_suse* cliente_suse, int socket_cliente );
/*
 * @NAME: find_thread_by_tid_in_parent
 * @DESC: Retorna el thread con el tid buscando dentro del proceso padre, NULL en caso de no existir
 * busco solo en RUNNING, READY, y en las Listas Globles EXIT y BLOCKED, si esta en NEW no lo busco
 */
t_client_thread_suse* 	find_thread_by_tid_in_parent		( int tid, t_client_suse* proceso_padre );

/*
 *  @NAME: find_sem_by_name
 *  @DESC: Retorna el semaforo con nombre sem_name, NULL en caso de no existir
 */
t_semaforo_suse* 		find_sem_by_name 					( char* sem_name );
int 					cant_hilos_proceso_padre			( t_list* lista_threads, t_client_suse* proceso_padre );
void 					get_runnning_procs_stats			( void );
double 					indice_sjf							( t_client_thread_suse* thread_to_compare);
void 					snd_schd_nxt_blockd_process			( t_client_thread_suse* thread_t );
/*----------------------------------------------------------------------------*/

int main(void) {

	iniciar_logger();
	iniciar_config("/home/utnso/workspace/tp-2019-2c-No-C-Nada/configs/SUSE/suseServer.cfg");
	iniciar_g_metrics();
	update_metrics_continue();
	inicializar_estructuras();
	inicializar_semaforos();

	iniciarServidor(g_config_server->puerto, g_logger, (void*)atenderConexion);

	log_destroy( g_logger );
	log_destroy( g_metrics );
	config_destroy( g_config );
	return EXIT_SUCCESS;

}

void atenderConexion(int socketCliente) {
	log_debug(g_logger, "Atiende conexion con socket %d", socketCliente);
	t_paquete* package = recibirArmarPaquete(socketCliente);
	t_paquete* response;

	log_debug(g_logger, "Checkeo que el paquete sea handshake");
	// Espero recibir el handshake y trato segun quien se conecte
	int handshake_code = recibirHandshake(package);

	destruirPaquete( package );
	switch (handshake_code) {
	case BIBLIO_SUSE_CLIENT_ID:
		;
		log_debug(g_logger, "Recibi el handshake del cliente");
		t_client_suse* cliente_suse = malloc( sizeof( t_client_suse ) );

		enviarMultiProg( socketCliente );
		esperarPaqueteCreateMain( cliente_suse, socketCliente );
		while (1) {
			package = recibirArmarPaquete(socketCliente);
			log_debug(g_logger, "Recibo paquete");

			if ( package == NULL || package->codigoOperacion == ENVIAR_AVISO_DESCONEXION ) {
				log_warning(g_logger, "Cierro esta conexion del cliente_suse %d", socketCliente);
				bool socket_buscado(void* proceso){
					t_client_suse* proceso_t = (t_client_suse*) proceso;
					return proceso_t->self_socket == socketCliente;
				}
				list_remove_by_condition(g_running_procs,socket_buscado);
				break;
			};

			response = procesarPaqueteLibSuse(package, cliente_suse, socketCliente);
			if( response != NULL )
				enviarPaquetes(socketCliente, response);
		}
		break;
	default:
		log_warning(g_logger, "El paquete recibido no es handshake");
		break;
	}

	close(socketCliente);
}

t_paquete* procesarPaqueteLibSuse(t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente) {
	log_debug(g_logger, "Proceso codigo op %d", paquete->codigoOperacion);
	t_paquete* response = NULL;

	switch (paquete->codigoOperacion) {
	/* nunca entra por aca porque el handshake lo recibo cuando entro a "attendConnection" */
	case SUSE_CREATE:
		response = procesarThreadCreate( paquete, cliente_suse, 0, socket_cliente );
		break;
	case SUSE_CLOSE:
		response = procesarThreadClose( paquete, cliente_suse, socket_cliente);
		break;
	case SUSE_JOIN:
		response = procesarThreadJoin( paquete, cliente_suse, socket_cliente);
		break;
	case SUSE_SCHEDULE_NEXT:
		response = procesarThreadScheduleNext( paquete, cliente_suse, socket_cliente);
		break;
	case SUSE_SIGNAL:
		response = procesarSemSignal( paquete, cliente_suse, socket_cliente );
		break;
	case SUSE_WAIT:
		response = procesarSemWait( paquete, cliente_suse, socket_cliente );
		break;
	}

	destruirPaquete( paquete );
	return response;
}

t_paquete* procesarThreadCreate(t_paquete* paquete, t_client_suse* cliente_suse, int is_main_thread, int socket_cliente){
	log_info( g_logger, "Recibi un create");

	int tid = deserializarNumero( paquete->buffer );
	t_client_thread_suse* nuevo_thread = malloc( sizeof( t_client_thread_suse ) );
	nuevo_thread->tid = tid;
	nuevo_thread->time_created = time( NULL );
	nuevo_thread->proceso_padre = cliente_suse;
	nuevo_thread->threads_bloqueados = list_create();
	if( is_main_thread == 1 ) {
		// Cuando llega el create del main thread ya esta corriendo
		nuevo_thread->estado = RUNNING;
		nuevo_thread->time_last_run = time( NULL );
		nuevo_thread->time_last_yield = time( NULL );

		cliente_suse->main_tid = tid;
		cliente_suse->running_thread = nuevo_thread;
		cliente_suse->self_socket = socket_cliente;
		cliente_suse->ready = list_create();
		pthread_mutex_init(&( cliente_suse->being_externally_scheduled ), NULL);
		list_add(g_running_procs, cliente_suse);
		log_info( g_logger, "Operacion suse_create Ok, hilo_principal %d en ejecucion para el socket %d", cliente_suse->running_thread->tid, socket_cliente );
	}
	else if( g_multiprog_max > 0 ) {
		// Cuando llega el create de un thread adicional
		// El proceso padre ya tiene su running thread
		// Valido si el grado de multiprogramación permite ejecutar al nuevo thread,
		nuevo_thread->estado = READY;
		nuevo_thread->time_last_run = 0 ;		// 0 para simbolizar que nunca corrio
		nuevo_thread->time_last_yield = 0 ;
		list_add( cliente_suse->ready, nuevo_thread );
		log_info( g_logger, "Operacion suse_create Ok, hilo_adicional %d en lista Ready del socket %d", nuevo_thread->tid, socket_cliente );
		g_multiprog_max--;
	}
	else {
		nuevo_thread->estado = NEW;
		nuevo_thread->time_last_run = 0;		// 0 para simbolizar que nunca corrio
		nuevo_thread->time_last_yield = 0;
		queue_push( g_new_threads, nuevo_thread ); // en la cola Global New Threads
		log_info( g_logger, "Operacion suse_create Ok, hilo_adicional %d en cola New para el socket %d", nuevo_thread->tid, socket_cliente );
	}
	return NULL;
}

t_paquete* procesarThreadClose(t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente){
	log_info( g_logger, "Recibi un close");

	t_paquete* respuesta = NULL;

	if( cliente_suse->main_tid < 0){
		log_warning( g_logger, "Proceso %d inexistente en socket %d", cliente_suse->main_tid, socket_cliente );
		respuesta = armarPaqueteNumeroConOperacion( -ECHILD, SUSE_CLOSE );
	}
	else if ( cliente_suse->running_thread == NULL){
		log_warning( g_logger, "Proceso %d sin hilos en ejecucion en socket %d", cliente_suse->main_tid, socket_cliente );
		respuesta = armarPaqueteNumeroConOperacion( -EINVAL, SUSE_CLOSE );
	}
	else{
		t_client_thread_suse* thread_a_cerrar = cliente_suse->running_thread;  	// obtengo el thread en ejecucion
		list_add(g_exit_threads,thread_a_cerrar);								// se agregó el running thread la lista Global EXIT Threads.
		thread_a_cerrar->estado = EXIT;											// cambio el estado a de *thread_a_cerrar* a EXIT.
		thread_a_cerrar->time_last_yield = time(NULL);							// actualizamos "time_last_yield" porque deja de ejecutarse
		t_list* pasar_a_ready = thread_a_cerrar->threads_bloqueados;			// en el "proceso_padre" de thread_a_cerrar
																				// muevo elementos de la lista "blocked" a "ready";
		list_iterate(pasar_a_ready, trancisionar_bloqueado_a_ready);            // contenidos en la lista "thread_bloqueados" de *thread_a_cerrar*
		list_clean(pasar_a_ready);
		/*
		 * porque una vez que transicionan a ready los tenemos que sacar de la lista global bloqueados
		 * solo llamar a trancisionar_bloqueado_a_ready con cada elemento de la lista los saca de la lista de bloqueados
		 * del proceso pero quedan dentro de la lista de bloqueados por el thread
		 */
		if( thread_a_cerrar->proceso_padre->main_tid == thread_a_cerrar->tid ){
			log_info( g_logger, "Cliente de socket %d termino ejecucion", socket_cliente );
		} else if( queue_is_empty( g_new_threads ) ){
			// al hacer close de un thread aumento en "1" el grado de multiprogramación
			g_multiprog_max++;
			log_info( g_logger, "Capacidad de multiprog %d de %d", g_multiprog_max, g_config_server->max_multiprog );
		} else {
			// si hay threads esperando en new, saco uno
			t_client_thread_suse* thread_to_ready = (t_client_thread_suse*) queue_pop(g_new_threads);
			// habilito al thread a estar entre los próximos a ejecutar
			// lo agregamos a la lista ready del proceso que realizó el request
			thread_to_ready->estado = READY;
			list_add(thread_to_ready->proceso_padre->ready, thread_to_ready);
			log_info( g_logger, "Nuevo hilo %d en la lista Ready del socket %d", thread_to_ready->tid, socket_cliente );
		}
		log_info( g_logger, "Operacion suse_close Ok para hilo en ejecucion %d del socket %d", cliente_suse->running_thread->tid , socket_cliente );
		respuesta = armarPaqueteNumeroConOperacion( 0, SUSE_CLOSE );
	}
	return respuesta;
}

t_paquete* procesarThreadJoin(t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente){

	log_info( g_logger, "Recibi un join");
	int tid_to_join = deserializarNumero( paquete->buffer );
	t_paquete* respuesta = NULL;

	if( cliente_suse->main_tid < 0){
		log_warning( g_logger, "Proceso %d inexistente en socket %d", cliente_suse->main_tid, socket_cliente);
		respuesta = armarPaqueteNumeroConOperacion( -ECHILD, SUSE_JOIN );
	}
	if ( cliente_suse->running_thread == NULL){
		log_warning( g_logger, "Proceso %d sin hilos en ejecucionen socket %d", cliente_suse->main_tid, socket_cliente );
		respuesta = armarPaqueteNumeroConOperacion( -EINVAL, SUSE_JOIN );
	}
	else{
		t_client_thread_suse* thread_buscado_en_exit;
		bool compare_thread_in_thread_lists( void* thread ){
			t_client_thread_suse* thread_t = (t_client_thread_suse*) thread;
			bool condition_1 = tid_to_join == thread_t->tid;
			bool condition_2 = cliente_suse == thread_t->proceso_padre;
			return condition_1 && condition_2;
		}
		thread_buscado_en_exit = list_find( g_exit_threads, compare_thread_in_thread_lists );
		// busco en la Lista Global de EXIT threads el TID enviado por el request SUSE_JOIN (diferente al TID del running_thread).

		if (thread_buscado_en_exit != NULL) {
		//encontré al thread requested to join la Lista Global de EXIT threads
			log_info( g_logger, "Operacion suse_join Ok, Hilo %d de socket %d ya estaba en EXIT", tid_to_join, socket_cliente );
			respuesta = armarPaqueteNumeroConOperacion( 0, SUSE_JOIN );
		}
		else {
			cliente_suse->running_thread->estado = BLOCKED;													// le seteamos el estado BLOCKED al thread
			cliente_suse->running_thread->time_last_yield = time(NULL);										// actualizamos "time_last_yield" porque va a dejar de ejecutarse
			void* runningThread = cliente_suse->running_thread;
			list_add(g_blocked_threads,runningThread);														// agregamos al thread en ejecución a la lista Global Blocked Threeads
			t_client_thread_suse* thread_to_join = find_thread_by_tid_in_parent(tid_to_join, cliente_suse); // busco el thread que fue requerido a hacer Join
			list_add(thread_to_join->threads_bloqueados,runningThread); 									// agregamos a la lista "thread_bloqueados" del thread solicitado a hacer Join el running_thread al thread
			log_info( g_logger, "Operacion suse_join Ok, hilo %d en ejecucion a Blocked esperando %d para el socket %d", cliente_suse->running_thread->tid, tid_to_join, socket_cliente );

			respuesta = armarPaqueteNumeroConOperacion( 0, SUSE_JOIN );
		}

	}
	return respuesta;
}

t_paquete* procesarThreadScheduleNext(t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente){

	log_info( g_logger, "Recibi un schedule_next");
	t_paquete* respuesta = NULL;

	if( cliente_suse->main_tid < 0){
		log_warning( g_logger, "Proceso %s inexistente", cliente_suse->main_tid );
		respuesta = armarPaqueteNumeroConOperacion( -ECHILD, SUSE_SCHEDULE_NEXT);
	}
	if ( cliente_suse->running_thread == NULL){
		log_warning( g_logger, "Proceso %s sin hilos en ejecucion", cliente_suse->main_tid );
		respuesta = armarPaqueteNumeroConOperacion( -EINVAL, SUSE_SCHEDULE_NEXT );
	}
	else {
		t_client_thread_suse* next_running_thread;
		t_client_thread_suse* prev_running_thread = cliente_suse->running_thread;

		bool comparo_threads_por_indice_sjf( void* thread, void* otro_thread ){
			t_client_thread_suse* thread_t = (t_client_thread_suse*) thread;
			t_client_thread_suse* otro_thread_t = (t_client_thread_suse*) otro_thread;
			return indice_sjf(thread_t) <= indice_sjf(otro_thread_t);
		}

		if( prev_running_thread->estado == RUNNING ){
			list_add(cliente_suse->ready, prev_running_thread);			// agregamos el running_thread actual a la lista de Ready, se dejará de ejecutar,
			prev_running_thread->time_last_yield = time(NULL);				// actualizamos time_last_yield del thread que se dejará de ejecutar,
			prev_running_thread->estado = READY;
		}

		list_sort(cliente_suse->ready,comparo_threads_por_indice_sjf);	// ordeno la lista Ready del proceso que envió el request, los threads por indice_sjf, el de menor indice en 1° lugar
		if( list_is_empty(cliente_suse->ready) ){
			cliente_suse->running_thread = NULL;
			log_warning(g_logger, "Operacion suse_schedule_next sin hilos para ejecutar en socket %d", socket_cliente );
			return NULL;
		}
		next_running_thread = list_get(cliente_suse->ready, 0);			// obtengo el 1° thread de la lista Ready,
		list_remove(cliente_suse->ready, 0);							// quitamos el 1° thread de la lista Ready,

		cliente_suse->running_thread = next_running_thread;				// ponemos a ejecutar el thread seleccionado de la lista Ready,
		cliente_suse->running_thread->estado = RUNNING;
		cliente_suse->running_thread->time_last_run = time(NULL);				// actualizamos time_last_run del thread que entrará en ejecucion,

		log_info( g_logger, "Operacion suse_schedule_next Ok para socket %d, hilo %d proximo a ejecutar", socket_cliente , cliente_suse->running_thread->tid );
		respuesta = armarPaqueteNumeroConOperacion( cliente_suse->running_thread->tid , SUSE_SCHEDULE_NEXT ); // en la respuesta al request envío el TID del nuevo running thread,
	}
	return respuesta;
}

t_client_thread_suse* find_thread_by_tid_in_parent( int tid, t_client_suse* proceso_padre ){

	bool compare_thread_id( void* thread ){
		t_client_thread_suse* thread_t = (t_client_thread_suse*) thread;
		bool condition_1 = tid == thread_t->tid;
		bool condition_2 = proceso_padre == thread_t->proceso_padre;
		return condition_1 && condition_2;
	}
	// Lo busco en varios lados porque puede estar en cualquier estado
	if( proceso_padre->running_thread->tid == tid)
		return proceso_padre->running_thread;

	t_client_thread_suse* thread_buscado;

	thread_buscado = list_find( proceso_padre->ready, compare_thread_id );
	if( thread_buscado != NULL )
		return thread_buscado;

	thread_buscado = list_find( g_exit_threads, compare_thread_id );
	if( thread_buscado != NULL )
		return thread_buscado;

	thread_buscado = list_find( g_blocked_threads, compare_thread_id );
	if( thread_buscado != NULL )
		return thread_buscado;

	return list_find( g_new_threads->elements, compare_thread_id );
}

t_paquete* procesarSemSignal(t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente){
	t_semaforo_request_suse* sem_req_info = deserializarSemaforoRequest( paquete->buffer );
	log_info( g_logger, "Sem signal de %s, para tid %d", sem_req_info->name, sem_req_info->tid );

	t_paquete * respuesta = NULL;

	t_semaforo_suse* semaforo = find_sem_by_name( sem_req_info->name );

	if( semaforo == NULL ){
		log_warning( g_logger, "Sem %s no existe", sem_req_info->name );
		respuesta = armarPaqueteNumeroConOperacion( -EINVAL, SUSE_SIGNAL );
	} else if( semaforo->current_value == semaforo->max_value ) {
		log_warning( g_logger, "Sem %s ya esta en su valor maximo %d", sem_req_info->name, semaforo->max_value );
		respuesta = armarPaqueteNumeroConOperacion( -EOVERFLOW, SUSE_SIGNAL );
	} else {
		pthread_mutex_lock( &( semaforo->sem_mutex ) );
		if( queue_is_empty( semaforo->threads_bloquedos ) ) {
			semaforo->current_value++;
			log_info( g_logger, "Sem %s nuevo valor %d", sem_req_info->name, semaforo->current_value );
		}
		else {
			t_client_thread_suse* thread_desbloqueado = queue_pop( semaforo->threads_bloquedos );
			// quitamos el primer hilo de la cola Blocked Threads del semaforo
			// cambiamos el estado del hilo desbloqueado
			if( thread_desbloqueado->proceso_padre->running_thread == NULL ){	// pudiera ser que el proceso no tenga hilos en ejecución
				pthread_mutex_lock( &( thread_desbloqueado->proceso_padre->being_externally_scheduled ) );
				if( thread_desbloqueado->proceso_padre->running_thread == NULL ){
					quitar_thread_de_bloqueados( thread_desbloqueado );				// lo quitamos de la lista Global Blocked Thereads.
					thread_desbloqueado->proceso_padre->running_thread = thread_desbloqueado;
					thread_desbloqueado->estado = RUNNING;
					// lo pasamos a Running del proceso padre del hilo desbloqueado
					snd_schd_nxt_blockd_process( thread_desbloqueado );
					// El proceso padre del hilo desbloqueado está esperando una respuesta de Schedule_Next, en ese momento no había más hilos para ejecutar
					log_info( g_logger, "Operacion suse_schedule_next Ok para socket %d, hilo %d proximo a ejecutar", thread_desbloqueado->proceso_padre->self_socket , thread_desbloqueado->tid );
				} else {
					trancisionar_bloqueado_a_ready( ( void*) thread_desbloqueado );
					thread_desbloqueado->estado = READY;
					// El proceso padre del thread desbloquedo tenía un hilo en ejecucion, se agregó el hilo desbloquedado a su lista Ready Threads
					log_info( g_logger, "Sem signal en %s desbloqueo tid %d", sem_req_info->name, thread_desbloqueado->tid );
				}
				pthread_mutex_unlock( &( thread_desbloqueado->proceso_padre->being_externally_scheduled ) );
			} else {
				trancisionar_bloqueado_a_ready( ( void*) thread_desbloqueado );
				thread_desbloqueado->estado = READY;
				// El proceso padre del thread desbloquedo tenía un hilo en ejecucion, se agregó el hilo desbloquedado a su lista Ready Threads
				log_info( g_logger, "Sem signal en %s desbloqueo tid %d", sem_req_info->name, thread_desbloqueado->tid );
			}
		}
		respuesta = armarPaqueteNumeroConOperacion( 0, SUSE_SIGNAL );
		pthread_mutex_unlock( &( semaforo->sem_mutex ) );
	}
	return respuesta;
}

t_paquete* procesarSemWait(t_paquete* paquete, t_client_suse* cliente_suse, int socket_cliente){
	t_semaforo_request_suse* sem_req_info = deserializarSemaforoRequest( paquete->buffer );
	log_info( g_logger, "Sem wait de %s, para tid %d", sem_req_info->name, sem_req_info->tid );

	t_paquete * respuesta = NULL;

	t_semaforo_suse* semaforo = find_sem_by_name( sem_req_info->name );
	t_client_thread_suse* running_thread = cliente_suse->running_thread; // Porque si hizo un wait tiene que estar en running

	if( semaforo == NULL ){
		log_warning( g_logger, "Sem %s no existe", sem_req_info->name );
		respuesta = armarPaqueteNumeroConOperacion( -EINVAL, SUSE_WAIT );
	} else {
		pthread_mutex_lock( &( semaforo->sem_mutex ) );
		if( semaforo->current_value == 0 ) {
			log_info( g_logger, "Sem %s esta en 0, se bloquea el tid %d", sem_req_info->name, sem_req_info->tid );
			running_thread->estado = BLOCKED;
			queue_push( semaforo->threads_bloquedos, running_thread );
			list_add( g_blocked_threads, running_thread );
			// agrego al thread en ejecución a la lista Global Blocked Threads,
			respuesta = armarPaqueteNumeroConOperacion( 0, SUSE_WAIT );
		} else { // aca asumo que el semaforo puede ser restado porque no es 0
			semaforo->current_value--;
			log_info( g_logger, "Sem %s nuevo valor %d", sem_req_info->name, semaforo->current_value );
			respuesta = armarPaqueteNumeroConOperacion( 0, SUSE_WAIT );
		}
		pthread_mutex_unlock( &( semaforo->sem_mutex ) );
	}
	return respuesta;
}

t_paquete* armarPaqueteNumeroConOperacion( int numero, int codigo_op ){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigoOperacion = codigo_op;
	serializarNumero(paquete, numero);
	return paquete;
}

void quitar_thread_de_bloqueados( void* thread ){
	t_client_thread_suse* thread_t = (t_client_thread_suse*) thread;
	bool compare_thread( void* otro_thread ){
		t_client_thread_suse* otro_thread_t = (t_client_thread_suse*) otro_thread;
		bool condition_1 = otro_thread_t->tid == thread_t->tid;
		bool condition_2 = otro_thread_t->proceso_padre == thread_t->proceso_padre;
		return condition_1 && condition_2;
	}
	list_remove_by_condition( g_blocked_threads, compare_thread);
	// Quitamos al thread de la lista Global Blocked Threads,
}

void trancisionar_bloqueado_a_ready( void* thread ){
	quitar_thread_de_bloqueados(thread);
	t_client_thread_suse* thread_t = (t_client_thread_suse*) thread;
	list_add( thread_t->proceso_padre->ready, thread );
	// Agregamos al thread en la lista Ready Threads de su Proceso Padre,
}


void trancisionar_ready_a_bloqueado( void* thread ){
	t_client_thread_suse* thread_t = (t_client_thread_suse*) thread;
	bool compare_thread( void* otro_thread ){
		t_client_thread_suse* otro_thread_t = (t_client_thread_suse*) otro_thread;
		return otro_thread_t->tid == thread_t->tid;
	}
	list_remove_by_condition( thread_t->proceso_padre->ready, compare_thread);
	list_add( g_blocked_threads, thread );
	// pasamos el thread a la lista Global Blocked Threads
}

t_semaforo_suse* find_sem_by_name( char* sem_name ){
	bool compare_sem_name( void* sem ){
		t_semaforo_suse* sem_t = (t_semaforo_suse*) sem;
		return strcmp( sem_name, sem_t->nombre ) == 0;
	}
	return list_find( g_semaforos, compare_sem_name );
}

void esperarPaqueteCreateMain( t_client_suse* cliente_suse, int socket_cliente ){
	t_paquete *paqueteCreate = recibirArmarPaquete( socket_cliente );
	if( paqueteCreate->codigoOperacion == SUSE_CREATE ){
		procesarThreadCreate( paqueteCreate, cliente_suse, 1, socket_cliente);
	} else {
		log_error( g_logger, "Recibi algo que no es el grado del multiprog");
	}
	destruirPaquete( paqueteCreate );
}

void enviarMultiProg( int socket_dst ){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigoOperacion = SUSE_GRADO_MULTIPROG;

	log_debug(g_logger, "Envio este grado de multiprogramacion %d", g_multiprog_max);
	serializarNumero(unPaquete, g_multiprog_max);
	enviarPaquetes(socket_dst, unPaquete);
}

int cant_hilos_proceso_padre( t_list* lista_threads, t_client_suse* proceso_padre ) {
	int socket_proceso_padre = proceso_padre->self_socket;
	bool compare_thread( void* otro_thread ){
		t_client_thread_suse* otro_thread_t = (t_client_thread_suse*) otro_thread;
		return otro_thread_t->proceso_padre->self_socket == socket_proceso_padre;
	}
	t_list* hilos_del_proceso_padre = list_filter( lista_threads, compare_thread);
	return list_size(hilos_del_proceso_padre);
}

void get_runnning_procs_stats( void ) {
	int qty = list_size(g_running_procs);

	if (qty == 0){
		log_info(g_metrics,"SUSE esperando clientes, sin procesos en ejecución");
		return;
	}
	for (int i = 0; i < qty; i++) {
		void* proceso = list_get(g_running_procs, i);
		t_client_suse* proceso_t = (t_client_suse*) proceso;
		int blockd_qty  = cant_hilos_proceso_padre(g_blocked_threads, proceso_t);
		int exit_qty  = cant_hilos_proceso_padre(g_exit_threads, proceso_t);
		int new_qty  = cant_hilos_proceso_padre(g_new_threads->elements, proceso_t);

		if (!list_is_empty(proceso_t->ready)) {
			int ready_qty = list_size(proceso_t->ready);
			log_info(g_metrics, "Process_Socket=%d -> Cantidad de Hilos: RUNNING= %d; READY=%d; NEW= %d, BLOCKED= %d; EXIT= %d.", proceso_t->self_socket, 1, ready_qty, new_qty, blockd_qty, exit_qty);
			int grado_multiprog_proceso = ready_qty + new_qty + blockd_qty + exit_qty + 1;
			log_info(g_metrics, "Process_Socket=  %d - Grado de multiprogramacion: %d \n", proceso_t-> self_socket, grado_multiprog_proceso);
		}

		else if (proceso_t->running_thread != NULL) {
			log_info(g_metrics, "Process_Socket=%d --> Cantidad de Hilos: RUNNING= %d; READY=%d; NEW= %d, BLOCKED= %d; EXIT= %d.", proceso_t->self_socket, 1, 0, new_qty, blockd_qty, exit_qty);
			int grado_multiprog_proceso = new_qty + blockd_qty + exit_qty + 1;
			log_info(g_metrics, "Process_Socket=  %d - Grado de multiprogramacion: %d \n", proceso_t-> self_socket, grado_multiprog_proceso);
		}
		else {
		log_info(g_metrics, "Process_Socket=%d ->> Cantidad de Hilos: RUNNING= %d; READY=%d; NEW= %d, BLOCKED= %d; EXIT= %d.", proceso_t->self_socket, 0, 0, new_qty, blockd_qty, exit_qty);
		int grado_multiprog_proceso = new_qty + blockd_qty + exit_qty;
		log_info(g_metrics, "Process_Socket=  %d - Grado de multiprogramacion: %d \n", proceso_t-> self_socket, grado_multiprog_proceso);
		}

	}
}

void iniciar_config(char* path){
	g_config = config_create(path);
	g_config_server = malloc( sizeof( t_config_suse ) );

	g_config_server->puerto = config_get_string_value( g_config, "LISTEN_PORT" );
	g_config_server->metrics_timer = config_get_int_value( g_config, "METRICS_TIMER" );
	g_config_server->max_multiprog = config_get_int_value( g_config, "MAX_MULTIPROG" );

	char ** array = config_get_array_value( g_config, "SEM_IDS" );
	g_config_server->sem_ids = list_create();
	for( int i = 0; array[ i ] != NULL; i++ ){
		list_add( g_config_server->sem_ids, array[ i ] );
	}
	free( array );

	array = config_get_array_value( g_config, "SEM_INIT" );
	g_config_server->sem_init = list_create();
	for( int i = 0; array[ i ] != NULL; i++ ){
		list_add( g_config_server->sem_init, array[ i ] );
	}
	free( array );

	array = config_get_array_value( g_config, "SEM_MAX" );
	g_config_server->sem_max = list_create();
	for( int i = 0; array[ i ] != NULL; i++ ){
		list_add( g_config_server->sem_max, array[ i ] );
	}
	free( array );
}

void inicializar_semaforos(){
	t_semaforo_suse* nuevo_semaforo;
	for( int i = 0; i < list_size( g_config_server->sem_ids ); i++ ){
		nuevo_semaforo = malloc( sizeof( t_semaforo_suse ) );
		nuevo_semaforo->nombre = strdup( list_get( g_config_server->sem_ids, i ) );
		nuevo_semaforo->current_value = atoi( list_get( g_config_server->sem_init, i ) );
		nuevo_semaforo->max_value = atoi( list_get( g_config_server->sem_max, i ) );
		nuevo_semaforo->threads_bloquedos = queue_create();
		pthread_mutex_init(&( nuevo_semaforo->sem_mutex ), NULL);

		list_add( g_semaforos, nuevo_semaforo );
	}
}

void inicializar_estructuras(){
	g_semaforos 		= list_create();
	g_blocked_threads 	= list_create();
	g_exit_threads		= list_create();
	g_running_procs		= list_create();
	g_new_threads		= queue_create();
	g_multiprog_max 	= g_config_server->max_multiprog;
}

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/workspace/tp-2019-2c-No-C-Nada/SUSE/logFiles/suseServer.log", "SUSE-Server", 1, LOG_LEVEL_TRACE);
	log_info(g_logger, "Iniciando SuseServer");
}



void iniciar_g_metrics(void) {
	g_metrics = log_create("/home/utnso/workspace/tp-2019-2c-No-C-Nada/SUSE/logFiles/suseMetric.log", "Metricas - SUSE", 0, LOG_LEVEL_TRACE);
	log_info(g_metrics, "Iniciando Métricas de SuseServer:");
}

void get_global_statistics( void ) {
	get_runnning_procs_stats();
}

void actualizar_metricas( void ) {
	int intervalo = g_config_server->metrics_timer ;
	log_info(g_logger, "Intervalo para la toma de métricas: %d segundos. \n", intervalo);
	while (1) {
		sleep(intervalo);
		get_global_statistics();
    }
}

void update_metrics_continue( void ) {

	pthread_mutex_init( &g_mutex_scheduler_metrics, NULL );

    int thread_status = pthread_create( &g_thread_scheduler_metrics, NULL, (void*) actualizar_metricas, NULL );

    if ( thread_status != 0 ) {
        log_error(g_logger, "Error al crear el thread para actualizar metricas");
        exit(EXIT_FAILURE);
    }
    else {
    	pthread_detach( g_thread_scheduler_metrics );
    }

}

double indice_sjf(t_client_thread_suse* thread_to_compare) {
	long ready_time = thread_to_compare->time_last_run - thread_to_compare->time_created;
	long exec_time 	= thread_to_compare->time_last_yield - thread_to_compare->time_last_run;

	double alfa = g_config_server->alpha_sjf;
	double comp_alfa = 1 - g_config_server->alpha_sjf;

	double valor_calculado = alfa * ready_time + comp_alfa * exec_time;
	return valor_calculado;
}

void snd_schd_nxt_blockd_process( t_client_thread_suse* thread_t ){
	t_client_suse* procesoPadre = thread_t->proceso_padre;
	int socket_cliente = procesoPadre->self_socket;
	t_paquete* rta_schd_nxt = armarPaqueteNumeroConOperacion(thread_t->tid, SUSE_SCHEDULE_NEXT);
	enviarPaquetes( socket_cliente, rta_schd_nxt );
}
