/**
  ******************************************************************************
  * @file    gateway.h
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
    bool joining(Device_data_t & device);
    bool dataUp(Device_data_t & device, bool confirmed=false);
    
};

/**************** Exported Global Variables *******************/

extern TaskHandle_t hTGateway;
extern TaskHandle_t hFGateway;
extern GatewayClass Gateway;

void joinTaskEntry(void * parameter);

#endif /* __GATEWAY_H__ */

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/