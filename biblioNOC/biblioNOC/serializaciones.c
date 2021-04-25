#include "serializaciones.h"

/*-------------------------Serializacion-------------------------*/
void serializarNumero(t_paquete* unPaquete, int numero) {
	int tamNumero = sizeof(int);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamNumero;
	unPaquete->buffer->data = malloc(tamNumero);

	memcpy(unPaquete->buffer->data, &numero, tamNumero);
}

void serializarMensaje(t_paquete* unPaquete, char * palabra) {
	int tamPalabra = strlen(palabra) + 1;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamPalabra);
	unPaquete->buffer->size = tamPalabra;

	memcpy(unPaquete->buffer->data, palabra, tamPalabra);
}

void serializarHandshake(t_paquete * unPaquete, int emisor) {
	serializarNumero(unPaquete, emisor);
}

void serializarArchvivo(t_paquete * unPaquete, char * rutaArchivo) {
	size_t tamArch;

	FILE * archivofd;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch, &archivofd);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamArch);
	unPaquete->buffer->size = tamArch;

	memcpy(unPaquete->buffer->data, archivo, tamArch);

	munmap(archivo, tamArch);

	fclose(archivofd);
}

void serializarClave(t_paquete * unPaquete, char * clave) {
	serializarMensaje(unPaquete, clave);
}


void serializarSelectQuery(t_paquete * unPaquete,char* nombreTabla, uint16_t clave) {

	char* query = string_from_format("%s %d", nombreTabla, clave);

	int tamQuery = strlen(query) + 1;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamQuery;

	unPaquete->buffer->data = malloc(tamQuery);

	memcpy(unPaquete->buffer->data, query, tamQuery);

}


void serializarSelect(t_paquete * unPaquete,char* nombreTabla, uint16_t clave) {

	int tamClave       = sizeof(uint16_t);
	int tamNombreTabla = strlen(nombreTabla) + 1;
	int tamTotal       = tamNombreTabla + tamClave;

	int desplazamiento = 0;

	unPaquete->buffer       = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	memcpy(unPaquete->buffer->data + desplazamiento, nombreTabla, tamNombreTabla);
	desplazamiento += tamNombreTabla;

	memcpy(unPaquete->buffer->data + desplazamiento, &clave, tamClave);
	desplazamiento += tamClave;

}


void serializarInsertQuery(t_paquete * unPaquete,char* nombreTabla, uint16_t clave, char* value, uint64_t timestamp) {

	char* query = string_from_format("%s %d %s %lld", nombreTabla, clave, value, timestamp);

	int tamQuery = strlen(query) + 1;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamQuery;

	unPaquete->buffer->data = malloc(tamQuery);

	memcpy(unPaquete->buffer->data, query, tamQuery);
}


void serializarInsert(t_paquete * unPaquete,char* nombreTabla, uint16_t clave,char * value) {

	int tamClave       = sizeof( uint16_t );
	int tamNombreTabla = strlen(nombreTabla) + 1;
	int tamValue       = strlen(value) + 1;
	int tamTotal       = tamNombreTabla + tamClave + tamValue;

	int desplazamiento = 0;

	unPaquete->buffer       = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	memcpy(unPaquete->buffer->data + desplazamiento, nombreTabla, tamNombreTabla);
	desplazamiento += tamNombreTabla;

	memcpy(unPaquete->buffer->data + desplazamiento, &clave, tamClave);
	desplazamiento += tamClave;

	memcpy(unPaquete->buffer->data + desplazamiento, value, tamValue);
	desplazamiento += tamValue;
}

