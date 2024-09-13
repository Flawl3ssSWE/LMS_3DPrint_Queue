#include "startScreenUI.h"
#include "ui/ui.h"
#include "Arduino.h"
#include "queueHandler.h"
#include "sqlFunctions.h"

int selectedQueueSpot = 7;

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