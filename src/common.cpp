/**
  ******************************************************************************
  * @file    common.cpp
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

#include "common.h"
#include "log.h"

#define GET_TIME_TIMEOUT   5000
ESP32Time RTC;

/********************* Global Function Implementations **********************/

uint32_t hexStringToArray(const String & hexString, uint8_t *pArray){
    char hex2Bytes[3]  = "FF";
    uint32_t size = hexString.length()/2;
    for (int i = 0; i < size; i += 1) 
    {
        hex2Bytes[0] = hexString[2*i];
        hex2Bytes[1] = hexString[2*i+1];
        pArray[i] = (uint8_t)strtol(hex2Bytes, NULL, 16);
    }
    return size;
}

uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, String & hexString){
    char hexBytes[3]  = "FF";
    hexString = "";
    for (int i = 0; i < size; i += 1)
    {
        sprintf(hexBytes, "%.2x", pArray[i]);
        hexString += hexBytes;
    }
    return 2*size+1;
}


void timeAdjustmentNotification(struct timeval * t){
    SYSTEM_PRINT_LN("Got time adjustment from NTP!");
    RTC.setTime(t->tv_sec, t->tv_usec); 
}


void genericPacket(JSONVar & packet){
    packet["rxpk"][0]["time"] = "1970-01-01T00:00:00Z";
    //packet["rxpk"][0]["tmms"] = 0;
    packet["rxpk"][0]["tmst"] = 0;
    packet["rxpk"][0]["chan"] = 1;
    packet["rxpk"][0]["rfch"] = 0;
    packet["rxpk"][0]["freq"] = 868.3;
    packet["rxpk"][0]["stat"] = 1;
    packet["rxpk"][0]["modu"] = "LORA";
    packet["rxpk"][0]["datr"] = "SF7BW125";
    packet["rxpk"][0]["codr"] = "4/5";
    packet["rxpk"][0]["rssi"] = -35;
    packet["rxpk"][0]["lsnr"] = 5.0;
    packet["rxpk"][0]["size"] = 23;
    packet["rxpk"][0]["data"] = "";
}


uint32_t parseUInt32LittleEndian(const uint8_t *bytes) {
    return (((uint32_t) bytes[0]) << 0u) | (((uint32_t) bytes[1]) << 8u) | (((uint32_t) bytes[2]) << 16u) | (((uint32_t) bytes[3]) << 24u);
}


/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/