void serializarRegistro(t_paquete * unPaquete, uint64_t timeStamp, uint16_t clave, char * valor) {

	int tamClave     = sizeof(uint16_t);
	int tamValor     = strlen(valor) + 1;
	int tamTimeStamp = sizeof(uint64_t);
	int tamTotal     = tamValor + tamClave + tamTimeStamp;

	int desplazamiento = 0;

	unPaquete->buffer       = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	memcpy(unPaquete->buffer->data + desplazamiento, &clave, tamClave);
	desplazamiento += tamClave;

	memcpy(unPaquete->buffer->data + desplazamiento, valor, tamValor);
	desplazamiento += tamValor;

	memcpy(unPaquete->buffer->data + desplazamiento, &timeStamp, tamTimeStamp);
}


void serializarCreateQuery(t_paquete * unPaquete, char * nombreTabla, char * tipoConsistencia, int numeroParticiones, int tiempoCompactacion) {

	char* query = string_from_format("%s %s %d %d", nombreTabla, tipoConsistencia, numeroParticiones, tiempoCompactacion);

	int tamQuery = strlen(query) + 1;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamQuery;

	unPaquete->buffer->data = malloc(tamQuery);

	memcpy(unPaquete->buffer->data, query, tamQuery);
}


void serializarCreate(t_paquete * unPaquete,char* nombreTabla, char* tipoConsistencia, int numeroParticiones, int tiempoCompactacion) {

	int tamNumero           = sizeof(int);
	int tamNombreTabla      = strlen(nombreTabla) + 1;
	int tamTipoConsistencia = strlen(tipoConsistencia) + 1;
	int tamTotal            = tamNombreTabla + tamTipoConsistencia + tamNumero + tamNumero;

	int desplazamiento = 0;

	unPaquete->buffer       = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	memcpy(unPaquete->buffer->data + desplazamiento, &numeroParticiones, tamNumero);
	desplazamiento+= tamNumero;

	memcpy(unPaquete->buffer->data + desplazamiento, &tiempoCompactacion, tamNumero);
	desplazamiento+= tamNumero;

	memcpy(unPaquete->buffer->data + desplazamiento, nombreTabla, tamNombreTabla);
	desplazamiento += tamNombreTabla;

	memcpy(unPaquete->buffer->data + desplazamiento, tipoConsistencia, tamTipoConsistencia);
	desplazamiento += tamTipoConsistencia;
}

void serializarConexionMemoria(t_paquete * unPaquete, int numeroMemoria, t_list * listaMemorias) {

	int cantidadMemorias = list_size( listaMemorias );
	int tamanioListaMemorias = 0;

	// Primero recorro para calcular que va a ocupar la lista de memorias
	for( int i = 0; i < cantidadMemorias; i++ ){
		t_conexionGossiping * memoria = list_get( listaMemorias, i );

		tamanioListaMemorias += strlen( memoria->ip ) + 1;
		tamanioListaMemorias += strlen( memoria->puerto ) + 1;
		tamanioListaMemorias += sizeof( memoria->conexionActiva );
		tamanioListaMemorias += sizeof( int );
	}

	// Soy memoria numero N
	int tamNumero = sizeof(int);

	/* Paquete:
	*  numero propio de memoria + cant de memorias + ( por cada memoria ) *
	*      ( ip + puerto + bool conexion activa + numero de memoria asociado )
	*/
	int tamTotal = tamNumero + tamNumero + tamanioListaMemorias ;

	// Reservo memoria para el paquete
	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	// Copio en buffer numero propio de memoria y cantidad de memorias
	memcpy( unPaquete->buffer->data , &numeroMemoria, tamNumero );
	int desplazamiento = tamNumero;

	memcpy( unPaquete->buffer->data + desplazamiento, &cantidadMemorias, tamNumero );
	desplazamiento += sizeof( int );

	// Recorro y copio en buffer cada memoria de mi lista
	int tamanioIp;
	int tamanioPuerto;
	for( int i = 0; i < cantidadMemorias; i++ ){
		t_conexionGossiping * memoria = list_get( listaMemorias, i );

		tamanioIp     = strlen( memoria->ip     ) + 1;
		tamanioPuerto = strlen( memoria->puerto ) + 1;

		memcpy(unPaquete->buffer->data + desplazamiento, memoria->ip, tamanioIp);
		desplazamiento += tamanioIp;

		memcpy(unPaquete->buffer->data + desplazamiento, memoria->puerto, tamanioPuerto);
		desplazamiento += tamanioPuerto;

		memcpy(unPaquete->buffer->data + desplazamiento, &(memoria->numeroMemoria), sizeof( int ));
		desplazamiento += sizeof( int );

		memcpy(unPaquete->buffer->data + desplazamiento, &(memoria->conexionActiva), sizeof( bool ));
		desplazamiento += sizeof( bool );
	}
}


