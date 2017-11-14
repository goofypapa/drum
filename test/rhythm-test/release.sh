#!/bin/bash


if [ ! -f "./rhythm-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./rhythm-test /mnt/hgfs/tftpboot/

echo "release success"