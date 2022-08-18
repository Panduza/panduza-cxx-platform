FROM ubuntu:20.04

ENV TZ=Europe/Paris


RUN apt-get install -y locale-gen

RUN echo "LC_ALL=en_US.UTF-8" >> /etc/environment && \
         echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && \
         echo "LANG=en_US.UTF-8" > /etc/locale.conf && \
         locale-gen en_US.UTF-8
         
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install -y build-essential gcc make git cmake libssl-dev wget pkg-config libboost-program-options-dev libjsoncpp-dev libboost-filesystem-dev libboost-system-dev
RUN apt-get install -y python3
RUN apt-get install -y sudo
RUN apt-get install -y kmod

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

COPY ./compile.sh /
RUN chmod +x /compile.sh
WORKDIR /

RUN useradd -m builder
RUN echo "builder:builder" | chpasswd
RUN adduser builder sudo
USER builder

WORKDIR /home/builder
RUN git clone https://github.com/Panduza/panduza-cxx-class-boundary-scan.git
WORKDIR panduza-cxx-class-boundary-scan
RUN git checkout origin/create-first-version-of-boundary-scan-plugin

WORKDIR /
ENTRYPOINT ["./compile.sh"]