void serializarExistenciaClaveValor(t_paquete * unPaquete, bool claveExistente, void * valor) {

	int tamClaveExistente = sizeof(bool);

	int tamValor;

	if (valor != NULL) {
		tamValor = strlen(valor) + 1;
	} else {
		tamValor = 0;
	}

	int tamTotal = tamClaveExistente + tamValor;

	int desplazamiento = 0;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamTotal);
	unPaquete->buffer->size = tamTotal;

	memcpy(unPaquete->buffer->data + desplazamiento, &claveExistente,
			tamClaveExistente);
	desplazamiento += tamClaveExistente;

	memcpy(unPaquete->buffer->data + desplazamiento, valor, tamValor);
}

/*-------------------------Deserializacion-------------------------*/

int recibirRespuestaLql(t_paquete* unPaquete) {
	return deserializarNumero(unPaquete->buffer);
}

int deserializarNumero(t_stream* buffer) {
	return *(int*) (buffer->data);
}

char * deserializarMensaje(t_stream * buffer) {
	char * palabra = strdup(buffer->data);

	return palabra;
}

int deserializarHandshake(t_stream * buffer) {
	return deserializarNumero(buffer);
}

void * deserializarArchivo(t_stream * buffer) {
	void * archivo = malloc(buffer->size);

	memcpy(archivo, buffer->data, buffer->size);

	return archivo;
}

char * deserializarClave(t_stream * buffer) {
	return deserializarMensaje(buffer);
}

t_registro* deserializarRegistro(t_stream * buffer) {
	t_registro* registro = malloc(sizeof(t_registro));
	int desplazamiento = 0;

	memcpy( &registro->clave, buffer->data + desplazamiento, sizeof( registro->clave ) );
	desplazamiento +=  sizeof( registro->clave );

	registro->valor = strdup(buffer->data + desplazamiento + 1);
	desplazamiento += string_length(registro->valor);

	memcpy( &registro->timeStamp, buffer->data + desplazamiento, sizeof( registro->timeStamp ) );
	desplazamiento +=  sizeof( registro->timeStamp );

	return registro;
}

t_registro* deserializarRegistroString(t_stream * buffer) {
	char* data   = (char*) buffer->data;
	char** split = string_split( data, ";" ); // ts;key;value

	t_registro* registro = malloc(sizeof(t_registro));
	registro->timeStamp  = strtoul( split[ 0 ], NULL, 10 );
	registro->clave      = ( uint16_t ) strtoul( split[ 1 ], NULL, 10 );
	registro->valor      = strdup( split[ 2 ] );

	return registro;
}

