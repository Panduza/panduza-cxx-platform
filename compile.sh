#!/bin/bash

# sudo groupadd usb
# sudo usermod -a -G usb builder

cd /panduza-cxx-platform
mkdir -p build
cd build
cmake ..
make

sudo mkdir /usr/share/panduza-cxx
sudo mkdir /usr/share/panduza-cxx/libraries
sudo mkdir /usr/share/panduza-cxx/includes
sudo mkdir /usr/share/panduza-cxx/includes/paho.mqtt.c-src
sudo mkdir /usr/share/panduza-cxx/includes/paho.mqtt.cpp-src

sudo cp --preserve=links _deps/paho.mqtt.c-build/src/*.so* /usr/local/lib/
sudo cp --preserve=links _deps/paho.mqtt.cpp-build/src/*.so* /usr/local/lib/
sudo cp libMetaplatform.so /usr/share/panduza-cxx/libraries/
sudo cp -R _deps/paho.mqtt.c-src /usr/share/panduza-cxx/includes
sudo cp -R _deps/paho.mqtt.cpp-src /usr/share/panduza-cxx/includes

cd ~/panduza-cxx-class-boundary-scan
mkdir build && cd build
cmake ..
make
sudo cp libBoundaryScan.so /usr/share/panduza-cxx/libraries/

rmmod ftdi_sio
rmmod usbserial

cd /panduza-cxx-platform

sudo mkdir /etc/panduza
sudo cp panduza/tree.json /etc/panduza/
# sudo cp BoundaryScan/* /etc/BoundaryScan/

cd /panduza-cxx-platform/build

./Scan -v 9