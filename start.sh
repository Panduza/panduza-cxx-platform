#!/bin/bash

docker build -t boundaryscan .
docker run --privileged --network=host -i -t boundaryscan /bin/bash

cd /usr/local/lib
sudo ldconfig

sudo rmmod ftdi_sio
sudo rmmod usbserial
