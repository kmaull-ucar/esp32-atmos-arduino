/*
    AtmosNode.ino

		Atmospheric measurement node with
	  the Grove bme280.

		Copyright (c) 2020 keith maull
    Website    : iotwx.github.io
    Author     : kmaull
    Create Time:
    Change Log :
*/
#include "Seeed_BME280.h" /// https://github.com/seeedstudio/
#include "seeed_bme680.h" /// https://github.com/seeedstudio/
#include "IoTwx.h"        /// https://github.com/iotwx
#include <MQTT.h>         /// https://github.com/256dpi/arduino-mqtt
#include <NTPClient.h>    /// https://github.com/arduino-libraries/NTPClient
#include <WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>      /// https://github.com/FastLED/FastLED

#define NUM_LEDS 1
#define NEO_PIN 27
#define LED_WARN CRGB::Yellow
#define LED_FAIL CRGB::Red
#define LED_OK CRGB::Green
#define LED_BLACK CRGB::Black
#define LED_WIFI CRGB::Blue
#define LED_MQTT CRGB::Purple
#define LED_SLOW 1
#define LED_MED 3
#define LED_FAST 5

// Define the array of leds
CRGB leds[NUM_LEDS];

MQTTClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiClient net;
Seeed_BME680 bme680(BMEX80_IIC_ADDR);
BME280 sensor_bme280;

unsigned long retryCount = 0;
unsigned long lastMillis = 0;
unsigned long t;
bool bme680_attached = false;
bool bme280_attached = false;


void blink_led(CRGB color, int speed){
  leds[0] = color;
  
  for (int i = 0; i < speed ; i++)
  {
    FastLED.setBrightness(1);
    leds[0] = color;
    FastLED.show();
    delay(125);

    // Now turn the LED off, then pause
    leds[0] = LED_BLACK;
    FastLED.show();
    delay(250);
  }
}


void publish_bme280_measurements() {
  char data[128] = {0};
  
  float humidity;
  float temperature;
  float pressure;
  float altitude;
 
  t = timeClient.getEpochTime(); // sec since 1970

  humidity = sensor_bme280.getHumidity();
  temperature = sensor_bme280.getTemperature();
  pressure = sensor_bme280.getPressure();
  altitude = sensor_bme280.calcAltitude(pressure);

  Serial.print("[info]: publishing = ");
  
  sprintf(data,
    "device: %s\nsensor: %s/humidity\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, humidity, t);
  Serial.print(data);
  client.publish(TOPIC, data);

  sprintf(data,
    "device: %s\nsensor: %s/temperature\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, temperature, t);
  Serial.print(data);
  client.publish(TOPIC, data);

  sprintf(data,
    "device: %s\nsensor: %s/pressure\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, pressure, t);
  Serial.print(data);
  client.publish(TOPIC, data);

  sprintf(data,
    "device: %s\nsensor: %s/altitude\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, altitude, t);
  Serial.print(data);
  client.publish(TOPIC, data);

  // publish went OK
  Serial.print(" OK ");
  Serial.println("");
  
}


void publish_bme680_measurements() {
  char data[128] = {0};
  
  float humidity;
  float temperature;
  float pressure;
  float gas;

  t = timeClient.getEpochTime(); // sec since 1970
  
  if (bme680.read_sensor_data()) {
    blink_led(LED_FAIL, LED_FAST);
    return;
  } else {
    // read the sensor data
    humidity = bme680.sensor_result_value.humidity;
    temperature = bme680.sensor_result_value.temperature;
    pressure = bme680.sensor_result_value.pressure;
    gas = bme680.sensor_result_value.gas / 1000.0;
  
    Serial.print("[info]: publishing = ");
  
    sprintf(data,
      "device: %s\nsensor: %s/humidity\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, humidity, t);
    Serial.print(data);
    client.publish(TOPIC, data);
  
    sprintf(data,
      "device: %s\nsensor: %s/temperature\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, temperature, t);
    Serial.print(data);
    client.publish(TOPIC, data);
  
    sprintf(data,
      "device: %s\nsensor: %s/pressure\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, pressure, t);
    Serial.print(data);
    client.publish(TOPIC, data);
  
    sprintf(data,
      "device: %s\nsensor: %s/gas\nm: %f\nt: %d\n",
       DEVICE_ID, SENSOR, gas, t);
    Serial.print(data);
    client.publish(TOPIC, data);
  
    // publish went OK
    Serial.print(" OK ");
    Serial.println("");
  }
}


void connect() {
  // connect to WiFi
  retryCount = 0; 
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  Serial.print("[info]: checking wifi...");
       
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    blink_led(LED_WIFI, LED_MED);
    delay(7500);
    
    retryCount++;
    if (retryCount > 10) { 
      blink_led(LED_FAIL, LED_MED); blink_led(LED_WARN, LED_MED); blink_led(LED_OK, LED_MED);
      delay(30000);
      ESP.restart();
    }
  }

  // connect to MQTT
  retryCount = 0;
  client.begin(MQTT_SERVER, MQTT_PORT, net);
  Serial.print("\n[info] MQTT connecting...");
  while (!client.connect("arduino", "", "")) {
    Serial.print(".");
    blink_led(LED_MQTT, LED_MED);
    delay(5000);
    
    retryCount++;
    if (retryCount > 10) { 
      blink_led(LED_FAIL, LED_MED); blink_led(LED_WARN, LED_MED); blink_led(LED_OK, LED_MED);
      delay(30000);
      ESP.restart();
    }
  }

  // start NTP client for clock and time
  timeClient.begin();
  timeClient.setTimeOffset(TIMEZONE);

  Serial.println("\n[info]: connection sequence done connecting!");

  blink_led(LED_OK, LED_SLOW);
  
}


void setup() {
  Serial.begin(9600);

  // set up AtomLite LED
  FastLED.addLeds<NEOPIXEL, NEO_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  connect();

  // initialize the I2C bus
  Wire.begin(26, 32, 10000);
  Wire.beginTransmission((uint8_t)BMEX80_IIC_ADDR);
  while (Wire.endTransmission() != 0) {
    Serial.println("[warn]: sensor did not ACK; check your connections.");
    blink_led(LED_FAIL, LED_FAST);
    delay(5000);
  } 

  // initiliaze the BME680 sensor
  if (!bme680.init()) {
    Serial.println("[error]: Device BME680 initialization failed.");    
    blink_led(LED_FAIL, LED_FAST);
  }
  else {
    Serial.println("[info]: Device BME680 initialization OK");
    blink_led(LED_OK, LED_SLOW);
    bme680_attached = true;
  }
  
  if (!sensor_bme280.init()) {
    Serial.println("[error]: Device BME280 initialization failed.");
    blink_led(LED_FAIL, LED_FAST);
  }
  else {
    Serial.println("[info]: Device BME280 initialization OK");
    blink_led(LED_OK, LED_SLOW);
    bme280_attached = true;    
  }
}



void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly MEASUREMENT_INTERVAL seconds
  if (millis() - lastMillis > MEASUREMENT_INTERVAL) {
    Serial.println("[info]: measuring");

    lastMillis = millis();
    if (bme680_attached) publish_bme680_measurements();
    else if (bme280_attached) publish_bme280_measurements();
    blink_led(LED_OK, LED_SLOW);
  }

  if ( millis() > 1000*60*60*4 ) ESP.restart(); // restart every 4 hours
}
