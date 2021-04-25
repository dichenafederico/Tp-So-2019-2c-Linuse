#ifndef MUSEutil_h
#define MUSEutil_h

#include "estructuras.h"
#include <biblioNOC/estructurasAdministrativas.h>
#include <biblioNOC/paquetes.h>



//si queres podemos cambiar parametros por estructuras que los contengan
void serializarUINT32(t_paquete* unPaquete, uint32_t numero);
int deserializarUINT32(t_stream* buffer);

void serializarRespuestaGet( t_paquete* unPaquete, int operacionSatisfactoria, size_t size, void* buffer );
t_registromget* deserializarGet(t_stream * buffer);

void serialzarCopy(t_paquete* unPaquete, void* src,uint32_t dst, int n);
t_registromcopy* deserializarCopy(t_stream * buffer);

void serializarMap(t_paquete * unPaquete, char * path, size_t length, int flags);
t_registromap* deserealizarMap(t_stream * buffer);

void serializarMsync(t_paquete * unPaquete,uint32_t addr, size_t len);
t_registrosync* deserealizarMsync(t_stream * buffer);

void serializarUnmap(t_paquete * paquete, uint32_t dir);
t_registrounmap* deserealizarUnmap(t_stream * buffer);

void enviarRespuestaAlloc(int server_socket, uint32_t tamanio);

void enviarRespuestaGet(int server_socket, int operacionSatisfactoria, size_t size, void* buffer);

void enviarRespuestaCopy(int server_socket, int operacionSatisfactoria);

void enviarRespuestaMap(int server_socket, uint32_t posicion);

void enviarRespuestaMsync(int server_socket, int operacionSatisfactoria);

void enviarRespuestaUnmap(int server_socket,int operacionSatisfactoria);

void destruirRequestCopy( t_registromcopy* registroCopy );

#endif
