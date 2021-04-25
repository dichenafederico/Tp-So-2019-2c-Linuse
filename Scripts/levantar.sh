#! /bin/bash -e
#Flag error -> cierre del script
#sudo apt install dos2unix --Para eliminar saltos de DOS
#Repo tp-2019-2c-No-C-Nada: git clone https://github.com/sisoputnfrba/tp-2019-2c-No-C-Nada.git
#Repo Hilolay: git clone https://github.com/sisoputnfrba/hilolay.git


MYPATH="/home/utnso/workspace/tp-2019-2c-No-C-Nada"

 

export LD_LIBRARY_PATH=/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC/Debug:/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioSuse/Debug:/home/utnso/workspace/tp-2019-2c-No-C-Nada/libMUSE/Debug

echo "Ready - Variable de Entorno"



#Levanto  .so

cd "$MYPATH/biblioNOC/Debug" 
make clean
make all

cd "$MYPATH/libMUSE/Debug"
make clean
make all

cd "$MYPATH/biblioSuse/Debug"
make clean
make all

cd "$MYPATH/MUSE/Debug"
make clean
make all

cd "$MYPATH/SUSE/Debug"
make clean
make all

cd "$MYPATH/FUSE/Debug"
make clean
make all

cd "$MYPAT/SACServer/Debug"
make clean
make all

echo "prueba"

cd "/home/utnso/workspace"

git clone https://github.com/sisoputnfrba/linuse-tests-programs.git


cd "/home/utnso/workspace/linuse-tests-programs"
cp archivo_de_swap_supermasivo.c ../tp-2019-2c-No-C-Nada/pruebaMuse/src
cp recursiva.c ../tp-2019-2c-No-C-Nada/pruebaMuse/src
cd "$MYPATH/pruebaMuse"
make clean
make all


echo "Realizado - Fin de script"




