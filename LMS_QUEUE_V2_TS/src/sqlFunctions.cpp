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

char INSERT_SQL[1024];
char SELECT_SQL[1024];

printData printsInQueue[6];
printData printsPrinting[2];

userData userDataFromSQLite;
printData printDataFromSQLite;

int callbackUserdata(void *data, int argc, char **argv, char **azColName){
  userDataFromSQLite.Name = argv[0] ? argv[0]: "No user found";
  userDataFromSQLite.PhoneNumber = argv[1] ? argv[1]: "No user found";
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
  printDataFromSQLite.PhoneNumber = argv[1] ? argv[1]: "No print found";
  printDataFromSQLite.uniqueSHA256ID = argv[2] ? argv[2]: "No print found";
  printDataFromSQLite.printTime = argv[3] ? argv[3]: "No print found";
  printDataFromSQLite.printWeight = argv[4] ? argv[4]: "-1";
  printDataFromSQLite.date = argv[5] ? argv[5]: "No print found";
  printDataFromSQLite.printer = argv[6] ? argv[6]: "-1";
  printDataFromSQLite.startedPrintingTimestamp = argv[7] ? argv[7]: "No print found";
  printDataFromSQLite.id = atoi(argv[8]) ? atoi(argv[8]) : -1;

  #ifdef DEBUG
    Serial.printf(argv[0]);
    Serial.printf(argv[1]);
    Serial.printf(argv[2]);
    Serial.printf(argv[3]);
    Serial.printf(argv[4]);
    Serial.printf(argv[5]);
    Serial.printf(argv[6]);
    Serial.printf(argv[7]);
  #endif

  Serial.println("Added all data");

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
    printsInQueue[callbackPrintqueueCounter].PhoneNumber = argv[i+1] ? argv[i+1] : "NULL";
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
    printsInQueue[i].id = 0;
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
  Serial.println("Before add To print print");
  Serial.printf("\n\n");
  Serial.printf("Id is %d\n", printqueueID);
  Serial.printf("Printer is %d\n", printer);
  Serial.printf("Amount of prints in variable %d", sizeof(printsInQueue));
  Serial.printf("\n\n");
  // Serial.printf("Adding user: %s to currently printing", printsInQueue[printqueueID].Name);
  Serial.printf("Adding user: %s to currently printing", printsInQueue[printqueueID].Name);


  Serial.println("After add To print print, before SQL statement");
  sprintf(INSERT_SQL, "INSERT INTO currentlyPrinting (Name, PhoneNumber, uniqueSHA256UID, printTime, printWeight, printer, startedPrintingTimestamp) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s')", printsInQueue[printqueueID].Name, printsInQueue[printqueueID].PhoneNumber, printsInQueue[printqueueID].uniqueSHA256ID, printsInQueue[printqueueID].printTime, printsInQueue[printqueueID].printWeight, printsInQueue[printqueueID].printer, printsInQueue[printqueueID].startedPrintingTimestamp);
  Serial.println("After SQL statement");
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

bool updatePrintBasedOnID(int id, int printer) {
  #ifdef DEBUG
      Serial.println("Updating print based on ID");
  #endif
  // lv_timer_handler();
  getPrintdataBasedOnIDSQLite(id);
  Serial.printf("Out of getPrintData \n User is %s\n", printDataFromSQLite.Name);
  deletePrintBasedOnIDSQLite(id);
  Serial.println("Out of deleted print based on ID");
  // addPrintToCurrentlyPrintingSQLite(id, printer);

  updateQueueUi();

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

bool getCurrentlyPrinting() {
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(printqueDB, "SELECT Name, PhoneNumber, printWeight, printTime, printer, startedPrintingTimestamp, id FROM currentlyPrinting WHERE printer = 1 ORDER BY id DESC LIMIT 1", NULL, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(printqueDB);
    return false;
  } else {
    
  }


}
