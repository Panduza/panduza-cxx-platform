#!/bin/bash

docker build -t boundaryscan .
docker run --privileged --network=host -v $PWD:/panduza-cxx-platform -i -t boundaryscan /bin/bash
