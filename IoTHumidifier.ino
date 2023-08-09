#include <WiFi.h>
#include <MQTT.h>
#include "secrets.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif
#define PIN        5 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 8 // Popular NeoPixel ring size
#define MIST 2
#define DELAYVAL 50

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *topic = "/mist";
MQTTClient client;
WiFiClient net;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int freq = 108000; // this frequency depends on piezoelectric resonance
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
  client.setWill("/mist1", "OFF", true, 0);

  while (!client.connect("humidifier1", BROKER_USER, BROKER_PASSWORD))
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected MQTT!");
  client.publish("/mist1", "ON", true, 0);
  client.subscribe(topic);
}

void showRGB(){
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    pixels.setPixelColor(i, pixels.Color(0, 200, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }
  delay(1000);

  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    pixels.setPixelColor(i, pixels.Color(0, 0, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }
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
    showRGB();
  }
}

void setup()
{

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(MIST, ledChannel);
  ledcWrite(ledChannel, 0);
  Serial.begin(115200);
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  pixels.begin(); 
  pixels.clear();

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