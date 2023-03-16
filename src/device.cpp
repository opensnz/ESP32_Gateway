/**
  ******************************************************************************
  * @file    device.cpp
  * @author  OpenSnz IoT Team
  * @version 1.0
  ******************************************************************************
  * @attention
  * 
    Copyright (C) 2023 OpenSnz Technology - All Rights Reserved.

    THE CONTENTS OF THIS PROJECT ARE PROPRIETARY AND CONFIDENTIAL.
    UNAUTHORIZED COPYING, TRANSFERRING OR REPRODUCTION OF THE CONTENTS OF THIS PROJECT, VIA ANY MEDIUM IS STRICTLY PROHIBITED.

    The receipt or possession of the source code and/or any parts thereof does not convey or imply any right to use them
    for any purpose other than the purpose for which they were provided to you.

    The software is provided "AS IS", without warranty of any kind, express or implied, including but not limited to
    the warranties of merchantability, fitness for a particular purpose and non infringement.
    In no event shall the authors or copyright holders be liable for any claim, damages or other liability,
    whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software
    or the use or other dealings in the software.

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  *
  ******************************************************************************
  */ 

#include "device.h"
#include <Arduino_JSON.h>
#include "common.h"
#include "system.h"
#include "log.h"


DeviceClass Device;

bool DeviceClass::isDeviceConfigured(Device_info_t & info){
    String path;
    arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_INFO_EXT;
    return System.exists(path);
}

bool DeviceClass::addDevice(Device_info_t & info){
    String DevEUI;
    arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, DevEUI);
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
                if(this->saveDevice(info))
                {
                    config[length] = DevEUI;
                    content = JSON.stringify(config);
                    return System.writeFile(path, content);
                }
            }
        }
    }
    return false;
}

bool DeviceClass::removeDevice(Device_info_t & info)
{
    if(this->isDeviceConfigured(info))
    {
        String DevEUI, path;
        arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, DevEUI);
        // Remove files
        path = String("/") + DevEUI + DEVICE_FILE_INFO_EXT;
        System.removeFile(path);
        // Reconfigure config file
        path = DEVICE_FILE_CONFIG;
        String content;
        if(System.readFile(path, content))
        {
            String conf = "[]";
            JSONVar config, new_config = JSON.parse(conf);
            config = JSON.parse(content);
            uint8_t length = (uint8_t)config.length();
            uint8_t count = 0;
            for(uint8_t i=0; i<length; i++)
            {
                if(strcmp((const char *)config[i], DevEUI.c_str()) != 0)
                {
                    new_config[count++] = (const char *)config[i];
                }
            }
            content = JSON.stringify(new_config);
            return System.writeFile(path, content);
        }
    }

    return false;
}

bool DeviceClass::loadDevice(Device_info_t & info){
    String path;
    arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_INFO_EXT;
    String content;
    return System.readFile(path, (uint8_t *)(&info), DEVICE_FILE_INFO_SIZE);
}
bool DeviceClass::saveDevice(Device_info_t & info)
{
    String path;
    arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_INFO_EXT;
    return System.writeFile(path, (uint8_t *)(&info), DEVICE_FILE_INFO_SIZE);
}
bool DeviceClass::saveFCnt(Device_info_t & info)
{
    String path;
    arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_INFO_EXT;
    return System.writeFile(path, (uint8_t *)(&info), DEVICE_FILE_INFO_SIZE, DEVICE_FILE_FCNT_POS);
}
bool DeviceClass::saveDevNonce(Device_info_t & info)
{
    String path;
    arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, path);
    path = String("/") + path + DEVICE_FILE_INFO_EXT;
    return System.writeFile(path, (uint8_t *)(&info), DEVICE_FILE_INFO_SIZE, DEVICE_FILE_DEVNONCE_POS);
}




/********************* Global Function Implementations **********************/


void printDeviceData(Device_data_t &device){

    SYSTEM_PRINT_LN("\n################ Device Data ################");
    SYSTEM_LOG ("DevEUI = ");
    for(int i=0; i<DEVICE_DEV_EUI_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.info.DevEUI[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOG ("AppEUI = ");
    for(int i=0; i<DEVICE_APP_EUI_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.info.AppEUI[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOGF_LN("DevAddr = %04X", device.info.DevAddr);
    SYSTEM_LOG ("NwkSKey = ");
    for(int i=0; i<DEVICE_NWK_SKEY_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.info.NwkSKey[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOG ("AppSKey = ");
    for(int i=0; i<DEVICE_APP_SKEY_SIZE; i++){
        SYSTEM_PRINTF("%02X", device.info.AppSKey[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOGF_LN("DevNonce = %d", device.info.DevNonce);
    SYSTEM_LOGF_LN("FPort = %d", device.info.FPort);
    SYSTEM_LOGF_LN("FCnt = %d", device.info.FCnt);
    SYSTEM_LOGF_LN("SIZE   = %d", device.payloadSize);
    SYSTEM_LOG("DATA   = ");
    for(int i=0; i<device.payloadSize; i++){
        SYSTEM_PRINTF("%02X", device.payload[i]);
    }
    SYSTEM_PRINT_LN("\n##################################################");

}

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/