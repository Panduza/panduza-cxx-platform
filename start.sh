#!/bin/bash

docker build -t boundaryscan .
docker run --privileged --network=host -i -t boundaryscan /bin/bash
