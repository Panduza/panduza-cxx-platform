#!/bin/bash

groupadd usb
usermod -a -G usb root

cd panduza-cxx-platform
mkdir -p build
cd build
cmake ..
make

rmmod ftdi_sio
rmmod usbserial

./Scan