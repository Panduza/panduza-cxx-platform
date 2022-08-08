# panduza-cxx-platform
Panduza C++ Meta Drivers Manager 

### Installation steps

This is the list of dependencies to install and steps to do it.
1. Install Docker 
    
5. Install mosquitto and add the configuration (for ubuntu)
   ```sh
    sudo apt update
    sudo apt install -y mosquitto
    cd /etc/mosquitto	
    echo 'listener 1883 0.0.0.0'|sudo tee -a mosquitto.conf 
    echo 'listener 9001 0.0.0.0'|sudo tee -a mosquitto.conf 
    echo 'protocol websockets'|sudo tee -a mosquitto.conf 
    sudo systemctl restart mosquitto
   ```

6. Start the program (privilege needed):
   ```sh
    ./start.sh
   ```
