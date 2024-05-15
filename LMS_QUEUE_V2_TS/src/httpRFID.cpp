#include <Arduino.h>
#include <HTTPClient.h>
#include <string.h>
#define DEBUG
String requestRFIDRemote() {
  HTTPClient http;

  #ifdef DEBUG 
    Serial.println("Sending GET request for RFID");
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
    Serial.print("Device IP: ");
    Serial.println(WiFi.localIP());
  #endif


  // Start the request
  // http.begin("http://192.168.1.13:8000/test.txt");
  http.begin("http://192.168.1.43/get");
  http.setTimeout(30000);
  
  // Get the HTTP response code
  int httpCode = http.GET();

  // Response
  if (httpCode >= 200 && httpCode <= 300) {
    // Get the payload (response body)
    String payload = http.getString();
    #ifdef DEBUG 
      Serial.print("Success with code: ");
      Serial.println(httpCode);
      Serial.print("Payload: ");
      Serial.println(payload);
    #endif
    return payload;
  } else {
    // HTTP failed
    #ifdef DEBUG
      Serial.print("Failed with code: ");
      Serial.println(httpCode);
    // requestRFIDRemote();
    #endif
  }

  // Close the connection
  //http.end();

  return "-1";
}