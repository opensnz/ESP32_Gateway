#include "encoder.h"


EncoderClass Encoder;

uint32_t EncoderClass::arrayToHexString(const uint8_t *pArray, uint32_t size, char * hexString){
    for (int i = 0; i < size; i += 1)
    {
        sprintf(&hexString[2*i], "%.2x", pArray[i]);
    }
    hexString[2*size+1] = '\0';
    return 2*size+1;
}

uint32_t EncoderClass::hexStringToArray(const char * hexString, uint8_t *pArray){
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

t_http_codes EncoderClass::httpPOSTRequest(String serverName, String body, String & response)
{
    WiFiClient client;
    HTTPClient http;

    SYSTEM_PRINT_LN("\n################ Encoder Data ################");
    SYSTEM_PRINT_LN(String("HTTP to server " + serverName).c_str());
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    t_http_codes httpResponseCode = (t_http_codes)http.POST(body);
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

EncoderClass::EncoderClass(){

}

bool EncoderClass::joinRequest(Device_data_t & device, JSONVar & packet){
    JSONVar body;
    String response;
    char hexData[64];
    this->arrayToHexString(device.DevEUI, DEVICE_DEV_EUI_SIZE, hexData);
    body["DevEUI"] = hexData;
    this->arrayToHexString(device.AppEUI, DEVICE_APP_EUI_SIZE, hexData);
    body["AppEUI"] = hexData;
    this->arrayToHexString(device.AppKey, DEVICE_APP_KEY_SIZE, hexData);
    body["AppKey"] = hexData;
    sprintf(hexData, "%.4x", device.DevNonce);
    body["DevNonce"] = hexData;
    t_http_codes httpResponseCode;
    httpResponseCode = this->httpPOSTRequest(String(ENCODER_SERVER)+ENCODER_JOIN_REQUEST, 
                                                JSON.stringify(body), response);
    if(httpResponseCode == HTTP_CODE_OK)
    {
        JSONVar responseJSON = JSON.parse(response);
        String data = responseJSON["PHYPayload"];
        packet["rxpk"][0]["size"] = (uint32_t)responseJSON["size"];
        packet["rxpk"][0]["data"] = data;
        SYSTEM_LOG("Encoder JoinRequest Response : ");SYSTEM_PRINT_LN(responseJSON);
        return true;
    }
    return false;
}

bool EncoderClass::joinAccept(Device_data_t & device, String PHYPayload){
    JSONVar body;
    String response;
    char hexData[64];
    this->arrayToHexString(device.AppKey, DEVICE_APP_KEY_SIZE, hexData);
    body["AppKey"] = hexData;
    sprintf(hexData, "%.4x", device.DevNonce);
    body["DevNonce"] = hexData;
    body["PHYPayload"] = PHYPayload;
    t_http_codes httpResponseCode;
    httpResponseCode = this->httpPOSTRequest(String(ENCODER_SERVER)+ENCODER_JOIN_ACCEPT, 
                                                JSON.stringify(body), response);
    if(httpResponseCode == HTTP_CODE_OK)
    {
        JSONVar responseJSON = JSON.parse(response);
        this->hexStringToArray((const char*)responseJSON["DevAddr"], device.DevAddr);
        this->hexStringToArray((const char*)responseJSON["NwkSKey"], device.NwkSKey);
        this->hexStringToArray((const char*)responseJSON["AppSKey"], device.AppSKey);
        SYSTEM_LOG("Encoder JoinAccept Response : ");SYSTEM_PRINT_LN(responseJSON);
        return true;
    }
    return false;
}

bool EncoderClass::unconfirmedDataUp(Device_data_t & device, JSONVar & packet){
    JSONVar body;
    String response;
    char hexData[2*DEVICE_PAYLOAD_MAX_SIZE];
    this->arrayToHexString(device.DevAddr, DEVICE_DEV_ADDR_SIZE, hexData);
    body["DevAddr"] = hexData;
    sprintf(hexData, "%.4x", device.FCnt);
    body["FCnt"] = hexData;
    sprintf(hexData, "%.2x", device.FPort);
    body["FPort"] = hexData;
    this->arrayToHexString(device.payload, device.payloadSize, hexData);
    body["payload"] = hexData;
    this->arrayToHexString(device.NwkSKey, DEVICE_NWK_SKEY_SIZE, hexData);
    body["NwkSKey"] = hexData;
    this->arrayToHexString(device.AppSKey, DEVICE_APP_SKEY_SIZE, hexData);
    body["AppSKey"] = hexData;
    t_http_codes httpResponseCode;
    httpResponseCode = this->httpPOSTRequest(String(ENCODER_SERVER)+ENCODER_JOIN_ACCEPT, 
                                                JSON.stringify(body), response);
    if(httpResponseCode == HTTP_CODE_OK)
    {
        JSONVar responseJSON = JSON.parse(response);
        String data = responseJSON["PHYPayload"];
        packet["rxpk"][0]["size"] = (uint32_t)responseJSON["size"];
        packet["rxpk"][0]["data"] = data;
        SYSTEM_LOG("Encoder UnconfirmedDataUp Response : ");SYSTEM_PRINT_LN(responseJSON);
        return true;
    }
    return false;
}

bool EncoderClass::unconfirmedDataDown(Device_data_t & device, JSONVar & packet){
    return false;
}

bool EncoderClass::confirmedDataUp(Device_data_t & device, JSONVar & packet){
    return false;
}

bool EncoderClass::confirmedDataDown(Device_data_t & device, JSONVar & packet){
    return false;
}


