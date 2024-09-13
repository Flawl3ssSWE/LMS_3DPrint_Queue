#include <Arduino.h>

#include <SPI.h>
#include <WiFi.h>
#include <esp32_smartdisplay.h>
#include <ui/ui.h>
#include <HTTPClient.h>

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <FS.h>
#include "SD.h"

#include "TotalltNotJonatansMasterPassword.h"
#include "connectWIFIandMySQL.h"
#include "sqlFunctions.h"
#include "httpRFID.h"
#include "lvgl.h"
#include "queueHandler.h"

#include "startScreenUI.h"
#include "addPrintScreenUI.h"

#define DEBUG

TaskHandle_t Task1;
void Task1code(void * pvParameters);
void updateQueueUi();

int selectedQueueSpot = 7;
sqlite3 *printqueDB;

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

  _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, "Created by");
  _ui_label_set_property(ui_queuespot2, _UI_LABEL_PROPERTY_TEXT, "Anton \"Krets\" Lundh");
  _ui_label_set_property(ui_queuespot3, _UI_LABEL_PROPERTY_TEXT, "and");
  _ui_label_set_property(ui_queuespot4, _UI_LABEL_PROPERTY_TEXT, "Jonatan \"Taktik\" Svensson");
  _ui_label_set_property(ui_queuespot5, _UI_LABEL_PROPERTY_TEXT, "Sticky4President");
  _ui_label_set_property(ui_queuespot6, _UI_LABEL_PROPERTY_TEXT, "Styrelsen 21/22");

  // lv_timer_handler();
    char *zErrMsg = 0;
    int rc;
 
    SPI.begin();
    SD.begin();
 
    sqlite3_initialize();
 
     if (openDBSQLite("/sd/LMS_Printque.db", &printqueDB))
         return;
 
    //sqlite3_close(printqueDB);
}

void loop() {
  delay(5);
  lv_timer_handler();
}

// Code to run the WiFi and MySQL connection on the second core
void Task1code(void * pvParameters) {
//   //connectToWifi();
//   //connectToMySQL();
   for (;;) {
    delay(1000);
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Missing WiFi connection, trying to reconnect: ");
         connectToWifi();
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
    userToAddToQueue = getUserFromSQLite(SHA256UIDtoQueue);
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
      addPrintIntoSQLite(userToAddToQueue, "0", printTime, SHA256UIDtoQueue);
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



void updateButtonEventAction(lv_event_t * e) {
	updateQueueUi();
}




void queueSpot1Click(lv_event_t * e) {
  Serial.println("Queue spot 1 clicked");
  
  String queueSpot1Text = lv_label_get_text(ui_queuespot1);
  if (queueSpot1Text[0] == '*') {
    queueSpot1Text = queueSpot1Text.substring(1, queueSpot1Text.length()-1);
    lv_color_t blackColor = lv_color_hex(0x000000);
    lv_obj_set_style_text_color(ui_queuespot1, blackColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    selectedQueueSpot = 7;
  } else {
    lv_color_t new_color = lv_color_hex(0xFF0BB0);
    lv_obj_set_style_text_color(ui_queuespot1, new_color, LV_PART_MAIN | LV_STATE_DEFAULT);
    queueSpot1Text = "*" + queueSpot1Text + "*";
    selectedQueueSpot = 0;
  }
  _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, queueSpot1Text.c_str());

}

void queueSpot1LongClick(lv_event_t * e) {
  Serial.println("Queue spot 1 Long Pressed");
  _ui_label_set_property(ui_queue, _UI_LABEL_PROPERTY_TEXT, "Queue spot 1 Long Pressed");
  lv_color_t new_color = lv_color_hex(0x00FF00); 
  lv_obj_set_style_text_color(ui_queuespot1, new_color, LV_PART_MAIN | LV_STATE_DEFAULT); 
  String label1text = lv_label_get_text(ui_queuespot1);
  if (label1text[0] == '*') {
    label1text = label1text.substring(1, label1text.length()-1);
  }
  _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, label1text.c_str());
}


void printer1Click(lv_event_t * e) {
  String queueSpot1Text = lv_label_get_text(ui_queuespot1);
  String queueSpot2Text = lv_label_get_text(ui_queuespot2);
	// Check which spot has been pressed and perform action

  // If no print is selected
  if (selectedQueueSpot == 7) {
    Serial.println("No print selected");
    String printerSpot1 = lv_label_get_text(ui_printerspot1);
    _ui_label_set_property(ui_printerspot1, _UI_LABEL_PROPERTY_TEXT, "No print selected");
    delay(1000);
    _ui_label_set_property(ui_printerspot1, _UI_LABEL_PROPERTY_TEXT, printerSpot1.c_str());
  } else {
    Serial.println("Print selected");
    _ui_label_set_property(ui_printerspot1, _UI_LABEL_PROPERTY_TEXT, printsInQueue[selectedQueueSpot].Name.c_str());
  
  // Remove formatting from pressed name
    if (selectedQueueSpot == 0 && queueSpot1Text[0] == '*') {
      // Modify the label
      Serial.print(queueSpot1Text);
      Serial.println(": Print 0 add to queue");
      queueSpot1Text = queueSpot1Text.substring(1, queueSpot1Text.length()-1);
      lv_color_t blackColor = lv_color_hex(0x000000);
      lv_obj_set_style_text_color(ui_queuespot1, blackColor, LV_PART_MAIN | LV_STATE_DEFAULT);
      _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, queueSpot1Text.c_str());

      // Modify the SQL database
      // Create request
      Serial.println(printsInQueue[0].Name);
      updatePrintBasedOnID(printsInQueue[0].id, Klumpen);
      for (int i = 0; i < 6; i++) {
        Serial.println(printsInQueue[i].Name);
        Serial.println(printsInQueue[i].id);
      }
    } else if (selectedQueueSpot == 1 && queueSpot2Text[0] == '*') {
      // Modify the label
      Serial.print(queueSpot2Text);
      Serial.println(": Print 1 add to queue");
      queueSpot2Text = queueSpot1Text.substring(1, queueSpot2Text.length()-1);
      lv_color_t blackColor = lv_color_hex(0x000000);
      lv_obj_set_style_text_color(ui_queuespot2, blackColor, LV_PART_MAIN | LV_STATE_DEFAULT);
      _ui_label_set_property(ui_queuespot2, _UI_LABEL_PROPERTY_TEXT, queueSpot2Text.c_str());

      // Modify the SQL database
      // Create request
      Serial.println(printsInQueue[1].Name);
      updatePrintBasedOnID(printsInQueue[1].id, Klumpen);
      for (int i = 0; i < 6; i++) {
        Serial.println(printsInQueue[i].Name);
        Serial.println(printsInQueue[i].id);
      }
    }
    selectedQueueSpot = 7;
  }
}

