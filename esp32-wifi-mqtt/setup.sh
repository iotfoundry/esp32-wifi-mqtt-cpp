#!/bin/bash

# ESP32 WiFi MQTT Client Setup Script
# This script helps set up the configuration files for the ESP32 WiFi MQTT project

echo "ESP32 WiFi MQTT Client Setup"
echo "============================="
echo ""

# Check if we're in the right directory
if [ ! -f "esp32-wifi-mqtt.ino" ]; then
    echo "Error: Please run this script from the esp32-wifi-mqtt directory"
    exit 1
fi

echo "Setting up configuration files..."
echo ""

# Create wifi_config.h if it doesn't exist
if [ ! -f "wifi_config.h" ]; then
    echo "Creating wifi_config.h from template..."
    cp wifi_config_template.h wifi_config.h
    echo "✓ wifi_config.h created"
    echo "  Please edit wifi_config.h with your WiFi credentials"
else
    echo "✓ wifi_config.h already exists"
fi

# Create mqtt_config.h if it doesn't exist
if [ ! -f "mqtt_config.h" ]; then
    echo "Creating mqtt_config.h from template..."
    cp mqtt_config_template.h mqtt_config.h
    echo "✓ mqtt_config.h created"
    echo "  Please edit mqtt_config.h with your MQTT broker settings"
else
    echo "✓ mqtt_config.h already exists"
fi

echo ""
echo "Setup complete!"
echo ""
echo "Next steps:"
echo "1. Edit wifi_config.h with your WiFi credentials"
echo "2. Edit mqtt_config.h with your MQTT broker settings"
echo "3. Open esp32-wifi-mqtt.ino in Arduino IDE"
echo "4. Install required libraries (PubSubClient, ArduinoJson)"
echo "5. Configure board settings for ESP32C3"
echo "6. Upload to your ESP32 device"
echo ""
echo "For detailed instructions, see the README.md file"
