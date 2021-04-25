/*
 * suseServer.h
 *
 *  Created on: 1 nov. 2019
 *      Author: utnso
 */

#ifndef SUSESERVER_H_
#define SUSESERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <commons/log.h>
#include <commons/config.h>
#include <i386-linux-gnu/bits/pthreadtypes.h>
#include <pthread.h>
#include "biblioNOC/conexiones.h"
#include "suseDefs.h"

#define LIBSUSE 10
/*-----------variables globales------ */
t_log* 				g_logger;
t_log* 				g_metrics;
t_config* 			g_config;
t_list* 			g_semaforos;
t_queue* 			g_new_threads;
t_list* 			g_blocked_threads;
t_list* 			g_exit_threads;
t_list*				g_running_procs;
int 				g_multiprog_max;
pthread_mutex_t		g_mutex_scheduler_metrics;
pthread_t			g_thread_scheduler_metrics;

/*--------------definiciones--------- */
void				atenderConexion					( int socketCliente );
void 				enviarMultiProg					( int socket_dst );
void 				iniciar_logger					( void );
void				iniciar_g_metrics				( void );
void				get_global_statistics			( void );
void				actualizar_metricas				( void );
void				update_metrics_continue			( void );
void 				inicializar_estructuras			( void );
void 				iniciar_config					( char* path );
void				inicializar_semaforos			( void );
void 				quitar_thread_de_bloqueados		( void* thread );

/*
 * @NAME: trancisionar_bloqueado_a_ready
 * @DESC: Transiciona el thread de bloqueado a ready, moviendolo de la cola de bloqueados a su cola de ready correspondiente
 */
void 				trancisionar_bloqueado_a_ready	( void* thread );
void 				trancisionar_ready_a_bloqueado	( void* thread );

#endif /* SUSESERVER_H_ */
