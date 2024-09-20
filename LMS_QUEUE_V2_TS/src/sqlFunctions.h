#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H

#include <sqlite3.h>
#include <Arduino.h>
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
        String uniqueSHA256ID;
        String printTime;
        String printWeight;
        String date;
        String printer;
        String startedPrintingTimestamp;
        int id;
    };

    extern printData printsInQueue[6];
    extern printData printsPrinting[2];
    extern printData printKlumpen;
    extern printData printKnut;
    extern printData printDataFromSQLite;

    enum printStatus {
        Knut,
        Klumpen
    };

    extern int callbackPrintqueueCounter;

    bool updateQueue();
    bool updatePrintBasedOnID(int id, int printer);

    int openDBSQLite(const char *filename, sqlite3 **db);
    userData getUserFromSQLite(String SHA256UID);
    bool addPrintIntoSQLite(userData userInfo, String printWeight, String printTime, String SHA256UID);
    bool updateQueueSQLite();
    bool checkIfUserExistsInSQLite(String SHA256UID);
    int getCurrentQueueLengthSQLite();
    void addUserIntoSQLite(userData user);
    bool deleteEntireQueueSQLite();
    bool deleteUsersPrintBasedOnSHA256UIDSQLite(String SHA256UID);
    bool deletePrintBasedOnIDSQLite(int id);
    bool deleteEntireCurrentlyPrintingQueueSQLite();
    bool getPrintdataBasedOnIDSQLite(int id);

#endif //SQLFUNCTIONS_H