// This #include statement was automatically added by the Particle IDE.
#include "PietteTech_DHT/PietteTech_DHT.h"

// DHT parameters
#define DHTTYPE  DHT22              // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   D1         	      // Digital pin for communications
#define DHT_SAMPLE_INTERVAL   2000  // Sample every two seconds

// Use uFL External Antenna
STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

// Variables
int sampInt = 5000; //sample interval in milliseconds
int tempf;
int temperature;
int humidity;
int targettemp = 70;
int blue = D7;
int garageDoorRelay = D5;
int furnaceRelay = D6;

// furnaceHeat shows whether the furnace is running or not and furnaceActive enables the furnace to run
int furnaceHeat = 0;
bool furnaceActive = false;


//Post Function Declarations
int garageDoor(String command);
int furnOnOff(String command);
int setFurTemp(String command);

// Declaration
void dht_wrapper();

// Lib Initialize
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

// globals
unsigned int DHTnextSampleTime; // Next time we want to start sample
bool bDHTstarted;		            // flag to indicate we started acquisition
int n;                          // counter

void setup() {
  //Hardware pin assignments
  pinMode(blue, OUTPUT);
  pinMode(garageDoorRelay, OUTPUT);
  pinMode(furnaceRelay, OUTPUT);

  // Variables Exposed to Cloud
  Particle.variable("temperature", temperature);
  Particle.variable("targettemp", targettemp);
  Particle.variable("humidity", humidity);
  Particle.variable("furnaceheat", furnaceHeat);

  // Functions exposed to Cloud
  Particle.function("garagedoor", garageDoor);
  Particle.function("furnonoff", furnOnOff);
  Particle.function("setfurtemp", setFurTemp);

  //DHT
  DHTnextSampleTime = 0;

  //Start out with our relays turned on
  digitalWrite(garageDoorRelay, HIGH);
  digitalWrite(furnaceRelay, HIGH);
}

void dht_wrapper() {
  DHT.isrCallback();
}

void loop()
{
  //Wait a bit between loops then grab the temperature
  delay(sampInt);
  temperature = tempf;

  // Turn the furnace on or off if necessary
  heatGarage();

  // Check if we need to start the next sample
  if (millis() > DHTnextSampleTime) {
    if (!bDHTstarted) {		// start the sample
      DHT.acquire();
      bDHTstarted = true;
    }

    if (!DHT.acquiring()) {		// has sample completed?

      // get DHT status
      //int result = DHT.getStatus();
      humidity = DHT.getHumidity();
      tempf = DHT.getFahrenheit();
      //int tempc = DHT.getCelsius();
      //int tempk = DHT.getKelvin();
      //int dp = DHT.getDewPoint();
      //int dpslow = DHT.getDewPointSlow();


      //Turn the built in LED on to indicate publishing
      digitalWrite(blue, HIGH);

      //* Publish readings - uncomment/comment the values you'd like to publish

      Particle.publish("Humidity", String(humidity) + "%");
      Particle.publish("Temperature", String(tempf) + " °F");
      //Particle.publish("Temperature", String(tempc) + " °C");
      //Particle.publish("Temperature", String(tempk) + " °K");
      //Particle.publish("Dew Point", String(dp) + " %");
      //Particle.publish("Dew Point", String(dpslow) + " %");

      //Turn the built in LED off indicate publishing finished
      delay(250);
      digitalWrite(blue, LOW);

      n++;  // increment counter
      bDHTstarted = false;  // reset the sample flag so we can take another
      DHTnextSampleTime = millis() + DHT_SAMPLE_INTERVAL;  // set the time for next sample
    }
  }
}

// After Loop - POST functions
// These  function automagically get called upon a matching POST request
int garageDoor(String command)
{
  if(command == "activate")
  {
    activateGarageDoor();
    return 1;
  }
  else return -1;
}
int furnOnOff(String command)
{
  if (command == "on") {
    //furnaceOn();
    return 1;
  }
  if(command == "off") {
    //furnaceOff();
    return 1;
  }
  return -1;
}
int setFurTemp(String command)
{
    if (command == "on") {
        setFurnaceTemp();
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
    return 1;
}
int heatGarage() {
    if (targettemp >= temperature) {
        if (furnaceActive = true) {
            digitalWrite(furnaceRelay, LOW);
            furnaceHeat = 1;
            return 1;
        } else {
            digitalWrite(furnaceRelay, HIGH);
            furnaceHeat = 0;
            return -1;
        }
    } else {
        digitalWrite(furnaceRelay, HIGH);
        furnaceHeat = 0;
        return -1;
    }
}
int setFurnaceTemp() {
    targettemp = 70;
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
