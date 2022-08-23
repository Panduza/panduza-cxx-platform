#!/bin/bash

# sudo groupadd usb
# sudo usermod -a -G usb builder

cd /panduza-cxx-platform
mkdir -p build
cd build
cmake ..
sudo make install

sudo mkdir -p /usr/share/panduza-cxx/includes/paho.mqtt.c-src
sudo mkdir -p /usr/share/panduza-cxx/includes/paho.mqtt.cpp-src

sudo cp -R _deps/paho.mqtt.c-src /usr/share/panduza-cxx/includes
sudo cp -R _deps/paho.mqtt.cpp-src /usr/share/panduza-cxx/includes

sudo ldconfig
cd ~/panduza-cxx-class-boundary-scan
mkdir -p build && cd build
cmake ..
sudo make install
# sudo cp libBoundaryScan.so /usr/share/panduza-cxx/libraries/

rmmod ftdi_sio
rmmod usbserial

cd /panduza-cxx-platform

sudo mkdir /etc/panduza
sudo cp panduza/tree.json /etc/panduza/
# sudo cp BoundaryScan/* /etc/BoundaryScan/

cd /panduza-cxx-platform/build
./Scan -v 9