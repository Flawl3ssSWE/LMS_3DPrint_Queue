
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

bool modifyUser() {
    userData userInfo;
    userInfo.uniqueSHA256ID = requestRFIDRemote();
    if (userInfo.uniqueSHA256ID == "-1" ){return false;}

    if (checkIfUserExistsInMySQL(userInfo.uniqueSHA256ID)) {
        Serial.println("User exists");
    } else {
        Serial.println("User does not exist");
        return false;
    }

    userInfo.Name = "Anton";
    userInfo.PhoneNumber = "23456";
    userInfo.Role = "Member";
    userInfo.Printquota = "100";
    if (modifyUserInMySQL(userInfo)) {
        return true;
    } else {
        return false;
    }
}

