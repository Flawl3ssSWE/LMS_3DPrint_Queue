#include <Arduino.h>
#include <ui/ui.h>
#include "queueHandler.h"
#include "sqlFunctions.h"
#include "readRFID.h"

userData scannedCardUser;
String SHA256UIDtoAdmin;

void backCancelAdminButtonClick(lv_event_t * e)
{
	updateQueueUi();

    lv_obj_add_flag(ui_adminScreenAddMemberButton, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_adminScreenQueueManagmentButton, LV_OBJ_FLAG_HIDDEN);
    
    // set label to empty
    _ui_label_set_property(ui_adminScanStatusLabel, _UI_LABEL_PROPERTY_TEXT, "Please Scan Your Union Card");
}

void adminScanButtonClick(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    scannedCardUser.Name = "ERROR";
    scannedCardUser.PhoneNumber = "ERROR";
    scannedCardUser.uniqueSHA256ID = "ERROR";
    scannedCardUser.Role = "ERROR";

    SHA256UIDtoAdmin = "-1";

	if(event_code == LV_EVENT_CLICKED) {
        SHA256UIDtoAdmin = requestRFID();

        if (SHA256UIDtoAdmin == "-1" ) {
            return;
        }
        
        scannedCardUser = getUserFromSQLite(SHA256UIDtoAdmin);

        if (scannedCardUser.Name == "No user found") {
            _ui_label_set_property(ui_adminScanStatusLabel, _UI_LABEL_PROPERTY_TEXT, "No user found, try again.");
            return;
        } else if (scannedCardUser.Role == "1" || scannedCardUser.Role == "2") {
            _ui_label_set_property(ui_adminScanStatusLabel, _UI_LABEL_PROPERTY_TEXT, ("Hello " + scannedCardUser.Name).c_str());
        } else {
            _ui_label_set_property(ui_adminScanStatusLabel, _UI_LABEL_PROPERTY_TEXT, "You are not an admin, this incident has been reported!");
            return;
        }

        lv_obj_clear_flag(ui_adminScreenAddMemberButton, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_adminScreenQueueManagmentButton, LV_OBJ_FLAG_HIDDEN);
    }
}
