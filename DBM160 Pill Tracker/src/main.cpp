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
int boxButton=GPIO_NUM_33;
int boxButtonState = 0, boxButtonStatePrev=0;


void setup() {

  pinMode(boxButton, INPUT);

  //begin serial
  Serial.begin(115200);

  //Run after waking up from sleep
  Serial.println("I am awake");
  boxButtonState=1;

  //serial feedback
  Serial.println("box is open");

  //send OOCSI message
  oocsi.newMessage(DF_Channel);
  oocsi.addInt("Box", 1);
  oocsi.sendMessage();
  delay(500);



  //WIFI Connection
  oocsi.connect(OOCSIName, hostserver, ssid, password);
  Serial.print("Successfully connected to: ");
  Serial.println(ssid);
}

void loop() {

  boxButtonStatePrev=boxButtonState;
  if (digitalRead(boxButton)==HIGH)
  boxButtonState=1; //open
  else
  boxButtonState=0; //close

  if (boxButtonState!=boxButtonStatePrev)
{
  if (boxButtonState)
  {
    //serial feedback
    Serial.println("box is open");

    //send OOCSI message
    oocsi.newMessage(DF_Channel);
    oocsi.addInt("Box", 1);
    oocsi.sendMessage();
  
  } else
  {
    //serial feedback
    Serial.println("box is closed");

    //send OOCSI message
    oocsi.newMessage(DF_Channel);
    oocsi.addInt("Box", 0);
    oocsi.sendMessage();

    //go to sleep
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1);
    Serial.println("I am going to sleep");
    esp_deep_sleep_start();

  }
  delay(10);

}
}