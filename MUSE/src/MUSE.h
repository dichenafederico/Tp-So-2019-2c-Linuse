#ifndef MUSE_H_
#define MUSE_H_

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <biblioNOC/serializaciones.h>
#include <biblioNOC/conexiones.h>
#include <biblioNOC/estructurasAdministrativas.h>
#include <biblioNOC/paquetes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <signal.h>
#include "estructuras.h"
#include "operaciones.h"
#include "util.h"

#define RUTACONFIG "/home/utnso/workspace/tp-2019-2c-No-C-Nada/configs/muse.cfg"
#define RUTASWAP "/home/utnso/workspace/tp-2019-2c-No-C-Nada/configs/swap.txt"
#define LIBMUSE 400
#define DIRECCIONAMIENTO 4294967296‬

//Variables globales
int idSegmento;
int nroPrograma;
int punteroClock;
size_t lengthPagina;

//Semafotos

sem_t g_mutexPaginas;

sem_t g_mutexSwap;
sem_t g_mutexPaginasEnSwap;
sem_t g_mutexgBitarray_swap;

sem_t g_mutextablasDePaginas;
sem_t g_mutexMapeosAbiertosCompartidos;
sem_t g_mutexgBitarray_marcos;
sem_t g_mutexgContenidoFrames;

sem_t g_mutexTablaProgramas;



//Estructuras Swap
void* g_archivo_swap;
int fdSwap;
FILE * disco_swap;
t_bitarray * g_bitarray_swap;
t_list * paginasEnSwap;
int maxPaginasEnSwap;

//Servidor
t_log* g_logger;
t_log* g_loggerDebug;
int g_cantidadFrames;
t_configuracion * g_configuracion;
t_config* g_config;
t_list* g_diccionarioConexiones; //de programas
pthread_t hiloServidor;
void* g_granMalloc;

//Estructuras memoria principal
t_bitarray * g_bitarray_marcos;
t_list * contenidoFrames;
t_list* tablasDePaginas; // Tabla global de paginas en memoria
t_list* mapeosAbiertosCompartidos;
t_list* programas;
int tamanio_heap;

/*------------------------Funciones-------------------------*/
int				arrancarServer					(char* puertostring);
void 			attendConnection				(int socketCliente);
void			reservarEspacioMemoriaPrincipal	();
void            destruirGlobales                ();
void 			armarConfigMemoria				();
t_paquete* 		procesarPaqueteLibMuse			(t_paquete* paquete, int cliente_fd);
void 			abrirArchivoGral				(FILE ** archivo);
void 			InicializarNuevoPrograma		(int socket);
void 			FinalizarPrograma				(int socket);
void* 			crearArchivoSwap				(char* path, int size);

#endif

