#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <Arduino.h>
#include <SPIFFS.h>
#include <LittleFS.h>
#include "log.h"

#define SYSTEM_LOG_BAUDRATE  115200

#ifdef SYSTEM_LOG_ENABLED
    #if SYSTEM_LOG_ENABLED
        #define SYSTEM_LOG_BEGIN(baudrate)   Serial.begin(baudrate)
    #else
        #define SYSTEM_LOG_BEGIN(baudrate)
    #endif
#else
    #define SYSTEM_LOG_BEGIN(baudrate)
#endif

// File System : SPIFFS (will be deprecated)
#define FILE_SYSTEM          SPIFFS

class SystemClass
{
private:
    bool taskBeforePowerOFF(void);

public:
    bool begin(void);
    bool readFile(const char * path, String & content);
    bool writeFile(const char * path, String & content);
    void restart(void);
};


/******************* Exported Global Variables ************************/

extern SystemClass System;


#endif /* __SYSTEM_H__ */