#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string.h>
#include "TotalltNotJonatansMasterPassword.h"

void connectToWifi();

AsyncWebServer server(80);

int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  connectToWifi();

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        counter++;
        String counterSTR = String(counter);
        request->send(200, "text/plain", "Hello, GET, Counter: " + counterSTR);
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void connectToWifi() {
  WiFi.begin(WiFiSSID, WiFiPswd);
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

