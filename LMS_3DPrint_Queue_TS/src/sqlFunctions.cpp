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

void getUserFromMySQL(String SHA256UID) {
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