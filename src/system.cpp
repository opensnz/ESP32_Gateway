
#include "system.h"


bool SystemClass::begin(void){
    SYSTEM_LOG_BEGIN(SYSTEM_LOG_BAUDRATE);
    if (!FILE_SYSTEM.begin(true)) {
        SYSTEM_PRINT_LN("An error has occurred while mounting FileSystem");
        return false;
    }
    SYSTEM_PRINT_LN("FileSystem mounted successfully");
    return true;
}

bool SystemClass::readFile(const char * path, String & content){
    SYSTEM_PRINTF_LN("Reading file: %s", path);
    File file = FILE_SYSTEM.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        SYSTEM_PRINT_LN("Failed to open file");
        return false;
    }
    while(file.available()){
        content = file.readStringUntil('\n');
    }
    return true;
}

bool SystemClass::writeFile(const char * path, String & content){
    SYSTEM_PRINTF_LN("Writing file : %s", path);
    File file = FILE_SYSTEM.open(path, FILE_WRITE);
    if(!file)
    {
        SYSTEM_PRINT_LN("Failed to open file");
        return false;
    }
    if(file.print(content+'\n'))
    {
        SYSTEM_PRINT_LN("File written");
        return true;
    }else 
    {
        SYSTEM_PRINT_LN("Failed to write");
        return false;
    }
}

bool SystemClass::taskBeforePowerOFF(void){
    // Perform task before power off
    return true;
}

void SystemClass::restart(void){
    while(!this->taskBeforePowerOFF())
    {
        // retry after 10ms
        delay(10);
    }
    // Everything is ok, restart now
    ESP.restart();
}


SystemClass System;