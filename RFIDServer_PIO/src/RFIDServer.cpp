#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string.h>
#include "TotalltNotJonatansMasterPassword.h"
#include "mbedtls/md.h"
#include <SPI.h>
#include <MFRC522.h>

#define DEBUG
#define SS_PIN 21
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

byte previousUID[4] = {0, 0, 0, 0}; // Initialize with an invalid UID

String calculateSHA256FromUID(byte *buffer, byte bufferSize);
bool compareUID(byte *uid1, byte *uid2);
void printHex(byte *buffer, byte bufferSize);
String readRFID();
void connectToWifi();

AsyncWebServer server(80);

boolean exec_ota_flag = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  connectToWifi();
  SPI.begin();          // Init SPI bus
  rfid.PCD_Init();      // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String UIDSHA256 = readRFID();
        request->send(200, "text/plain", UIDSHA256);
  });

  server.begin();
}

void loop() {
  //Do nothing
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


String readRFID() {
  unsigned long startTime = millis();
  byte uniqueID[4] = {};

  while ((uniqueID[0] == 0 || uniqueID[1] == 0 || uniqueID[2] == 0 || uniqueID[3] == 0) && (millis() - startTime < 4800)) {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
      continue;

    } else {
      for (byte i = 0; i < 4; i++) {
        uniqueID[i] = rfid.uid.uidByte[i];
        Serial.print(rfid.uid.uidByte[i], HEX);
      }

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      String SHA256UID = calculateSHA256FromUID(uniqueID, rfid.uid.size);

      #ifdef DEBUG
      Serial.println(SHA256UID);
      #endif  

      return SHA256UID;
    }
  }
  return "-1";
}

String calculateSHA256FromUID(byte *buffer, byte bufferSize) {
  char payload[2 * bufferSize + 1]; // Each byte represented by 2 characters in HEX + null terminator
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
  byte shaResult[32];

  for (byte i = 0; i < bufferSize; i++) {
    sprintf(payload + 2 * i, "%02x", buffer[i]);
  }
  payload[2 * bufferSize] = '\0';

  // Print payload
  #ifdef DEBUG
  Serial.println(payload);
  #endif

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *)payload, strlen(payload));
  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);

  
  String UIDSTRING =  "";
  for (byte i = 0; i < sizeof(shaResult); i++) {
    char str[3];
    #ifdef DEBUG
    sprintf(str, "%02x", (int)shaResult[i]);
    Serial.print(str);
    #endif
    UIDSTRING += str;
  }
  #ifdef DEBUG
  Serial.println();
  Serial.println(UIDSTRING);
  Serial.println();
  #endif
  return UIDSTRING;
}
