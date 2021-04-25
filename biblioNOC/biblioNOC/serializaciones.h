#ifndef BIBLIONOC_SERIALIZACION_H_
#define BIBLIONOC_SERIALIZACION_H_

#include "estructurasAdministrativas.h"
#include <commons/string.h>
#include <commons/collections/list.h>

/*----------------------------------------Serializacion----------------------------------------*/
void	serializarNumero				(t_paquete * unPaquete, int numero);
void	serializarMensaje				(t_paquete * unPaquete, char * mensaje);
void	serializarHandshake				(t_paquete * unPaquete, int emisor);
void	serializarArchvivo				(t_paquete * unPaquete, char * rutaArchivo);
void 	serializarClave					(t_paquete * unPaquete, char * clave);
void 	serializarRegistro				(t_paquete * unPaquete, uint64_t timeStamp, unsigned short clave, char * valor);
void    serializarSelectQuery           (t_paquete * unPaquete, char* nombreTabla, uint16_t clave);
void    serializarSelect                (t_paquete * unPaquete, char* nombreTabla, uint16_t clave);
void    serializarCreateQuery           (t_paquete * unPaquete, char * nombreTabla, char * tipoConsistencia, int numeroParticiones, int tiempoCompactacion);
void    serializarCreate                (t_paquete * unPaquete, char* nombreTabla, char* tipoConsistencia, int numeroParticiones, int tiempoCompactacion);
void    serializarInsertQuery           (t_paquete * unPaquete, char* nombreTabla, uint16_t clave, char* value, uint64_t timestamp);
void    serializarInsert                (t_paquete * unPaquete, char* nombreTabla, uint16_t clave, char * value);
void 	serializarConexionMemoria		(t_paquete * unPaquete, int numeroMemoria, t_list * listaMemorias);
void	serializarRespuestaStatus		(t_paquete * unPaquete, char * valor, char * nomInstanciaActual, char * nomIntanciaPosible);
void	serializarExistenciaRegistro	(t_paquete * unPaquete, bool claveExistente, void * valor);

/*----------------------------------------Deserializacion----------------------------------------*/
int 				recibirRespuestaLql					(t_paquete* unPaquete);
int 				deserializarNumero					(t_stream * buffer);
int                 deserializarTamanioCreate           (t_stream * buffer);
t_CREATE *          deserializarCreate                  (t_stream * buffer);
t_SELECT *          deserializarSelect                  (t_stream * buffer);
t_INSERT *          deserializarInsert                  (t_stream * buffer);
char * 				deserializarMensaje					(t_stream * buffer);
int 				deserializarHandshake				(t_stream * buffer);
void * 				deserializarArchivo					(t_stream * buffer);
char * 				deserializarClave					(t_stream * buffer);
t_registro* 		deserializarRegistro				(t_stream * buffer);
t_registro*         deserializarRegistroString          (t_stream * buffer);
t_list*				deserializarConexionMemoria			(t_stream * buffer);
t_respuestaValor * 	deserializarExistenciaClaveValor	(t_stream * buffer);
bool 				deserializarBool					(t_stream* buffer);

/*----------------------------------------Funciones auxiliares----------------------------------------*/
void *	abrirArchivo	(char * rutaArchivo, size_t * tamArc, FILE ** archivo);

#endif /* BIBLIONOC_SERIALIZACION_H_ */
