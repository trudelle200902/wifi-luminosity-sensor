# ESP32 Luminosity Sensor for Smart Home

This project implements a luminosity sensor using an ESP32 DevKit. The sensor measures ambient light levels using a photoresistor and sends the data over Wi-Fi, making it suitable for integration into a smart home system. A Python script is also included to demonstrate how to interface with the sensor.

---

## Hardware Components
1. **ESP32 DevKit**
2. **Photoresistor**
3. **10kΩ Resistor**
4. **Connecting Wires**

### Circuit Diagram
- **Photoresistor**: 
  - One leg connected to `5V` of the ESP32.
  - Other leg connected to `Pin 33` of the ESP32.
- **10kΩ Resistor**:
  - Connected between the photoresistor's leg on `Pin 33` and `GND`.

---

## Software Setup

### Prerequisites
- [PlatformIO](https://platformio.org/) installed in your IDE (e.g., VSCode).
- Python installed.

### PlatformIO Setup
1. Clone the repository:
   ```bash
   git clone git@github.com:trudelle200902/wifi-luminosity-sensor.git
   cd wifi-luminosity-sensor
    ```
2. Open the project in ypur PlatformIO IDE.

3. Setup your WIFI credentials
    - Copy wifi_config_template.h in src/
    - Rename copied file to wifi_config.h
    - Enter your SSID and password in the new file
    - VERIFY wifi_config.h IS IN .gitignore IF YOU INTEND TO PUSH

4. First upload

    The code is setup to be uploaded with through wifi with OTA(Over The Air) but for the first upload you will need to do it manually.
    - Comment the upload_protocol and upload_port line in platformio.ini
    - Upload the code in the esp32
    - Take note of the outputed IP address
    
    You can now uncomment the line in plaformio.ini and replace the IP address with the one of your esp32 and you will now be able to upload code to the esp32 without being connected to it

5. Python request example
    
    An example on how to receive the luminosity from the sensor is shown in the python code python_request_example.py. To receive the luminosity a request message must be sent first to the esp32 then it will send back the luminosity level on a scale of 0-4095. The IP address in the python code might need to be changed for your device.