void printer1LongClick(lv_event_t * e)
{
	// Your code here
}

void queueSpot2Click(lv_event_t * e)
{
	 Serial.println("Queue spot 2 clicked");
  
  String queueSpot2Text = lv_label_get_text(ui_queuespot2);
  if (queueSpot2Text[0] == '*') {
    queueSpot2Text = queueSpot2Text.substring(1, queueSpot2Text.length()-1);
    lv_color_t blackColor = lv_color_hex(0x000000);
    lv_obj_set_style_text_color(ui_queuespot2, blackColor, LV_PART_MAIN | LV_STATE_DEFAULT);
    selectedQueueSpot = 7;
  } else {
    lv_color_t new_color = lv_color_hex(0xFF0BB0); // Red color
    lv_obj_set_style_text_color(ui_queuespot2, new_color, LV_PART_MAIN | LV_STATE_DEFAULT);
    queueSpot2Text = "*" + queueSpot2Text + "*";
    selectedQueueSpot = 1;
  }
  _ui_label_set_property(ui_queuespot2, _UI_LABEL_PROPERTY_TEXT, queueSpot2Text.c_str());

}

void queueSpot2LongClick(lv_event_t * e)
{
	// Your code here
}

void printer2Click(lv_event_t * e)
{
  String queueSpot1Text = lv_label_get_text(ui_queuespot1);
  String queueSpot2Text = lv_label_get_text(ui_queuespot2);
	// Check which spot has been pressed and perform action

  // If no print is selected
  if (selectedQueueSpot == 7) {
    Serial.println("No print selected");
    String printerSpot2 = lv_label_get_text(ui_printerspot2);
    _ui_label_set_property(ui_printerspot2, _UI_LABEL_PROPERTY_TEXT, "No print selected");
    delay(1000);
    _ui_label_set_property(ui_printerspot2, _UI_LABEL_PROPERTY_TEXT, printerSpot2.c_str());
  } else {
    Serial.println("Print selected");
    _ui_label_set_property(ui_printerspot2, _UI_LABEL_PROPERTY_TEXT, printsInQueue[selectedQueueSpot].Name.c_str());
  
  // Remove formatting from pressed name
    if (selectedQueueSpot == 0 && queueSpot1Text[0] == '*') {
      // Modify the label
      Serial.print(queueSpot1Text);
      Serial.println(": Print 0 add to queue");
      queueSpot1Text = queueSpot1Text.substring(1, queueSpot1Text.length()-1);
      lv_color_t blackColor = lv_color_hex(0x000000);
      lv_obj_set_style_text_color(ui_queuespot1, blackColor, LV_PART_MAIN | LV_STATE_DEFAULT);
      _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, queueSpot1Text.c_str());

      // Modify the SQL database
      // Create request
      Serial.println(printsInQueue[0].Name);
      updatePrintBasedOnID(printsInQueue[0].id, Knut);
    } else if (selectedQueueSpot == 1 && queueSpot2Text[0] == '*') {
      // Modify the label
      Serial.print(queueSpot2Text);
      Serial.println(": Print 1 add to queue");
      queueSpot2Text = queueSpot1Text.substring(1, queueSpot2Text.length()-1);
      lv_color_t blackColor = lv_color_hex(0x000000);
      lv_obj_set_style_text_color(ui_queuespot2, blackColor, LV_PART_MAIN | LV_STATE_DEFAULT);
      _ui_label_set_property(ui_queuespot2, _UI_LABEL_PROPERTY_TEXT, queueSpot2Text.c_str());

      // Modify the SQL database
      // Create request
      Serial.println(printsInQueue[1].Name);
      updatePrintBasedOnID(printsInQueue[1].id, Knut);
    }
    selectedQueueSpot = 7;

    #ifdef DEBUG
      for (int i = 0; i < 6; i++) {
          Serial.println(printsInQueue[i].Name);
          Serial.println(printsInQueue[i].id);
      }
    #endif
  }
}

void printer2LongClick(lv_event_t * e)
{
	// Your code here
}

