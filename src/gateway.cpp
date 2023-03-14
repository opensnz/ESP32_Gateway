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
    sntp_set_time_sync_notification_cb(timeAdjustmentNotification);
    sntp_servermode_dhcp(1);
    configTime(GATEWAY_GMT_OFFSET_SEC, GATEWAY_DAY_OFFSET_SEC, GATEWAY_NTP_SERVER_1, GATEWAY_NTP_SERVER_2);
    xTaskCreatePinnedToCore(joinTaskEntry, "joinTask", 10000, NULL, 1, &joinTaskHandler, 0);
    this->semaphore = xSemaphoreCreateBinary();
    if(this->semaphore == NULL)
    {
        SYSTEM_LOGF_LN("Failed to create semaphore= %p", this->semaphore);
    }
}

void GatewayClass::tLoop(void){
    Transceiver_data_t tData;
    Device_data_t device;
    BaseType_t status = pdFALSE;
    struct tm receivedTimeInfo;
    time_t timestamp; // overflow at 19 January 2038, at 03:14:07 UTC (end of time)
    JSONVar packet;
    char datetime[32];
    while(true){
        if(qTransceiverToGateway == NULL)
        {
            SYSTEM_PRINT_LN("Waiting for Queue TransceiverToGateway Init");
            delay(100);
        }else
        {
            receiveLabel : 
            SYSTEM_PRINT_LN("Welcome to receiveLabel");
            status = xQueueReceive(qTransceiverToGateway, &tData, portMAX_DELAY);
            if(status == pdTRUE)
            {
                timestamp = getTime(&receivedTimeInfo);
                SYSTEM_PRINT_LN("Device's data received successfully");
                memcpy(device.info.DevEUI, tData.DevEUI, DEVICE_DEV_EUI_SIZE);
                memcpy(device.payload, tData.payload, tData.payloadSize);
                device.payloadSize = tData.payloadSize;
                if(!Device.isDeviceConfigured(device.info))
                {
                    // device not configured (go to receiveLabel)
                    SYSTEM_PRINT_LN("Device not configured (go to receiveLabel)");
                    goto receiveLabel;
                }
                Device.loadDevice(device.info);
                if(device.info.FCnt == DEVICE_FCNT_DEFAULT || device.info.FCnt > DEVICE_FCNT_MAX)
                {
                    // JoinRequest needed
                }else
                {
                    // UnconfirmedDataUp needed
                    genericPacket(packet);
                    if(timestamp > 0)
                    {
                        strftime(datetime, 32, "%Y-%m-%dT%H:%M:%SZ", &receivedTimeInfo);
                        packet["rxpk"][0]["time"] = datetime;
                        //packet["rxpk"][0]["tmms"] = (double)(timestamp - GATEWAY_GPS_START_TIMESTAMP) * 1000;
                        packet["rxpk"][0]["tmst"] = (uint32_t)timestamp;
                    }
                    if(Encoder.unconfirmedDataUp(device, packet))
                    {
                        device.info.FCnt += 1;
                        Device.saveFCnt(device.info);
                        xSemaphoreTake(this->semaphore, portMAX_DELAY);
                        xSemaphoreGive(this->semaphore);
                        SYSTEM_LOG_LN("UnconfirmedDataUp done");
                        this->forward(packet);
                    }else
                    {
                        SYSTEM_LOG_LN("UnconfirmedDataUp failed");
                    }
                }
                
            }else
            {
                SYSTEM_LOG_LN("qTransceiverToGateway reception failed");
            }
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
    while(true){
        if(qForwarderToGateway == NULL)
        {
            SYSTEM_PRINT_LN("Waiting for Queue ForwarderToGateway Init");
            delay(100);
        }else
        {

            status = xQueueReceive(qForwarderToGateway, &fData, portMAX_DELAY);
        
            if(status == pdTRUE)
            {
                JSONVar packet = JSON.parse(String(fData.packet, fData.packetSize));
                String PHYPayload = (const char *)packet["txpk"]["data"];
                type = Encoder.packetType(PHYPayload);
                if(type == LORAWAN_JOIN_ACCEPT)
                {
                    SYSTEM_LOG("JoinAccept PHYPayload : ");SYSTEM_PRINT_LN(PHYPayload);
                    status = xQueueReceive(qJGateway, device.info.DevEUI, portMAX_DELAY);
                    if(status == pdTRUE)
                    {
                        Device.loadDevice(device.info);
                        device.info.DevNonce--;
                        if(Encoder.joinAccept(device, PHYPayload))
                        {
                            SYSTEM_LOG_LN("JoinAccept done");
                            device.info.DevNonce++;
                            device.info.FCnt = DEVICE_FCNT_DEFAULT + 1;
                            Device.saveDevice(device.info);
                        }else
                        {
                            SYSTEM_LOG_LN("JoinAccept failed");
                        }
                    }
                }else if(type == LORAWAN_CONFIRMED_DATA_DOWN || type == LORAWAN_UNCONFIRMED_DATA_DOWN)
                {
                
                }

            }
        }

    }
}




bool GatewayClass::forward(JSONVar & packet){
    // Send Packet to LoRaWAN Server
    SYSTEM_LOG("Packet : ");SYSTEM_PRINT_LN(packet);
    if(qGatewayToForwarder != NULL)
    {
        Forwarder_data_t fData;
        uint32_t packetSize = JSON.stringify(packet).length();
        memcpy(fData.packet, JSON.stringify(packet).c_str(), packetSize);
        fData.packetSize = packetSize;
        xQueueSend(qGatewayToForwarder, &fData, portMAX_DELAY);
        return true;
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
    time_t timestamp;
    struct tm timeInfo;
    char datetime[32];
    String path = DEVICE_FILE_CONFIG;
    JSONVar config, packet;
    String content;
    SYSTEM_PRINT_LN("joinTaskEntry");
    while(true)
    {
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
                timestamp = getTime(&timeInfo);
                genericPacket(packet);
                if(timestamp > 0)
                {
                    strftime(datetime, 32, "%Y-%m-%dT%H:%M:%SZ", &timeInfo);
                    packet["rxpk"][0]["time"] = datetime;
                    //packet["rxpk"][0]["tmms"] = (double)(timestamp - GATEWAY_GPS_START_TIMESTAMP) * 1000;
                    packet["rxpk"][0]["tmst"] = (uint32_t)timestamp;
                }

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
                    }
                }

            }

        }
        xSemaphoreGive(Gateway.semaphore);
        delay(GATEWAY_JOIN_REQUEST_FREQUENCY * S_TO_MS_FACTOR);

    }
}