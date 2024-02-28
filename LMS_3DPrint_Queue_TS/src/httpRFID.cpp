#include <Arduino.h>
#include <HTTPClient.h>
#include <string.h>

String requestRFIDRemote() {
  HTTPClient http;

  // Start the request
  http.begin("http://192.168.1.27/get");

  // Get the HTTP response code
  int httpCode = http.GET();

  // Response
  if (httpCode > 0) {
    // Get the payload (response body)
    String payload = http.getString();
    return payload;
  } else {
    // HTTP failed
  }

  // Close the connection
  http.end();

  return "-1";
}