#!bin/bash

mkdir pruebas
cd pruebas
mkdir histeria
cd histeria
touch histeria.txt
truncate histeria.txt --size 3500000000
ls -lh
rm histeria.txt

counter=1
while [ $counter -le 100000000 ]
do
    echo "va a andar" >> control.txt
    counter=$((counter+10))
done