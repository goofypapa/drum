#!/bin/bash


if [ ! -f "./gain-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./gain-test /mnt/hgfs/tftpboot/

echo "release success"