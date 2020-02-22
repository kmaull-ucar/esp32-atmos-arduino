# esp32 IoTwx Atmos Node for Arduino
This repo contains the Arduino reference implementation
for IoTwx ATMOS node.

The reference implementation includes the code that
obtains data from the Grove I2C attached [BME280 environmental sensor](https://www.seeedstudio.com/Grove-BME280-Environmental-Sensor-Temperature-Humidity-Barometer.html)
and over Wifi pushes that data to a [CHORDS portal ](https://www.earthcube.org/group/chords).  
Thus in order to see the fully implemented use case of this
code, you will need to have a CHORDS portal running that
is minimally accessible on the same network as the
running ATMOS node.  

**NOTE:** If you choose not to use the CHORDS portal, you may
listen to the measurements from the MQTT broker and do what
you wish with the data (push to another service, archive, etc.).

To get data to the portal we are using the [MQTT protocol]()
and you will need to install and run the [CHORDS MQTT orchestrator](https://github.com/iotwx/chords-mqtt-orchestrator)
which translates the IoTwx MQTT data and pushes it to CHORDS.
See the instructions on how to setup the portal and the orchestrator.

## Dependencies

### Grove BME280 Arduino library
You will need to first install the [SeeedStudio Grove BME280](https://github.com/Seeed-Studio/Grove_BME280)
Arduino library as it contains the core library for the
main sensor of the node.

To install, you may use the Arduino IDE, or a faster
route would be to clone the repository into
your `Arduino/libraries` directory (most often located
under `~` or `HOME` directory).

The basic steps would be as follows:

1. go into your `~/Arduino/libraries` directory
2. type `git clone https://github.com/Seeed-Studio/Grove_BME280`
3. if successful, you will see the `Grove_BME280` directory in your `~/Arduino/libraries` and you're done with this
step!

### MQTT libraries
Data from the sensors is obtained using MQTT as the transport
mechanism.  You will need to install two MQTT libraries
[PubSubClient](https://github.com/knolleary/pubsubclient) and [EspMQTTClient](https://github.com/plapointe6/EspMQTTC
lient).
The former library provides the base MQTT client and server
code, while the latter provides an wrapper implementation
to the client simplifying the entire connection management
process.  To install, perform the following steps from your command line while in the `Arduino/libraries` directory:

1. `git clone https://github.com/plapointe6/EspMQTTClient.git`
2. `git clone https://github.com/knolleary/pubsubclient.git`

After these two commands you will see the `EspMQTTClient` and
`pubsubclient` directories in your `~Arduino/libraries`
directory.  If you do not, try troubleshooting what may have
gone wrong with your clone.  If that does not work, _you may
download the library ZIP file directly_ from the respective repositories
and unzip each of the contents into the `~Arduino/libraries` directory.

### Arduino-CLI

To efficiently operate the software installation and testing
we encourage the use of the Arduino CLI (Command Line Interface).
It greatly streamlines the operations required to install
and run the software, and while the thought of a command line
tool may be a bit of a leap, especially for new Arduino users,
it is a lot of fun and before long, you'll begin to appreciate
the optimizations this tool has to offer.

Begin by using the installation instructions found at the
[official Arduino-CLI repository](https://github.com/arduino/arduino-cli).

### ESP32 board library

Once you have successfully installed the CLI, you will need
to install the board definitions for the ESP32 chipset which
this node is based on.  The reference implementation is
utilizing the [m5stack Atom Lite](https://m5stack.com/products/atom-lite-esp32-development-kit) microcontroller which
is fully ESP32-based, but you should find the code will
work on any ESP32-based architecture as long as you have
installed and use the correct ESP32 board library for your
microcontroller.  We encourage the use of the m5stack Atom
Lite because of its simplicity and built-in Grove connectors,
making it easy to use and install.

To install the ESP32 board library, follow these instructions for adding the board definition:

1. begin by executing
```sh
$ arduino-cli config init
```
make note of the config file location (often `~/.arduino15/arduino-cli.yaml`

2. open the newly created config file in a text editor.  
You will see the section `board_manager:` and inside that section
add the board definition for the ESP32: `https://dl.espressif.com/dl/package_esp32_index.json`.  The section should
look like:
```yaml
board_manager:
  additional_urls: [
        https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266
.com/stable/package_esp8266com_index.json
]
```
**WARNING:** Do not alter any other aspects of config file, as it may
disturb other settings that may be important to the proper functioning
of the CLI.

3. clone this repo anywhere you'd like on your filesystem (`~/` may be appropriate).

4. compile the code with
```bash
arduino-cli compile -fqbn esp32:esp32:m5stick-c
```
5. upload

6. have fun!
