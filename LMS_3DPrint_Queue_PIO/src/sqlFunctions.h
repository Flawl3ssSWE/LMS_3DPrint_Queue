#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H
    #include <Arduino.h>
    #include <MySQL_Connection.h>
    #include <MySQL_Cursor.h>
    #include "connectWIFIandMySQL.h"


    void insertIntoMySQL(String SHA256UID);
    void selectFromMySQL(String SHA256UID);




#endif //SQLFUNCTIONS_H