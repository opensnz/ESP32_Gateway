#include "transceiver.h"

QueueHandle_t qTransceiverToGateway;
TransceiverClass Transceiver = TransceiverClass(LORA_FREQUENCY_DEFAULT);

void TransceiverClass::setup(void){
    qTransceiverToGateway = xQueueCreate(TRANSCEIVER_QUEUE_SIZE, sizeof(Transceiver_data_t));
    if (qTransceiverToGateway == NULL){
        printf("Failed to create queue= %p\n", qTransceiverToGateway);
    }
    LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);
    if (!LoRa.begin(868E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    LoRa.onReceive(onReceiveLoRa);
}


void TransceiverClass::loop(void){
    while(true)
    {
        Transceiver_data_t tData;
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
        delay(5000);
    }
}

TransceiverClass::TransceiverClass(uint32_t freq){
    this->freq = freq;
}
void TransceiverClass::main(void){
    printf("TransceiverClass setting...\n");
    this->setup();
    printf("TransceiverClass running...\n");
    this->loop();
}



/*********************** Global Function Implementations ************************/

void onReceiveLoRa(int packetSize){
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
    Serial.println("\n################ TransceiverClass Data ################");
    Serial.print ("DevEUI = ");
    for(int i=0; i<4; i++){
        Serial.printf("%02X", tData->DevEUI[i]);
    }
    Serial.println();
    Serial.printf("RSSI   = %d\n", tData->rssi);
    Serial.printf("SNR    = %d\n", tData->snr);
    Serial.printf("SIZE   = %d\n", tData->payloadSize);
    Serial.print ("DATA   = ");
    for(int i=0; i<tData->payloadSize; i++){
        Serial.printf("%02X", tData->payload[i]);
    }
    Serial.println("\n##################################################");
}
