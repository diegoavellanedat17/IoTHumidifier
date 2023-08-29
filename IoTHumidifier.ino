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
#define WATER_LEVEL 4
#define DELAYVAL 50


const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *topic = "/mist";
int waterLevelValue = 0;
MQTTClient client;
WiFiClient net;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int freq = 108000; // this frequency depends on piezoelectric resonance
const int ledChannel = 0;
const int resolution = 8;

const struct ColorDefinition {
  String name;
  uint32_t rgbValue;
} colorDefinitions[] = {
  {"red", pixels.Color(255, 0, 0)},
  {"green", pixels.Color(0, 255, 0)},
  {"blue", pixels.Color(0, 0, 255)},
  {"yellow", pixels.Color(255, 255, 0)},
  {"purple", pixels.Color(128, 0, 128)},
  {"orange", pixels.Color(255, 165, 0)},
  {"pink", pixels.Color(255, 105, 180)},
  {"cyan", pixels.Color(0, 255, 255)},
  {"white", pixels.Color(255, 255, 255)},
  {"off", pixels.Color(0, 0, 0)},  // Off (black) color
};

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

void colorWipeByName(const String& colorName, int wait) {
  for (int i = 0; i < sizeof(colorDefinitions) / sizeof(colorDefinitions[0]); i++) {
    if (colorName.equalsIgnoreCase(colorDefinitions[i].name)) {
      colorWipe(colorDefinitions[i].rgbValue, wait);
      return;
    }
  }
  Serial.println("Color not found");
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycle(int wait) {
  for (int j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
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
   if(payload == "rainbow"){
     rainbowCycle(10);
   }
   else{
     colorWipeByName(payload,1000);
   }

  }
}

void setup()
{
  pinMode(WATER_LEVEL, INPUT);
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
  waterLevelValue = digitalRead(WATER_LEVEL);   // read the input pin
  Serial.println(waterLevelValue);
  delay(100);
  client.loop();

  if (!client.connected())
  {
    connect();
  }
}