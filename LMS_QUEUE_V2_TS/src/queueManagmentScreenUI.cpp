#include "addPrintScreenUI.h"
#include "ui/ui.h"
#include "Arduino.h"
#include "sqlFunctions.h"

void emptyQueueButtonClick(lv_event_t * e)
{
	// Remove all users from the queue
    bool status = deleteEntireQueueSQLite();
    if (status) {
        _ui_label_set_property(ui_queueManagmentStatusLabel, _UI_LABEL_PROPERTY_TEXT, "Queue is empty");
    } else {
        _ui_label_set_property(ui_queueManagmentStatusLabel, _UI_LABEL_PROPERTY_TEXT, "Error emptying queue");
    }
}

void backCancelButtonQueueManagmentClick(lv_event_t * e)
{
	_ui_label_set_property(ui_queueManagmentStatusLabel, _UI_LABEL_PROPERTY_TEXT, "STATUS LABEL");
}
