#! /bin/bash

if [ $# -ge 2 ]
then
 echo "Usage: $0"
 echo "Only project name should be transmited like paramete"
 exit 1
fi

if [ $# -eq 0 ]
then
 read  -t 10 -p "Please, enter the name of the project to be generated: " PrjName
else
 PrjName=$1
fi

mkdir $PrjName
cd ./$PrjName/

mkdir build
mkdir include
mkdir misc
mkdir src
touch ./misc/contains.conf

cd ./../

chmod -R 770 ./$PrjName/

