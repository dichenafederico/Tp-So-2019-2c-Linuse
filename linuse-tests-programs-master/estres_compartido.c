#include <stdio.h>
#include <stdlib.h>
#include "libmuse.h"
#include <hilolay/hilolay.h>
#include <string.h>
#include <unistd.h>

#define RUTA_ARCHIVO "una/ruta"
#define MAP_SHARED 1

struct hilolay_sem_t *presion_emitida;
struct hilolay_sem_t *presion_recibida;
struct hilolay_sem_t *revolucion_emitida;
struct hilolay_sem_t *revolucion_recibida;

void *grabar_archivo(uint32_t arch, char* palabra)
{
	uint32_t offset;
	hilolay_wait(presion_recibida);
	muse_get(&offset, arch, sizeof(uint32_t));
	muse_cpy(arch + offset, palabra, strlen(palabra) + 1);

	offset += strlen(palabra) + 1;
	muse_cpy(arch, &offset, sizeof(uint32_t));
	hilolay_signal(presion_emitida);

	sleep(1);
	return 0;
}

uint32_t leer_archivo(uint32_t arch, uint32_t leido)
{
	uint32_t offset;
	char * palabra = malloc(100);
	hilolay_wait(revolucion_emitida);
	muse_get(&offset, arch, sizeof(uint32_t));

	uint32_t len = offset - leido;
	muse_get(palabra, arch + leido, len);
	offset += strlen(palabra) + 1;

	muse_cpy(arch, &offset, sizeof(uint32_t));
	hilolay_signal(revolucion_recibida);

	puts(palabra);
	free(palabra);
	return offset;
}

void *presionar()
{
	uint32_t arch = muse_map(RUTA_ARCHIVO, 4096, MAP_SHARED);
	uint32_t offset = 0;
	offset = sizeof(uint32_t);
	muse_cpy(arch, &offset, sizeof(uint32_t));

	grabar_archivo(arch, "Estas atrapado con la espalda contra la pared\n");

	grabar_archivo(arch, "No hay solucion ni salida\n");

	grabar_archivo(arch, "Estas luchando y nadie puede ver\n");

	grabar_archivo(arch, "La presion crece exponencialmente\n");

	grabar_archivo(arch, "Intentas mantener el paso y no podes\n");

	grabar_archivo(arch, "No aprobaras. Muajaja\n");

	muse_sync(arch, 4096);

	printf("Inicia la revolucion!!!\n\n");

	offset = sizeof(uint32_t);
	for(int i = 0; i<4;i++)
		offset = leer_archivo(arch, offset);

	muse_unmap(arch);

	printf("\nAprobado\n");
	return 0;
}


int main(void)
{
	struct hilolay_t presion;

	hilolay_init();
	muse_init(getpid(), "127.0.0.1", 3306);

	presion_emitida = hilolay_sem_open("presion_emitida");
	presion_recibida = hilolay_sem_open("presion_recibida");
	revolucion_emitida = hilolay_sem_open("revolucion_emitida");
	revolucion_recibida = hilolay_sem_open("revolucion_recibida");

	hilolay_create(&presion, NULL, &presionar, NULL);

	hilolay_join(&presion);

	hilolay_sem_close(presion_emitida);
	hilolay_sem_close(presion_recibida);
	hilolay_sem_close(revolucion_emitida);
	hilolay_sem_close(revolucion_recibida);


return hilolay_return(0);
}
