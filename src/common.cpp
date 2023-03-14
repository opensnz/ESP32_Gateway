#include "common.h"
#include "log.h"

#define GET_TIME_TIMEOUT   5000

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
}


void genericPacket(JSONVar & packet){
    packet["rxpk"][0]["time"] = "";
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



time_t getTime(struct tm * timeinfo) {
    uint32_t start = millis();
    time_t now;
    while((millis()-start) <= GET_TIME_TIMEOUT)
    {
        time(&now);
        localtime_r(&now, timeinfo);
        if(timeinfo->tm_year > (2016 - 1900))
        {
            return now;
        }
        delay(10);
    }
    return 0;
}