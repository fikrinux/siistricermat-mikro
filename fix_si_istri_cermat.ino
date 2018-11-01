#include "ACS712.h"
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>

// Set these to run example.
#define FIREBASE_HOST "si-istri-cermat.firebaseio.com"
#define FIREBASE_AUTH "sDkN5QyEl2ph6FRhB1zHLis9GrgB0rBNlZsCSMnI"
#define WIFI_SSID "@wife.id"
#define WIFI_PASSWORD "fikrimam"

/*
  This example shows how to measure the power consumption
  of devices in 230V electrical system
  or any other system with alternative current
*/

// We have 30 amps version sensor connected to A1 pin of arduino
// Replace with your version if necessary
ACS712 sensor(ACS712_30A, A0);

uint32_t durasi;
long waktu_start = 0;
long periode = 0;

void setup() {
  Serial.begin(9600);

 

  // This method calibrates zero point of sensor,
  // It is not necessary, but may positively affect the accuracy
  // Ensure that no current flows through the sensor at this moment
  sensor.calibrate();
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


void loop() {
 
  
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& dataPush = jsonBuffer.createObject();
  JsonObject& dataLog = jsonBuffer.createObject();
 // start_time = 0;
  // We use 230V because it is the common standard in European countries
  // Change to your local, if necessary
  float U = 220;

  // To measure current we need to know the frequency of current
  // By default 50Hz is used, but you can specify own, if necessary
  float I = sensor.getCurrentAC();

  // To calculate the power we need voltage multiplied by current
  float P = U * I; 
  
  delay(5000);
  durasi = millis();
  periode = durasi - waktu_start;
  
//  Firebase.setFloat("hasil/watt", P);
//  Firebase.setFloat("hasil/ampere", I);
//  Firebase.set("hasil/waktu", start_time);


  dataPush["watt"] = P;
  dataPush["ampere"] = I;
  

  dataLog["watt"] = P;
  dataLog["ampere"] = I;
  dataLog["waktu"] = periode;

  Firebase.set("hasil", dataPush);
  Firebase.push("log", dataLog);
  
  Serial.print(String("P = ") + P + " Watt | ");
  Serial.print(String("I = ") + I + " A | ");
  Serial.println(String("Waktu = ") + periode + " ms");
  //dataPush.printTo(Serial);

  waktu_start = durasi;
}
