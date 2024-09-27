#include <Arduino.h>
#include "connectWIFI.h"
#include "sqlFunctions.h"
#include "TotalltNotJonatansMasterPassword.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD.h"
#include "queueHandler.h"
#include "ui/ui.h"
#define DEBUG

extern sqlite3 *printqueDB;

char INSERT_SQL[2048];
char SELECT_SQL[2048];

printData printsInQueue[6];
printData printsPrinting[2];
printData printOnKlumpen;
printData printOnKnut;

userData userDataFromSQLite;
printData printDataFromSQLite;

int callbackUserdata(void *data, int argc, char **argv, char **azColName){
  userDataFromSQLite.Name = argv[0] ? argv[0]: "No user found";
  userDataFromSQLite.PhoneNumber = argv[1] ? argv[1]: "-1";
  userDataFromSQLite.uniqueSHA256ID = argv[2] ? argv[2]: "No user found";
  userDataFromSQLite.Role  = argv[5] ? argv[5]: "No user found";

  Serial.printf("Arg 0: %s \n", argv[0]);
  Serial.printf("Arg 1: %s \n", argv[1]);
  Serial.printf("Arg 2: %s \n", argv[2]);
  Serial.printf("Arg 3: %s \n", argv[3]);
  Serial.printf("Arg 4: %s \n", argv[4]);
  Serial.printf("Arg 5: %s \n", argv[5]);
  Serial.printf("Arg 6: %s \n", argv[6]);
  Serial.printf("Arg 7: %s \n", argv[7] ? argv[7]: "NULL");

  return 0;
}

int callbackPrintdata(void *data, int argc, char **argv, char **azColName){
  Serial.println("Inserted data is:");
  printDataFromSQLite.Name = argv[0] ? argv[0]: "No print found";
    Serial.println("is the name"); Serial.printf(argv[0]);
  printDataFromSQLite.PhoneNumber = argv[1] ? argv[1]: "-1";
    Serial.println("is the PhoneNumber"); Serial.printf(argv[1]);
  printDataFromSQLite.uniqueSHA256ID = argv[2] ? argv[2]: "No print found";
    Serial.println("is the SHA256UID");Serial.printf(argv[2]);
  printDataFromSQLite.printTime = argv[3] ? argv[3]: "No print found";
     Serial.println("is the printTime"); Serial.printf(argv[3]);
  printDataFromSQLite.printWeight = argv[4] ? argv[4]: "-1";
    Serial.println("is the printWeight"); Serial.printf(argv[4]);
  printDataFromSQLite.date = argv[5] ? argv[5]: "No print found";
    Serial.println("is the date"); Serial.printf(argv[5]);
  printDataFromSQLite.printer = argv[6] ? argv[6]: "-1";
    Serial.println("is the printer");Serial.printf(argv[6]);
  printDataFromSQLite.startedPrintingTimestamp = argv[7] ? argv[7]: "No print found";
    Serial.println("is the starttimestamp"); Serial.printf(argv[7]);
  printDataFromSQLite.id = atoi(argv[8]) ? atoi(argv[8]) : -1;
    Serial.println("is the id"); Serial.printf(argv[8]);

  #ifdef DEBUG
  #endif

  Serial.println("Added all data");

  return 0;
}

int callbackCurrentlyPrinting(void *data, int argc, char **argv, char **azColName){
  #ifdef DEBUG
    Serial.println("In callbackCurrentlyPrinting");
  #endif

  if (argv[6] && atoi(argv[6]) == 0) {
    printOnKlumpen.Name = argv[0] ? argv[0]: "No print found";
    printOnKlumpen.PhoneNumber = argv[1] ? argv[1]: "-1";
    printOnKlumpen.uniqueSHA256ID = argv[2] ? argv[2]: "No print found";
    printOnKlumpen.printTime = argv[3] ? argv[3]: "No print found";
    printOnKlumpen.printWeight = argv[4] ? argv[4]: "-1";
    printOnKlumpen.date = argv[5] ? argv[5]: "No print found";
    printOnKlumpen.printer = argv[6] ? argv[6]: "-1";
    printOnKlumpen.startedPrintingTimestamp = argv[7] ? argv[7]: "No print found";
    printOnKlumpen.id = atoi(argv[8]) ? atoi(argv[8]) : -1;
  } else if (argv[6] && atoi(argv[6]) == 1) {
    printOnKnut.Name = argv[0] ? argv[0]: "No print found";
    printOnKnut.PhoneNumber = argv[1] ? argv[1]: "-1";
    printOnKnut.uniqueSHA256ID = argv[2] ? argv[2]: "No print found";
    printOnKnut.printTime = argv[3] ? argv[3]: "No print found";
    printOnKnut.printWeight = argv[4] ? argv[4]: "-1";
    printOnKnut.date = argv[5] ? argv[5]: "No print found";
    printOnKnut.printer = argv[6] ? argv[6]: "-1";
    printOnKnut.startedPrintingTimestamp = argv[7] ? argv[7]: "No print found";
    printOnKnut.id = atoi(argv[8]) ? atoi(argv[8]) : -1;
  } else {
    #ifdef DEBUG
      Serial.println("No print found");
    #endif
  }

  return 0;
}



