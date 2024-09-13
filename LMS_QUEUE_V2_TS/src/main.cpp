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
#include "connectWIFI.h"
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

sqlite3 *printqueDB;

void setup() {
  #ifdef ARDUINO_USB_CDC_ON_BOOT
    delay(5000);
  #endif
  Serial.begin(250000);

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
      1 /* Core where the task should run */
  ); 

  lv_timer_handler();

  _ui_label_set_property(ui_queuespot1, _UI_LABEL_PROPERTY_TEXT, "Created by");
  _ui_label_set_property(ui_queuespot2, _UI_LABEL_PROPERTY_TEXT, "Anton \"Krets\" Lundh");
  _ui_label_set_property(ui_queuespot3, _UI_LABEL_PROPERTY_TEXT, "and");
  _ui_label_set_property(ui_queuespot4, _UI_LABEL_PROPERTY_TEXT, "Jonatan \"Taktik\" Svensson");
  _ui_label_set_property(ui_queuespot5, _UI_LABEL_PROPERTY_TEXT, "Sticky4President");
  _ui_label_set_property(ui_queuespot6, _UI_LABEL_PROPERTY_TEXT, "Styrelsen 21/22");
 
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

// Code to run the WiFi connection on the second core
void Task1code(void * pvParameters) {
   while(1) {
    delay(1000);
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Missing WiFi connection, trying to reconnect: ");
         connectToWifi();
      }
    }
}

