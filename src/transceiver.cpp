/**
  ******************************************************************************
  * @file    transceiver.cpp
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

#include "transceiver.h"
#include "common.h"
#include "system.h"
#include "lorawan.h"

QueueHandle_t qTransceiverToGateway = NULL;
TaskHandle_t  hTransceiver = NULL;
TransceiverClass Transceiver = TransceiverClass(LORA_FREQUENCY_DEFAULT);

void TransceiverClass::setup(void){
    vTaskPrioritySet(hTransceiver, TASK_PRIORITY+1);
    qTransceiverToGateway = xQueueCreate(TRANSCEIVER_QUEUE_SIZE, sizeof(Transceiver_data_t));
    if (qTransceiverToGateway == NULL){
        SYSTEM_LOGF("Failed to create queue= %p\n", qTransceiverToGateway);
    }

    LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);
    if (!LoRa.begin(LORA_FREQUENCY_DEFAULT)) {
        SYSTEM_LOG("Starting LoRa failed!");
        while(1);
    }
    LoRa.onReceive(onReceiveLoRaNotification);
    LoRa.receive();
}


void TransceiverClass::loop(void){
    Transceiver_data_t tData;
    uint32_t packetSize;
    String path;
    while(true)
    {

        xTaskNotifyWait(0x00,            /* Don't clear any notification bits on entry. */
                        ULONG_MAX,       /* Reset the notification value to 0 on exit. */
                        &packetSize,     /* Notified value pass out in packetSize. */
                        portMAX_DELAY ); /* Block indefinitely. */
        if(packetSize <= DEVICE_DEV_EUI_SIZE + 4)
        {
            continue;
        }
        tData.rssi = LoRa.packetRssi();
        tData.snr = LoRa.packetSnr();
        char payload[packetSize];
        memset(payload, 0x00, packetSize);
        // read packet
        LoRa.read(); // NetID
        LoRa.read(); // MsgID
        for (int i = 0; i < packetSize-3; i++) {
            payload[i] = (char)LoRa.read();
        }
        LoRa.read(); // EndMsg
        SYSTEM_PRINT_LN(payload);
        if(packetSize <= TRANSCEIVER_DEV_EUI_SIZE+3)
        {
            SYSTEM_PRINT_LN("Incorrect Frame");
            continue;
        }
        tData.payloadSize = packetSize-3;
        memcpy(tData.DevEUI, payload, DEVICE_DEV_EUI_SIZE);
        memcpy(tData.payload, &payload[DEVICE_DEV_EUI_SIZE], tData.payloadSize-DEVICE_DEV_EUI_SIZE);
        arrayToHexString(tData.DevEUI, DEVICE_DEV_EUI_SIZE, path);
        path = String("/") + path + DEVICE_FILE_INFO_EXT;
        if(!System.exists(path))
        {
            SYSTEM_PRINT_LN("Device not configured");
            continue;
        }
        printTransceiverData(&tData);
        if(qTransceiverToGateway != NULL)
        {
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

bool TransceiverClass::isBase64(const char * data)
{
    uint8_t i = 0;
    while(data[i] != '\0')
    {
        if(data[i] >= '0' && data[i] <= '9')
        {
            // 0 - 9
        }else if(data[i] >= 'A' && data[i] <= 'Z')
        {
            // A - Z
        }
        else if(data[i] >= 'a' && data[i] <= 'z')
        {
            // a - z
        }
        else if(data[i] == '/' || data[i] == '=' || data[i] == '+')
        {
            // Special characters 
        }
        else
        {
            return false;
        }
        i++;
    }
    return true;
}



/*********************** Global Function Implementations ************************/

void onReceiveLoRaNotification(int packetSize){
    xTaskNotify(hTransceiver, packetSize, eNotifyAction::eSetValueWithOverwrite);
}


void printTransceiverData(Transceiver_data_t *tData)
{
    SYSTEM_PRINT_LN("################ Transceiver Data ################");
    SYSTEM_LOG ("DevEUI = ");
    for(int i=0; i<DEVICE_DEV_EUI_SIZE; i++){
        SYSTEM_PRINTF("%02X", tData->DevEUI[i]);
    }
    SYSTEM_PRINT_LN();
    SYSTEM_LOGF_LN("RSSI   = %d", tData->rssi);
    SYSTEM_LOGF_LN("SNR    = %f", tData->snr);
    SYSTEM_LOGF_LN("SIZE   = %d", tData->payloadSize);
    SYSTEM_LOG("DATA   = ");
    for(int i=0; i<tData->payloadSize; i++){
        SYSTEM_PRINTF("%02X", tData->payload[i]);
    }
    SYSTEM_PRINT_LN();
    SYSTEM_PRINT_LN("##################################################");
}


/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/