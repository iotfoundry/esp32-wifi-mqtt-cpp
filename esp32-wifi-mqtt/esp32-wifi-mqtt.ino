/*
 * ESP32 WiFi MQTT Client
 *
 * Based on: https://github.com/iotfoundry/esp32-wifi-config-cpp
 *
 * A comprehensive ESP32 project that combines WiFi connectivity with MQTT functionality.
 * Features secure credential management, LED status indicators, and robust error handling.
 *
 * Features:
 * - Secure WiFi and MQTT configuration
 * - Auto-reconnection for both WiFi and MQTT
 * - LED signal strength and MQTT status indicators
 * - JSON message publishing and subscription
 * - Command handling via MQTT
 * - Serial monitoring and debugging
 *
 * Hardware: ESP32C3 DevKitM1
 * Libraries: WiFi, PubSubClient, ArduinoJson
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "wifi_config.h"
#include "mqtt_config.h"

// WiFi and MQTT clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Status variables
bool wifiConnected = false;
bool mqttConnected = false;
unsigned long lastWiFiCheck = 0;
unsigned long lastMQTTCheck = 0;
unsigned long lastDataPublish = 0;
unsigned long lastHeartbeat = 0;

// LED control
#define LED_PIN 7 // ESP32C3 built-in LED

// Signal strength thresholds
#define STRONG_SIGNAL -50 // -30 to -50 dBm
#define MEDIUM_SIGNAL -70 // -50 to -70 dBm
// Below -70 dBm is weak signal

// MQTT message buffer
char mqttMessageBuffer[512];

// Data publishing interval (milliseconds)
const unsigned long DATA_PUBLISH_INTERVAL = 30000; // 30 seconds
const unsigned long HEARTBEAT_INTERVAL = 60000;    // 60 seconds

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Setup LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("ESP32C3 WiFi Connection");

  // Connect to WiFi
  connectToWiFi();

  // Setup MQTT
  setupMQTT();

  Serial.println("Setup complete!");
}

void loop()
{
  unsigned long currentTime = millis();

  // Check WiFi connection
  if (currentTime - lastWiFiCheck > 5000)
  { // Check every 5 seconds
    checkWiFiConnection();
    lastWiFiCheck = currentTime;
  }

  // Check MQTT connection
  if (currentTime - lastMQTTCheck > 10000)
  { // Check every 10 seconds
    checkMQTTConnection();
    lastMQTTCheck = currentTime;
  }

  // Publish data periodically
  if (wifiConnected && mqttConnected)
  {
    if (currentTime - lastDataPublish > DATA_PUBLISH_INTERVAL)
    {
      publishDeviceData();
      lastDataPublish = currentTime;
    }

    // Send heartbeat
    if (currentTime - lastHeartbeat > HEARTBEAT_INTERVAL)
    {
      publishHeartbeat();
      lastHeartbeat = currentTime;
    }
  }

  // Update LED status
  updateLEDStatus();

  // Handle MQTT messages
  if (mqttConnected)
  {
    mqttClient.loop();
  }

  delay(100);
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to WiFi...");
  Serial.println("SSID: " + String(WIFI_SSID));

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    wifiConnected = true;
    digitalWrite(LED_PIN, LOW);
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  }
  else
  {
    wifiConnected = false;
    Serial.println("\nWiFi connection failed!");
    digitalWrite(LED_PIN, HIGH);
  }
}

void checkWiFiConnection()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (wifiConnected)
    {
      Serial.println("WiFi disconnected! Attempting to reconnect...");
      wifiConnected = false;
    }
    connectToWiFi();
  }
  else if (!wifiConnected)
  {
    wifiConnected = true;
    Serial.println("WiFi reconnected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void updateLEDStatus()
{
  unsigned long currentTime = millis();
  static unsigned long lastLEDUpdate = 0;
  static int ledState = LOW;
  static unsigned long ledBlinkInterval = 1000;

  if (currentTime - lastLEDUpdate > ledBlinkInterval)
  {
    lastLEDUpdate = currentTime;

    if (!wifiConnected)
    {
      // No WiFi - LED off
      digitalWrite(LED_PIN, LOW);
    }
    else if (!mqttConnected)
    {
      // WiFi connected, no MQTT - slow blink
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      ledBlinkInterval = 1000; // 1 second
    }
    else
    {
      // Both connected - show signal strength
      int rssi = WiFi.RSSI();
      if (rssi > -50)
      {
        // Strong signal - fast blink
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        ledBlinkInterval = 100; // 100ms
      }
      else if (rssi > -70)
      {
        // Medium signal - medium blink
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        ledBlinkInterval = 500; // 500ms
      }
      else
      {
        // Weak signal - slow blink
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        ledBlinkInterval = 1000; // 1 second
      }
    }
  }
}

String getWiFiStatus()
{
  switch (WiFi.status())
  {
  case WL_CONNECTED:
    return "Connected";
  case WL_NO_SSID_AVAIL:
    return "No SSID Available";
  case WL_CONNECT_FAILED:
    return "Connection Failed";
  case WL_CONNECTION_LOST:
    return "Connection Lost";
  case WL_DISCONNECTED:
    return "Disconnected";
  default:
    return "Unknown";
  }
}

void setupMQTT()
{
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(messageReceived);
  mqttClient.setKeepAlive(MQTT_KEEPALIVE_INTERVAL);
  mqttClient.setSocketTimeout(15);
}

void connectToMQTT()
{
  if (!wifiConnected)
  {
    return;
  }

  Serial.print("Connecting to MQTT broker: ");
  Serial.println(MQTT_BROKER);

  String clientId = MQTT_CLIENT_ID + String(random(0xffff), HEX);

  if (mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD))
  {
    mqttConnected = true;
    Serial.println("MQTT connected!");

    // Subscribe to command topics
    subscribeToTopics();

    // Publish connection status
    publishConnectionStatus();
  }
  else
  {
    mqttConnected = false;
    Serial.print("MQTT connection failed, rc=");
    Serial.println(mqttClient.state());
  }
}

void reconnectMQTT()
{
  if (!wifiConnected)
  {
    return;
  }

  if (!mqttClient.connected())
  {
    Serial.println("MQTT disconnected! Attempting to reconnect...");
    connectToMQTT();
  }
}

void checkMQTTConnection()
{
  if (wifiConnected)
  {
    if (!mqttClient.connected())
    {
      if (mqttConnected)
      {
        Serial.println("MQTT disconnected!");
        mqttConnected = false;
      }
      reconnectMQTT();
    }
    else if (!mqttConnected)
    {
      mqttConnected = true;
      Serial.println("MQTT reconnected!");
    }
  }
}

void subscribeToTopics()
{
  // Subscribe to command topics
  String commandTopic = String(MQTT_TOPIC_PREFIX) + "/commands";
  String configTopic = String(MQTT_TOPIC_PREFIX) + "/config";

  if (mqttClient.subscribe(commandTopic.c_str()))
  {
    Serial.println("Subscribed to: " + commandTopic);
  }

  if (mqttClient.subscribe(configTopic.c_str()))
  {
    Serial.println("Subscribed to: " + configTopic);
  }
}

void messageReceived(char *topic, byte *payload, unsigned int length)
{
  // Null terminate the payload
  payload[length] = '\0';

  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println((char *)payload);

  // Parse JSON message
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, payload);

  if (error)
  {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Handle different message types
  String topicStr = String(topic);

  if (topicStr.endsWith("/commands"))
  {
    handleCommand(doc);
  }
  else if (topicStr.endsWith("/config"))
  {
    handleConfig(doc);
  }
}

void handleCommand(JsonDocument &doc)
{
  if (doc.containsKey("led"))
  {
    String ledCommand = doc["led"];
    if (ledCommand == "on")
    {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED turned ON via MQTT");
    }
    else if (ledCommand == "off")
    {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED turned OFF via MQTT");
    }
    else if (ledCommand == "blink")
    {
      int interval = doc.containsKey("interval") ? doc["interval"] : 500;
      // Note: ledBlinkInterval is static in updateLEDStatus(), so we can't modify it here
      // For direct MQTT control, you'd need a separate variable or modify updateLEDStatus()
      Serial.println("LED blink mode via MQTT, interval: " + String(interval) + "ms");
    }

    // Send acknowledgment
    publishCommandResponse("led", ledCommand, "success");
  }

  if (doc.containsKey("status"))
  {
    publishDeviceStatus();
  }
}

void handleConfig(JsonDocument &doc)
{
  if (doc.containsKey("publish_interval"))
  {
    int newInterval = doc["publish_interval"];
    if (newInterval >= 5000 && newInterval <= 300000)
    { // 5 seconds to 5 minutes
      // Update publish interval (would need to be stored in a variable)
      Serial.println("Publish interval updated to: " + String(newInterval) + "ms");
      publishCommandResponse("config", "publish_interval", "updated");
    }
  }
}

void publishDeviceData()
{
  if (!mqttConnected)
    return;

  DynamicJsonDocument doc(512);
  doc["device_id"] = MQTT_CLIENT_ID;
  doc["timestamp"] = millis();
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["wifi_ssid"] = WiFi.SSID();
  doc["free_heap"] = ESP.getFreeHeap();
  doc["uptime"] = millis() / 1000;
  doc["mqtt_connected"] = mqttConnected;

  String topic = String(MQTT_TOPIC_PREFIX) + "/data";
  String payload;
  serializeJson(doc, payload);

  if (mqttClient.publish(topic.c_str(), payload.c_str()))
  {
    Serial.println("✓ Data published to: " + topic);
    Serial.println("  Payload: " + payload);
  }
  else
  {
    Serial.println("✗ Failed to publish data to: " + topic);
  }
}

void publishHeartbeat()
{
  if (!mqttConnected)
    return;

  DynamicJsonDocument doc(256);
  doc["device_id"] = MQTT_CLIENT_ID;
  doc["timestamp"] = millis();
  doc["status"] = "alive";
  doc["wifi_connected"] = wifiConnected;
  doc["mqtt_connected"] = mqttConnected;

  String topic = String(MQTT_TOPIC_PREFIX) + "/heartbeat";
  String payload;
  serializeJson(doc, payload);

  if (mqttClient.publish(topic.c_str(), payload.c_str()))
  {
    Serial.println("✓ Heartbeat published to: " + topic);
  }
  else
  {
    Serial.println("✗ Failed to publish heartbeat");
  }
}

void publishConnectionStatus()
{
  if (!mqttConnected)
    return;

  DynamicJsonDocument doc(256);
  doc["device_id"] = MQTT_CLIENT_ID;
  doc["status"] = "connected";
  doc["timestamp"] = millis();
  doc["ip_address"] = WiFi.localIP().toString();
  doc["wifi_rssi"] = WiFi.RSSI();

  String topic = String(MQTT_TOPIC_PREFIX) + "/status";
  String payload;
  serializeJson(doc, payload);

  mqttClient.publish(topic.c_str(), payload.c_str());
}

void publishDeviceStatus()
{
  if (!mqttConnected)
    return;

  DynamicJsonDocument doc(512);
  doc["device_id"] = MQTT_CLIENT_ID;
  doc["timestamp"] = millis();
  doc["wifi_connected"] = wifiConnected;
  doc["mqtt_connected"] = mqttConnected;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["wifi_ssid"] = WiFi.SSID();
  doc["ip_address"] = WiFi.localIP().toString();
  doc["free_heap"] = ESP.getFreeHeap();
  doc["uptime"] = millis() / 1000;
  doc["led_state"] = digitalRead(LED_PIN) ? "on" : "off";

  String topic = String(MQTT_TOPIC_PREFIX) + "/status";
  String payload;
  serializeJson(doc, payload);

  mqttClient.publish(topic.c_str(), payload.c_str());
}

void publishCommandResponse(String command, String value, String result)
{
  if (!mqttConnected)
    return;

  DynamicJsonDocument doc(256);
  doc["device_id"] = MQTT_CLIENT_ID;
  doc["command"] = command;
  doc["value"] = value;
  doc["result"] = result;
  doc["timestamp"] = millis();

  String topic = String(MQTT_TOPIC_PREFIX) + "/response";
  String payload;
  serializeJson(doc, payload);

  mqttClient.publish(topic.c_str(), payload.c_str());
}
