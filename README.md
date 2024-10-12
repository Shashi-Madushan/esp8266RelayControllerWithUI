# ESP8266 Relay Control with Web Interface

This project uses an ESP8266 microcontroller to control four relays via a web interface. Each relay can be toggled on or off using the web dashboard, which is accessible through both Access Point (AP) mode and Station mode (Wi-Fi connection). The system also allows configuration of Wi-Fi credentials and IP settings through the web interface.

## Features
- Control 4 relays connected to appliances or devices
- Web-based interface with real-time relay status updates
- Wi-Fi configuration through the web
- Switch between AP mode and Wi-Fi connection
- Stores configuration (Wi-Fi credentials, relay states) in SPIFFS

## Components
- ESP8266
- 4-channel Relay Module
- Various appliances or devices (e.g., lights, fans)

## Pin Configuration

| Relay  | GPIO Pin   | Device      |
|--------|------------|-------------|
| Relay 1| GPIO 5     | Light       |
| Relay 2| GPIO 4     | Fan         |
| Relay 3| GPIO 0     | Heater      |
| Relay 4| GPIO 2     | Pump        |

## Wiring Diagram

![Wiring Diagram](A_diagram_showing_the_connection_of_an_ESP8266_mic.png)

- ESP8266 GPIOs control the relay pins.
- Power connections for ESP8266 and relay module: VCC and GND.
  
## How it Works

1. The ESP8266 first attempts to connect to saved Wi-Fi credentials.
2. If it fails, it starts an AP mode where you can connect and configure Wi-Fi.
3. The web interface allows you to toggle each relay and monitor its status in real-time.
4. You can also configure the network settings from the `/config` page.
5. Relay states and network settings are saved in the SPIFFS filesystem and restored after a reboot.

## Setup Instructions

### 1. Upload Files to SPIFFS
Ensure that the HTML and CSS files are uploaded to the ESP8266's SPIFFS. You can do this via the Arduino IDE using the **ESP8266 Sketch Data Upload** plugin.

### 2. Flash the ESP8266
Upload the Arduino sketch to your ESP8266. After the flash is complete, the device will either connect to Wi-Fi or create an access point.

### 3. Web Interface
Access the web interface by entering the IP address of the ESP8266 in your browser.

### Default AP Mode Credentials:
- **SSID:** ESP8266_AP
- **Password:** ap_password

### Web Interface Preview
![Web UI](./Screenshot%202024-10-12%20140050.png)

## License
This project is open-source and free to use under the MIT License.
