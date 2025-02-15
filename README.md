# ESP32-C5-WiFi-Scanner

This firmware is a simple wifi scanner built using ESP-IDF, made for the new ESP32 C5 chip which features dual band wifi capabilities. This firmware is deisgned to scan the wireless airspace and display Wi-Fi Ap information on a visual web interface. 

<p align="center">
  <img src="https://github.com/user-attachments/assets/a83a7130-de28-42f8-8629-abf6e2984340" alt="Picture of WebUI" />
</p>


## Requriments 
- ESP-IDF v5.4 or newer
- ESP32 C5

## Build and Flash Instructions 
1. Create new esp-idf project 
2. Select esp32 c5 as target, requires preview verison `idf.py --preview esp32c5`
3. Place firmware files in the `/main` directory of your project. (Replace the CMakeLists file that is auto generated with new one)
4. Build the firmware `idf.py build`
5. Flash Firmware to C5 `idf.py flash` 

Once your ESP32 C5 is flashed and booted, it will host a wifi AP by the name of `ESP32C5_AP` on the 5ghz band, specifically on channel 40. 

By default, the password is is `yourpassword` if you wish to change this, you can modify the AP name and AP password on lines `15` & `16`

Once connected to the ESP32 C5s ap, simply open a web browser and navigate to `192.168.4.1` and click/touch the Wifi Recon, the section will drop down revealing the graph and wifi information, this displayed information is updated every 8 seconds. 
