#ifndef CONNECT_WIFI_AND_MYSQL_H
#define CONNECT_WIFI_AND_MYSQL_H

    #include <WiFi.h>
    #include <MySQL_Connection.h>

    extern WiFiClient client;                 // Declare WiFiClient as extern
    extern MySQL_Connection conn;              // Declare MySQL_Connection as extern

    void connectToWifi();
    void connectToMySQL();

#endif // CONNECT_WIFI_AND_MYSQL_H