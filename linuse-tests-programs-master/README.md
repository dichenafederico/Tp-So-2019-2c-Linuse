# Pruebas de Linuse

Para probarlo instalar previamente [Hilolay](https://github.com/sisoputnfrba/hilolay) y correr
`make && make entrega`

Los nombres de las pruebas son:

```
archivo_de_swap_supermasivo
audiencia
caballeros_de_SisOp_Afinador
caballeros_de_SisOp_Solo
estres_compartido
estres_privado
recursiva
revolucion_compartida
revolucion_privada
```

Para ejecutar una prueba correr:
`make NOMBRE_DE_LA_PRUEBA` (por ejemplo `make audiencia`)

## Configuración inicial

Como primer paso para ver que los tests funcionen correctamente en el entorno, recomendamos correr todas las pruebas sin modificaciones. Esto garantizará que no haya errores de compilación ni de bibliotecas, más allá de que las funciones que se llaman se encuentran vacías.

Una vez hecho esto, hay dos archivos que deben modificarse:
- libmuse.h
- hilolay_alumnos.c

Ambos archivos están con el único propósito de que las pruebas puedan compilarse correctamente, independientemente de cada trabajo práctico. Para poder probar realmente su implementación es necesario realizar una de las siguientes opciones

- Agregar las bibliotecas compartidas de cada grupo en LD_LIBRARY_PATH
- Modificar el Makefile de este proyecto para que incluya las bibliotecas del grupo
- Como última opción (no recomendada, pero sirve como prueba) modificar ambos archivos para que reflejen la implementación de cada grupo.

Es posible que, dependiendo de la solución usada en el paso anterior, necesiten modificar el include de `"libmuse.h"` por `<libmuse.h>`. También es posible que, para ciertas pruebas, necesiten modificar ips y puertos incluidos dentro de los archivos de prueba, y luego volver a correr `make entrega`.

Cualquier duda o inconveniente, por favor reportarlo en el foro. Son válidas las modificaciones a la configuración que cada grupo quiera hacer, respetando el código de la prueba.
