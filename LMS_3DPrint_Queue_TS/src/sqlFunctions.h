#ifndef SQLFUNCTIONS_H
#define SQLFUNCTIONS_H

    struct userData {
        String Name;
        String PhoneNumber;
        String uniqueSHA256ID;
        String Printquota;
        String Role;
    }; 

    void addUserIntoMySQL(userData user);
    userData getUserFromMySQL(String SHA256UID);

#endif //SQLFUNCTIONS_H