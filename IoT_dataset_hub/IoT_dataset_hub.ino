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
const char* ssid = "TMNL-3AC721";
// Password of your Wifi network.
const char* password = "D67R4UWWS4J3PRNW";
const char* device_id = "d0e3c3abbef0f4609";


#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "ESP_p25sensor";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "oocsi.id.tue.nl";



int currentWinState; // current state of door sensor
int lastWinState = LOW;
const int win_SENSOR_PIN  = D5;
long previousMillis = 0;
long interval = 1800000;
OOCSI oocsi = OOCSI();
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
PM25_AQI_Data pm25Data;

float temp = 0.0;
float hum = 0.0;
float oldTemp = 0.0;
float oldHum = 0.0;




void setup() {
  Serial.begin(9600);

  //  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //     Serial.println("Could not find PM 2.5 sensor!");
  //     while (1) delay(10);
  //  }

  //Serial.println("PM25 found!");
  //}

  // Wait one second for sensor to boot up!
  delay(1000);

  dht.begin(); // initialize the sensor
  delay(100);

  pinMode(win_SENSOR_PIN, INPUT_PULLUP);
  currentWinState = digitalRead(win_SENSOR_PIN);

  oocsi.connect(OOCSIName, hostserver, ssid, password);

  delay(500);
  //  oocsi.newMessage("allergy_status");
  //  oocsi.addInt("status", 1);
  //  oocsi.sendMessage();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(t) && !isnan(h)) {
    hum  = h;
    temp = t;
  }
  if ((abs(hum - oldHum) > 0.5) || (abs(temp - oldTemp) > 0.2)) {
    oldHum = hum;
    oldTemp = temp;
    sendDataOverOocsi(&pm25Data, currentWinState, temp, hum, device_id);
  }
Serial.println(h);
Serial.println(t);
  if (! aqi.read(&pm25Data)) {
  // Serial.println("Could not read from AQI");
  delay(500);  // try again in a bit!
    //return;
  }

  currentWinState  = digitalRead(win_SENSOR_PIN); // read new state

  if (currentWinState != lastWinState) {
  lastWinState = currentWinState;
  oocsi.newMessage("allergy_status");
    oocsi.addInt("changed", 1);
    oocsi.sendMessage();
    sendDataOverOocsi(&pm25Data, currentWinState, temp, hum, device_id);
  }

  if (millis() - previousMillis > interval ) {
  sendDataOverOocsi(&pm25Data, currentWinState, temp, hum, device_id);
    previousMillis = millis();
  }
}

void sendDataOverOocsi(PM25_AQI_Data* pmData, int window, float temperature, float humidity, const char* deviceId) {
  oocsi.newMessage("allergy_hub");
  oocsi.addInt("PM_1", pmData->pm10_standard );
  oocsi.addInt("PM_2_5", pmData->pm25_standard );
  oocsi.addInt("PM_10", pmData->pm100_standard );
  oocsi.addInt("PMenv_10", pmData->pm10_env );
  oocsi.addInt("PMenv_2_5", pmData->pm25_env );
  oocsi.addInt("PMenv_10", pmData->pm100_env );
  oocsi.addInt("Particles_higher_10um", pmData->particles_100um);
  oocsi.addInt("Particles_higher_5um", pmData->particles_50um);
  oocsi.addInt("Particles_higher_1um", pmData->particles_10um);
  oocsi.addInt("Particles_higher_2_5um", pmData->particles_25um);
  oocsi.addInt("Particles_higher_0_5um", pmData->particles_05um);
  oocsi.addInt("Particles_higher_0_3um", pmData->particles_03um);
  oocsi.addFloat("Temperature", temperature);
  oocsi.addFloat("Humidity", humidity);
  oocsi.addInt("Window", window);
  oocsi.addString("device_id", deviceId);
  oocsi.sendMessage();
}