/**
* @NAME: deserializarConexionMemoria
* @DESC: Deserealiza lista de conexiones
* recibida como buffer
*/
t_list* deserializarConexionMemoria(t_stream * buffer) {
	/* Paquete:
	*  numero propio de memoria + cant de memorias + ( por cada memoria ) *
	*      ( ip + puerto + bool conexion activa + numero de memoria asociado )
	*/
	int desplazamiento = 0;
	int numeroDeMemoriaEmisora, cantDeMemoriasEnBuffer;

	memcpy( &numeroDeMemoriaEmisora, buffer->data + desplazamiento, sizeof( int ) );
	desplazamiento += sizeof( int );

	memcpy( &cantDeMemoriasEnBuffer, buffer->data + desplazamiento, sizeof( int ) );
	desplazamiento += sizeof( int );

	t_list *  listaDeConexionesRecibida = list_create();

	for( int i = 0; i < cantDeMemoriasEnBuffer; i++ ){
		// ( ip + puerto + bool conexion activa + numero de memoria asociado )

		t_conexionGossiping* conexion = malloc(sizeof(t_conexionGossiping));

		conexion->ip = strdup( buffer->data + desplazamiento );
		desplazamiento += ( string_length( conexion->ip ) + 1 );

		conexion->puerto = strdup( buffer->data + desplazamiento );
		desplazamiento += ( string_length( conexion->puerto ) + 1 );

		memcpy( &conexion->numeroMemoria, buffer->data + desplazamiento, sizeof( int ) );
		desplazamiento += sizeof( int );

		memcpy( &conexion->conexionActiva, buffer->data + desplazamiento, sizeof( bool ) );
		desplazamiento += sizeof( bool );

		list_add( listaDeConexionesRecibida, conexion );
	}

	return listaDeConexionesRecibida;
}


int deserializarTamanioCreate( t_stream * buffer ){
	return atoi( (char*)buffer->data );
}


t_CREATE * deserializarCreate(t_stream * buffer){
	int desplazamiento = 0;

	t_CREATE* create = malloc(sizeof(t_CREATE));

	memcpy( &create->nroParticiones, buffer->data + desplazamiento, sizeof( create->nroParticiones ) );
	desplazamiento +=  sizeof( create->nroParticiones );

	memcpy( &create->tiempoCompactacion, buffer->data + desplazamiento, sizeof( create->tiempoCompactacion ) );
	desplazamiento +=  sizeof( create->tiempoCompactacion );

	create->nombreTabla = strdup(buffer->data + desplazamiento);
	desplazamiento += ( string_length(create->nombreTabla) + 1 );

	create->tipoConsistencia = strdup(buffer->data + desplazamiento);
	desplazamiento += ( string_length(create->tipoConsistencia) + 1 );

	return create;
}

t_SELECT * deserializarSelect(t_stream * buffer){
	t_SELECT* selectAPI = malloc(sizeof(t_SELECT));

	int desplazamiento = 0;

	selectAPI->nombreTabla = strdup( buffer->data + desplazamiento );
	desplazamiento += ( string_length( selectAPI->nombreTabla ) + 1 );

	memcpy( &selectAPI->key, buffer->data + desplazamiento, sizeof( selectAPI->key ) );
	desplazamiento +=  sizeof( selectAPI->key );

	return selectAPI;
}

t_INSERT * deserializarInsert(t_stream * buffer){
	t_INSERT* insert = malloc(sizeof(t_INSERT));

	int desplazamiento = 0;

	insert->nombreTabla = strdup(buffer->data + desplazamiento);
	desplazamiento += ( string_length(insert->nombreTabla) + 1 );

	memcpy( &insert->key, buffer->data + desplazamiento, sizeof( insert->key ) );
	desplazamiento +=  sizeof( insert->key );

	insert->value = strdup(buffer->data + desplazamiento);
	desplazamiento += string_length( insert->value );

	return insert;
}

/*-------------------------Funciones auxiliares-------------------------*/
void * abrirArchivo(char * rutaArchivo, size_t * tamArc, FILE ** archivo) {
	//Abro el archivo
	*archivo = fopen(rutaArchivo, "r");

	if (*archivo == NULL) {
		printf("%s: No existe el archivo o el directorio", rutaArchivo);
		return NULL;
	}

	//Copio informacion del archivo
	struct stat statArch;

	stat(rutaArchivo, &statArch);

	//Tamaño del archivo que voy a leer
	*tamArc = statArch.st_size;

	//Leo el total del archivo y lo asigno al buffer
	int fd = fileno(*archivo);
	void * dataArchivo = mmap(0, *tamArc, PROT_READ, MAP_SHARED, fd, 0);

	return dataArchivo;
}
