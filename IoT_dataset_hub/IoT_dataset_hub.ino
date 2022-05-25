/*********************************************************************
   Example of the ESP connecting to WiFi and accessing items in an
   Entity dataset on the Data Foundry.
   This works only with an existing account, project and dataset on
   the Data Foundry (https://data.id.tue.nl)
 *********************************************************************/

#include "OOCSI.h"
#include "Adafruit_PM25AQI.h"
//#include "SoftwareSerial.h"
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "Royal Connection";
// Password of your Wifi network.
const char* password = "HertogJan11!";


#include "DHT.h"
#define DHTPIN 2 // could need serial pin
#define DHTTYPE DHT11

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_p25sensordendsensordata";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "data.id.tue.nl";
//SoftwareSerial pmSerial(D1, D2);
// put the adress of Data Foundry here
//const char* datafoundry = "data.id.tue.nl";

// create connection to dataset with server address, dataset id, and the access token
//DFDataset iot(datafoundry, 2111, "d1dzOEliN2hzVTFJbXlCbC9pR2s5bi9YUUNhcU5kTk50TFdlNzNqWjZqWT0=");

int currentwinState; // current state of door sensor
int lastwinState = LOW;
const int win_SENSOR_PIN  = D5;
long previousMillis = 0;
long interval = 1800000;
OOCSI oocsi = OOCSI();
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
// put your setup code here, to run once:
void setup() {
  Serial.begin(9600);
  //pmSerial.begin(9600);
  // There are 3 options for connectivity!
  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
//  if (! aqi.begin_UART(&Serial)) { // connect to the sensor over hardware serial
 // if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
//    //Serial.println("Could not find PM 2.5 sensor!");
   // while (1) delay(10);
  }

  //Serial.println("PM25 found!");
//}

  // Wait one second for sensor to boot up!
  delay(1000);

   dht.begin(); // initialize the sensor
delay(100);
  //Serial.println("Connected to the WiFi network");
  pinMode(win_SENSOR_PIN, INPUT_PULLUP);
  currentwinState = digitalRead(win_SENSOR_PIN);
  // you can also already set the device and activity here,
  // if they don't change throughout the program execution
  //  iot.device("mydevice");
  //  iot.activity("idle");
  oocsi.connect(OOCSIName, hostserver, ssid, password);

  delay(500);
  oocsi.newMessage("allergy_status");
  oocsi.addInt("status", 1);
  oocsi.sendMessage();
}

void loop() {
unsigned long currentMillis = millis();
  // specify activity, can be empty
  bool sent = true;
  PM25_AQI_Data data;

  // read humidity
  float humi  = dht.readHumidity();
  // read temperature as Celsius
  float tempC = dht.readTemperature();
  
  if (! aqi.read(&data)) {
    // Serial.println("Could not read from AQI");
    sent = false;
    delay(500);  // try again in a bit!
    //return;
  }
  
  currentwinState  = digitalRead(win_SENSOR_PIN); // read new state
  if (currentwinState != lastwinState) {
    oocsi.newMessage("allergy_status");
  oocsi.addInt("changed", 1);
  oocsi.sendMessage();
    
    if (currentwinState == HIGH) { // state change: LOW -> HIGH
      oocsi.newMessage("allergy_hub");
      oocsi.addString("window", "window opened");
      oocsi.addInt("PM_1 ", data.pm10_standard );
      oocsi.addInt("PM_2_5 ", data.pm25_standard );
      oocsi.addInt("PM_10 ", data.pm100_standard );
      oocsi.addInt("PMenv_10 ", data.pm10_env );
      oocsi.addInt("PMenv_2_5 ", data.pm25_env );
      oocsi.addInt("PMenv_10 ", data.pm100_env );
      oocsi.addInt("Particles_higher_10um", data.particles_100um);
      oocsi.addInt("Particles_higher_5um", data.particles_50um);
      oocsi.addInt("Particles_higher_1um", data.particles_10um);
      oocsi.addInt("Particles_higher_2_5um", data.particles_25um);
      oocsi.addInt("Particles_higher_0_5um", data.particles_05um);
      oocsi.addInt("Particles_higher_0_3um ", data.particles_03um);
      oocsi.addInt("Temprature", tempC);
      oocsi.addInt("Humidity",humi);
      oocsi.addString("device_id", "d0e3c3abbef0f4609");
      oocsi.sendMessage();
      lastwinState = currentwinState;              // save the last state
    }
    else if (currentwinState == LOW) { // state change: HIGH -> LOW
      oocsi.newMessage("allergy_hub");
      oocsi.addString("window", "window closed");
      oocsi.addInt("PM_1 ", data.pm10_standard );
      oocsi.addInt("PM_2_5 ", data.pm25_standard );
      oocsi.addInt("PM_10 ", data.pm100_standard );
      oocsi.addInt("PMenv_10 ", data.pm10_env );
      oocsi.addInt("PMenv_2_5 ", data.pm25_env );
      oocsi.addInt("PMenv_10 ", data.pm100_env );
      oocsi.addInt("Particles_higher_10um", data.particles_100um);
      oocsi.addInt("Particles_higher_5um", data.particles_50um);
      oocsi.addInt("Particles_higher_1um", data.particles_10um);
      oocsi.addInt("Particles_higher_2_5um", data.particles_25um);
      oocsi.addInt("Particles_higher_0_5um", data.particles_05um);
      oocsi.addInt("Particles_higher_0_3um ", data.particles_03um);
      oocsi.addInt("Temprature", tempC);
      oocsi.addInt("Humidity",humi);
      oocsi.addString("device_id", "d0e3c3abbef0f4609");
      oocsi.sendMessage();
      lastwinState = currentwinState;              // save the last state
    }
  }
  // specify device, can be empty

  
  // fill in some data for the item (very similar to OOCSI)
  if (currentMillis - previousMillis > interval ) {
    oocsi.newMessage("allergy_hub");
    oocsi.addInt("PM_1 ", data.pm10_standard );
    oocsi.addInt("PM_2_5 ", data.pm25_standard );
    oocsi.addInt("PM_10 ", data.pm100_standard );
    oocsi.addInt("PMenv_10 ", data.pm10_env );
    oocsi.addInt("PMenv_2_5 ", data.pm25_env );
    oocsi.addInt("PMenv_10 ", data.pm100_env );
    oocsi.addInt("Particles_higher_10um", data.particles_100um);
    oocsi.addInt("Particles_higher_5um", data.particles_50um);
    oocsi.addInt("Particles_higher_1um", data.particles_10um);
    oocsi.addInt("Particles_higher_2_5um", data.particles_25um);
    oocsi.addInt("Particles_higher_0_5um", data.particles_05um);
    oocsi.addInt("Particles_higher_0_3um ", data.particles_03um);
    oocsi.addInt("Temprature", tempC);
      oocsi.addInt("Humidity",humi);
    if (currentwinState == HIGH) { // state change: LOW -> HIGH
      oocsi.addString("window", "window opened");
    }
    else if (currentwinState == LOW) { // state change: HIGH -> LOW
      oocsi.addString("window", "window closed");
    }
    oocsi.addString("device_id", "d0e3c3abbef0f4609");
    oocsi.sendMessage();
    previousMillis = currentMillis;
  }






  // log the item data to the dataset



}
