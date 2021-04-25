#include "MUSE.h"

int main(void) {

	idSegmento = 1;
	punteroClock = 0;
	nroPrograma = 1;
	tamanio_heap = 5;

	g_logger = log_create("MUSE.log", "MUSE", true, LOG_LEVEL_TRACE);
	g_loggerDebug = log_create("MUSEDebug.log", "MUSE", false, LOG_LEVEL_DEBUG);
	log_info( g_logger, "Inicio proceso de MUSE" );

	programas = crearTablaProgramas();
	paginasEnSwap = crearListaPaginasSwap();
	mapeosAbiertosCompartidos = crearListaMapeos();
	tablasDePaginas = crearTablaPaginas();
	contenidoFrames = crearTablaPaginas();

	sem_init(&g_mutexPaginas, 0, 1); // el salvador

	sem_init(&g_mutexTablaProgramas, 0, 1);
	sem_init(&g_mutextablasDePaginas, 0, 1);
	sem_init(&g_mutexMapeosAbiertosCompartidos, 0, 1);
	sem_init(&g_mutexPaginasEnSwap, 0, 1);
	sem_init(&g_mutexSwap, 0, 1);
	sem_init(&g_mutexgBitarray_swap, 0, 1);
	sem_init(&g_mutexgBitarray_marcos, 0, 1);
	sem_init(&g_mutexgContenidoFrames, 0, 1);

	armarConfigMemoria();
	lengthPagina = g_configuracion->tamanioPagina;

	reservarEspacioMemoriaPrincipal();

	g_archivo_swap = crearArchivoSwap( RUTASWAP, g_configuracion->tamanioSwap  );

	iniciarServidor(g_configuracion->puertoConexion,g_logger, (void*)attendConnection);

	return prueba();

}

int arrancarServer(char* puertostring){

	int ok = iniciarServidor(puertostring, g_logger, (void*)attendConnection);
	return ok;
}

void attendConnection( int socketCliente) {
	// int socketCliente = *(int *)socket_fd;
	log_debug( g_loggerDebug, "Attend connection con este socket %d", socketCliente );
	t_paquete* package = recibirArmarPaquete(socketCliente);
	t_paquete* response;

	log_debug( g_loggerDebug, "Checkeo que el paquete sea handshake" );
	// Espero recibir el handshake y trato segun quien se conecte
	int handshake_code = recibirHandshake(package);
	destruirPaquete( package );

	switch(handshake_code){
		case LIBMUSE: ;
			log_debug( g_loggerDebug, "Recibi el handshake del cliente" );

			while (1) {
				package = recibirArmarPaquete(socketCliente);
				//TODO: me rompe aca en el free, tira error rancio, pero free ejecuto bien
				//log_debug( g_loggerDebug, "Recibo paquete" );

				if ( package == NULL || package->codigoOperacion == ENVIAR_AVISO_DESCONEXION ){
					log_warning( g_loggerDebug, "Cierro esta conexion del LibMuse %d", socketCliente );
					break;
				};

				response = procesarPaqueteLibMuse( package, socketCliente );
				// enviarPaquetes(socketCliente, response);
				// destruirPaquete(response);
			}
			break;
		default:
			log_warning( g_loggerDebug, "El paquete recibido no es handshake" );
			break;
	}
	close(socketCliente);
	// removeThreadFromActualThreads( pthread_self() );
}


