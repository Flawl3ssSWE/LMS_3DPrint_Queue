#ifndef sqlInteraction_h
#define sqlInteraction_h

    bool addUser();
    bool checkIfAdmin();
    bool modifyUser(userData userInfo);
    bool deleteUser();
    bool addPrintquota();
    bool addPrintToQueue();
    bool deleteUsersPrintFromQueue();

#endif