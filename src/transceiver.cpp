#include "transceiver.h"

QueueHandle_t qTransceiverToGateway = NULL;
TransceiverClass Transceiver = TransceiverClass(LORA_FREQUENCY_DEFAULT);

void TransceiverClass::setup(void){
    qTransceiverToGateway = xQueueCreate(TRANSCEIVER_QUEUE_SIZE, sizeof(Transceiver_data_t));
    if (qTransceiverToGateway == NULL){
        SYSTEM_LOGF("Failed to create queue= %p\n", qTransceiverToGateway);
    }
    LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);
    if (!LoRa.begin(868E6)) {
        SYSTEM_LOG("Starting LoRa failed!");
        while (1);
    }
    LoRa.onReceive(onReceiveLoRaNotification);
}


void TransceiverClass::loop(void){
    Transceiver_data_t tData;
    while(true)
    {
        delay(15000);
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
    SYSTEM_PRINT_LN("Transceiver setting...");
    this->setup();
    SYSTEM_PRINT_LN("Transceiver running...");
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
    SYSTEM_PRINT_LN("\n################ Transceiver Data ################");
    SYSTEM_LOG ("DevEUI = ");
    for(int i=0; i<4; i++){
        SYSTEM_PRINTF("%02X", tData->DevEUI[i]);
    }
    SYSTEM_PRINT("\n");
    SYSTEM_LOGF_LN("RSSI   = %d", tData->rssi);
    SYSTEM_LOGF_LN("SNR    = %d", tData->snr);
    SYSTEM_LOGF_LN("SIZE   = %d", tData->payloadSize);
    SYSTEM_LOG("DATA   = ");
    for(int i=0; i<tData->payloadSize; i++){
        SYSTEM_PRINTF("%02X", tData->payload[i]);
    }
    SYSTEM_PRINT_LN("\n##################################################");
}
