#pragma once
#ifndef IOTWX_CONFIG_H
#define IOTWX_CONFIG_H

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWD "YOUR_PASS"
#define MQTT_SERVER "YOUR_MQTT"
#define MQTT_PORT 1883

#define MEASUREMENT_INTERVAL 10000 // default 60s
#define TIMEZONE 21600 // GMT_n * 3600 where is is your GMT+n, GMT_n

#define DEVICE_ID "<mfg>/<chipset>/<uid>" 	// m5stackatom/esp32/aaffbb01
#define SENSOR "<interface>/<bus>/<sensor>" 	// grove/i2c/bme680
#define TOPIC "<root_topic>/<sub_topic>/<uid>"	// measurements/iotwx/aaffbb01

#define BMEX80_IIC_ADDR  uint8_t(0x76)
#endif //IOTWX_CONFIG_H
