#ifndef util_h
#define util_h

#include <biblioNOC/estructurasAdministrativas.h>
#include <biblioNOC/paquetes.h>
#include <setjmp.h>
#include <signal.h>
#include <biblioNOC/conexiones.h>
#include "util.h"
#define LIBMUSE 400

	int socketConexion;
	int pid;

	typedef enum  {
		MUSE_INIT = 723,
		MUSE_ALLOC,
		MUSE_FREE,
		MUSE_GET,
		MUSE_COPY,
		MUSE_MAP,
		MUSE_SYNC,
		MUSE_UNMAP,
		MUSE_CLOSE
	}t_cod_operaciones_MUSE;

	typedef struct muse_get_response{
		int errno_value;
		size_t size;
		void* data;
	} t_get_response;

void VerificoOperacion(int operacion);

void serializarUINT32(t_paquete* unPaquete, uint32_t numero);
uint32_t deserializarUINT32(t_stream* buffer);

void serializarGet(t_paquete* unPaquete, uint32_t src, size_t n);
t_get_response* deserealizarMuseGet( t_stream* buffer );

void serialzarCopy(t_paquete* unPaquete, void* src,uint32_t dst, int n);
void serializarMap(t_paquete * unPaquete, char * path, size_t length, int flags);

void serializarMsync(t_paquete * unPaquete,uint32_t addr, size_t len);

void serializarUnmap(t_paquete * paquete, uint32_t dir);

void enviarMuseClose(int server_socket);
void enviarMuseInit(int server_socket);
void enviarAlloc(int server_socket, uint32_t tamanio);
void enviarFree(int server_socket, uint32_t direccionLogica);
void enviarGet(int server_socket, uint32_t src, size_t n);
void enviarCopy(int server_socket,void* src,  uint32_t dst, int n);
void enviarMap(int server_socket,char * path, size_t length, int flags);
void enviarMsync(int server_socket,uint32_t addr, size_t len);
void enviarUnmap(int server_socket, uint32_t dir);

void destruirGetResponse( t_get_response* read_resultado );

#endif
