#ifndef operaciones_h
#define operaciones_h

#include "estructuras.h"
#include "manejoEstructuras.h"

//Operaciones
uint32_t		 procesarAlloc		(uint32_t tam, int socket);
void			 procesarFree		(uint32_t dir, int socket);
int				 procesarGet		(void* dst, uint32_t src, size_t n, int socket);
int				 procesarCopy		(uint32_t dst, void* src, int n, int socket);
uint32_t		 procesarMap		(char *path, size_t length, int flags, int socket);
int 			 procesarSync		(uint32_t addr, size_t len, int socket);
uint32_t		 procesarUnMap		(uint32_t dir, int socket);

//Funciones principales manipular memoria MUSE
uint32_t		 allocarEnHeapLibre	(uint32_t cantidadBytesNecesarios, t_segmentos_programa* segmentos);
int 			 allocarHeapNuevo	(t_programa* programa, t_segmento* segmento, int cantBytesNecesarios);
int 		     allocarEnPaginasNuevas(t_programa* programa, t_segmento* segmentoAExtender, int cantPaginasNecesarias );
int				 verificarCompactacionFree(t_list* heaps, int indiceHeap);
int			 	 cambiarFramesPorHeap(t_segmento* segmento, uint32_t direccionLogica, uint32_t tamanio, bool cargo);

//Funciones auxiliares memoria MUSE
//si al page fault le paso el ultimo bool en true, hace la operacion inversa
int 			 pageFault				(int socket_programa, t_segmento* segmento, int i , void* contenidoDestinoOsrc, int offsetInicial, int desplazamiento, bool operacionInversa, int offsetContenido);
int 			 copiarContenidoDeFrames(int socket,t_segmento* segmento, uint32_t direccionLogica, size_t tamanio,void* contenidoDestino);
int 			 copiarContenidoAFrames(int socket,t_segmento* segmento, uint32_t direccionLogica, int tamanio,void* porcionMemoria);
void*			 sacarFrameSwap		(int nroMarco, FILE** archivo);
void			 escribirFrameSwap	(int nroMarco, void* contenido,FILE** archivo);
void* 			 traerContenidoMap		(int indiceBuscado, void* mapeo);
void* 			 traerContenidoSwap(int indiceBuscado);
void 			 TraerPaginaDeSwap	(int socketPrograma, t_pagina* pagina, int idSegmento);
void 			 TraerPaginaDeMap	(int socketPrograma,t_segmento* segmento, t_pagina* pagina);
void			 cargarPaginaEnSwap	(void* bytes,int nroPagina, int socketPrograma, int idSegmento);
void 			 escribirContenidoEnSwap(int indiceLibre,void* contenido,int desplazamiento);
void 			 paginasDeMapAPrincipal(size_t tamanioMap,t_segmento* unSegmento,int socket);

//Funciones de mapeo
void*			 leerArchivoCompartido();
void*			 escribirEnArchivoCompartido();
void*			 mapearArchivoMUSE(char * rutaArchivo, size_t size, int flags);

//METRICAS
void			 ActualizarLogMetricas();
void 			 RegistrarMetricasPrograma(t_programa* programa);
uint32_t		 EspacioLibre(t_segmento* segmento);
int				 PorcentajeAsignacionMemoria(t_programa* programa);
int				 SistemaMemoriaDisponible();

#endif
