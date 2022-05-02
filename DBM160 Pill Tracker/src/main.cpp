#include <Arduino.h>

//OOCSI lib
#include <OOCSI.h>
#include <HTTPClient.h>

//name of the current device on the OOCSI network
const char *OOCSIName = "DBM160_PillTracker";
//the address of the OOCSI server here
const char *hostserver = "oocsi.id.tue.nl";
//name of the general oocsi channel
const char *DF_Channel = "allergy_data";

// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "The Donut Wifi";
// Password of your Wifi network.
const char* password = "donutismad";

// OOCSI reference for the entire sketch
OOCSI oocsi = OOCSI();

//declare pins
const int button=2;


void setup() {

  pinMode(button, INPUT_PULLUP);

  //begin serial
  Serial.begin(115200);

  //WIFI Connection
  oocsi.connect(OOCSIName, hostserver, ssid, password);
  Serial.print("Successfully connected to: ");
  Serial.println(ssid);
}

void loop() {

  if(digitalRead(button)==LOW){
    oocsi.newMessage(DF_Channel);
    oocsi.addInt("Pill Dispenser", 1);
    oocsi.sendMessage();

    Serial.println("Detected pill dispenser press");
    delay(250);
  }
  //Serial.println("Waiting...");
  delay(10);

}