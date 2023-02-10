#ifndef __GATEWAY_H__
#define __GATEWAY_H__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <freertos/queue.h>
#include <Arduino_JSON.h>
#include <time.h>
#include <sntp.h>
#include "device.h"
#include "transceiver.h"
#include "forwarder.h"
#include "encoder.h"


#define GATEWAY_NTP_SERVER_1     "pool.ntp.org"
#define GATEWAY_NTP_SERVER_2     "time.nist.gov"
#define GATEWAY_GMT_OFFSET_SEC   0
#define GATEWAY_DAY_OFFSET_SEC   0
#define GATEWAY_GPS_START_TIMESTAMP 315964800 // January 6, 1980, 00:00:00 (seconds)



class GatewayClass {

private:

    void tSetup(void);
    void tLoop(void);

    void fSetup(void);
    void fLoop(void);

    bool joinRequestPacket(Device_data_t & device, JSONVar & packet);
    bool joinAcceptPacket(Device_data_t & device, JSONVar & packet);
    bool unconfirmedDataUpPacket(Device_data_t & device, JSONVar & packet);
    bool unconfirmedDataDownPacket(Device_data_t & device, JSONVar & packet);
    bool confirmedDataUpPacket(Device_data_t & device, JSONVar & packet);
    bool confirmedDataDownPacket(Device_data_t & device, JSONVar & packet);
    
public:
    GatewayClass();
    Device_data_t device;
    void tMain(void);
    void fMain(void);

};

/**************** Exported Global Variables *******************/

extern GatewayClass Gateway;



/*************** Global Function Prototypes *******************/

uint32_t hexStringToArray(const char * hexString, uint8_t *pArray);

uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, char * hexString);

void timeAdjustmentNotification(struct timeval *t);

void genericPacket(JSONVar & packet);

time_t getTime(struct tm * timeinfo);

#endif /* __GATEWAY_H__ */