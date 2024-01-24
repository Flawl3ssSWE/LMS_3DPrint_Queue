#include <Arduino.h>


#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include "mbedtls/md.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "TotalltNotJonatansMasterPassword.h"

#define DEBUG
#define SS_PIN 21
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

byte previousUID[4] = {0, 0, 0, 0}; // Initialize with an invalid UID


char INSERT_SQL[1024];
char SELECT_SQL[1024];

IPAddress server_addr(192,168,1,38);  // IP of the MySQL *server* here
char user[] = MySQLUsr;              // MySQL user login username
char password[] = MySQLPsw;        // MySQL user login password
char ssid[] = WiFiSSID;         // your SSID
char pass[] = WiFiPswd;     // your SSID Password

WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;


void setup() {
  Serial.begin(250000);
  SPI.begin();          // Init SPI bus
  rfid.PCD_Init();      // Init MFRC522

  pinMode(27, INPUT_PULLDOWN);

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  connectToWifi();
  connectToMySQL();
}

void loop() {
  Serial.println(digitalRead(27));

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  byte nuidPICC[4];
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  switch mode:
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

void insertIntoMySQL(String SHA256UID){
  char SHA256UIDtoCharArray[64];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 64);
  sprintf(INSERT_SQL, "INSERT INTO printingQueue.queue (Name, PhoneNumber, RFID, Printquota) VALUE ('Test', '07000000', '%s', '100000')", SHA256UIDtoCharArray);
  cursor = new MySQL_Cursor(&conn);
  if (conn.connected()){
      cursor->execute(INSERT_SQL);
      Serial.println("*** INSERTED THIS MATE");
      #ifdef DEBUG
        Serial.println(INSERT_SQL);
        Serial.println(SHA256UID);
      #endif
  }else{
    Serial.println("cannot connect cannot insert wääh");
  }
}

void selectFromMySQL(String SHA256UID){
  char SHA256UIDtoCharArray[64];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 64);
  sprintf(SELECT_SQL, "SELECT Name, PhoneNumber, Printquota FROM printingQueue.queue WHERE RFID = '%s'", SHA256UIDtoCharArray);
  
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Supply the parameter for the query
  // Here we use the QUERY_POP as the format string and query as the
  // destination. This uses twice the memory so another option would be
  // to allocate one buffer for all formatted queries or allocate the
  // memory as needed (just make sure you allocate enough memory and
  // free it when you're done!).

  // Execute the query
  cur_mem->execute(SELECT_SQL);
  // Fetch the columns and print them
  column_names *cols = cur_mem->get_columns();
  for (int f = 0; f < cols->num_fields; f++) {
    Serial.print(cols->fields[f]->name);
    if (f < cols->num_fields-1) {
      Serial.print(',');
    }
  }
  Serial.println();
  // Read the rows and print them
  row_values *row = NULL;
  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
      for (int f = 0; f < cols->num_fields; f++) {
        Serial.print(row->values[f]);
        if (f < cols->num_fields-1) {
          Serial.print(',');
        }
      }
      Serial.println();
    }
  } while (row != NULL);
  // Deleting the cursor also frees up memory used
  delete cur_mem;
}

void connectToMySQL(){
  
  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password)){
    Serial.println("OK.");

      
  }
  else{
    Serial.println("FAILED.");
  }
   
  
}

void connectToWifi(){
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