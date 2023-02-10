#include "gateway.h"
#include "transceiver.h"
#include "forwarder.h"
#include <freertos/queue.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <time.h>
#include <sntp.h>

GatewayClass Gateway;
String EncoderServer = "http://192.168.217.188:5050/";
const char DevEUI[] = "9ff6b57d643b0681";
const char AppEUI[] = "0000000000000000";
const char AppKey[] = "e326d7974f9fa5b0ea0a8cee84c5e3a6";


void GatewayClass::tSetup(void){
    hexStringToArray(DevEUI, this->device.DevEUI);
    hexStringToArray(AppEUI, this->device.AppEUI);
    hexStringToArray(AppKey, this->device.AppKey);
    sntp_set_time_sync_notification_cb(timeAdjustmentNotification);
    sntp_servermode_dhcp(1);
    configTime(GMT_OFFSET_SEC, DAY_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2);
}
void GatewayClass::tLoop(void){
    Transceiver_data_t data;
    BaseType_t status = pdFALSE;
    while(true){
        if(qTransceiverToGateway == NULL)
        {
            SYSTEM_PRINT_LN("Waiting for Queue TransceiverToGateway Init");
            delay(100);
        }else
        {
            status = xQueueReceive(qTransceiverToGateway, &data, portMAX_DELAY);
            if(status == pdTRUE)
            {
                struct tm receivedTimeInfo;
                time_t timestamp; // overflow at 19 January 2038, at 03:14:07 UTC (end of time)
                timestamp = getTime(&receivedTimeInfo);
                char datetime[32];
                if(timestamp > 0)
                {
                    strftime(datetime, 32, "%Y-%m-%dT%H:%M:%SZ", &receivedTimeInfo);
                    SYSTEM_LOG_LN(datetime);
                }
                printTransceiverData(&data);
                if(device.FCnt == 1 || device.FCnt >= 65530){
                    // JoinRequest needed
                    JSONVar body, packet;
                    String response;
                    char hexData[64];
                    arrayToHexString(device.DevEUI, DEV_EUI_SIZE, hexData);
                    body["DevEUI"] = hexData;
                    arrayToHexString(device.AppEUI, APP_EUI_SIZE, hexData);
                    body["AppEUI"] = hexData;
                    arrayToHexString(device.AppKey, APP_KEY_SIZE, hexData);
                    body["AppKey"] = hexData;
                    arrayToHexString((uint8_t*)&device.DevNonce, 2, hexData);
                    body["DevNonce"] = hexData;
                    int httpResponseCode;
                    httpResponseCode = httpPOSTRequest(EncoderServer+"JoinRequest", JSON.stringify(body), response);
                    genericPacket(packet);
                    if(timestamp > 0)
                    {
                        packet["rxpk"][0]["time"] = datetime;
                        packet["rxpk"][0]["tmms"] = ((uint32_t)timestamp - GPS_START_TIMESTAMP) * 1000;
                        packet["rxpk"][0]["tmst"] = (uint32_t)timestamp;
                    }
                    if(httpResponseCode == 200)
                    {
                        JSONVar json = JSON.parse(response);
                        String payload = json["PHYPayload"];
                        packet["rxpk"][0]["size"] = (uint32_t)json["size"];
                        packet["rxpk"][0]["data"] = payload;
                        SYSTEM_LOG("Packet : ");SYSTEM_PRINT_LN(packet);
                    }

                    // Send Packet to LoRaWAN Server
                    if(qGatewayToForwarder != NULL)
                    {
                        forwarder_data_t fData;
                        hexStringToArray(DevEUI, fData.DevEUI);
                        uint32_t packetSize = JSON.stringify(packet).length();
                        memcpy(fData.packet, JSON.stringify(packet).c_str(), packetSize);
                        fData.packetSize = packetSize;
                        xQueueSend(qGatewayToForwarder, &fData, portMAX_DELAY);
                    }

                }else
                {
                    SYSTEM_LOGF_LN("Device.FCnt = %d", device.FCnt);
                }
                
            }else
            {
                SYSTEM_LOG_LN("qTransceiverToGateway reception failed");
            }
        }
    }
}

void GatewayClass::fSetup(void){
    
}
void GatewayClass::fLoop(void){
    Forwarder_data_t fData;
    BaseType_t status = pdFALSE;
    while(true){
        if(qForwarderToGateway == NULL)
        {
            SYSTEM_PRINT_LN("Waiting for Queue ForwarderToGateway Init");
            delay(100);
        }else
        {
            status = xQueueReceive(qForwarderToGateway, &fData, portMAX_DELAY);
            // Traitement
        }

    }
}

GatewayClass::GatewayClass(){

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



/********************* Global Function Implementations **********************/

int httpPOSTRequest(String serverName, String body, String & response)
{
    WiFiClient client;
    HTTPClient http;

    SYSTEM_PRINT_LN("\n################ Encoder Data ################");
    SYSTEM_PRINT_LN(String("HTTP to server " + serverName).c_str());
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(body);
    response = http.getString();
    SYSTEM_LOG("HTTP POST Response code: ");
    SYSTEM_PRINT_LN(httpResponseCode);
    SYSTEM_LOG("HTTP POST Response: ");
    SYSTEM_PRINT_LN(response);
    SYSTEM_PRINT_LN("##################################################");
    // Free resources
    http.end();
    return httpResponseCode;
}


uint32_t hexStringToArray(const char * hexString, uint8_t *pArray){
    char hex2Bytes[3]  = "FF";
    uint32_t size = String(hexString).length()/2;
    for (int i = 0; i < size; i += 1) 
    {
        hex2Bytes[0] = hexString[2*i];
        hex2Bytes[1] = hexString[2*i+1];
        pArray[i] = (uint8_t)strtol(hex2Bytes, NULL, 16);
    }
    return size;
}

uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, char * hexString){
    for (int i = 0; i < size; i += 1)
    {
        sprintf(&hexString[2*i], "%.2x", pArray[i]);
    }
    hexString[2*size+1] = '\0';
    return 2*size+1;
}

void timeAdjustmentNotification(struct timeval * t){
    SYSTEM_PRINT_LN("Got time adjustment from NTP!");
}


void genericPacket(JSONVar & packet){
    packet["rxpk"][0]["time"] = "";
    packet["rxpk"][0]["tmms"] = 0;
    packet["rxpk"][0]["tmst"] = 0;
    packet["rxpk"][0]["chan"] = 1;
    packet["rxpk"][0]["rfch"] = 0;
    packet["rxpk"][0]["freq"] = 868.3;
    packet["rxpk"][0]["stat"] = 1;
    packet["rxpk"][0]["modu"] = "LORA";
    packet["rxpk"][0]["datr"] = "SF7BW125";
    packet["rxpk"][0]["codr"] = "4/5";
    packet["rxpk"][0]["rssi"] = -35;
    packet["rxpk"][0]["lsnr"] = 5.0;
    packet["rxpk"][0]["size"] = 23;
    packet["rxpk"][0]["data"] = "";
}



time_t getTime(struct tm * timeinfo) {
  time_t now;
  if (!getLocalTime(timeinfo)) {
    //Failed to obtain time
    return 0;
  }
  time(&now);
  return now;
}