int callbackPrintqueueCounter = 0;

int callbackPrintqueue(void *data, int argc, char **argv, char **azColName) {
  int i;

  for (i = 0; i<argc; i=i+7){
    #ifdef DEBUG
      Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      Serial.printf("%s = %s\n", azColName[i+1], argv[i+1] ? argv[i+1] : "NULL");
      Serial.printf("%s = %s\n", azColName[i+2], argv[i+2] ? argv[i+2] : "NULL");
      Serial.printf("%s = %s\n", azColName[i+3], argv[i+3] ? argv[i+3] : "NULL");
      Serial.printf("%s = %s\n", azColName[i+4], argv[i+4] ? argv[i+4] : "NULL");
      Serial.printf("%s = %s\n", azColName[i+5], argv[i+5] ? argv[i+5] : "NULL");
      Serial.printf("%s = %s\n", azColName[i+6], argv[i+6] ? argv[i+6] : "NULL");
    #endif

    printsInQueue[callbackPrintqueueCounter].Name = argv[i] ? argv[i] : "NULL";
    printsInQueue[callbackPrintqueueCounter].PhoneNumber = argv[i+1] ? argv[i+1] : "-1";
    printsInQueue[callbackPrintqueueCounter].printWeight = argv[i+2] ? argv[i+2] : "NULL";
    printsInQueue[callbackPrintqueueCounter].printTime = argv[i+3] ? argv[i+3] : "NULL";
    printsInQueue[callbackPrintqueueCounter].printer = argv[i+4] ? argv[i+4] : "NULL";
    printsInQueue[callbackPrintqueueCounter].startedPrintingTimestamp = argv[i+5] ? argv[i+5] : "NULL";
    printsInQueue[callbackPrintqueueCounter].id = String(argv[i+6]).toInt();
  }

  callbackPrintqueueCounter++;

  if (callbackPrintqueueCounter >= 5) {
    callbackPrintqueueCounter = 0;
  }

  return 0;
}

int callbackCounter(void *data, int argc, char **argv, char **azColName) {
  callbackPrintqueueCounter = String(argv[0]).toInt();
  return 0;
}

int openDBSQLite(const char *filename, sqlite3 **db) {
   int rc = sqlite3_open(filename, db);
   if (rc) {
       Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
       return rc;
   } else {
       Serial.printf("Opened database successfully\n");
   }
   return rc;
}

const char* data = "Callback function called";

void addUserIntoSQLite(userData user) {
  if (checkIfUserExistsInSQLite(user.uniqueSHA256ID)) {
    Serial.println("User already exists");
    return;
  }

  char SHA256UIDtoCharArray[65];
  user.uniqueSHA256ID.toCharArray(SHA256UIDtoCharArray, 65);
  sprintf(INSERT_SQL, "INSERT INTO users (Name, PhoneNumber, uniqueSHA256ID, Printquota, Date, Role) VALUES ('%s', '%s', '%s', 0, '%s', '%s')", user.Name, user.PhoneNumber, SHA256UIDtoCharArray, "0", user.Role);
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, INSERT_SQL, callbackUserdata, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
  } else {
    Serial.println("Added user to SQLite");
  }
}

