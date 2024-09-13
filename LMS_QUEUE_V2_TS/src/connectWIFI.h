#ifndef CONNECT_WIFI
#define CONNECT_WIFI

    #include <WiFi.h>

    extern WiFiClient client;                 // Declare WiFiClient as extern

    void connectToWifi();

#endif // CONNECT_WIFI