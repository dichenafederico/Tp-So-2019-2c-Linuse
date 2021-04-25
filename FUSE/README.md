    Primero hay crear un archivo con la especificacion del FS
Esto lo hacemos con el SAC-Tools de la catedra

cd
mkdir pruebaFS
cd pruebaFS
mkdir tmp
dd if=/dev/urandom of=disco.bin bs=1024 count=10240
~/workspace/sac-tools/sac-format disco.bin

    Despues podemos montar el FS

~/workspace/tp-2019-2c-No-C-NadaUSE/Debug/SAC-cli --disk /home/utnso/pruebaFS/disco.bin -f -d tmp/
// en otra terminal
cd ~/pruebaFS/tmp/
touch testFoo
cat testFoo
ls -lha
rm testFoo
ls -lha

