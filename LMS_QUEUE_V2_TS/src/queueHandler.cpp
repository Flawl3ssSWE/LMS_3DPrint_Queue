#include <Arduino.h>
#include <ui/ui.h>
#include "sqlFunctions.h"

void updateQueueUi() {
  if (updateQueueSQLite()) {
    Serial.println("Queue updated");
  } else {
    Serial.println("Queue update failed");
  }
  for (int i = 0; i < 6; i++) {
    printData print = printsInQueue[i];
    if (print.Name != "NULL") {
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
  lv_timer_handler();
}