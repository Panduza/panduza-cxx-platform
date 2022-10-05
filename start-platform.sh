#!/bin/bash

rmmod ftdi_sio
rmmod usbserial

cd /panduza-cxx-platform/build

./panduza-cxx-platform -v 9