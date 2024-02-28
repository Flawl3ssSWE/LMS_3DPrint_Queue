#include <Arduino.h>

#include <SPI.h>
#include <WiFi.h>
#include <esp32_smartdisplay.h>
#include <ui/ui.h>
#include <HTTPClient.h>

#include "TotalltNotJonatansMasterPassword.h"
#include "connectWIFIandMySQL.h"
#include "sqlFunctions.h"
#include "httpRFID.h"
#include "sqlInteraction.h"

#define DEBUG

void setup() {
  Serial.begin(250000);
  SPI.begin();          // Init SPI bus

  smartdisplay_init();
  ui_init();

  connectToWifi();
  connectToMySQL();
}

void loop() {
  // userData userAdd;
  // userAdd.Name = "Jonatan";
  // userAdd.PhoneNumber = "0701234567";
  // userAdd.uniqueSHA256ID = readRFID();
  // userAdd.Role = "Admin";
  // addUserIntoMySQL(userAdd);
  // Serial.println(userAdd.uniqueSHA256ID);
  // bool UID = addUser();
  // Serial.println(UID);
  
  // bool check = checkIfAdmin();
  // if (check) {
  //   Serial.println("Admin");
  // } else {
  //   Serial.println("Not admin");
  // }
  lv_timer_handler();
  delay(10000);
}
