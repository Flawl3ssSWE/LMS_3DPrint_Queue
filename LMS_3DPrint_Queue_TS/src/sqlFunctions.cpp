#include <Arduino.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "connectWIFIandMySQL.h"
#include "sqlFunctions.h"

MySQL_Cursor* cursor;


char INSERT_SQL[1024];
char SELECT_SQL[1024];



void addUserIntoMySQL(userData user) {
  // TODO verify that user does not exist 
  char SHA256UIDtoCharArray[64];
  user.uniqueSHA256ID.toCharArray(SHA256UIDtoCharArray, 64);
  sprintf(INSERT_SQL, "INSERT INTO printingQueue.queue (Name, PhoneNumber, uniqueSHA256ID, Printquota, Role) VALUE ('%s', '%s', '%s', 0, '%s')", user.Name, user.PhoneNumber, SHA256UIDtoCharArray, user.Role);
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

userData getUserFromMySQL(String SHA256UID) {
  // Convert SHA256UID to char array
  char SHA256UIDtoCharArray[65];
  SHA256UID.toCharArray(SHA256UIDtoCharArray, 65);

  // Create and execute the query
  sprintf(SELECT_SQL, "SELECT Name, PhoneNumber, Printquota, Role FROM printingQueue.queue WHERE uniqueSHA256ID = '%s'", SHA256UIDtoCharArray);
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