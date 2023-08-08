#include <WiFi.h>
#include <MQTT.h>
#include "secrets.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
MQTTClient client;
WiFiClient net;
unsigned long lastMillis = 0;
int led = 15;
int mist = 2;

const int freq = 108000;
const int ledChannel = 0;
const int resolution = 8;

void messageReceived(String &topic, String &payload)
{
  digitalWrite(led, HIGH); // Set GPIO22 active high
  Serial.println("incoming: " + topic + " - " + payload);
  if (payload == "ON")
  {
    Serial.println("Mist ON");
    ledcWrite(ledChannel, 125);
  }
  else if (payload == "OFF")
  {
    Serial.println("Mist OFF");
    ledcWrite(ledChannel, 0);
  }
  delay(2000);
  digitalWrite(led, LOW); // Set GPIO22 active high

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup()
{

  ledcSetup(ledChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(mist, ledChannel);
  ledcWrite(ledChannel, 0);
  Serial.begin(115200);
  delay(1000);
  pinMode(led, OUTPUT); // Set GPIO22 as digital output pin
  WiFi.mode(WIFI_STA);  // Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting Wifi ...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  client.begin(BROKER_HOST, BROKER_PORT, net);
  client.onMessage(messageReceived);
  while (!client.connect("esp32", BROKER_USER, BROKER_PASSWORD))
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected MQTT!");
  client.subscribe("/mist");
}

void loop()
{
  // ledcWrite(ledChannel, 0);
  client.loop();

  // publish a message roughly every second.
  if (millis() - lastMillis > 10000)
  {
    lastMillis = millis();
    // client.publish("/hello", "world");
  }
}