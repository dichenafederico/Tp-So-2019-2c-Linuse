#ifndef BIBLIONOC_ESTRUCTURAS_H_
#define BIBLIONOC_ESTRUCTURAS_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <commons/collections/list.h>

/*------------------------------Estructuras------------------------------*/
#define TAM_BLOQUE 1048576

typedef struct{
	char* nombre;
	int socket;
	pthread_t tid;
}t_conexion;

typedef struct {
	size_t size;
	void * data;
} t_stream;

typedef struct {
	int codigoOperacion;
	t_stream * buffer;
} t_paquete;

enum emisor {
	KERNEL = 900, MEMORIA = 901, FILESYSTEM = 902
};

typedef enum  {
	HANDSHAKE = 100,
	ENVIAR_MENSAJE,
	ENVIAR_ARCHIVO,
	ENVIAR_ERROR,
	ENVIAR_AVISO_DESCONEXION,
	SOLICITAR_TAMANIO_VALOR = 998,
	RESPUESTA_TAMANIO_VALOR = 111,
	RESPUESTA_DROP,
	RESPUESTA_DESCRIBE,
	RESPUESTA_CREATE,//KERNEL
	CONTINUA_LQL,
	ABORTO_LQL,
	SOLICITUD_EJECUCION,
	RESPUESTA_SOLICITUD,
	RESPUESTA_SOLICITUD_POOL,
	SOLICITUD_POOL,
	SOLICITUD_CLAVE,
	RESPUESTA_SOLICITUD_CLAVE,
	RESPUESTA_METADATA,
	CREATE_OK,
	CREATE_EXISTS,
}t_cod_operaciones;

typedef enum  {
	OK_GENERICO = 678,
	ERROR_GENERICO,
	UNDEFINED_GENERICO
}t_cod_respuesta_generico;

typedef enum  {
	SELECT,
	INSERT,
	CREATE,
	DESCRIBE,
	DROP,
	JOURNAL,
	GOSSIPING,
	COMPACTAR,
	REQUEST_GOSSIPING
}t_API_operaciones;

enum cod_respuesta {
	OK,
	ERROR,
};


typedef struct {
	char* nombreTabla;
	uint16_t key;
} t_SELECT;

typedef struct {
	char* nombreTabla;
	uint16_t key;
	char* value;
} t_INSERT;


typedef struct {
	char* nombreTabla;
	char* tipoConsistencia; // cambiarpor estructura
	int nroParticiones;
	int tiempoCompactacion;
} t_CREATE;
//------------------------------Estructuras de comunicacion MUSE LibMuse - Registros - ------------------------------//

typedef struct{
	uint32_t src;
	size_t n;
}t_registromget;

typedef struct{
	void* src;
	uint32_t dst;
	int n;
}t_registromcopy;

typedef struct {
	char * path;
	size_t length;
	int flags;
}t_registromap;

typedef struct{
	uint32_t addr;
	size_t len;
}t_registrosync;

typedef struct{
	uint32_t dir;
}t_registrounmap;

//------------------------------Estructuras de comunicacion Kernel - Memorias------------------------------//
typedef struct {
	uint64_t timeStamp;
	uint16_t clave;
	char * valor;
} t_registro;

//------------------------------Estructuras de comunicacion Memorias FileSystem------------------------------//

typedef struct {
	bool existenciaClave;
	void * valor;
} t_respuestaValor;

typedef struct{
	char * puerto;
	char * ip;
	int numeroMemoria;
	bool conexionActiva;
}t_conexionGossiping;


#endif /* BIBLIONOC_ESTRUCTURAS_H_ */
