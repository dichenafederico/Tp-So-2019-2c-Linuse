/*
 * biblioSuse.h
 *
 *  Created on: 31 oct. 2019
 *      Author: utnso
 */

#ifndef BIBLIOSUSE_H_
#define BIBLIOSUSE_H_

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <hilolay/hilolay.h>
#include <hilolay/alumnos.h>
#include <hilolay/internal.h>
#include <commons/config.h>
#include <commons/log.h>
#include <biblioNOC/conexiones.h>
#include "libSuseUtils.h"

#define BIBLIO_SUSE_CLIENT_ID 99


t_config_lib_suse *g_config;
t_log* g_logger;
t_config *g_config_commons;

int suse_create(int tid);
int suse_schedule_next(void);
int suse_join(int tid);
int suse_close(int tid);
int suse_wait(int tid, char *sem_name);
int suse_signal(int tid, char *sem_name);

struct hilolay_operations operaciones;

void hilolay_init(void);
void iniciar_log(void);

void enviarThreadCreate(int socket, int tid);

void enviarThreadJoin(int socket, int tid);
int  esperarRespuestaThreadJoin( int socket );

void enviarThreadClose(int socket, int tid);
int  esperarRespuestaThreadClose( int socket );

void enviarThreadScheduleNext( int socket );
int  esperarRespuestaThreadScheduleNext( int socket );

void esperarRespuestaConfig(int);

void enviarSemWait( int socket_dst, int tid, char* nombre );
int  esperarRespuestaSemWait( int socket );

void enviarSemPost( int socket_dst, int tid, char* nombre );
int  esperarRespuestaSemPost( int socket );

#endif /* BIBLIOSUSE_H_ */
