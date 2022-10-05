FROM ubuntu:20.04

ENV TZ=Europe/Paris

RUN apt-get update
RUN apt-get install -y locales

RUN echo "LC_ALL=en_US.UTF-8" >> /etc/environment && \
         echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && \
         echo "LANG=en_US.UTF-8" > /etc/locale.conf && \
         locale-gen en_US.UTF-8
         
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install -y build-essential gcc make git cmake libssl-dev wget pkg-config python3 kmod libjsoncpp-dev libboost-filesystem-dev libboost-system-dev

WORKDIR /temp
RUN echo "Install libftdi..."
RUN wget -c https://ftdichip.com/wp-content/uploads/2021/09/libftd2xx-x86_64-1.4.24.tgz
RUN gunzip libftd2xx-x86_64-1.4.24.tgz
RUN tar -xvf libftd2xx-x86_64-1.4.24.tar
RUN cp -r release/build/lib* /usr/local/lib
WORKDIR /usr/local/lib
RUN ln -s libftd2xx.so.1.4.24 libftd2xx.so
RUN chmod 0755 libftd2xx.so.1.4.24

RUN ldconfig

RUN groupadd usb
RUN usermod -a -G usb root

WORKDIR /
RUN git clone https://github.com/Panduza/panduza-cxx-platform.git
WORKDIR /panduza-cxx-platform

COPY ./start-platform.sh /
RUN chmod +x /start-platform.sh

RUN mkdir -p build
WORKDIR /panduza-cxx-platform/build
RUN cmake .. && make install 
RUN mkdir -p /usr/share/panduza-cxx/includes/paho.mqtt.c-src
RUN mkdir -p /usr/share/panduza-cxx/includes/paho.mqtt.cpp-src

RUN cp -R _deps/paho.mqtt.c-src /usr/share/panduza-cxx/includes
RUN cp -R _deps/paho.mqtt.cpp-src /usr/share/panduza-cxx/includes

RUN ldconfig

WORKDIR /home/builder
RUN git clone https://github.com/Panduza/panduza-cxx-class-boundary-scan.git
WORKDIR /home/builder/panduza-cxx-class-boundary-scan

RUN mkdir -p build
WORKDIR /home/builder/panduza-cxx-class-boundary-scan/build
RUN cmake .. && make install

RUN mkdir /etc/panduza
RUN cp /home/builder/panduza-cxx-class-boundary-scan/examples/elsys-board-arty-s7/panduza/tree.json /etc/panduza


WORKDIR /
ENTRYPOINT ["./start-platform.sh"]
