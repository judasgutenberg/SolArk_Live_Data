//Gus Mueller, June 29, 2024
//uses an ESP8266 wired with the swap serial pins (D8 as TX and D7 as RX) connected to the exposed serial header on the ESP32 in the SolArk's WiFi dongle.
//this means you won't get to use a serial port to help you debug, but you can still debug by looking at the web output on the web page the ESP8266 serves.
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
  //look for the packet of data containing the hex-encoded values we care about
  char incomingByte = ' ';
  String startValidIndication = "MB_real data,seg_cnt:3\r\r";

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    serialContent += incomingByte;
    if(goodDataMode) {
      if (incomingByte == '\r')
      {
        //dataToDisplay = goodData; //when you want to see the whole packet
        dataToDisplay += "\n" + parseData(goodData); //otherwise send a *-delimited list of values
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

int generateDecimalFromStringPositions(String inData, int start, int stop) {
  String hexValue = removeSpaces(inData.substring(start, stop));
  int out = strtol(hexValue.c_str(), nullptr, 16);
  if (out & 0x8000) {
    // If the MSB is set, adjust the value for two's complement
    out -= 0x10000;
  }
  return out;
}


String parseData(String inData){
  //mysteryValues and changers are values whose meanings I haven't yet determined. i log them on the backend and try to figure them out by context
  int firstChanger = generateDecimalFromStringPositions(inData, 7, 13);
  int secondChanger = generateDecimalFromStringPositions(inData, 25, 31);
  int thirdChanger = generateDecimalFromStringPositions(inData, 67, 73);
  int fourthChanger = generateDecimalFromStringPositions(inData, 75, 81);
  int fifthChanger = generateDecimalFromStringPositions(inData, 91, 97);
  int sixthChanger =  generateDecimalFromStringPositions(inData, 385, 391);
  int seventhChanger =  generateDecimalFromStringPositions(inData, 481, 487);
  int batteryPercent = generateDecimalFromStringPositions(inData, 604, 606);
  int loadPower = generateDecimalFromStringPositions(inData, 607, 613);
  int solarString1  = generateDecimalFromStringPositions(inData, 613, 619);
  int solarString2 = generateDecimalFromStringPositions(inData, 619, 625);
  int mysteryValue1 = generateDecimalFromStringPositions(inData, 625, 631);
  int mysteryValue2 = generateDecimalFromStringPositions(inData, 631, 637);
  int batteryPower = generateDecimalFromStringPositions(inData, 637, 643);
  int gridPower = generateDecimalFromStringPositions(inData, 121, 127);
  int batteryVoltage = generateDecimalFromStringPositions(inData, 595, 601);
  int mysteryValue3 = generateDecimalFromStringPositions(inData, 643, 649);
  //1st is gridPower, 2nd is batteryPercentage, 3rd loadPower, 4th is battery power  (2's complement for negative), 5th and 6th are solar strings
  String out = (String)millis() + "*" + String(gridPower) + "*" + String(batteryPercent) + "*" + String(batteryPower) + "*" + String(loadPower) + "*" + String(solarString1) + "*" + (String)solarString2;
  out += "*" + (String)batteryVoltage;
  out += "*" + (String)mysteryValue3;
  out += "*" + (String)mysteryValue1 + "*" + (String)mysteryValue2;
  out += "*" + (String)firstChanger;
  out += "*" + (String)secondChanger;
  out += "*" + (String)thirdChanger;
  out += "*" + (String)fourthChanger;
  out += "*" + (String)fifthChanger;
  out += "*" + (String)sixthChanger;
  out += "*" + (String)seventhChanger;
  
  //feedbackSerial.println(out);
  return out;
}

