//Gus Mueller, June 29, 2024
//uses an ESP8266 wired with the swap serial pins (D8 as TX and D7 as RX) connected to the exposed serial header on the ESP32 in the SolArk's WiFi dongle.
//this intercepts the communication data between the SolArk and the dongle to get frequent updates (that is, every few seconds) of the power and battery levels.
//the data still makes it to PowerView (now MySolArk) but you have access to it much sooner, locally, and at much finer granularity
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "config.h"
String serialContent = "";
String ipAddress;
String goodData;
String dataToDisplay;
ESP8266WebServer server(80); //Server on port 80

bool goodDataMode = false;

void setup() {
  Serial.begin(115200);
  wiFiConnect();
  delay(2000);
  server.on("/", handleRoot);
  server.begin();
  
  
  Serial.println("about to swap");
  delay(4000);
  Serial.swap();

}

void loop() {
  // put your main code here, to run repeatedly:
  // send data only when you receive data:
  char incomingByte = ' ';
  String startValidIndication = "MB_real data,seg_cnt:3\r\r";

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    serialContent += incomingByte;
    if(goodDataMode) {
      if (incomingByte == '\r')
      {
        dataToDisplay = goodData;
        dataToDisplay += "\n" + parseData(goodData);
        goodData = "";
        goodDataMode = false;
      } else {
        goodData += incomingByte;
      } 
    }
    if(endsWith(serialContent, startValidIndication)){ //!goodDataMode && 
      //dataToDisplay += "foundone\n";
      serialContent = "";
      goodDataMode = true;
      
    }
  }
  server.handleClient();

}

void handleRoot() {
 String s = dataToDisplay; //Read HTML contents
 server.send(200, "text/plain", s);//Send web page
}


void wiFiConnect() {
  WiFi.begin(wifi_ssid, wifi_password);     //Connect to your WiFi router
  Serial.println();
  // Wait for connection
  int wiFiSeconds = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    wiFiSeconds++;

  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(wifi_ssid);
  Serial.print("IP address: ");
  ipAddress =  WiFi.localIP().toString();
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

bool endsWith(const String& str1, const String& str2) {
  if (str2.length() > str1.length()) {
    return false;
  }
  return str1.substring(str1.length() - str2.length()) == str2;
}

String removeSpaces(String str) {
  String result = "";
  for (int i = 0; i < str.length(); i++) {
    if (str[i] != ' ') {
      result += str[i];
    }
  }
  return result;
}



String parseData(String inData){
  String hexValue = inData.substring(604, 606);
  int batteryPercent = strtol(hexValue.c_str(), nullptr, 16);
  hexValue = removeSpaces(inData.substring(607, 613));
  int loadPower = strtol(hexValue.c_str(), nullptr, 16);
  hexValue = removeSpaces(inData.substring(613, 619));
  int batteryPower = strtol(hexValue.c_str(), nullptr, 16);
  hexValue = removeSpaces(inData.substring(619, 625));
  int solarPower = strtol(hexValue.c_str(), nullptr, 16);
  hexValue = removeSpaces(inData.substring(625, 631));
  int mysteryValue = strtol(hexValue.c_str(), nullptr, 16);
  hexValue = removeSpaces(inData.substring(631, 637));
  int mysteryValue2 = strtol(hexValue.c_str(), nullptr, 16);
  hexValue = removeSpaces(inData.substring(637, 643));
  int mysteryValue3 = strtol(hexValue.c_str(), nullptr, 16);
  //first is battery percentage, 2nd loadPower, 7th is battery power (have to see what happens when negative), 4th is likely solar
  return String(batteryPercent) + "*" + String(loadPower) + "*" + String(batteryPower) + "*" + String(solarPower) + "*" + mysteryValue + "*" + mysteryValue2 + "*" + mysteryValue3;
  
}

