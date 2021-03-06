#include "PietteTech_DHT/PietteTech_DHT.h"

// DHT parameters
#define DHTTYPE  DHT22  // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   D1  // Digital pin for communications
#define DHT_SAMPLE_INTERVAL   2000  // How often to sample in milliseconds

// Use uFL External Antenna
STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

// Variables
int lastPub;
int pubDelay = 5000;  // How often to publish in milliseconds
int tempf;
int humidity;
int targettemp = 60;
int garageDoorPos = D7;
int garageDoorRelay = D5;
int furnaceRelay = D6;

// garageOpen shows whether or not the garage door is in the open position
int garageOpen;

// furnaceHeat shows whether the furnace is running or not
int furnaceHeat = 0;

// furnaceActive enables the furnace to run
bool furnaceActive = false;

// Post Function Declarations
int garageDoor(String command);
int furnOnOff(String command);
int setFurTemp(String command);

// Declaration
void dht_wrapper();

// Lib Initialize
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

// globals
unsigned int DHTnextSampleTime;  // Next time we want to start sample
bool bDHTstarted;  // flag to indicate we started acquisition
int n;  // counter

void setup() {
  // Hardware pin assignments
  pinMode(garageDoorPos, INPUT);
  pinMode(garageDoorRelay, OUTPUT);
  pinMode(furnaceRelay, OUTPUT);

  // Variables exposed to cloud
  Particle.variable("temperature", tempf);
  Particle.variable("targettemp", targettemp);
  Particle.variable("humidity", humidity);
  Particle.variable("furnaceheat", furnaceHeat);
  Particle.variable("garageopen", garageOpen);

  // Functions exposed to Cloud
  Particle.function("garagedoor", garageDoor);
  Particle.function("furnonoff", furnOnOff);
  Particle.function("setfurtemp", setFurTemp);

  // DHT
  DHTnextSampleTime = 0;

  // Start out with our relays turned on
  digitalWrite(garageDoorRelay, HIGH);
  digitalWrite(furnaceRelay, HIGH);
}

void dht_wrapper() {
  DHT.isrCallback();
}


// START OF LOOP
void loop() {
  // Check if we need to start the next sample
  if (millis() > DHTnextSampleTime) {
    if ( !bDHTstarted ) {  // start the sample
      DHT.acquire();
      bDHTstarted = true;
    }

    if ( !DHT.acquiring() ) {  // has sample completed?
      humidity = DHT.getHumidity();
      tempf = DHT.getFahrenheit();

      n++;  // increment counter
      bDHTstarted = false;  // reset the sample flag so we can take another
      DHTnextSampleTime = millis() + DHT_SAMPLE_INTERVAL;
    }
  }

  if ( (millis() - lastPub) > pubDelay ) {
    // Publish to cloud
    pub();

    // Turn the furnace on or off if necessary
    heatGarage();

    // update lastPub variable
    lastPub = millis();
  }

    // Check if the garage door is open
   if (digitalRead(garageDoorPos) == HIGH) {
       garageOpen = 1;
   } else {
       garageOpen = 0;
   }

}
// END OF LOOP

int pub() {

  // Publish readings to cloud
  Particle.publish("Humidity", String(humidity) + "%");
  Particle.publish("Temperature", String(tempf) + " °F");
}

// POST functions
// These  function automagically get called upon a matching POST request
int garageDoor(String command) {
  if ( command == "activate" ) {
    activateGarageDoor();
    return 1;
  }
  return -1;
}

int furnOnOff(String command) {
  if ( command == "on" ) {
    furnaceOn();
    return 1;
  }
  if ( command == "off" ) {
    furnaceOff();
    return 1;
  }
  return -1;
}
int setFurTemp(String command) {
  if (command.length() > 0) {
    setFurnaceTemp(command);
    return 1;
  } else {
    return -1;
  }
}

// POST helper functions
int activateGarageDoor() {
  digitalWrite(garageDoorRelay, LOW);
  delay(250);
  digitalWrite(garageDoorRelay, HIGH);
  Particle.publish("Garage Door", "Active");
  return 1;
}
int heatGarage() {
  if (furnaceActive == true) {
    if (targettemp >= tempf) {
      digitalWrite(furnaceRelay, LOW);
      furnaceHeat = 1;
      Particle.publish("Furnace", "On");
      return 1;
    } else {
      digitalWrite(furnaceRelay, HIGH);
      furnaceHeat = 0;
      Particle.publish("Furnace", "Off");
      return -1;
    }
  } else {
    digitalWrite(furnaceRelay, HIGH);
    furnaceHeat = 0;
    Particle.publish("Furnace", "Off");
    return -1;
  }
}
int setFurnaceTemp(String command) {
  targettemp = command.toInt();
  return 1;
}
int furnaceOn() {
  furnaceActive = true;
  return 1;
}
int furnaceOff() {
  furnaceActive = false;
  return 1;
}
