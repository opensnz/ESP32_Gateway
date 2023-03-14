#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <Arduino.h>
#include <SPIFFS.h>
//#include <LittleFS.h>
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
    bool exists(String path);
    bool readFile(String path, String & content);
    bool writeFile(String path, String & content);
    bool removeFile(String path);
    bool readFile(String path, uint8_t *pContent, uint32_t size, uint32_t pos = 0);
    bool writeFile(String path, uint8_t *pContent, uint32_t size, uint32_t pos = 0);
    bool getSystemInfo(String & info);
    void restart(void);
};


/******************* Exported Global Variables ************************/

extern SystemClass System;


#endif /* __SYSTEM_H__ */