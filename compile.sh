#!/bin/bash

cd panduza-cxx-platform
mkdir -p build 
cd build
cmake ..
make

systemctl restart mosquitto
rmmod ftdi_sio
rmmod usbserial

./Scan