bool checkIfUserExistsInSQLite(String SHA256UID) {
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);

  sprintf(SELECT_SQL, "SELECT uniqueSHA256ID FROM users WHERE uniqueSHA256ID = '%s'", SHA256UIDtoCharArray);
  
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, SELECT_SQL, callbackUserdata, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    Serial.println("Error occured in checkIfUserExistsInSQLite");
    return false;
  } else {
    if (userDataFromSQLite.uniqueSHA256ID == SHA256UID) {
      #ifdef DEBUG
        Serial.println("User exists in SQLite");
      #endif

      return true;
    } else {
      #ifdef DEBUG
        Serial.println("User does not exist in SQLite");
      #endif

      return false;
    }
  }
}

userData getUserFromSQLite(String SHA256UID) {
  Serial.println("Getting user from SQLite");
  
  // Convert SHA256UID to char array
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);

  // Reset userData
  userDataFromSQLite.Name = "No user found";
  userDataFromSQLite.PhoneNumber = "No user found";
  userDataFromSQLite.uniqueSHA256ID = "No user found";
  userDataFromSQLite.Role = "No user found";

  int rc;
  char *zErrMsg = 0;
  sprintf(SELECT_SQL, "SELECT * FROM users WHERE uniqueSHA256ID = '%s'", SHA256UIDtoCharArray);

  rc = sqlite3_exec(printqueDB, SELECT_SQL, callbackUserdata, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    //sqlite3_close(printqueDB);
    return userDataFromSQLite;
  } else {
    return userDataFromSQLite;
  }

}

bool addPrintIntoSQLite(userData userInfo, String printWeight, String printTime, String SHA256UID) {
  // Convert SHA256UID to char array
  #ifdef DEBUG
    Serial.print("SHA256UID is: ");
    Serial.println(SHA256UID);
    Serial.print("printTime is: ");
    Serial.println(printTime);
    Serial.print("printWeight is: ");
    Serial.println(printWeight);
    Serial.print("Name is: "); 
    Serial.println(userInfo.Name);
    Serial.print("PhoneNumber is: ");
    Serial.println(userInfo.PhoneNumber);
  #endif

  // Create and execute the query
  int rc;
  char *zErrMsg = 0;
  sprintf(INSERT_SQL, "INSERT INTO queue (Name, PhoneNumber, uniqueSHA256UID, printTime, printWeight, date, startedPrintingTimestamp, printer) VALUES ('%s', '%s', '%s', '%s', '%s', '0', '0', '0')", userInfo.Name, userInfo.PhoneNumber, SHA256UID.c_str(), printTime, printWeight);

  #ifdef DEBUG
    Serial.print("Insert String: ");
    Serial.println(INSERT_SQL);
  #endif

  rc = sqlite3_exec(printqueDB, INSERT_SQL, NULL, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    return true;
  }
}

bool deleteEntireQueueSQLite() {
  sprintf(INSERT_SQL, "DELETE FROM queue");
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, INSERT_SQL, NULL, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    Serial.println("Deleted entire queue");
    return true;
  }
}

bool deleteEntireCurrentlyPrintingQueueSQLite() {
  sprintf(INSERT_SQL, "DELETE FROM currentlyPrinting");
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, INSERT_SQL, NULL, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    Serial.println("Deleted entire queue");
    return true;
  }
}

bool updateQueueSQLite() {
  #ifdef DEBUG
    Serial.println("Updating queue (updateQueue) with data from SQLite");
  #endif

  for (int i = 0; i < 6; i++) {
    printsInQueue[i].Name = "";
    printsInQueue[i].PhoneNumber = "";
    printsInQueue[i].printWeight = "";
    printsInQueue[i].printTime = "";
    printsInQueue[i].printer = "";
    printsInQueue[i].startedPrintingTimestamp = "";
    printsInQueue[i].id = -1;
  }

  sprintf(SELECT_SQL, "SELECT Name, PhoneNumber, printWeight, printTime, printer, startedPrintingTimestamp, id FROM queue LIMIT 6");
  
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, SELECT_SQL, callbackPrintqueue, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    Serial.println("Leaving updateQueueSQLite");
    callbackPrintqueueCounter = 0;
    return false;
  } else {
      Serial.println("Leaving updateQueueSQLite");
      sqlite3_free(zErrMsg);
      callbackPrintqueueCounter = 0;
    return true;
  }

  
}



// Right now it deletes all prints from a specific user, probably best to add granularity
bool deleteUsersPrintBasedOnSHA256UIDSQLite(String SHA256UID) {
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);
  sprintf(INSERT_SQL, "DELETE FROM queue WHERE uniqueSHA256UID = '%s'", SHA256UIDtoCharArray);

  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, INSERT_SQL, NULL, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    Serial.println("Deleted prints for user based on SHA256UID");
    return true;
  } 
}

