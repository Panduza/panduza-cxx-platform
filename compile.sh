#!/bin/bash

cd panduza-cxx-platform
mkdir -p build
cd build
cmake ..
make

rmmod ftdi_sio
rmmod usbserial

./Scan