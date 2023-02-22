#ifndef __COMMON_H__
#define __COMMON_H__


#include <Arduino.h>
#include <Arduino_JSON.h>

uint32_t hexStringToArray(const String & hexString, uint8_t *pArray);

uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, String & hexString);

void timeAdjustmentNotification(struct timeval *t);

void genericPacket(JSONVar & packet);

time_t getTime(struct tm * timeinfo);



#endif /* __COMMON_H__ */