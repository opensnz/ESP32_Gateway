#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include <SPIFFS.h>
#include <LittleFS.h>
#include "log.h"


class StorageClass
{
private:

public:
    bool begin(void);
    bool readFile(const char * path, String & content);
    bool writeFile(const char * path, String & content);
    bool taskBeforeShutdown(void);
};


/******************* Exported Global Variables ************************/

extern StorageClass Storage;


#endif /* __STORAGE_H__ */