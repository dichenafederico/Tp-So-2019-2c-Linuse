#! /bin/bash -e

cd /home/utnso/workspace/tp-2019-2c-No-C-Nada/configs/SACServer

#Crear un archivo para disco binario.

dd if=/dev/urandom of=disco.bin bs=1024 count=10240

#Modificar "count" para la cantidad de kilobytes necesarios.

#Formatear el disco binario

./sac-format disco.bin

#Hacer un dump de sus estructuras internas.

./sac-dump disco.bin
