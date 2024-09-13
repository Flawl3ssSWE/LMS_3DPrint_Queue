#include <Arduino.h>
#include <WiFi.h>

#include "TotalltNotJonatansMasterPassword.h"

char ssid[] = WiFiSSID;                   // your SSID
char pass[] = WiFiPswd;                   // your SSID Password

WiFiClient client;                        // Define WiFiClient

void connectToWifi() {
  WiFi.begin(ssid, pass);
  int timeout = 40;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());
}
