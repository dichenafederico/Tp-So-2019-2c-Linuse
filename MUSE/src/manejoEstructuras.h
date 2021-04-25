#ifndef MANEJOESTRUCTURAS_H_
#define MANEJOESTRUCTURAS_H_

#include "MUSE.h"
#include <stdbool.h>
#include <commons/bitarray.h>

//CREO LISTAS
t_list*			crearDiccionarioConexiones		();
t_segmentos_programa* crearSegmentosPrograma	();
t_list*			crearListaHeapsMetadata			();
t_list*			crearTablaProgramas				();
t_list*			crearTablaPaginas				();
t_list*         crearTablaSegmentos             ();
t_list* 		crearListaPaginasSwap			();
t_list* 		crearListaMapeos				();


//SEGMENTOS
t_segmento*     crearSegmento                   (int direccionBase, int tamanio);
t_segmento*		crearSegmentoMmap				(int direccionBase, int tamanio, t_mapAbierto* mapeo );
t_segmento*		crearSegmentoMmapCompartido		(int direccionBase, int tamanio, bool tablaDePaginasCompartida, t_mapAbierto* mapeo);
t_mapAbierto* crearMapeo(char* path, void* contenido, size_t size);
t_segmento*	    buscarSegmento					(t_list* segmentos,uint32_t direccionVirtual);
t_segmento*		buscarSegmentoId				(t_list* segmentos,int idSemgneto);
t_segmento* 	ultimoSegmentoPrograma			(t_programa* programa);
void			registrarYAgregarEnSegmento		( int cantidadDeBytes, t_programa* programa, t_segmento* segmentoElegido );
bool			esSegmentoExtendible			(t_segmentos_programa* segmentos, t_segmento* segmento);

//PAGINAS
t_pagina*       crearPagina                     (int numeroDeMarco, int nroPagina);
t_pagina* 		crearPaginaMap					(int nroFrame, int nroPagina);
int				huecoUltimaPagina				(t_segmento * segmento);
void 			agregarPaginaEnSegmento		    (int socket, t_segmento * segmento, int numeroDeMarco);
void			modificarPagina					(t_pagina* pagina ,void* nuevoContenido, bool presencia);
t_paginaAdministrativa* crearPaginaAdministrativa(int socketPrograma, int idSegmento,int nroPagina, int nroFrame);
t_paginaAdministrativa* buscarPaginaAdministrativaPorPagina(t_list* SwapOPrincipal, int socketPrograma, int idSegmento, int nroPagina);
t_paginaAdministrativa* buscarPaginaAdministrativa(t_list* SwapOPrincipal, int nroFrame);
int				nroPaginaSegmento				(uint32_t direccionVirtual, int baseLogica);
int				desplazamientoPaginaSegmento	(uint32_t direccionVirtual, int baseLogica);
void			modificarPresencia				(t_pagina* pagina , bool presencia, bool modifica);
void			modificarContenidoPagina		(t_pagina* pagina ,void* nuevoContenido, bool presencia);
void            borrarPaginaAdministrativaPorFrame(t_list* SwapOPrincipal, int nroFrameSwapOPrincipal);
t_mapAbierto*   buscarMapeoAbierto              (char* path);


//FRAMES Y CONTENIDO
t_heapSegmento* crearHeap(uint32_t tamanio, bool isFree);
t_programa* 	buscarPrograma				    (int socket);
void			cambiarContenidoFrameMemoria	(int nroFrame, void* nuevoContenido);
int             buscarFrameLibre                ();
t_pagina*		buscarFrameEnTablasDePaginas	(t_paginaAdministrativa* paginaABuscar);
int				traerFrameDePaginaEnSwap		(int socketPrograma,int idSegmento, int nroPagina);
t_heapSegmento* buscarHeapConEspacioLibre		(int cantidadBytesNecesarios, t_segmento* segmento);
int             ClockModificado                 ();
int				bytesNecesariosUltimoFrame		(int cantidadBytes);
int				framesNecesariosPorCantidadMemoria(int cantidadBytes);
void            cargarFrameASwap                (int nroFrame, t_paginaAdministrativa * paginaAdmin);
int             esDireccionLogicaValida         (uint32_t direccionLogica, t_segmento* segmento);
t_contenidoFrame* buscarContenidoFrameMemoria	(int nroFrame);
void            agregarContenido                (int nroFrame, void* contenido);

/**** Destuir estructuras ****/
void		borrarPrograma				(int socket);
void       destruirSegmento             (t_segmento* segmento);
void       destruirPagina               (t_pagina* pagina);
void	   destruirPrograma				( t_programa* programa );
void 	   destruirSegmentosPrograma	( t_segmentos_programa* segmentos );
void	   destruirHeap					( t_heapSegmento* heap );
void 	   destruirSegmentoMap			( t_segmento* segmento, bool borrarTodo );
void       borrarMapeoAbierto           (char* path);

#endif /* MANEJOESTRUCTURAS_H_ */
