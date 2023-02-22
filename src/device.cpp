#include "device.h"
#include <Arduino_JSON.h>
#include "common.h"
#include "system.h"
#include "log.h"


DeviceClass Device;

bool DeviceClass::isDeviceConfigured(Device_data_t & device){
    String path;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_KEYS_EXT;
    return System.exists(path);
}

bool DeviceClass::addDevice(Device_data_t & device){
    String DevEUI;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, DevEUI);
    String path = DEVICE_FILE_CONFIG;
    JSONVar config;
    String content;
    if(System.readFile(path, content))
    {
        config = JSON.parse(content);
        uint8_t length = (uint8_t)config.length();
        if(length < DEVICE_TOTAL)
        {
            bool isConfigured = false;
            for(uint8_t i=0; i<length; i++)
            {
                if(strcmp((const char *)config[i], DevEUI.c_str()) == 0)
                {
                    isConfigured = true;
                    break;
                }
            }
            if(!isConfigured)
            {
                // Add Device's DevEUI to config file
                if(!this->saveKeys(device))
                {
                    return false;
                }
                if(!this->saveDevNonce(device))
                {
                    return false;
                }
                if(!this->saveFCnt(device))
                {
                    return false;
                }
                config[length] = DevEUI;
                content = JSON.stringify(config);
                return System.writeFile(path, content);
            }
        }
    }
    return false;
}
bool DeviceClass::loadDevice(Device_data_t & device){
    String DevEUI;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, DevEUI);
    String path = String("/") + DevEUI + DEVICE_FILE_KEYS_EXT;
    String content;
    if(System.readFile(path, content))
    {
        JSONVar keys = JSON.parse(content);
        hexStringToArray((const char *)keys["AppEUI"], device.AppEUI);
        hexStringToArray((const char *)keys["AppKey"], device.AppKey);
        hexStringToArray((const char *)keys["DevAddr"], device.DevAddr);
        hexStringToArray((const char *)keys["NwkSKey"], device.NwkSKey);
        hexStringToArray((const char *)keys["AppSKey"], device.AppSKey);
        device.FPort = (uint8_t)keys["FPort"];

        path = String("/") + DevEUI + DEVICE_FILE_FCNT_EXT;
        if(System.readFile(path, content))
        {
            device.FCnt = (uint16_t)atoi(content.c_str());
            path = String("/") + DevEUI + DEVICE_FILE_DEVNONCE_EXT;
            if(System.readFile(path, content))
            {
                device.DevNonce = (uint16_t)atoi(content.c_str());
                return true;
            }
        }
    }
    return false;
}
bool DeviceClass::saveFCnt(Device_data_t & device){
    String path;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_FCNT_EXT;
    String content = String(device.FCnt);
    if(System.writeFile(path, content))
    {
        return true;
    }
    return false;
}
bool DeviceClass::saveDevNonce(Device_data_t & device){
    String path;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_DEVNONCE_EXT;
    String content = String(device.DevNonce);
    if(System.writeFile(path, content))
    {
        return true;
    }
    return false;
}
bool DeviceClass::saveKeys(Device_data_t & device){
    String hexData;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, hexData);
    String path = String("/") + hexData + DEVICE_FILE_KEYS_EXT;
    String content;
    JSONVar contentJSON;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, hexData);
    contentJSON["DevEUI"] = hexData;
    arrayToHexString(device.AppEUI, DEVICE_APP_EUI_SIZE, hexData);
    contentJSON["AppEUI"] = hexData;
    arrayToHexString(device.AppKey, DEVICE_APP_KEY_SIZE, hexData);
    contentJSON["AppKey"] = hexData;
    arrayToHexString(device.DevAddr, DEVICE_DEV_ADDR_SIZE, hexData);
    contentJSON["DevAddr"] = hexData;
    arrayToHexString(device.NwkSKey, DEVICE_NWK_SKEY_SIZE, hexData);
    contentJSON["NwkSKey"] = hexData;
    arrayToHexString(device.AppSKey, DEVICE_APP_SKEY_SIZE, hexData);
    contentJSON["AppSKey"] = hexData;
    contentJSON["FPort"] = device.FPort;
    content = JSON.stringify(contentJSON);
    if(System.writeFile(path, content))
    {
        return true;
    }
    return false;
}
bool DeviceClass::savePacket(Device_data_t & device, String & packet){
    String path;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_PACKET_EXT;
    return System.writeFile(path, packet);
}
bool DeviceClass::loadPacket(Device_data_t & device, String & packet){
    String path;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_PACKET_EXT;
    return System.readFile(path, packet);
}
bool DeviceClass::removePacket(Device_data_t & device){
    String path;
    arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_PACKET_EXT;
    return System.removeFile(path);
}


/********************* Global Function Implementations **********************/


void printDeviceData(Device_data_t &device){

    SYSTEM_PRINT_LN("\n################ Device Data ################");
    SYSTEM_LOG ("DevEUI = ");
    for(int i=0; i<DEVICE_DEV_EUI_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.DevEUI[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOG ("AppEUI = ");
    for(int i=0; i<DEVICE_APP_EUI_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.AppEUI[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOG ("DevAddr = ");
    for(int i=0; i<DEVICE_DEV_ADDR_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.DevAddr[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOG ("NwkSKey = ");
    for(int i=0; i<DEVICE_NWK_SKEY_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.NwkSKey[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOG ("AppSKey = ");
    for(int i=0; i<DEVICE_APP_SKEY_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.AppSKey[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOGF_LN("DevNonce = %d", device.DevNonce);
    SYSTEM_LOGF_LN("FPort = %d", device.FPort);
    SYSTEM_LOGF_LN("FCnt = %d", device.FCnt);
    SYSTEM_LOGF_LN("SIZE   = %d", device.payloadSize);
    SYSTEM_LOG("DATA   = ");
    for(int i=0; i<device.payloadSize; i++){
        SYSTEM_PRINTF("%02X", device.payload[i]);
    }
    SYSTEM_PRINT_LN("\n##################################################");

}