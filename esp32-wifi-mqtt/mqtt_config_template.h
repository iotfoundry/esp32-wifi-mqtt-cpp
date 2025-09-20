/*
 * MQTT Configuration Template
 *
 * Copy this file to mqtt_config.h and update with your MQTT broker settings.
 * mqtt_config.h is gitignored for security.
 */

#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// MQTT Broker Configuration
const char *MQTT_BROKER = "test.mosquitto.org"; // Public test broker
const int MQTT_PORT = 1883;
const char *MQTT_CLIENT_ID = "ESP32_Client";

// MQTT Authentication (leave empty for anonymous access)
const char *MQTT_USERNAME = "";
const char *MQTT_PASSWORD = "";

// MQTT Topics
const char *MQTT_TOPIC_PREFIX = "esp32/device"; // Will be used as: esp32/device/data, esp32/device/commands, etc.

// MQTT Settings
const int MQTT_QOS = 1;         // Quality of Service level (0, 1, or 2)
const bool MQTT_RETAIN = false; // Retain messages

// Connection settings
const int MQTT_RECONNECT_DELAY = 5000;  // Delay between reconnection attempts (ms)
const int MQTT_KEEPALIVE_INTERVAL = 60; // Keep alive interval (seconds)

#endif
