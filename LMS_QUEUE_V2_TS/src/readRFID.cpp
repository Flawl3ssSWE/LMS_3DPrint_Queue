#include "readRFID.h"
#include <Arduino.h>
#include <string.h>
#include <TotalltNotJonatansMasterPassword.h>
//#define DEBUG

#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include "mbedtls/md.h"

PN532_I2C pn532_i2c(Wire1);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
byte uniqueID[4];

void hexStringToArray(String uidString);
String calculateSHA256FromUID(byte *buffer, byte bufferSize);

String requestRFID() {
  
String requestRFID();
String tagId = "None";

  if (nfc.tagPresent()) {
      NfcTag tag = nfc.read();
      tagId = tag.getUidString();

      hexStringToArray(tagId.c_str());
      
      #ifdef DEBUG
        Serial.print("RFID UID: ");
        for (int i = 0; i < 4; i++) {
          Serial.printf("%02x ", uniqueID[i]);
        }
      #endif
  }

  String SHA256UID = calculateSHA256FromUID(uniqueID, sizeof(uniqueID));

  #ifdef DEBUG
    Serial.printf("SHA256: %s\n", SHA256UID.c_str());
  #endif

  return SHA256UID;
}

void hexStringToArray(String uidString) {
  int index = 0;

  for (int i = 0; i < uidString.length() && index < 4; i += 3) {
    String hexPair = uidString.substring(i, i + 2);
    uniqueID[index++] = (byte) strtol(hexPair.c_str(), nullptr, 16);
  }
}


String calculateSHA256FromUID(byte *buffer, byte bufferSize) {
  char payload[2 * bufferSize + 1]; // Each byte represented by 2 characters in HEX + null terminator
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
  byte shaResult[32];

  // Convert buffer to hex string
  for (byte i = 0; i < bufferSize; i++) {
    sprintf(payload + 2 * i, "%02x", buffer[i]);
  }
  payload[2 * bufferSize] = '\0';

  // Create a new string to hold the concatenated payload and salt
  char concatenated[sizeof(payload) + sizeof(SHA256Salt)]; // Make sure this is large enough
  snprintf(concatenated, sizeof(concatenated), "%s%s", payload, SHA256Salt); // Concatenate payload and salt

  // Hash the concatenated string
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *)concatenated, strlen(concatenated));
  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);
  
  // Convert the SHA256 result to a string
  String UIDSTRING = "";
  for (byte i = 0; i < sizeof(shaResult); i++) {
    char str[3];
    sprintf(str, "%02x", (int)shaResult[i]);
    UIDSTRING += str;
  }

  return UIDSTRING;
}
