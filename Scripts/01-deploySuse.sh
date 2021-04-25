#!/bin/bash
echo "welcome"
ll
cd /home/utnso/workspace
git clone https://github.com/sisoputnfrba/linuse-tests-programs.git
cd linuse-tests-programs/
cp audiencia.c ../tp-2019-2c-No-C-Nada/hilolay-example/src
cp caballeros_de_SisOp_Afinador.c ../tp-2019-2c-No-C-Nada/hilolay-example/src
cp caballeros_de_SisOp_Solo.c ../tp-2019-2c-No-C-Nada/hilolay-example/src
cd ../tp-2019-2c-No-C-Nada/hilolay-example/
make clean
make all