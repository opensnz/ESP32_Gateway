
#include "system.h"
#include <Arduino_JSON.h>

SystemClass System;

bool SystemClass::begin(void){
    SYSTEM_LOG_BEGIN(SYSTEM_LOG_BAUDRATE);
    SYSTEM_PRINT_LN("FileSystem mounting...");
    if (!FILE_SYSTEM.begin(true)) {
        SYSTEM_PRINT_LN("An error has occurred while mounting FileSystem");
        return false;
    }
    SYSTEM_PRINT_LN("FileSystem mounted successfully");
    return true;
}

bool SystemClass::exists(String path){
    return FILE_SYSTEM.exists(path);
}

bool SystemClass::readFile(String path, String & content){
    SYSTEM_PRINT("Reading file : ");SYSTEM_PRINT_LN(path);
    File file = FILE_SYSTEM.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        SYSTEM_PRINT_LN("Failed to open file");
        file.close();
        return false;
    }
    content = "";
    if(file.available()){
        content = file.readStringUntil('\n');
    }
    SYSTEM_PRINT("File content : ");SYSTEM_PRINT_LN(content);
    file.close();
    return true;
}

bool SystemClass::writeFile(String path, String & content){
    SYSTEM_PRINT("Writing file : ");SYSTEM_PRINT_LN(path);
    SYSTEM_PRINT("File content : ");SYSTEM_PRINT_LN(content);
    File file = FILE_SYSTEM.open(path, FILE_WRITE);
    if(!file)
    {
        SYSTEM_PRINT_LN("Failed to open file");
        file.close();
        return false;
    }
    if(file.print(content+'\n'))
    {
        SYSTEM_PRINT_LN("File written");
        file.close();
        return true;
    }else 
    {
        SYSTEM_PRINT_LN("Failed to write");
        file.close();
        return false;
    }
}

bool SystemClass::readFile(String path, uint8_t *pContent, uint32_t size, uint32_t pos)
{
    SYSTEM_PRINT("Reading file : ");SYSTEM_PRINT_LN(path);
    File file = FILE_SYSTEM.open(path, FILE_READ);
    if(!file || file.isDirectory()){
        SYSTEM_PRINT_LN("Failed to open file");
        file.close();
        return false;
    }
    file.seek(pos);
    if(file.available()){
        size = file.readBytes((char *)pContent, size);
    }
    SYSTEM_PRINT("File content : ");SYSTEM_PRINT_LN(String(pContent, size));
    file.close();
    return true;
}

bool SystemClass::writeFile(String path, uint8_t *pContent, uint32_t size, uint32_t pos)
{
    SYSTEM_PRINT("Writing file : ");SYSTEM_PRINT_LN(path);
    SYSTEM_PRINT("File content : ");SYSTEM_PRINT_LN(String(pContent, size));
    File file = FILE_SYSTEM.open(path, FILE_WRITE);
    if(!file)
    {
        SYSTEM_PRINT_LN("Failed to open file");
        file.close();
        return false;
    }
    file.seek(pos);
    if(file.write(pContent, size))
    {
        SYSTEM_PRINT_LN("File written");
        file.close();
        return true;
    }else 
    {
        SYSTEM_PRINT_LN("Failed to write");
        file.close();
        return false;
    }
}


bool SystemClass::removeFile(String path){
    SYSTEM_PRINT("Removing file : ");SYSTEM_PRINT_LN(path);
    if(FILE_SYSTEM.exists(path))
    {
        return FILE_SYSTEM.remove(path);
    }
    SYSTEM_PRINT_LN("File not found");
    return true;
}

bool SystemClass::taskBeforePowerOFF(void){
    // Perform task before power off
    
    return true;
}

bool SystemClass::getSystemInfo(String & info)
{
    try
    {
        JSONVar system;
        system["cpu"]["freq"] = ESP.getCpuFreqMHz()*(1000000);
        system["cpu"]["core"] = ESP.getChipCores();
        system["chip"]["mdl"] = ESP.getChipModel();
        system["chip"]["rev"] = ESP.getChipRevision();
        system["flash"]["freq"] = ESP.getFlashChipSpeed();
        system["flash"]["size"] = ESP.getFlashChipSize();
        system["heap"]["size"] = ESP.getHeapSize();
        system["heap"]["free"] = ESP.getFreeHeap();
        system["disk"]["size"] = FILE_SYSTEM.totalBytes();
        system["disk"]["used"] = FILE_SYSTEM.usedBytes();
        info = JSON.stringify(system);
        return true;
    }
    catch(const std::exception& e)
    {
        return false;
    }
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

