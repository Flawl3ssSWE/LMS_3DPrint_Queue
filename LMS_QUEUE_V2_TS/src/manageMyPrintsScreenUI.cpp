#include "manageMyPrintsScreenUI.h"
#include "ui/ui.h"
#include "Arduino.h"
#include "sqlFunctions.h"
#include "queueHandler.h"
#include "readRFID.h"


userData userToManage;
String SHA256UIDtoManage;

void backCancelManageMyPrintsButtonClick(lv_event_t * e)
{
    _ui_label_set_property(ui_mangeMyPrintsStatusLabel, _UI_LABEL_PROPERTY_TEXT, "Please scan your Union Card");
    updateQueueUi();

    userToManage.Name = "ERROR";
    userToManage.PhoneNumber = "ERROR";
    userToManage.uniqueSHA256ID = "ERROR";
    userToManage.Role = "ERROR";
}

void manageMyPrintsScanButtonClick(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * target = lv_event_get_target(e);

    userToManage.Name = "ERROR";
    userToManage.PhoneNumber = "ERROR";
    userToManage.uniqueSHA256ID = "ERROR";
    userToManage.Role = "ERROR";

    SHA256UIDtoManage = "-1";

	if(event_code == LV_EVENT_CLICKED) {
    //_ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Scanning...");
    SHA256UIDtoManage = requestRFID();
    if (SHA256UIDtoManage == "-1" ){return;}
    userToManage = getUserFromSQLite(SHA256UIDtoManage);
    if (userToManage.Name == "No user found") {
      _ui_label_set_property(ui_mangeMyPrintsStatusLabel, _UI_LABEL_PROPERTY_TEXT, "No user found, try again.");
    } else {
      _ui_label_set_property(ui_mangeMyPrintsStatusLabel, _UI_LABEL_PROPERTY_TEXT, ("Hello " + userToManage.Name).c_str());
    } 
    lv_timer_handler();
	}
}

void deleteMyPrintsButtonClick(lv_event_t * e)
{
	bool status = deleteUsersPrintBasedOnSHA256UIDSQLite(userToManage.uniqueSHA256ID);
    if (status) {
        _ui_label_set_property(ui_mangeMyPrintsStatusLabel, _UI_LABEL_PROPERTY_TEXT, "Prints deleted!");
    } else {
        _ui_label_set_property(ui_mangeMyPrintsStatusLabel, _UI_LABEL_PROPERTY_TEXT, "Error!");
    }
}
