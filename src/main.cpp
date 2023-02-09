#include <Arduino.h>
#include <freertos/queue.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "main.h"
#include "Storage.h"
#include "web.h"
#include "gateway.h"
#include "transceiver.h"
#include "forwarder.h"

TaskHandle_t hTransceiver = NULL;
TaskHandle_t hForwarder   = NULL;
TaskHandle_t hTGateway    = NULL;
TaskHandle_t hFGateway    = NULL;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    while(!Storage.begin())
    {
        delay(10);
    }
    Web.begin();
    delay(100);
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
    APP_PRINT_LN("TGatewayTaskEntry");
    Gateway.tMain();
}

void FGatewayTaskEntry(void * parameter){
    APP_PRINT_LN("FGatewayTaskEntry");
    Gateway.fMain();
}

void TransceiverTaskEntry(void * parameter){
    APP_PRINT_LN("TransceiverTaskEntry");
    Transceiver.main();
}

void ForwarderTaskEntry(void * parameter){
    APP_PRINT_LN("ForwarderTaskEntry");
    Forwarder.main();
}


