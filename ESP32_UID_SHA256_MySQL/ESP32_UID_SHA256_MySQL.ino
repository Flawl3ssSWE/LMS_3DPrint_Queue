#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include "mbedtls/md.h"

#define SS_PIN 21
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

byte previousUID[4] = {0, 0, 0, 0}; // Initialize with an invalid UID

void setup() {
  Serial.begin(9600);
  SPI.begin();          // Init SPI bus
  rfid.PCD_Init();      // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
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
    calculateAndPrintSHA256(nuidPICC, rfid.uid.size);
    memcpy(previousUID, nuidPICC, sizeof(previousUID));
  } else {
    Serial.println("Same card as before");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void calculateAndPrintSHA256(byte *buffer, byte bufferSize) {
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

  Serial.print("UID: ");
  printHex(buffer, bufferSize);
  Serial.print(" | SHA-256: ");

  for (byte i = 0; i < sizeof(shaResult); i++) {
    char str[3];
    sprintf(str, "%02x", (int)shaResult[i]);
    Serial.print(str);
  }

  Serial.println();
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
