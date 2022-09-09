# panduza-cxx-platform
Panduza C++ Meta Drivers Manager 

## Installation and starting steps

This is the list of steps to install dependencies and run the platform

1. Install Docker 
    
2. In the case of an example, please refer to the example readme for the setup

3. Start the program:
   ```sh
    ./start.sh
   ```

<!-- If you're following the example stop to the step 2 above.

2. Install mosquitto and add the configuration (for ubuntu)
   ```sh
    sudo apt update
    sudo apt install -y mosquitto
    cd /etc/mosquitto	
    echo 'listener 1883 0.0.0.0'|sudo tee -a mosquitto.conf 
    echo 'listener 9001 0.0.0.0'|sudo tee -a mosquitto.conf 
    echo 'protocol websockets'|sudo tee -a mosquitto.conf 
    sudo systemctl restart mosquitto
   ```

3. Start the program:
   ```sh
    ./start.sh
   ``` -->