#ifndef __GATEWAY_H__
#define __GATEWAY_H__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <freertos/queue.h>
#include <Arduino_JSON.h>
#include <time.h>
#include <sntp.h>
#include "device.h"


#define GATEWAY_NTP_SERVER_1           "pool.ntp.org"
#define GATEWAY_NTP_SERVER_2           "time.nist.gov"
#define GATEWAY_GMT_OFFSET_SEC         0
#define GATEWAY_DAY_OFFSET_SEC         0
#define GATEWAY_GPS_START_TIMESTAMP    315964800 // January 6, 1980, 00:00:00 (seconds)
#define GATEWAY_JOIN_REQUEST_FREQUENCY 86400  // 1 day = 86400s  

#define GATEWAY_QUEUE_SIZE             DEVICE_TOTAL

class GatewayClass {

private:

    void tSetup(void);
    void tLoop(void);

    void fSetup(void);
    void fLoop(void);
    
public:

    TaskHandle_t joinTaskHandler;
    SemaphoreHandle_t semaphore;

    void tMain(void);
    void fMain(void);

    bool forward(JSONVar & packet);
    
};

/**************** Exported Global Variables *******************/

extern TaskHandle_t hTGateway;
extern TaskHandle_t hFGateway;
extern GatewayClass Gateway;

void joinTaskEntry(void * parameter);

#endif /* __GATEWAY_H__ */