#include <WiFi.h>
#include <MQTT.h>
#include "secrets.h"

#define RED_LED 5
#define GREEN_LED 15
#define BLUE_LED 4
#define MIST 2

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *topic = "/mist";
MQTTClient client;
WiFiClient net;
unsigned long lastMillis = 0;

const int freq = 108000;
const int ledChannel = 0;
const int resolution = 8;

void connect()
{
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  client.setWill("/mist1/", "OFF", true, 0);

  while (!client.connect("humidifier1", BROKER_USER, BROKER_PASSWORD))
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected MQTT!");
  client.publish("/mist1", "ON", true);
  client.subscribe(topic);
}

void colorSelector(String &payload)
{
  int r_bit = payload[0] - '0';
  int g_bit = payload[0] - '0';
  int b_bit = payload[0] - '0';

  digitalWrite(RED_LED, r_bit);
  digitalWrite(GREEN_LED, g_bit);
  digitalWrite(BLUE_LED, b_bit);
}

void messageReceived(String &topic, String &payload)
{
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
  else
  {
    colorSelector(payload);
  }
}

void setup()
{

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(MIST, ledChannel);
  ledcWrite(ledChannel, 0);
  Serial.begin(115200);
  delay(1000);

  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting Wifi ...");

  client.begin(BROKER_HOST, BROKER_PORT, net);
  client.onMessage(messageReceived);
  connect();
}

void loop()
{
  client.loop();

  if (!client.connected())
  {
    connect();
  }
}