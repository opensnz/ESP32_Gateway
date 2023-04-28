/**
  ******************************************************************************
  * @file    gateway.cpp
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

#include "gateway.h"
#include "common.h"
#include "system.h"
#include "transceiver.h"
#include "forwarder.h"
#include "encoder.h"

TaskHandle_t hTGateway = NULL;
TaskHandle_t hFGateway = NULL;
GatewayClass Gateway;
QueueHandle_t qJGateway = NULL;
QueueHandle_t qDGateway = NULL;

void GatewayClass::tSetup(void){
    RTC.offset = GATEWAY_GMT_OFFSET_SEC;
    RTC.setTime(TIMESTAMP_DEFAULT);
    sntp_set_time_sync_notification_cb(timeAdjustmentNotification);
    sntp_servermode_dhcp(1);
    configTime(GATEWAY_GMT_OFFSET_SEC, GATEWAY_DAY_OFFSET_SEC, GATEWAY_NTP_SERVER_1, GATEWAY_NTP_SERVER_2);
    this->semaphore = xSemaphoreCreateBinary();
    if(this->semaphore == NULL)
    {
        SYSTEM_LOGF_LN("Failed to create semaphore= %p", this->semaphore);
    }else
    {
        // Release semaphore for first use
        xSemaphoreGive(this->semaphore);
    }
    xTaskCreatePinnedToCore(joinTaskEntry, "joinTask", 10000, NULL, TASK_PRIORITY, &joinTaskHandler, 1);
}

void GatewayClass::tLoop(void){
    Transceiver_data_t tData;
    Device_data_t device;
    BaseType_t status = pdFALSE;
    while(true)
    {
        if(qTransceiverToGateway == NULL)
        {
            SYSTEM_PRINT_LN("Waiting for Queue TransceiverToGateway Init");
            delay(100);
            continue;
        }
        status = xQueueReceive(qTransceiverToGateway, &tData, portMAX_DELAY);
        if(status == pdFALSE)
        {
            SYSTEM_LOG_LN("qTransceiverToGateway reception failed");
            continue;
        }
        SYSTEM_PRINT_LN("Device's data received successfully");
        memcpy(device.info.DevEUI, tData.DevEUI, DEVICE_DEV_EUI_SIZE);
        memcpy(device.payload, tData.payload, tData.payloadSize);
        device.payloadSize = tData.payloadSize;
        if(!Device.isDeviceConfigured(device.info))
        {
            SYSTEM_PRINT_LN("Device not configured");
            continue;
        }
        Device.loadDevice(device.info);
        if(device.info.FCnt == DEVICE_FCNT_DEFAULT || device.info.FCnt > DEVICE_FCNT_MAX)
        {
            // JoinRequest needed
            this->joining(device);
        }else
        {
            // UnconfirmedDataUp needed
            this->dataUp(device);
        }
    }
}

void GatewayClass::fSetup(void){
    qJGateway = xQueueCreate(GATEWAY_QUEUE_SIZE, sizeof(uint8_t)*DEVICE_DEV_EUI_SIZE);
    if (qJGateway == NULL)
    {
        SYSTEM_LOGF_LN("Failed to create queue= %p", qJGateway);
    }
    qDGateway = xQueueCreate(GATEWAY_QUEUE_SIZE, sizeof(uint8_t)*DEVICE_DEV_EUI_SIZE);
    if (qDGateway == NULL)
    {
        SYSTEM_LOGF_LN("Failed to create queue= %p", qDGateway);
    }
}
void GatewayClass::fLoop(void){
    Forwarder_data_t fData;
    Device_data_t device;
    BaseType_t status = pdFALSE;
    LoRaWAN_Packet_Type_t type;
    while(true)
    {
        if(qForwarderToGateway == NULL)
        {
            SYSTEM_PRINT_LN("Waiting for Queue ForwarderToGateway Init");
            delay(100);
            continue;
        }
        status = xQueueReceive(qForwarderToGateway, &fData, portMAX_DELAY);
        if(status == pdFALSE)
        {
            continue;
        }
        JSONVar packet = JSON.parse(String(fData.packet, fData.packetSize));
        String PHYPayload = (const char *)packet["txpk"]["data"];
        type = Encoder.packetType(PHYPayload);
        if(type == LORAWAN_JOIN_ACCEPT)
        {
            SYSTEM_LOG("JoinAccept PHYPayload : ");SYSTEM_PRINT_LN(PHYPayload);
            status = xQueueReceive(qJGateway, device.info.DevEUI, portMAX_DELAY);
            if(status == pdFALSE)
            {
                continue;
            }
            Device.loadDevice(device.info);
            device.info.DevNonce--;
            if(!Encoder.joinAccept(device, PHYPayload))
            {
                SYSTEM_LOG_LN("JoinAccept failed");
                continue;
            }
            SYSTEM_LOG_LN("JoinAccept done");
            device.info.DevNonce++;
            device.info.FCnt = DEVICE_FCNT_DEFAULT + 1;
            Device.saveDevice(device.info);
            
        }else if(type == LORAWAN_CONFIRMED_DATA_DOWN || type == LORAWAN_UNCONFIRMED_DATA_DOWN)
        {
        
        }
    }
}




bool GatewayClass::forward(JSONVar & packet){
    // Send Packet to LoRaWAN Server
    if(qGatewayToForwarder == NULL)
    {
        return false;
    }
    Forwarder_data_t fData;
    uint32_t packetSize = JSON.stringify(packet).length();
    memcpy(fData.packet, JSON.stringify(packet).c_str(), packetSize);
    fData.packetSize = packetSize;
    xQueueSend(qGatewayToForwarder, &fData, portMAX_DELAY);
    return true;
}


bool GatewayClass::joining(Device_data_t & device)
{
    JSONVar packet;
    genericPacket(packet);
    packet["rxpk"][0]["time"] = RTC.getTime("%Y-%m-%dT%H:%M:%SZ");
    //packet["rxpk"][0]["tmms"] = (double)(timestamp - GATEWAY_GPS_START_TIMESTAMP) * 1000;
    packet["rxpk"][0]["tmst"] = (uint32_t)RTC.getEpoch();

    SYSTEM_LOG_LN("JoinRequest begins");
    if(Encoder.joinRequest(device, packet))
    {
        device.info.DevNonce += 1;
        Device.saveDevNonce(device.info);
        SYSTEM_LOG_LN("JoinRequest done");
        // Notify JGateway for joinAccept
        if(qJGateway != NULL)
        {
            SYSTEM_PRINT_LN("Notifying JGateway");
            xQueueSend(qJGateway, device.info.DevEUI, portMAX_DELAY);
            // Send JoinRequest
            Gateway.forward(packet);
            return true;
        }
    }
    return false;
}


bool GatewayClass::dataUp(Device_data_t & device, bool confirmed)
{
    JSONVar packet;
    bool status = false;
    genericPacket(packet);
    packet["rxpk"][0]["time"] = RTC.getTime("%Y-%m-%dT%H:%M:%SZ");
    //packet["rxpk"][0]["tmms"] = (double)(timestamp - GATEWAY_GPS_START_TIMESTAMP) * 1000;
    packet["rxpk"][0]["tmst"] = (uint32_t)RTC.getEpoch();

    if(!confirmed)
    {
        status = Encoder.unconfirmedDataUp(device, packet);
    }else
    {
        status = Encoder.confirmedDataUp(device, packet);
    }

    if(status)
    {
        device.info.FCnt += 1;
        Device.saveFCnt(device.info);
        xSemaphoreTake(this->semaphore, portMAX_DELAY);
        xSemaphoreGive(this->semaphore);
        SYSTEM_LOG_LN("DataUp done");
        this->forward(packet);
        return true;
    }else
    {
        SYSTEM_LOG_LN("DataUp failed");
    }
    return false;
}

void GatewayClass::tMain(void){
    SYSTEM_PRINT_LN("Gateway tMain setting...");
    this->tSetup();
    SYSTEM_PRINT_LN("Gateway tMain running...");
    this->tLoop();
}

void GatewayClass::fMain(void){
    SYSTEM_PRINT_LN("Gateway fMain setting...");
    this->fSetup();
    SYSTEM_PRINT_LN("Gateway fMain running...");
    this->fLoop();
}


void joinTaskEntry(void * parameter)
{
    Device_data_t device;
    String path = DEVICE_FILE_CONFIG;
    JSONVar config;
    String content;
    SYSTEM_PRINT_LN("joinTaskEntry");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }
    while(true)
    {
        if(Gateway.semaphore == NULL)
        {
            delay(10);
            SYSTEM_PRINT_LN("Waiting for Gateway Semaphore");
            continue;
        }
        xSemaphoreTake(Gateway.semaphore, portMAX_DELAY);
        if(System.readFile(path, content))
        {
            config = JSON.parse(content);
            uint8_t length = (uint8_t)config.length();
            for(uint8_t i=0; i<length; i++)
            {
                hexStringToArray((const char *)config[i], device.info.DevEUI);
                if(!Device.loadDevice(device.info))
                {
                    continue;
                }
                if(WiFi.status() != WL_CONNECTED)
                {
                    delay(1000);
                    continue;
                }
                Gateway.joining(device);
                delay(1000);
            }
        }
        xSemaphoreGive(Gateway.semaphore);
        delay(GATEWAY_JOIN_REQUEST_FREQUENCY * S_TO_MS_FACTOR);
    }
}


/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/