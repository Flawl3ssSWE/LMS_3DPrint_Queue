#include <Arduino.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "connectWIFIandMySQL.h"
#include "sqlFunctions.h"

MySQL_Cursor* cursor;


char INSERT_SQL[1024];
char SELECT_SQL[1024];



void addUserIntoMySQL(userData user) {
  if (checkIfUserExistsInMySQL(user.uniqueSHA256ID)) {
    Serial.println("User already exists");
    return;
  }

  char SHA256UIDtoCharArray[65];
  user.uniqueSHA256ID.toCharArray(SHA256UIDtoCharArray, 65);
  sprintf(INSERT_SQL, "INSERT INTO printingQueue.users (Name, PhoneNumber, uniqueSHA256ID, Printquota, Role) VALUE ('%s', '%s', '%s', 0, '%s')", user.Name, user.PhoneNumber, SHA256UIDtoCharArray, user.Role);
  cursor = new MySQL_Cursor(&conn);
  if (conn.connected()){
      cursor->execute(INSERT_SQL);
      Serial.println("*** INSERTED THIS MATE");
      #ifdef DEBUG
        Serial.println(INSERT_SQL);
        Serial.println(user.uniqueSHA256ID);
      #endif
  }else{
    Serial.println("Cannot add user to MySQL");
  }
}

bool checkIfUserExistsInMySQL(String SHA256UID) {
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);
  Serial.print("G");
  Serial.print(SHA256UID);
  Serial.print("G\n");
  sprintf(SELECT_SQL, "SELECT uniqueSHA256ID FROM printingQueue.users WHERE uniqueSHA256ID = '%s'", SHA256UIDtoCharArray);
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(SELECT_SQL);
  column_names *cols = cursor->get_columns();
  row_values *row = NULL;
  row = cursor->get_next_row();
  
  if (row == NULL) {
    #ifdef DEBUG
      Serial.println("No user found");
    #endif
    
    delete cursor;
    return false;
  }
  Serial.print(row->values[0]);
  delete cursor;
  return true;
}

userData getUserFromMySQL(String SHA256UID) {
  // Convert SHA256UID to char array
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);

  // Create and execute the query
  sprintf(SELECT_SQL, "SELECT Name, PhoneNumber, Printquota, Role FROM printingQueue.users WHERE uniqueSHA256ID = '%s'", SHA256UIDtoCharArray);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(SELECT_SQL);

  // Get the returned user
  column_names *cols = cur_mem->get_columns();
  row_values *row = cur_mem->get_next_row();

  if (row == NULL) {
    #ifdef DEBUG
      Serial.println("No user found");
    #endif  

    userData userInfo;
    userInfo.Name = "No user found";
    userInfo.PhoneNumber = "No user found";
    userInfo.Printquota = "No user found";
    userInfo.Role = "No user found";
    delete cur_mem;
    return userInfo;
  }

  // Add the info to the struct
  userData userInfo;
  userInfo.Name = row->values[0];
  userInfo.PhoneNumber = row->values[1];
  userInfo.Printquota = row->values[2];
  userInfo.Role = row->values[3];

  // Deleting the cursor also frees up memory used
  delete cur_mem;

  return userInfo;
}

bool modifyUserInMySQL(userData userInfo) {
  // Convert SHA256UID to char array
  char SHA256UIDtoCharArray[65];
  userInfo.uniqueSHA256ID.toCharArray(SHA256UIDtoCharArray, 65);

  // Create and execute the query
  sprintf(INSERT_SQL, "UPDATE printingQueue.users SET Name = '%s', PhoneNumber = '%s', Printquota = '%s', Role = '%s' WHERE uniqueSHA256ID = '%s'", userInfo.Name, userInfo.PhoneNumber, userInfo.Printquota, userInfo.Role, SHA256UIDtoCharArray);
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(INSERT_SQL);

  // Deleting the cursor also frees up memory used
  delete cursor;
  return true;
}

bool deleteUserFromMySQL(String SHA256UID) {
  // Convert SHA256UID to char array
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);

  // Create and execute the query
  sprintf(INSERT_SQL, "DELETE FROM printingQueue.users WHERE uniqueSHA256ID = '%s'", SHA256UIDtoCharArray);
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(INSERT_SQL);

  // Deleting the cursor also frees up memory used
  delete cursor;
  return true;
}

bool addPrintIntoMySQL(String SHA256UID, String printWeight, String printTime) {
  // Convert SHA256UID to char array
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);

    userData userInfo;
    userInfo = getUserFromMySQL(SHA256UID);
    if (userInfo.Name == "No user found") {
        Serial.println("User does not exist, cannot add print to queue");
        return false;
    }

  // Create and execute the query
  sprintf(INSERT_SQL, "INSERT INTO printingQueue.queue (Name, PhoneNumber, uniqueSHA256UID, printTime, printWeight) VALUE ('%s', '%s', '%s', '%s', '%s')", userInfo.Name, userInfo.PhoneNumber, SHA256UIDtoCharArray, printTime, printWeight);
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(INSERT_SQL);

  // Deleting the cursor also frees up memory used
  delete cursor;
  return true;
}

printData getFirstPrintFromQueueMySQL() {
  printData printInfo;
  sprintf(SELECT_SQL, "SELECT Name, PhoneNumber, printWeight, printTime FROM printingQueue.queue LIMIT 1");
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(SELECT_SQL);
  column_names *cols = cursor->get_columns();
  row_values *row = cursor->get_next_row();

  if (row == NULL) {
    #ifdef DEBUG
      Serial.println("No print found");
    #endif

    printInfo.Name = "No print found";
    printInfo.PhoneNumber = "No print found";
    printInfo.printWeight = "No print found";
    printInfo.printTime = "No print found";
    delete cursor;
    return printInfo;
  }

  printInfo.Name = row->values[0];
  printInfo.PhoneNumber = row->values[1];
  printInfo.printWeight = row->values[3];
  printInfo.printTime = row->values[4];
  
  return printInfo;
}

bool deleteFirstPrintFromQueueMySQL() {
  sprintf(INSERT_SQL, "DELETE FROM printingQueue.queue LIMIT 1");
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(INSERT_SQL);
  delete cursor;
  return true;
}

bool deleteUsersPrintFromQueueMySQL(String SHA256UID) {
  // Convert SHA256UID to char array
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);

  sprintf(INSERT_SQL, "DELETE FROM printingQueue.queue WHERE uniqueSHA256UID = '%s'", SHA256UIDtoCharArray);
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(INSERT_SQL);
  delete cursor;
  return true;
}

bool deleteEntireQueueMySQL() {
  sprintf(INSERT_SQL, "DELETE FROM printingQueue.queue");
  cursor = new MySQL_Cursor(&conn);
  cursor->execute(INSERT_SQL);
  delete cursor;
  return true;
}
