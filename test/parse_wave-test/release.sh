#!/bin/bash


if [ ! -f "./parse_wave-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./parse_wave-test /mnt/hgfs/tftpboot/

echo "release success"