# ESP32 WiFi MQTT Client

A comprehensive ESP32 project that combines WiFi connectivity with MQTT functionality, featuring secure credential management, LED status indicators, and robust error handling.

**Based on**: [esp32-wifi-config-cpp](https://github.com/iotfoundry/esp32-wifi-config-cpp)

## 🚀 Features

- **Secure Configuration**: WiFi and MQTT credentials stored in separate files (gitignored)
- **Auto-reconnection**: Automatically reconnects both WiFi and MQTT if connections drop
- **LED Status Indicators**: Built-in LED shows WiFi signal strength and MQTT connection status
- **JSON Messaging**: Publish and subscribe to MQTT topics with JSON payloads
- **Command Handling**: Remote control via MQTT commands (LED control, status requests)
- **Real-time Monitoring**: Serial output for debugging and status monitoring
- **Arduino IDE Compatible**: Easy to use with Arduino IDE

## 📋 Prerequisites

- Arduino IDE installed
- ESP32 board package installed in Arduino IDE
- ESP32C3 development board
- WiFi network with internet access
- MQTT broker (public test broker included)

## 🛠️ Installation

### 1. Install Arduino IDE

Download and install Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Install ESP32 Board Package

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. Add this URL to **Additional Board Manager URLs**:

   ```plaintext
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for "ESP32" and install "ESP32 by Espressif Systems"

### 3. Install Required Libraries

Install these libraries via **Tools** → **Manage Libraries**:

- **PubSubClient** by Nick O'Leary (MQTT client library)
- **ArduinoJson** by Benoit Blanchon (JSON message formatting)

### 4. Open Project in Arduino IDE

1. Open Arduino IDE
2. Go to **File** → **Open**
3. Navigate to this project folder
4. Open `esp32-wifi-mqtt/esp32-wifi-mqtt.ino`

### 5. Configure Board Settings

1. Go to **Tools** → **Board** → **ESP32 Arduino** → **ESP32C3 Dev Module**
2. Set **CPU Frequency** to `80MHz`
3. Set **Flash Mode** to `DIO`
4. Set **Partition Scheme** to `Default 4MB with spiffs`
5. Set **Upload Speed** to `115200`
6. Set **USB CDC On Boot** to `Enabled`

## ⚙️ Configuration

### Quick Setup

Use the provided setup scripts to create configuration files:

**Linux/Mac:**

```bash
cd esp32-wifi-mqtt
./setup.sh
```

**Windows:**

```cmd
cd esp32-wifi-mqtt
setup.bat
```

### Manual Configuration

### WiFi Configuration

1. Copy the WiFi configuration template:

   ```bash
   cp esp32-wifi-mqtt/wifi_config_template.h esp32-wifi-mqtt/wifi_config.h
   ```

2. Edit `esp32-wifi-mqtt/wifi_config.h` with your WiFi credentials:

   ```cpp
   const char* WIFI_SSID = "YourWiFiNetwork";
   const char* WIFI_PASSWORD = "YourWiFiPassword";
   ```

### MQTT Configuration

1. Copy the MQTT configuration template:

   ```bash
   cp esp32-wifi-mqtt/mqtt_config_template.h esp32-wifi-mqtt/mqtt_config.h
   ```

2. Edit `esp32-wifi-mqtt/mqtt_config.h` with your MQTT broker settings:

   ```cpp
   const char* MQTT_BROKER = "broker.hivemq.com";  // or your MQTT broker
   const int MQTT_PORT = 1883;
   const char* MQTT_CLIENT_ID = "ESP32_Client";
   const char* MQTT_USERNAME = "";  // leave empty for anonymous access
   const char* MQTT_PASSWORD = "";
   const char* MQTT_TOPIC_PREFIX = "esp32/device";
   ```

**Note**: Both `wifi_config.h` and `mqtt_config.h` are gitignored for security.

## 🚀 Usage

### 1. Connect ESP32

- Connect your ESP32C3 to your computer via USB
- Select the correct COM port in Arduino IDE

### 2. Upload Code

- Click the **Upload** button (→) in Arduino IDE
- Wait for upload to complete

### 3. Monitor Serial Output

- Open **Tools** → **Serial Monitor**
- Set baud rate to `115200`
- Watch for connection status and MQTT messages

**Expected output when working correctly:**

```plaintext
ESP32C3 WiFi Connection
Connecting to WiFi...
SSID: RWN
...
WiFi Connected!
IP: 192.168.12.11
RSSI: -51 dBm
Setup complete!
MQTT disconnected! Attempting to reconnect...
Connecting to MQTT broker: test.mosquitto.org
MQTT connected!
Subscribed to: esp32/device/commands
Subscribed to: esp32/device/config
✓ Data published to: esp32/device/data
  Payload: {"device_id":"ESP32_Client","timestamp":30032,"wifi_rssi":-52,"wifi_ssid":"RWN","free_heap":201648,"uptime":30,"mqtt_connected":true}
✓ Heartbeat published to: esp32/device/heartbeat
✓ Data published to: esp32/device/data
  Payload: {"device_id":"ESP32_Client","timestamp":60118,"wifi_rssi":-48,"wifi_ssid":"RWN","free_heap":199964,"uptime":60,"mqtt_connected":true}
```

## 💡 LED Status Indicators

The built-in LED provides visual feedback:

### WiFi Status

- **LED off**: No WiFi connection
- **Slow blink (1000ms)**: WiFi connected, no MQTT
- **Medium blink (500ms)**: WiFi + MQTT connected, weak signal (-70 to -50 dBm)
- **Fast blink (100ms)**: WiFi + MQTT connected, strong signal (above -50 dBm)

### MQTT Status

- **LED off**: No MQTT connection
- **Blinking**: MQTT connected (blink speed indicates signal strength)

## 📡 MQTT Topics

The device publishes to and subscribes to the following topics:

### Published Topics

| Topic | Frequency | Description |
|-------|-----------|-------------|
| `{TOPIC_PREFIX}/data` | Every 30 seconds | Device sensor data and status |
| `{TOPIC_PREFIX}/heartbeat` | Every 60 seconds | Device alive signal |
| `{TOPIC_PREFIX}/status` | On connect + on request | Connection and device status |
| `{TOPIC_PREFIX}/response` | After each command | Command execution responses |

### Subscribed Topics

- `{TOPIC_PREFIX}/commands` - Remote commands (LED control, status requests)
- `{TOPIC_PREFIX}/config` - Configuration updates (publish intervals)

## 🎮 MQTT Commands

Send JSON commands to `{TOPIC_PREFIX}/commands`:

### LED Control

```json
{
  "led": "on"
}
```

```json
{
  "led": "off"
}
```

```json
{
  "led": "blink",
  "interval": 500
}
```

### Status Request

```json
{
  "status": "request"
}
```

## 📊 MQTT Message Reference

### 📈 Device Data (published to `/data` - every 30 seconds)

```json
{
  "device_id": "ESP32_Client",
  "timestamp": 12345678,
  "wifi_rssi": -45,
  "wifi_ssid": "MyWiFi",
  "free_heap": 234567,
  "uptime": 1234,
  "mqtt_connected": true
}
```

**Fields:**

- `device_id`: Device identifier
- `timestamp`: Current time in milliseconds
- `wifi_rssi`: WiFi signal strength (dBm)
- `wifi_ssid`: Connected WiFi network name
- `free_heap`: Available RAM in bytes
- `uptime`: Device uptime in seconds
- `mqtt_connected`: MQTT connection status

### 💓 Heartbeat (published to `/heartbeat` - every 60 seconds)

```json
{
  "device_id": "ESP32_Client",
  "timestamp": 12345678,
  "status": "alive",
  "wifi_connected": true,
  "mqtt_connected": true
}
```

**Fields:**

- `device_id`: Device identifier
- `timestamp`: Current time in milliseconds
- `status`: Always "alive"
- `wifi_connected`: WiFi connection status
- `mqtt_connected`: MQTT connection status

### 🔌 Connection Status (published to `/status` - on MQTT connect)

```json
{
  "device_id": "ESP32_Client",
  "status": "connected",
  "timestamp": 12345678,
  "ip_address": "192.168.1.100",
  "wifi_rssi": -45
}
```

**Fields:**

- `device_id`: Device identifier
- `status`: Always "connected"
- `timestamp`: Current time in milliseconds
- `ip_address`: Device IP address
- `wifi_rssi`: WiFi signal strength (dBm)

### 📊 Device Status (published to `/status` - on request)

```json
{
  "device_id": "ESP32_Client",
  "timestamp": 12345678,
  "wifi_connected": true,
  "mqtt_connected": true,
  "wifi_rssi": -45,
  "wifi_ssid": "MyWiFi",
  "ip_address": "192.168.1.100",
  "free_heap": 234567,
  "uptime": 1234,
  "led_state": "on"
}
```

**Fields:**

- `device_id`: Device identifier
- `timestamp`: Current time in milliseconds
- `wifi_connected`: WiFi connection status
- `mqtt_connected`: MQTT connection status
- `wifi_rssi`: WiFi signal strength (dBm)
- `wifi_ssid`: Connected WiFi network name
- `ip_address`: Device IP address
- `free_heap`: Available RAM in bytes
- `uptime`: Device uptime in seconds
- `led_state`: Current LED state ("on" or "off")

### ✅ Command Response (published to `/response` - after each command)

```json
{
  "device_id": "ESP32_Client",
  "command": "led",
  "value": "on",
  "result": "success",
  "timestamp": 12345678
}
```

**Fields:**

- `device_id`: Device identifier
- `command`: Command that was executed
- `value`: Command value/parameter
- `result`: Execution result ("success" or "error")
- `timestamp`: Current time in milliseconds

## ⏰ Message Frequency Summary

| Message Type | Frequency | Topic | Trigger | Data Size |
|--------------|-----------|-------|---------|-----------|
| **Device Data** | Every 30 seconds | `/data` | Automatic | ~150 bytes |
| **Heartbeat** | Every 60 seconds | `/heartbeat` | Automatic | ~100 bytes |
| **Connection Status** | Once | `/status` | On MQTT connect | ~120 bytes |
| **Device Status** | On request | `/status` | Command triggered | ~200 bytes |
| **Command Response** | Per command | `/response` | After each command | ~100 bytes |

**Total messages per hour:** ~120 messages (2 per minute for data + 1 per minute for heartbeat)

## 🏗️ Project Structure

```plaintext
esp32-wifi-mqtt-cpp/
├── esp32-wifi-mqtt/
│   ├── esp32-wifi-mqtt.ino        # Main Arduino sketch
│   ├── wifi_config_template.h     # WiFi credentials template
│   ├── mqtt_config_template.h     # MQTT configuration template
│   ├── setup.sh                   # Linux/Mac setup script
│   └── setup.bat                  # Windows setup script
├── .gitignore                     # Git ignore rules
├── LICENSE                        # MIT License
└── README.md                      # This file
```

## 🔧 Troubleshooting

### Common Issues

#### 1. Board not found

- Check USB connection
- Install correct ESP32 drivers
- Verify COM port selection

#### 2. Upload fails

- Press and hold BOOT button while uploading
- Check board settings (CPU frequency must be 80MHz for ESP32C3)
- Try different USB cable

#### 3. WiFi connection fails

- Verify credentials in `wifi_config.h`
- Check WiFi network availability
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

#### 4. MQTT connection fails

- Verify MQTT broker settings in `mqtt_config.h`
- Check internet connectivity
- Try different MQTT broker (e.g., test.mosquitto.org)

#### 5. No serial output

- Set **USB CDC On Boot** to `Enabled`
- Check baud rate (115200)
- Try different COM port

#### 6. Compilation errors

- Ensure all required libraries are installed
- Check Arduino IDE version compatibility
- Verify ESP32 board package installation

### Board Settings for ESP32C3

- **Board**: ESP32C3 Dev Module
- **CPU Frequency**: 80MHz
- **Flash Mode**: DIO
- **Flash Size**: 4MB
- **Partition Scheme**: Default 4MB with spiffs
- **Upload Speed**: 115200
- **USB CDC On Boot**: Enabled

## 🔒 Security Notes

- WiFi and MQTT credentials are stored in separate files that are gitignored
- Never commit real credentials to version control
- Use template files for new deployments
- Change default credentials before production use
- Consider using encrypted MQTT connections (MQTTS) for production

## 📚 MQTT Brokers for Testing

### Public Test Brokers

- **HiveMQ**: `broker.hivemq.com:1883` (no authentication)
- **Mosquitto**: `test.mosquitto.org:1883` (no authentication)
- **Eclipse**: `mqtt.eclipse.org:1883` (no authentication)

### MQTT Client Tools

- **MQTT Explorer**: GUI client for testing
- **mosquitto_pub/sub**: Command line tools
- **Arduino IDE Serial Monitor**: For basic testing

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Based on [esp32-wifi-config-cpp](https://github.com/iotfoundry/esp32-wifi-config-cpp)
- Uses [PubSubClient](https://github.com/knolleary/pubsubclient) for MQTT
- Uses [ArduinoJson](https://github.com/bblanchon/ArduinoJson) for JSON handling
- ESP32 Arduino Core by Espressif Systems