t_paquete* procesarPaqueteLibMuse(t_paquete* paquete, int cliente_fd) {

	int socket = cliente_fd;

	log_debug( g_loggerDebug, "Proceso codigo op %d", paquete->codigoOperacion );

	switch (paquete->codigoOperacion) {

	/* nunca entra por aca porque el handshake lo recibo cuando entro a "attendConnection"
	case HANDSHAKE:
		procesarHandshake(paquete, cliente_fd);
		break;
    */
	case MUSE_INIT: ;
	    InicializarNuevoPrograma(socket);
		break;

	case MUSE_CLOSE: ;
		FinalizarPrograma(socket);
		break;

	case MUSE_ALLOC: ;

		sem_wait(&g_mutexPaginas);

		uint32_t tamanio  = deserializarUINT32(paquete->buffer);
		uint32_t direccionLogica = procesarAlloc(tamanio, socket);

		sem_post(&g_mutexPaginas);
		enviarRespuestaAlloc(cliente_fd,direccionLogica);
		break;

	case MUSE_FREE: ;

		sem_wait(&g_mutexPaginas);

		uint32_t direccionLogicaFree = deserializarUINT32(paquete->buffer);
		procesarFree(direccionLogicaFree,socket); //Libera una porcion de memoria reservada

		sem_post(&g_mutexPaginas);
		break;

	case MUSE_GET: ;
		t_registromget* registroGet = deserializarGet(paquete->buffer);

		void* buffer = malloc( registroGet->n );
		sem_wait(&g_mutexPaginas);
		uint32_t operacionSatisfactoriaGet = procesarGet( buffer, registroGet->src, registroGet->n, socket );

		sem_post(&g_mutexPaginas);
		enviarRespuestaGet( cliente_fd, operacionSatisfactoriaGet, registroGet->n, buffer );
		free( buffer );
		free( registroGet );
		break;

	case MUSE_COPY: ;
		t_registromcopy* registroCopy = deserializarCopy(paquete->buffer);

		sem_wait(&g_mutexPaginas);
		uint32_t operacionSatisfactoriaCopy = procesarCopy( registroCopy->dst,registroCopy->src,registroCopy->n,socket);

		sem_post(&g_mutexPaginas);

		enviarRespuestaCopy(cliente_fd, operacionSatisfactoriaCopy);
		destruirRequestCopy( registroCopy );
		break;

	case MUSE_MAP: ;
		t_registromap* registroMap = deserealizarMap(paquete->buffer);

		uint32_t pocision = procesarMap(registroMap->path,registroMap->length,registroMap->flags,socket);

		enviarRespuestaMap(cliente_fd, pocision);

		break;

	case MUSE_SYNC: ;
		t_registrosync* registroSync = deserealizarMsync(paquete->buffer);

		uint32_t resultadoSync =  procesarSync( registroSync->addr,registroSync->len,socket);

		enviarRespuestaMsync(cliente_fd,resultadoSync);

		break;

	case MUSE_UNMAP: ;
		uint32_t direccionLogicaUnmap = deserializarUINT32(paquete->buffer);

		uint32_t resultadoUnMap =  procesarUnMap(direccionLogicaUnmap,socket);

		enviarRespuestaUnmap(cliente_fd,resultadoUnMap);

		break;

	default:
		log_warning( g_logger, "Codigo no reconocido: %d", paquete->codigoOperacion );
		break;
	}

	destruirPaquete(paquete);
	return NULL;
}



void armarConfigMemoria() {
	char* ruta = RUTACONFIG;

	log_info( g_logger, "Leyendo config: %s", ruta );

	g_config = config_create(ruta);
	g_configuracion = malloc( sizeof( t_configuracion ) );

	g_configuracion->puertoConexion    = strdup( config_get_string_value(g_config, "LISTEN_PORT") );
	g_configuracion->tamanioMemoria    = config_get_int_value(g_config, "MEMORY_SIZE");
	g_configuracion->tamanioPagina     = config_get_int_value(g_config, "PAGE_SIZE");
	g_configuracion->tamanioSwap       = config_get_int_value(g_config, "SWAP_SIZE");

	config_destroy(g_config);
}

void reservarEspacioMemoriaPrincipal(){

	log_debug( g_loggerDebug, "Reservando memoria (bytes) %d", g_configuracion->tamanioMemoria );
	g_granMalloc = malloc( g_configuracion->tamanioMemoria );

	g_cantidadFrames = ( int )( g_configuracion->tamanioMemoria /  g_configuracion->tamanioPagina );
	char * data = malloc( g_cantidadFrames );
	memset(data, 0, g_cantidadFrames); // Inicializo todos los marcos en 0 ( libres
	g_bitarray_marcos = bitarray_create_with_mode(data, g_cantidadFrames, MSB_FIRST);

	maxPaginasEnSwap =  ( int )( g_configuracion->tamanioSwap / g_configuracion->tamanioPagina );
	char * dataSwap = malloc( maxPaginasEnSwap );
	memset(dataSwap, 0, maxPaginasEnSwap); // Inicializo todos los marcos en 0 ( libres )
	g_bitarray_swap = bitarray_create_with_mode(dataSwap, maxPaginasEnSwap, MSB_FIRST);

}

void InicializarNuevoPrograma(int socket){
	log_info( g_logger, "Levanto programa nro %d",nroPrograma);
	t_programa * nuevoPrograma = crearPrograma(socket);
	list_add(programas,nuevoPrograma);

}

void FinalizarPrograma(int socket){
	destruirPrograma(buscarPrograma(socket));
	//ActualizarLogMetricas();
}

void* crearArchivoSwap( char* path, int size ){
	// Abro el archivo
	int fd = open( path, O_CREAT | O_RDWR );

	if ( fd == -1 ) {
		perror( strerror( errno ) );
		printf("%s: No existe el archivo o el directorio", path );
		return NULL;
	}

	// Le doy el tamanio de la config
	int truncate_result = ftruncate( fd, size );

	if( truncate_result != 0 ){
		printf( "No truncar archivo de swap: %d", truncate_result );
		perror( strerror( errno ) );
	}

	void * dataArchivo = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memset( dataArchivo, 0, size );
	msync( dataArchivo, size, MS_SYNC);

	return dataArchivo;
}

void destruirGlobales(){}


