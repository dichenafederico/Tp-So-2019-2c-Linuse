#include "libMUSE.h"
#include <stdio.h>
#include <stdlib.h>
#include <biblioNOC/paquetes.h>

int main(void) {
	return prueba();
}

//TODO: para los casos en que las api retornen -1, simular llamada a muse_close y lanzar syscall SIG_SET(SEGFAULT)

int muse_init(int id, char* ip, int puerto){

	pid = id;

	printf("Entro a la funcon init de LibMuse");

	socketConexion = conectarCliente(ip,puerto,LIBMUSE);

	enviarMuseInit(socketConexion);

	return socketConexion;

}

void muse_close(){

	enviarMuseClose(socketConexion);
	close(socketConexion);
}

uint32_t muse_alloc(uint32_t tam){

	enviarAlloc(socketConexion, tam);

	t_paquete * paquete  = recibirArmarPaquete(socketConexion);

	uint32_t direccionLogica = deserializarUINT32(paquete->buffer);

	destruirPaquete( paquete );

	return direccionLogica;

}

void muse_free(uint32_t dir){

	enviarFree(socketConexion, dir);
}

int muse_get(void* dst, uint32_t src, size_t n){

	enviarGet( socketConexion, src, n );

	t_paquete * paquete  = recibirArmarPaquete(socketConexion);

	t_get_response* read_resultado = deserealizarMuseGet( paquete->buffer );

	memcpy( dst, read_resultado->data, n );

	int errno_value =  read_resultado->errno_value;

	destruirGetResponse( read_resultado );
	destruirPaquete( paquete );

	VerificoOperacion( errno_value );

	return errno_value;
}

int muse_cpy(uint32_t dst, void* src, int n){

	enviarCopy(socketConexion,src, dst,n);

	t_paquete * paquete  = recibirArmarPaquete(socketConexion);

	int operacionSatisfactoria = deserializarNumero(paquete->buffer);

	destruirPaquete( paquete );

	VerificoOperacion(operacionSatisfactoria);

	return operacionSatisfactoria;
}

uint32_t muse_map(char *path, size_t length, int flags){

	enviarMap(socketConexion, path, length, flags);

	t_paquete * paquete  = recibirArmarPaquete(socketConexion);

	uint32_t pocision = deserializarUINT32(paquete->buffer);

	return pocision;
}

int muse_sync(uint32_t addr, size_t len){

	enviarMsync(socketConexion, addr, len);

	t_paquete * paquete  = recibirArmarPaquete(socketConexion);

	int operacionSatisfactoria = deserializarNumero(paquete->buffer);

	VerificoOperacion(operacionSatisfactoria);

	return operacionSatisfactoria;
}

int muse_unmap(uint32_t dir){

	enviarUnmap(socketConexion, dir);

	t_paquete * paquete  = recibirArmarPaquete(socketConexion);

	int operacionSatisfactoria = deserializarNumero(paquete->buffer);

	VerificoOperacion(operacionSatisfactoria);

	return operacionSatisfactoria;
}


int pruebaReferenciaMUSE(void) {
	puts("!!!prueba biblite si lo camibas de nuevo !!!"); /* prints !!!Hello World!!! */
	return 1;
}
