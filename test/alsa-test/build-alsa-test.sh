#!/bin/bash


if [ ! -d "./build" ]
then
    mkdir build
fi

g++ alsa-test.cpp -o build/alsa-test -lasound -lpthread

arm-linux-g++ alsa-test.cpp -o build/alsa-test-arm -lasound -lpthread

cp ./build/alsa-test-arm /media/psf/Home/tftpboot/

echo "build success"