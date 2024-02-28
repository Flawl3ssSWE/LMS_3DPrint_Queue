#include <Arduino.h>

#include <esp32_smartdisplay.h>
#include <ui/ui.h>
#include <Audio.h>
#include <string.h>

#define WIFI_SSID "<your ssid>"
#define WIFI_PASSWORD "<your ap password>"
#define RADIO_URL "http://www.wdr.de/wdrlive/media/einslive.m3u"


static uint8_t counter1 = 0;


void ui_event_Button3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        counter1++;
        //_ui_label_set_property(ui_Label3, _UI_LABEL_PROPERTY_TEXT, (char*)counter1);
        String string1 = "Krets is cute";
        lv_label_set_text_fmt(ui_Label3, "%d", counter1);
            //lv_label_set_text_fmt(ui_TempSetValueDisplay, "%d.%d", ui_set_temp / 10, ui_set_temp - (ui_set_temp / 10 * 10));  //uses integer maths to fake the display of a decimal

    }
}

void setup()
{
    delay(250);
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    log_i("CPU: %s rev%d, CPU Freq: %d Mhz, %d core(s)", ESP.getChipModel(), ESP.getChipRevision(), getCpuFrequencyMhz(), ESP.getChipCores());
    log_i("Free heap: %d bytes", ESP.getFreeHeap());
    log_i("Free PSRAM: %d bytes", ESP.getPsramSize());
    log_i("SDK version: %s", ESP.getSdkVersion());


    smartdisplay_init();

    auto disp = lv_disp_get_default();
    // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
    // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
    // lv_disp_set_rotation(disp, LV_DISP_ROT_270);

    ui_init();
}

ulong next_millis;

void loop()
{
   //counter1++;
    //lv_label_set_text_fmt(ui_Label2, "%d", counter1);

    lv_timer_handler();

        //lv_label_set_text(ui_Label2, "Krets4TheWin");
        //delay(1000);
    
}