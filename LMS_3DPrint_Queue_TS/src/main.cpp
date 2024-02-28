#include <Arduino.h>

#include <SPI.h>
#include <WiFi.h>
#include <esp32_smartdisplay.h>
#include <ui/ui.h>
#include <HTTPClient.h>

#include "TotalltNotJonatansMasterPassword.h"
#include "connectWIFIandMySQL.h"
#include "sqlFunctions.h"

#define DEBUG

String requestRFIDRemote();
bool addUser();

void setup() {
  Serial.begin(250000);
  SPI.begin();          // Init SPI bus

  smartdisplay_init();
  ui_init();

  connectToWifi();
  connectToMySQL();
}

void loop() {
  //userData userAdd;
  //userAdd.Name = "Jonatan";
  //userAdd.PhoneNumber = "0701234567";
  //userAdd.uniqueSHA256ID = readRFID();
  //userAdd.Role = "Admin";
  //addUserIntoMySQL(userAdd);
  //Serial.println(userAdd.uniqueSHA256ID);
  bool UID = addUser();
  Serial.println(UID);
  lv_timer_handler();
  delay(6000);
}

bool addUser() {
  userData userInfo;
  userInfo.uniqueSHA256ID = requestRFIDRemote();
  if (userInfo.uniqueSHA256ID == "-1" ){return false;}
  
  userInfo.Name = "Jonatan";
  userInfo.PhoneNumber = "0701234567";
  userInfo.Role = "Admin";
  addUserIntoMySQL(userInfo);

  return true;
}

String requestRFIDRemote() {
  HTTPClient http;

  // Start the request
  http.begin("http://192.168.1.27/get");

  // Get the HTTP response code
  int httpCode = http.GET();

  // Response
  if (httpCode > 0) {
    // Get the payload (response body)
    String payload = http.getString();
    return payload;
  } else {
    // HTTP failed
  }

  // Close the connection
  http.end();

  return "-1";
}