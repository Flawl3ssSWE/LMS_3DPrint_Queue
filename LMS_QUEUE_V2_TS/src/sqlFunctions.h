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

    extern printData printsInQueue[6];

    void addUserIntoMySQL(userData user);
    userData getUserFromMySQL(String SHA256UID);
    bool checkIfUserExistsInMySQL(String SHA256UID);
    bool modifyUserInMySQL(userData userInfo);
    bool deleteUserFromMySQL(String SHA256UID);
    bool addPrintIntoMySQL(userData userInfo, String printWeight, String printTime, String SHA256UID);
    printData getFirstPrintFromQueueMySQL();
    bool deleteFirstPrintFromQueueMySQL();
    bool deleteUsersPrintFromQueueMySQL(String SHA256UID);
    bool deleteEntireQueueMySQL();
    bool updateQueue();

#endif //SQLFUNCTIONS_H