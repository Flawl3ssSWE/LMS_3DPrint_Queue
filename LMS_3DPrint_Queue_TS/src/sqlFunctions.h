#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H

    struct userData {
        String Name;
        String PhoneNumber;
        String uniqueSHA256ID;
        String Printquota;
        String Role;
    }; 

    struct printData {
        String Name;
        String PhoneNumber;
        String printWeight;
        String printTime;
    };

    void addUserIntoMySQL(userData user);
    userData getUserFromMySQL(String SHA256UID);
    bool checkIfUserExistsInMySQL(String SHA256UID);
    bool modifyUserInMySQL(userData userInfo);
    bool deleteUserFromMySQL(String SHA256UID);
    bool addPrintIntoMySQL(String SHA256UID, String printWeight, String printTime);
    printData getFirstPrintFromQueueMySQL();
    bool deleteFirstPrintFromQueueMySQL();
    bool deleteUsersPrintFromQueueMySQL(String SHA256UID);
    bool deleteEntireQueueMySQL();

#endif //SQLFUNCTIONS_H