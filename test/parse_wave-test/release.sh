#!/bin/bash


if [ ! -f "./parse_wave_play-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./parse_wave_play-test /mnt/hgfs/tftpboot/

echo "release success"