#!/bin/bash

if lsmod | grep ftdi_sio &> /dev/null ; then
    rmmod ftdi_sio
fi
if lsmod | grep usbserial &> /dev/null ; then
    rmmod usbserial
fi

cd /panduza-cxx-platform/build

./panduza-cxx-platform

/bin/bash