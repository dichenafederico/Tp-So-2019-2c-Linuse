#ifndef SRC_PROCESAMIENTOPAQUETES_H_
#define SRC_PROCESAMIENTOPAQUETES_H_

#include "estructurasAdministrativas.h"
#include "serializaciones.h"

/*-----------------------------------Paquetes-----------------------------------*/
int prueba(void);
void	 						enviarPaquetes				(int socketfd, t_paquete * unPaquete);
t_paquete *                     recibirArmarPaquete         (int socketCliente);
int 							recibirTamPaquete			(int client_socket);
t_paquete * 					recibirPaquete				(int client_socket, int tamPaquete);
t_paquete * 					crearPaquete				(void * buffer);
t_paquete *						crearPaqueteError			(int client_socket);
void 							destruirPaquete				(t_paquete * unPaquete);
void 							mostrarPaquete				(t_paquete * unPaquete);

/*-----------------------------------Enviar paquetes-----------------------------------*/
void 							enviarHandshake				(int server_socket, int emisor);
void 							enviarMensaje				(int server_socket, char * mensaje);
void							enviarMensajeConCodigo		(int server_socket, char * mensaje, int codigoRespuesta);
void 							enviarArchivo				(int server_socket, char * rutaArchivo);
void 							enviarSolicitudEjecucion	(int server_socket);
void 							enviarEjecucionTerminada	(int server_socket);
void 							enviarNombreMemoria			(int server_socket, char * nombre);
void 							enviarRespuesta				(int server_socket, int codRespuesta);
void 							enviarCompactacion			(int server_socket);
void 							enviarRespSolicitudValor	(int server_socket, bool claveExistente, char * valor);
void							enviarAvisoDesconexion		(int server_socket);
void 							enviarSolicitudTamanioValor	(int server_socket);
void						    enviarSolicitudClave		(int server_socket, bool enviarQuery,char* nombreTabla, uint16_t clave);
void						    enviarInsert				(int server_socket, bool enviarQuery,char* nombreTabla, uint16_t clave, char* value, uint64_t timestamp);
void 							enviarSolicitudTamanioValor	(int server_socket);
void                            enviarRequestGossiping      (int server_socket, int numeroMemoria, t_list* listaMemorias);
void 							enviarDrop					(int server_socket, char * nombreTabla);
void 							enviarDescribe				(int server_socket, char * nombreTabla);
void 							enviarCreate				(int server_socket, bool enviarQuery, char * nombreTabla, char * tipoConsistencia, int numeroParticiones, int tiempoCompactacion);
void 							enviarPool					(int server_socket, int numeroMemoria, t_list* poolMemorias) ;


/*-----------------------------------Recibir paquetes-----------------------------------*/
int								recibirHandshake			(t_paquete * unPaquete);
char * 					  		recibirMensaje				(t_paquete * unPaquete);
void * 					  		recibirArchivo				(t_paquete * unPaquete);
char * 					  		recibirNombreMemoria		(t_paquete * unPaquete);
int 							recibirRespuesta			(t_paquete* unPaquete);
char *							recibirSolicitudValor		(t_paquete * unPaquete);
t_respuestaValor *				recibirRespSolicitudValor	(t_paquete * unPaquete);
char *							recibirClaveEliminada		(t_paquete * unPaquete);

#endif /* SRC_PROCESAMIENTOPAQUETES_H_ */
