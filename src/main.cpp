#include "main.h"
#include "system.h"
#include "web.h"
#include "gateway.h"
#include "transceiver.h"
#include "forwarder.h"

TaskHandle_t hTransceiver = NULL;
TaskHandle_t hForwarder   = NULL;
TaskHandle_t hTGateway    = NULL;
TaskHandle_t hFGateway    = NULL;

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
    xTaskCreatePinnedToCore(TGatewayTaskEntry, "tGatewayTask",  10000, NULL, 1, &hTGateway, 0);
    delay(100);          
    xTaskCreatePinnedToCore(FGatewayTaskEntry, "fGatewayTask",  10000, NULL, 1, &hFGateway, 1);
    delay(100);
    xTaskCreatePinnedToCore(TransceiverTaskEntry, "transceiverTask",  10000, NULL, 1, &hTransceiver, 0);
    delay(100);           
    xTaskCreatePinnedToCore(ForwarderTaskEntry,   "forwarderTask",    10000, NULL, 1, &hForwarder, 1);
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


