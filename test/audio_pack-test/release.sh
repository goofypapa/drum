#!/bin/bash


if [ ! -f "./audio_pack-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./audio_pack-test /mnt/hgfs/tftpboot/

echo "release success"