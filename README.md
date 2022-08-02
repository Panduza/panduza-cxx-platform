# panduza-cxx-platform
Panduza C++ Meta Drivers Manager 

### Installation steps

This is the list of dependencies to install and steps to do it.
1. Install these packages
   ```sh
   sudo apt-get install -y build-essential gcc make cmake git libssl-dev
   ```
    
2. Install libftd2xx
   ```sh
    wget -c https://ftdichip.com/wp-content/uploads/2021/09/libftd2xx-x86_64-1.4.24.tgz
    gunzip libftd2xx-x86_64-1.4.24.tgz
    tar -xvf libftd2xx-x86_64-1.4.24.tar
    sudo cp release/build/lib* /usr/local/lib
    cd /usr/local/lib
    sudo ln -s libftd2xx.so.1.4.24 libftd2xx.so
    sudo chmod 0755 libftd2xx.so.1.4.24
   ```

3. Install Paho MQTT C++
   ```sh
    git clone https://github.com/eclipse/paho.mqtt.c.git
    cd paho.mqtt.c
    git checkout v1.3.8
    cmake -Bbuild -H. -DPAHO_WITH_SSL=ON
    sudo cmake --build build/ --target install
    sudo ldconfig
    cd ..
    git clone https://github.com/eclipse/paho.mqtt.cpp
    cd paho.mqtt.cpp
    cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE
    sudo cmake --build build/ --target install
    sudo ldconfig
    cd ..
   ```

4. Install libjsoncpp
   ```sh
   sudo apt-get install -y libjsoncpp-dev
   ```

5. Install mosquitto and add the configuration
   ```sh
    sudo apt update
    sudo apt install -y mosquitto
    cd /etc/mosquitto	
    echo 'listener 1883 0.0.0.0'|sudo tee -a mosquitto.conf 
    echo 'listener 9001 0.0.0.0'|sudo tee -a mosquitto.conf 
    echo 'protocol websockets'|sudo tee -a mosquitto.conf 
    sudo systemctl restart mosquitto
   ```

6. Add user to usb group, blacklist ftdi_sio driver (optionnal, or unload it + usbserial every time you plug the probe)
   ```sh
   groupadd usb
    echo 'SUBSYSTEMS=="usb", ACTION=="add", MODE="0664", GROUP="usb"' >> /etc/udev/rules.d/99-usbftdi.rules
    echo "** Reloading udev rules"
    /etc/init.d/udev reload
    echo 'blacklist ftdi_sio' > /etc/modprobe.d/ftdi.conf
    rmmod ftdi_sio
    sudo usermod -a -G usb $(logname)
   ```

7. Clone the repo, build the project (CMake)

8. Copy config files to /etc (panduza + BoundaryScan)
