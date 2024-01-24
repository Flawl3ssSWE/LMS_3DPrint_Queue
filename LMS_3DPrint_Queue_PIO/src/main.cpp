#include <Arduino.h>

#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include "mbedtls/md.h"

#include "TotalltNotJonatansMasterPassword.h"
#include "connectWIFIandMySQL.h"
#include "sqlFunctions.h"

#define DEBUG
#define SS_PIN 21
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

byte previousUID[4] = {0, 0, 0, 0}; // Initialize with an invalid UID

String calculateSHA256FromUID(byte *buffer, byte bufferSize);
bool compareUID(byte *uid1, byte *uid2);
void printHex(byte *buffer, byte bufferSize);

void setup() {
  Serial.begin(250000);
  SPI.begin();          // Init SPI bus
  rfid.PCD_Init();      // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  connectToWifi();
  connectToMySQL();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  byte nuidPICC[4];
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  if (!compareUID(nuidPICC, previousUID)) {
    String SHA256UID = calculateSHA256FromUID(nuidPICC, rfid.uid.size);

    #ifdef DEBUG
    Serial.println(SHA256UID);
    #endif

    insertIntoMySQL(SHA256UID);
    memcpy(previousUID, nuidPICC, sizeof(previousUID));
  } else {
    String SHA256UID = calculateSHA256FromUID(nuidPICC, rfid.uid.size);
    Serial.println("Same card as before");
    selectFromMySQL(SHA256UID);
  }
  
  

  

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
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

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
}

bool compareUID(byte *uid1, byte *uid2) {
  return memcmp(uid1, uid2, sizeof(previousUID)) == 0;
}

