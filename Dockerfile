FROM ubuntu:20.04

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install -y build-essential gcc make git cmake libssl-dev wget pkg-config libboost-program-options-dev libjsoncpp-dev

WORKDIR /temp
RUN echo "Install libftdi..."
RUN wget -c https://ftdichip.com/wp-content/uploads/2021/09/libftd2xx-x86_64-1.4.24.tgz
RUN gunzip libftd2xx-x86_64-1.4.24.tgz
RUN tar -xvf libftd2xx-x86_64-1.4.24.tar
RUN cp -r release/build/lib* /usr/local/lib
WORKDIR /usr/local/lib
RUN ln -s libftd2xx.so.1.4.24 libftd2xx.so
RUN chmod 0755 libftd2xx.so.1.4.24

WORKDIR /
RUN mkdir setup
WORKDIR /setup
RUN git clone https://github.com/eclipse/paho.mqtt.c.git
WORKDIR /setup/paho.mqtt.c
RUN git checkout v1.3.8
RUN cmake -Bbuild -H. -DPAHO_WITH_SSL=ON
RUN cmake --build build/ --target install
RUN ldconfig
WORKDIR /setup
RUN git clone https://github.com/eclipse/paho.mqtt.cpp
WORKDIR /setup/paho.mqtt.cpp
RUN cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE
RUN cmake --build build/ --target install
RUN ldconfig

WORKDIR /

COPY /$0 /panduza-cxx-platform/
COPY /$0/panduza /etc/panduza/
COPY compile.sh compile.sh
RUN chmod +x compile.sh
RUN apt-get install -y kmod

ENTRYPOINT ["/compile.sh"]