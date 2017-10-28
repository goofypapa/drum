#!/bin/bash


if [ ! -f "./log-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./log-test /mnt/hgfs/tftpboot/

echo "release success"