#!/bin/bash


if [ ! -f "./play_wav-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./play_wav-test /mnt/hgfs/tftpboot/

echo "release success"