#!/bin/bash


if [ ! -f "./gain-test" ]
then
    echo "build"
    make
else
    make clean
    make
fi

cp ./gain-test /media/psf/Home/tftpboot/

echo "release success"