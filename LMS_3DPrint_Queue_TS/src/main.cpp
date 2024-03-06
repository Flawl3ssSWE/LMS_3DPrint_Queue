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
  // Add user
  // userData userAdd;
  // userAdd.Name = "Jonatan";
  // userAdd.PhoneNumber = "0701234567";
  // userAdd.uniqueSHA256ID = requestRFIDRemote();
  // userAdd.Role = "Admin";
  // if (userAdd.uniqueSHA256ID != "-1" ) {
  //   addUserIntoMySQL(userAdd);
  //   Serial.println(userAdd.uniqueSHA256ID);
  //   bool UID = addUser();
  //   Serial.println(UID);
  // }

  // Modify user
  // if (modifyUser()) {
  //   Serial.println("User modified");
  // } else {
  //   Serial.println("User not modified");
  // }

  // Check if admin
  // bool check = checkIfAdmin();
  // if (check) {
  //   Serial.println("Admin");
  // } else {
  //   Serial.println("Not admin");
  // }

  // Delete user
  // if (deleteUser()) {
  //   Serial.println("User deleted");
  // } else {
  //   Serial.println("User not deleted");
  // }

  // Add print to queue
  // if (addPrintToQueueMySQL()) {
  //   Serial.println("Print added");
  // } else {
  //   Serial.println("Print not added");
  // }

  // Get first print from queue
  // printData print = getFirstPrintFromQueueMySQL();
  // Serial.println(print.Name);
  // Serial.println(print.PhoneNumber);
  // Serial.println(print.printWeight);
  // Serial.println(print.printTime);

  // Delete print from queue
  // if (deleteFirstPrintFromQueue()) {
  //   Serial.println("Print deleted");
  // } else {
  //   Serial.println("Print not deleted");
  // }

  // Delete users print from queue
  // if (deleteUsersPrintFromQueue()) {
  //   Serial.println("Print deleted");
  // } else {
  //   Serial.println("Print not deleted");
  // }

  // Delete entire queue
  // if (deleteEntireQueue()) {
  //   Serial.println("Queue deleted");
  // } else {
  //   Serial.println("Queue not deleted");
  // }

  lv_timer_handler();
  delay(10000);
}
