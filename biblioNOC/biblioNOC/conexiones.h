#ifndef BIBLIONOC_SOCKETS_H_
#define BIBLIONOC_SOCKETS_H_

#include <pthread.h>
#include "estructurasAdministrativas.h"
#include <commons/log.h>
#include "paquetes.h"
#include <semaphore.h>
#include <sys/time.h>

/*------------------------------Clientes------------------------------*/

int 				conectarCliente					(const char * ip, int puerto, int cliente);
void 				gestionarSolicitudes			(int server_socket, void (*procesarPaquete)(void*, int*), t_log * logger);


/*------------------------------Servidor------------------------------*/

void	 			iniciarServer					(int puerto, void (*procesarPaquete)(void*, int*), t_log * logger);
int                 iniciarServidor                 (char* puerto, t_log* g_loggerDebug, void (*attendConnection)(int*));
int 				crearSocketServer				(char * puerto);
void 				gestionarNuevasConexiones		(int server_socket, fd_set * set_master, int * descriptor_mas_alto, t_log * logger);
void 				gestionarDatosCliente			(int client_socket, fd_set * set_master, void (*procesarPaquete)(void*, int*), t_log * logger);
t_conexion*		    buscarConexion					(t_list * diccionario, char * clave, int socket, sem_t semaforo);
void 				destruirDiccionario				(t_list* diccionario,sem_t semaforo);
void 				cerrarConexion					(void* conexion);
void			    destruirDiccionario				(t_list* diccionario,sem_t semaforo);
void 				sacarConexion					(t_list* diccionario, t_conexion* conexion);

#endif

