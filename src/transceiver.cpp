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
TransceiverClass Transceiver = TransceiverClass();

bool TransceiverClass::loadConfig(void){
    String content;
    JSONVar config;
    if(System.readFile(TRANSCEIVER_GATEWAY_FILE, content))
    {
        config = JSON.parse(content);
        this->freq = (uint32_t)config["freq"];
        this->bw = (uint32_t)config["bw"];
        this->sf = (uint32_t)config["sf"];
        return true;
    }
    return false;
}

void TransceiverClass::setup(void){
    vTaskPrioritySet(hTransceiver, TASK_PRIORITY+1);
    qTransceiverToGateway = xQueueCreate(TRANSCEIVER_QUEUE_SIZE, sizeof(Transceiver_data_t));
    if (qTransceiverToGateway == NULL){
        SYSTEM_LOGF("Failed to create queue= %p\n", qTransceiverToGateway);
    }
    while(!this->loadConfig())
    {
        SYSTEM_LOGF_LN("Loading Config Info failed");
        delay(100);
    }
    LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);

    while(!LoRa.begin(this->freq))
    {
        SYSTEM_LOG("Starting LoRa failed!");
        delay(100);
    }
    LoRa.setSignalBandwidth(this->bw);
    LoRa.setSpreadingFactor(this->sf);
    LoRa.onReceive(onReceiveLoRaNotification);
    // Put Transceiver to receive mode
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
#if TRANSCEIVER_DORJI_FRAME_FORMAT
        if(packetSize <= DEVICE_DEV_EUI_SIZE + TRANSCEIVER_DORJI_FRAME_MIN_SIZE)
        {
            SYSTEM_PRINT_LN("Incorrect Frame");
            continue;
        }
#else
        if(packetSize <= DEVICE_DEV_EUI_SIZE)
        {
            SYSTEM_PRINT_LN("Incorrect Frame");
            continue;
        }
#endif
        tData.rssi = LoRa.packetRssi();
        tData.snr = LoRa.packetSnr();
        char payload[packetSize];
        memset(payload, 0x00, packetSize);
#if TRANSCEIVER_DORJI_FRAME_FORMAT
        tData.payloadSize = packetSize-TRANSCEIVER_DORJI_FRAME_MIN_SIZE;
        LoRa.read(); // NetID
        LoRa.read(); // MsgID
        for (int i = 0; i < tData.payloadSize; i++) {
            payload[i] = (char)LoRa.read();
        }
        LoRa.read(); // EndMsg
#else
        tData.payloadSize = packetSize;
        for (int i = 0; i < tData.payloadSize; i++) {
            payload[i] = (char)LoRa.read();
        }
#endif
        SYSTEM_PRINT_LN(payload);
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

int TransceiverClass::sum(String & message)
{
    int response = 0;
    uint8_t length = message.length();
    for (uint8_t i = 0; i < length; i++)
    {
        response += (uint8_t)message[i];
    }
    return response;
}

int TransceiverClass::normalize(int sum)
{
    if (sum > 288)
    {
        do
        {
            sum = sum - 256;
        } while (sum > 288);
    }
    return sum;
}

uint8_t TransceiverClass::getMessageID(String & message)
{
    int normalizedSum = this->normalize(this->sum(message));
    if (normalizedSum <= 32)
    {
        return (32 - normalizedSum);
    }
    else
    {
        return (288 - normalizedSum);
    }
}

TransceiverClass::TransceiverClass(){
}
void TransceiverClass::main(void){
    SYSTEM_PRINT_LN("Transceiver setting...");
    this->setup();
    SYSTEM_PRINT_LN("Transceiver running...");
    this->loop();
}

void TransceiverClass::transmit(Device_data_t & device){
    LoRa.beginPacket();
#if TRANSCEIVER_DORJI_FRAME_FORMAT
    LoRa.write(TRANSCEIVER_DORJI_FRAME_NET_ID);
    LoRa.write(device.info.DevEUI, DEVICE_DEV_EUI_SIZE);
    LoRa.write(device.payload, device.payloadSize);
    String message = String(device.info.DevEUI, DEVICE_DEV_EUI_SIZE) + 
                     String(device.payload, device.payloadSize);
    uint8_t messageID = this->getMessageID(message);
    LoRa.write(messageID);
#else
    LoRa.write(device.info.DevEUI, DEVICE_DEV_EUI_SIZE);
    LoRa.write(device.payload, device.payloadSize);
#endif
    LoRa.endPacket();
    // Put Transceiver to receive mode
    LoRa.receive();
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