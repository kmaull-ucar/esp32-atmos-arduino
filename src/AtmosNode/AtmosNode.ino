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

#include "Seeed_BME280.h"
#include "EspMQTTClient.h" //  https://github.com/plapointe6/EspMQTTClient
// #include <Time.h>

BME280 sensor_bme280;
const char* device	= "m5stickc/esp32/aaffcc00";
const char* sensor	= "grove/bme280";
const char* topic		= "measurements";

EspMQTTClient client(
  // "SM-G930P054",
  // "1034396495",
	"godelnet",
	"escherbach",
  "pt2050dashboard-dev.tacc.utexas.edu",  // MQTT Broker server ip
  // "MQTTUsername",   // Can be omitted if not needed
  // "MQTTPassword",   // Can be omitted if not needed
  "TestClient"      // Client name that uniquely identify your device
);
int interval = 10000; // delay

void setup() {
	Serial.begin(9600);
	if (!sensor_bme280.init()) {
			Serial.println("[error]: Device initialization failed.");
	} else {
		Serial.println("[info]: Device initialization OK");
	}
}

void onConnectionEstablished() {
	// https://github.com/plapointe6/EspMQTTClient/issues/33
	// IMPORTANT, protect from spawning multiple delayed instructions when the connection is lost
 client.executeDelayed(interval, measure);
}

void measure() {
	// https://github.com/plapointe6/EspMQTTClient/issues/33
	// IMPORTANT, protect from spawning multiple delayed instructions when the connection is lost
	if(!client.isConnected())
	 return;

	char data[128] = {0};
	float humidity;
	float temperature;
	float pressure;
	float altitude;

	humidity = sensor_bme280.getHumidity();
	temperature = sensor_bme280.getTemperature();
	pressure = sensor_bme280.getPressure();
	altitude = sensor_bme280.calcAltitude(pressure);


	Serial.printf("[info]: publishing = ");

	sprintf(data,
		"device: %s\nsensor: %s/humidity\nm: %f\nt: --\n",
		 device, sensor, humidity);
	client.publish(topic, data);

	sprintf(data,
		"device: %s\nsensor: %s/temperature\nm: %f\nt: --\n",
		 device, sensor, temperature);
	client.publish(topic, data);

	sprintf(data,
		"device: %s\nsensor: %s/pressure\nm: %f\nt: --\n",
		 device, sensor, pressure);
	client.publish(topic, data);

	sprintf(data,
		"device: %s\nsensor: %s/altitude\nm: %f\nt: --\n",
			 device, sensor, altitude);
  client.publish(topic, data);

	// publish went OK
	Serial.print(" OK ");
	Serial.println("");

  // wait 10s
  client.executeDelayed(interval, measure);
}

void loop() {
	client.loop();
}
