#ifndef httpRFID_h
    #define httpRFID_h
    #include <Arduino.h>
    #include <PN532_I2C.h>
    #include <PN532.h>
    #include <NfcAdapter.h>

    extern PN532_I2C pn532_i2c;
    extern NfcAdapter nfc;

    String requestRFID();

#endif