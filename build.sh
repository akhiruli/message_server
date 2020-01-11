#!/bin/bash

cd client
make clean all

if [ $? -ne 0 ]; then
    echo "failed compilation of client"
    exit
fi

cd ../server

make clean all

if [ $? -ne 0 ]; then
    echo "failed compilation of server"
    exit
fi

cd ../.
mkdir -p bin

mv client/msgclient bin/
mv server/msgserver bin/
