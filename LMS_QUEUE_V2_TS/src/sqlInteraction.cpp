
#include "Arduino.h"
#include "sqlFunctions.h"
#include "httpRFID.h"

bool addUser() {
  userData userInfo;
  userInfo.uniqueSHA256ID = requestRFIDRemote();
  if (userInfo.uniqueSHA256ID == "-1" ){return false;}
  
  userInfo.Name = "Jonatan";
  userInfo.PhoneNumber = "0701234567";
  userInfo.Role = "Admin";
  addUserIntoMySQL(userInfo);

  return true;
}

bool checkIfAdmin() {
    userData userInfo;

    String sha256UID = requestRFIDRemote();
    Serial.println(sha256UID);
    if (sha256UID == "-1" ){return false;}

    userInfo = getUserFromMySQL(sha256UID);

    #ifdef DEBUG
        Serial.println(userInfo.Name);
        Serial.println(userInfo.PhoneNumber);
        Serial.println(userInfo.Printquota);
        Serial.println(userInfo.Role);
    #endif

    if (userInfo.Role == "Admin") {
        return true;
    } else {
        return false;
    }
}

bool modifyUser(userData userInfo) {
    userInfo.uniqueSHA256ID = requestRFIDRemote();
    if (userInfo.uniqueSHA256ID == "-1" ){return false;}

    if (checkIfUserExistsInMySQL(userInfo.uniqueSHA256ID)) {
        Serial.println("User exists");
    } else {
        Serial.println("User does not exist");
        return false;
    }

    if (modifyUserInMySQL(userInfo)) {
        return true;
    } else {
        return false;
    }
}

bool deleteUser() {
    userData userInfo;
    userInfo.uniqueSHA256ID = requestRFIDRemote();
    if (userInfo.uniqueSHA256ID == "-1" ){return false;}

    if (checkIfUserExistsInMySQL(userInfo.uniqueSHA256ID)) {
        Serial.println("User exists");
    } else {
        Serial.println("User does not exist");
        return false;
    }

    if (deleteUserFromMySQL(userInfo.uniqueSHA256ID)) {
        return true;
    } else {
        return false;
    }
}

// bool addPrintToQueue() {
//     String SHA256ID = requestRFIDRemote();
//     if (SHA256ID == "-1" ){return false;}

//     #ifdef DEBUG
//         Serial.print("SHA256ID is: ");
//         Serial.println(SHA256ID);
//     #endif

//     String printWeight;
//     String printTime;

//     // Add to printque
//     if (addPrintIntoMySQL(SHA256ID, printWeight, printTime)) {
//         return true;
//     } else {
//         return false;
//     }
// }

bool deleteUsersPrintFromQueue() {
    String SHA256UID = requestRFIDRemote();
    if (SHA256UID == "-1" ){return false;}

    if (deleteUsersPrintFromQueueMySQL(SHA256UID)) {
        return true;
    } else {
        return false;
    }
}
