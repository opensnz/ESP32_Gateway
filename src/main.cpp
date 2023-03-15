/**
  ******************************************************************************
  * @file    main.cpp
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

#include "main.h"
#include "system.h"
#include "web.h"
#include "common.h"
#include "gateway.h"
#include "transceiver.h"
#include "forwarder.h"

void setup() {
    while(!System.begin())
    {
        // System not started, retry after 10ms
        delay(10);
    }
    // System started fine
    // Start Web Server
    Web.begin();
    delay(100);

    // Start other Tasks

    xTaskCreatePinnedToCore(ForwarderTaskEntry,   "forwarderTask",    10000, NULL, TASK_PRIORITY, &hForwarder, 1);
    delay(100);           
    xTaskCreatePinnedToCore(FGatewayTaskEntry, "fGatewayTask", 10000, NULL, TASK_PRIORITY, &hFGateway, 1);
    delay(100);
    xTaskCreatePinnedToCore(TGatewayTaskEntry, "tGatewayTask", 10000, NULL, TASK_PRIORITY, &hTGateway, 0);
    delay(100);
    xTaskCreatePinnedToCore(TransceiverTaskEntry, "transceiverTask",  10000, NULL, TASK_PRIORITY, &hTransceiver, 0);
    
}

void loop() {
    // Sleep indefinitely
    delay(portMAX_DELAY);
}




/*********************** Task Entry ************************/

void TGatewayTaskEntry(void * parameter){
    SYSTEM_PRINT_LN("TGatewayTaskEntry");
    Gateway.tMain();
}

void FGatewayTaskEntry(void * parameter){
    SYSTEM_PRINT_LN("FGatewayTaskEntry");
    Gateway.fMain();
}

void TransceiverTaskEntry(void * parameter){
    SYSTEM_PRINT_LN("TransceiverTaskEntry");
    Transceiver.main();
}

void ForwarderTaskEntry(void * parameter){
    SYSTEM_PRINT_LN("ForwarderTaskEntry");
    Forwarder.main();
}


/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/