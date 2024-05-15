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

TaskHandle_t Task1;
void Task1code(void * pvParameters);
void updateQueueUi();

void setup() {
  #ifdef ARDUINO_USB_CDC_ON_BOOT
    delay(5000);
  #endif
  Serial.begin(250000);
  // SPI.begin();          // Init SPI bus

  smartdisplay_init();
  lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROT_90);
  ui_init();


  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      1); /* Core where the task should run */

  lv_timer_handler();
  //connectToWifi();
  lv_timer_handler();
    // connectToMySQL();

  _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, "Anton Lundh");
  _ui_label_set_property(ui_queuespot2, _UI_LABEL_PROPERTY_TEXT, "Jonatan Svensson");
  _ui_label_set_property(ui_queuespot3, _UI_LABEL_PROPERTY_TEXT, "Maxemilian Lundin");
  _ui_label_set_property(ui_queuespot4, _UI_LABEL_PROPERTY_TEXT, "Felix Sjöberg");

  // lv_timer_handler();
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
  delay(5);
  lv_timer_handler();
  // delay(10000);
}

// Code to run the WiFi and MySQL connection on the second core
void Task1code(void * pvParameters) {
  connectToWifi();
  connectToMySQL();


  IPAddress server_addr(192, 168, 1, 21);  // IP of the MySQL *server* here
  char user[] = MySQLUsr;                  // MySQL user login username
  char password[] = MySQLPsw;              // MySQL user login password

  for (;;) {
    delay(100);
    if (conn.connected()) {
      // do something
    } else {
      conn.close();
      Serial.println("Connecting...");
      if (conn.connect(server_addr, 3306, user, password)) {
        delay(500);
        Serial.println("Successful reconnect!");
      } else {
        Serial.println("Cannot reconnect! Drat.");
      }
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Missing WiFi connection, trying to reconnect: ");
        connectToWifi();
      }
    }
  }
}

userData userToAddToQueue;
int minutesArc, hoursArc;
String SHA256UIDtoQueue;
bool hasScannedCard = false;

void scanCardButtonAction(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * target = lv_event_get_target(e);

	if(event_code == LV_EVENT_CLICKED) {
    //_ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Scanning...");
    SHA256UIDtoQueue = requestRFIDRemote();
    if (SHA256UIDtoQueue == "-1" ){return;}
    userToAddToQueue = getUserFromMySQL(SHA256UIDtoQueue);
    if (userToAddToQueue.Name == "No user found") {
      _ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "No user found, try again.");
    } else {
      _ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, ("Hello " + userToAddToQueue.Name).c_str());
    } 
    lv_timer_handler();
	}
}

void arcHoursEventIncrement(lv_event_t * e)
{
	hoursArc = lv_arc_get_value(ui_arcHours);
	char hourArray[3];
	sprintf(hourArray, "%d", hoursArc);
    _ui_label_set_property(ui_labelTimerHours, _UI_LABEL_PROPERTY_TEXT, hourArray);

}

void arcMinutesEventIncrement(lv_event_t * e)
{
	minutesArc = lv_arc_get_value(ui_arcMinutes);
	char minuteArray[3];
	sprintf(minuteArray, "%d", minutesArc);
	_ui_label_set_property(ui_labelTimerMinutes, _UI_LABEL_PROPERTY_TEXT, minuteArray);
}

void addPrintToQueueButton(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * target = lv_event_get_target(e);

  #ifdef DEBUG
    Serial.println("Adding print to queue");
  #endif

  String printTime = String(hoursArc) + ":" + String(minutesArc);

	if(event_code == LV_EVENT_CLICKED) {
    if (SHA256UIDtoQueue != "") {
      addPrintIntoMySQL(userToAddToQueue, "0", printTime, SHA256UIDtoQueue);
      _ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Print added to queue");
    } else {
      _ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "No card scanned, try again.");
    }
  }
  
  userToAddToQueue.Name = "";
  userToAddToQueue.PhoneNumber = "";
  userToAddToQueue.uniqueSHA256ID = "";
  userToAddToQueue.Printquota = "";
  SHA256UIDtoQueue = "";
  //updateQueueUi();
}

void cancelButton(lv_event_t * e)
{
  userToAddToQueue.Name = "";
  userToAddToQueue.PhoneNumber = "";
  userToAddToQueue.uniqueSHA256ID = "";
  userToAddToQueue.Printquota = "";
  _ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Press button above to scan card");
  updateQueueUi();
}

void updateQueueUi() {
  if (updateQueue()) {
    Serial.println("Queue updated");
  } else {
    Serial.println("Queue update failed");
  }
  for (int i = 0; i < 6; i++) {
    printData print = printsInQueue[i];
    if (print.Name != "") {
      switch (i) {
        case 0:
          _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, print.Name.c_str());
          break;
        case 1:
          _ui_label_set_property(ui_queuespot2, _UI_LABEL_PROPERTY_TEXT, print.Name.c_str());
          break;
        case 2:
          _ui_label_set_property(ui_queuespot3, _UI_LABEL_PROPERTY_TEXT, print.Name.c_str());
          break;
        case 3:
          _ui_label_set_property(ui_queuespot4, _UI_LABEL_PROPERTY_TEXT, print.Name.c_str());
          break;
        case 4:
          _ui_label_set_property(ui_queuespot5, _UI_LABEL_PROPERTY_TEXT, print.Name.c_str());
          break;
        case 5:
          _ui_label_set_property(ui_queuespot6, _UI_LABEL_PROPERTY_TEXT, print.Name.c_str());
          break;
        default:
          break;
      }
    }
  }
}