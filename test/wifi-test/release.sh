#!/bin/bash


if [ ! -f "./wifi-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./wifi-test /media/psf/Home/tftpboot/

echo "release success"