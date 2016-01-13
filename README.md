# Garage Automation
A small home automation project, basically a wifi-enabled thermostat and garage door opener.

## Hardware Used

* Particle Photon
* DHT22 Temperature & Humidity Sensor
* 2 channel relay module
* uFL antenna

### DHT22
Configures a DHT22 humidity/temp sensor to work with a Particle Photon and publish readings to the Particle Dashboard.

I've included a copy of the PietteTech_DHT library, feel free to grab a more recent copy and replace what is bundled in if it's outdated.

The connection setup for the DHT22 is as follows...

* PIN 1 = 3.3V or 5V
* PIN 2 = Data (D1 is default for this project)
* PIN 3 = Unused
* PIN 4 = Ground

Between PIN 1 and PIN 2 I'm running a 10K pulldown resistor

Note: I did run into issues when trying to use D0 to connect to the DHT22 even though the library says it's supported.

### 2 Channel Relay Module

*info to be added*

### Remote Antenna

*info to be added*

## Software

The only library that's needed for this project is the PietteTech_DHT library for the DHT22, I've included it in the repository. Other than that everything needed is in the garage.ino file.

## Usage

*info to be added*
