#include "addPrintScreenUI.h"
#include "ui/ui.h"
#include "Arduino.h"
#include "readRFID.h"
#include "sqlFunctions.h"

String SHA256UIDtoAdd;
bool hasScannedCardAddMemberScreen = false;
userData memberToAdd;
bool memberExists = false;

void memberNameTextAreaClickChangeToNumbers(lv_event_t * e)
{
	lv_keyboard_set_mode(ui_memberScreenKeyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
}

void phoneNumberTextAreaClickChangeToNumbers(lv_event_t * e)
{
	lv_keyboard_set_mode(ui_memberScreenKeyboard, LV_KEYBOARD_MODE_NUMBER);
}

void backButtonClick(lv_event_t * e)
{
    lv_textarea_set_text(ui_memberNameTextArea, "");
    lv_textarea_set_text(ui_phoneNumberTextArea, "");
    lv_label_set_text(ui_scanMemberInfoLabel, "");
    SHA256UIDtoAdd = "";
}

void scanMemberCardButtonClick(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

	if(event_code == LV_EVENT_CLICKED) {
        //_ui_label_set_property(ui_timerAndInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Scanning...");
        SHA256UIDtoAdd = requestRFID();

        if (SHA256UIDtoAdd == "-1" ){return;}
            memberToAdd = getUserFromSQLite(SHA256UIDtoAdd);
        if (memberToAdd.Name != "NULL") { // User exists in database, cancel adding
            memberExists = true;
            _ui_label_set_property(ui_scanMemberInfoLabel, _UI_LABEL_PROPERTY_TEXT, "User already exists in database!");
        } else {
            memberExists = false;
            _ui_label_set_property(ui_scanMemberInfoLabel, _UI_LABEL_PROPERTY_TEXT, "New Member");
        } 
	}

    return;
}

void addMemberButtonClick(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    userData memberToAdd;

    if(event_code == LV_EVENT_CLICKED) {
        memberToAdd.Name = lv_textarea_get_text(ui_memberNameTextArea);
        memberToAdd.PhoneNumber = lv_textarea_get_text(ui_phoneNumberTextArea);
        memberToAdd.uniqueSHA256ID = SHA256UIDtoAdd;
        
        if (SHA256UIDtoAdd == "") {
            _ui_label_set_property(ui_scanMemberInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Please scan a member card!");
            return;
        }

        if (memberExists) {
            _ui_label_set_property(ui_scanMemberInfoLabel, _UI_LABEL_PROPERTY_TEXT, "User already exists! I already told you so!");
            return;
        }

        // Get Admin checkbox role with lv_obj_has_state
        if (lv_obj_has_state(ui_adminCheckBox, LV_STATE_CHECKED)) {
            memberToAdd.Role = "1";
        } else {
            memberToAdd.Role = "0";
        }

        if (memberToAdd.Name == "" || memberToAdd.PhoneNumber == "") {
            _ui_label_set_property(ui_scanMemberInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Please fill in all fields!");
            return;
        }

        addUserIntoSQLite(memberToAdd);
        _ui_label_set_property(ui_scanMemberInfoLabel, _UI_LABEL_PROPERTY_TEXT, "Member added to database!");
    }

    return;
}
