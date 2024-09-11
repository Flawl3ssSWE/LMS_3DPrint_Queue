#ifndef queueHandler_h
#define queueHandler_h

    #include <Arduino.h>
    #include "sqlFunctions.h"

    void updateQueueUi();

    extern printData printsInQueue[6];
    extern printData printsPrinting[2];

#endif //queueHandler_h