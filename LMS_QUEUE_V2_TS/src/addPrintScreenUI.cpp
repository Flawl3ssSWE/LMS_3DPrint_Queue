#include "addPrintScreenUI.h"
#include "ui/ui.h"
#include "Arduino.h"
#include "sqlFunctions.h"
#include "queueHandler.h"
#include "readRFID.h"

userData userToAddToQueue;
int minutesArc, hoursArc;
String SHA256UIDtoQueue;
bool hasScannedCard = false;

void scanCardButtonAction(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * target = lv_event_get_target(e);

    userToAddToQueue.Name = "ERROR";
    userToAddToQueue.PhoneNumber = "ERROR";
    userToAddToQueue.uniqueSHA256ID = "ERROR";
    userToAddToQueue.Role = "ERROR";

    SHA256UIDtoQueue = "-1";

	if(event_code == LV_EVENT_CLICKED) {
    //_ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Scanning...");
    SHA256UIDtoQueue = requestRFID();
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
      if (userToAddToQueue.Name == "No user found") {
        _ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "No user found, try again.");
        return;
      }
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