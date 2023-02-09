#include "transceiver.h"

QueueHandle_t qTransceiverToGateway = NULL;
TransceiverClass Transceiver = TransceiverClass(LORA_FREQUENCY_DEFAULT);

void TransceiverClass::setup(void){
    qTransceiverToGateway = xQueueCreate(TRANSCEIVER_QUEUE_SIZE, sizeof(Transceiver_data_t));
    if (qTransceiverToGateway == NULL){
        APP_LOGF("Failed to create queue= %p\n", qTransceiverToGateway);
    }
    LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);
    if (!LoRa.begin(868E6)) {
        APP_LOG("Starting LoRa failed!");
        while (1);
    }
    LoRa.onReceive(onReceiveLoRaNotification);
}


void TransceiverClass::loop(void){
    Transceiver_data_t tData;
    while(true)
    {
        delay(5000);
        tData.rssi = 4;
        tData.snr = 3;
        tData.payloadSize = 200;
        // read packet
        for (int i = 0; i < 200; i++) {
            tData.payload[i] = i;
        }
        if(qTransceiverToGateway != NULL){
            xQueueSend(qTransceiverToGateway, &tData, portMAX_DELAY);
        }
    }
}

TransceiverClass::TransceiverClass(uint32_t freq){
    this->freq = freq;
}
void TransceiverClass::main(void){
    APP_PRINT_LN("TransceiverClass setting...");
    this->setup();
    APP_PRINT_LN("TransceiverClass running...");
    this->loop();
}



/*********************** Global Function Implementations ************************/

void onReceiveLoRaNotification(int packetSize){
    Transceiver_data_t tData;
    tData.rssi = LoRa.packetRssi();
    tData.snr = LoRa.packetSnr();
    tData.payloadSize = packetSize;
    // read packet
    for (int i = 0; i < packetSize; i++) {
        tData.payload[i] = LoRa.read();
    }
    //printTransceiverData(&tData);
    if(qTransceiverToGateway != NULL){
        xQueueSend(qTransceiverToGateway, &tData, (TickType_t)0);
    }
}


void printTransceiverData(Transceiver_data_t *tData)
{
    APP_PRINT_LN("\n################ Transceiver Data ################");
    APP_LOG ("DevEUI = ");
    for(int i=0; i<4; i++){
        APP_PRINTF("%02X", tData->DevEUI[i]);
    }
    APP_PRINT("\n");
    APP_LOGF_LN("RSSI   = %d", tData->rssi);
    APP_LOGF_LN("SNR    = %d", tData->snr);
    APP_LOGF_LN("SIZE   = %d", tData->payloadSize);
    APP_LOG("DATA   = ");
    for(int i=0; i<tData->payloadSize; i++){
        APP_PRINTF("%02X", tData->payload[i]);
    }
    APP_PRINT_LN("\n##################################################");
}