bool deletePrintBasedOnIDSQLite(int id) {
  sprintf(INSERT_SQL, "DELETE FROM queue WHERE Id = '%d'", id);
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, INSERT_SQL, NULL, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    Serial.println("Deleted print based on ID");
    return true;
  } 
}

bool addPrintToCurrentlyPrintingSQLite(int printqueueID, int printer) {
  sprintf(INSERT_SQL, "INSERT INTO currentlyPrinting (Name, PhoneNumber, uniqueSHA256UID, printTime, printWeight, date, printer, startedPrintingTimestamp) VALUES ('%s', '%d', '%s', '%s', '%d', '%s', '%s', '%s')", printDataFromSQLite.Name, atoi(printDataFromSQLite.PhoneNumber.c_str()), printDataFromSQLite.uniqueSHA256ID.c_str(), printDataFromSQLite.printTime, atoi(printDataFromSQLite.printWeight.c_str()), printDataFromSQLite.date, String(printer), printDataFromSQLite.startedPrintingTimestamp);  

  #ifdef DEBUG
    Serial.println("Adding print to currently printing");
  #endif

  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, INSERT_SQL, NULL, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    Serial.println("Added print to currently printing");
    return true;
  }
  return true;
}


int getCurrentQueueLengthSQLite() {
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, "SELECT COUNT(*) FROM queue", callbackCounter, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return 0;
  } else {
    return callbackPrintqueueCounter;
  }
}

bool getPrintdataBasedOnIDSQLite(int id) {
  int rc;
  char *zErrMsg = 0;
  sprintf(SELECT_SQL, "SELECT Name, PhoneNumber, uniqueSHA256UID, printTime, printWeight, date, printer, startedPrintingTimestamp, id FROM queue WHERE id = %d", id);
  lv_timer_handler();
  rc = sqlite3_exec(printqueDB, SELECT_SQL, callbackPrintdata, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    sqlite3_free(zErrMsg);
    return true;
  }
}

bool getCurrentlyPrintingSQLite() {
  #ifdef DEBUG
    Serial.println("In getCurrentlyPrintingSQLite");
  #endif  
  // Reset the print data
  printOnKlumpen.Name = "";
  printOnKlumpen.PhoneNumber = "";
  printOnKlumpen.uniqueSHA256ID = "";
  printOnKlumpen.printTime = "";
  printOnKlumpen.printWeight = "";
  printOnKlumpen.date = "";
  printOnKlumpen.printer = "";
  printOnKlumpen.startedPrintingTimestamp = "";
  printOnKlumpen.id = -1;

  printOnKnut.Name = "";
  printOnKnut.PhoneNumber = "";
  printOnKnut.uniqueSHA256ID = "";
  printOnKnut.printTime = "";
  printOnKnut.printWeight = "";
  printOnKnut.date = "";
  printOnKnut.printer = "";
  printOnKnut.startedPrintingTimestamp = "";
  printOnKnut.id = -1;

  int rc;
  char *zErrMsg = 0;

  // Get the print on Klumpen
  Serial.println("Getting print on Klumpen");
  rc = sqlite3_exec(printqueDB, "SELECT Name, PhoneNumber, uniqueSHA256UID, printTime, printWeight, date, printer, startedPrintingTimestamp, id FROM currentlyPrinting WHERE printer = '0' ORDER BY id DESC LIMIT 1", callbackCurrentlyPrinting, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  }

  // Get the print on Knut
  rc = sqlite3_exec(printqueDB, "SELECT Name, PhoneNumber, uniqueSHA256UID, printTime, printWeight, date, printer, startedPrintingTimestamp, id FROM currentlyPrinting WHERE printer = '1' ORDER BY id DESC LIMIT 1", callbackCurrentlyPrinting, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  }

  #ifdef DEBUG
    Serial.println("Leaving getCurrentlyPrintingSQLite");
  #endif  

  return true;
}

bool updatePrintBasedOnID(int id, int printer) {
  #ifdef DEBUG
      Serial.println("Updating print based on ID");
  #endif

  getPrintdataBasedOnIDSQLite(id);
  deletePrintBasedOnIDSQLite(id);  
  addPrintToCurrentlyPrintingSQLite(id, printer);

  updateQueueUi();

  return true;
}