#!/bin/bash

sudo rmmod ftdi_sio
sudo rmmod usbserial

cd /panduza-cxx-platform/build

sudo ./panduza-cxx-platform -v 9