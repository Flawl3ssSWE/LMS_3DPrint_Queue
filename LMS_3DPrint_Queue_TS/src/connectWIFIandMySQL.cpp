#include <Arduino.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "TotalltNotJonatansMasterPassword.h"

IPAddress server_addr(192, 168, 1, 38);  // IP of the MySQL *server* here
char user[] = MySQLUsr;                  // MySQL user login username
char password[] = MySQLPsw;              // MySQL user login password
char ssid[] = WiFiSSID;                   // your SSID
char pass[] = WiFiPswd;                   // your SSID Password

WiFiClient client;                        // Define WiFiClient
MySQL_Connection conn(&client);           // Define MySQL_Connection

void connectToMySQL() {
  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password)) {
    Serial.println("OK.");
  } else {
    Serial.println("FAILED.");
  }
}

void connectToWifi() {
  WiFi.begin(ssid, pass);
  int timeout = 40;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());
}
