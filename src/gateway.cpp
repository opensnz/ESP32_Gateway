#include "gateway.h"
#include "transceiver.h"
#include "forwarder.h"
#include <freertos/queue.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

GatewayClass Gateway;
String EncoderServer = "http://192.168.217.188:5050/";
char EncoderBuffer[1024];
const char DevEUI[] = "7ab01b461f9323bd";
const char AppEUI[] = "0000000000000000";
const char AppKey[] = "4eed11c811b523018b9298975ef35446";


void GatewayClass::tSetup(void){
    hexStringToArray(DevEUI, this->device.DevEUI);
    hexStringToArray(AppEUI, this->device.AppEUI);
    hexStringToArray(AppKey, this->device.AppKey);
}
void GatewayClass::tLoop(void){
    Transceiver_data_t data;
    BaseType_t status = pdFALSE;
    while(true){
        status = xQueueReceive(qTransceiverToGateway, &data, portMAX_DELAY);
        if(status == pdTRUE)
        {
            status = pdFALSE;
            printTransceiverData(&data);
            if(device.FCnt == 1 || device.FCnt >= 65530){
                // JoinRequest needed
                String PHYPayload = "{\"PHYPayload\": \"YPs5NQCQAAAFmsoqWDs1HMEeMWC7cdXQA89h3Klssg==\"}";
                JSONVar body;
                char hexData[64];
                arrayToHexString(device.DevEUI, DEV_EUI_SIZE, hexData);
                body["DevEUI"] = hexData;
                arrayToHexString(device.AppEUI, APP_EUI_SIZE, hexData);
                body["AppEUI"] = hexData;
                arrayToHexString(device.AppKey, APP_KEY_SIZE, hexData);
                body["AppKey"] = hexData;
                arrayToHexString((uint8_t*)&device.DevNonce, 2, hexData);
                body["DevNonce"] = hexData;
                Serial.print("BODY content  = ");Serial.println(body);
                httpPOSTRequest(EncoderServer+"JoinRequest", PHYPayload);
                Serial.print("STRG response = ");Serial.println(EncoderBuffer);
                JSONVar encoded = JSON.parse(EncoderBuffer);
                Serial.print("JSON response = ");Serial.println(encoded);
                //hexStringToArray(encoded[""])
            }else
            {
                Serial.printf("\n####################### Device.FCnt = %d\n", device.FCnt);
            }
            
        }else
        {
            Serial.println("qTransceiverToGateway reception failed");
        }
    }
}

void GatewayClass::fSetup(void){

}
void GatewayClass::fLoop(void){
    Forwarder_data_t fData;
    BaseType_t status = pdFALSE;
    while(true){
        status = xQueueReceive(qForwarderToGateway, &fData, portMAX_DELAY);
        // Traitement
    }
}

GatewayClass::GatewayClass(){

}

void GatewayClass::tMain(void){
    printf("GatewayClass tMain setting...\n");
    this->tSetup();
    printf("GatewayClass tMain running...\n");
    this->tLoop();
}
void GatewayClass::fMain(void){
    printf("GatewayClass fMain setting...\n");
    this->fSetup();
    printf("GatewayClass fMain running...\n");
    this->fLoop();
}



/********************* Global Function Implementations **********************/

void httpPOSTRequest(String serverName, String body)
{
    WiFiClient client;
    HTTPClient http;

    Serial.println("\n################ EncoderClass Data ################");
    Serial.println("HTTP to server " + serverName);
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(body);
    Serial.print("HTTP POST Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("HTTP POST Response: ");
    Serial.println(http.getString());
    Serial.println("\n##################################################");

    if(httpResponseCode == 200){
        memcpy(EncoderBuffer, http.getString().c_str(),  http.getSize());
    }
    Serial.println("##################################################");

    // Free resources
    http.end();
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