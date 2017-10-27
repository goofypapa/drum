#!/bin/bash


if [ ! -f "./json-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./json-test /mnt/hgfs/tftpboot/

echo "release success"