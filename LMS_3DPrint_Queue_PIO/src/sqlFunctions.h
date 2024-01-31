#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H
    #include <Arduino.h>
    #include <MySQL_Connection.h>
    #include <MySQL_Cursor.h>
    #include "connectWIFIandMySQL.h"

    struct userData {
        String Name;
        String PhoneNumber;
        String uniqueSHA256ID;
        String Role;
    }; 

    void addUserIntoMySQL(userData user);
    void getUserFromMySQL(String SHA256UID);

#endif //SQLFUNCTIONS_H