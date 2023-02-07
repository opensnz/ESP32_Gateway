#include <Arduino.h>
#include <freertos/queue.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "main.h"
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
    WiFi.begin("OPPO", "Fostin2022");
    xTaskCreatePinnedToCore(TransceiverTaskEntry, "transceiverTask",  10000, NULL, 1, &hTransceiver, 0);            
    xTaskCreatePinnedToCore(ForwarderTaskEntry,   "forwarderTask",    10000, NULL, 1, &hForwarder, 1);
    // Wait to Launch transceiverTask and forwarderTask
    delay(100);
    xTaskCreatePinnedToCore(TGatewayTaskEntry, "tGatewayTask",  10000, NULL, 1, &hTGateway, 0);            
    xTaskCreatePinnedToCore(FGatewayTaskEntry, "fGatewayTask",  10000, NULL, 1, &hFGateway, 1);
}

void loop() {
    // put your main code here, to run repeatedly:
    delay(portMAX_DELAY);
}




/*********************** Task Entry ************************/

void TGatewayTaskEntry(void * parameter){
    delay(100);
    Serial.println("TGatewayTaskEntry");
    Gateway.tMain();
}

void FGatewayTaskEntry(void * parameter){
    delay(200);
    Serial.println("FGatewayTaskEntry");
    Gateway.fMain();
}

void TransceiverTaskEntry(void * parameter){
    delay(10);
    Serial.println("TransceiverTaskEntry");
    Transceiver.main();
}

void ForwarderTaskEntry(void * parameter){
    delay(20);
    Serial.println("ForwarderTaskEntry");
    Forwarder.main();
}


