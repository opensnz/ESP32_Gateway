#include "encoder.h"
#include "lorawan.h"
#include "common.h"
#include "system.h"
#include "log.h"


EncoderClass Encoder;
char EncoderB64Buffer[ENCODER_BASE64_BUFFER_MAX_SIZE];



EncoderClass::EncoderClass(){

}

LoRaWAN_Packet_Type_t EncoderClass::packetType(String PHYPayload)
{

    uint32_t size = LoRaWAN_Base64_To_Binary(PHYPayload.c_str(), PHYPayload.length(), 
                                            (uint8_t*)EncoderB64Buffer, ENCODER_BASE64_BUFFER_MAX_SIZE);
    MHDR_MType_t type = LoRaWAN_MessageType((uint8_t*)EncoderB64Buffer, size);
    return (LoRaWAN_Packet_Type_t)(type);
}

bool EncoderClass::joinRequest(Device_data_t & device, JSONVar & packet){

    memcpy(LoRaWAN.JoinRequest.DevEUI, device.info.DevEUI, DEVICE_DEV_EUI_SIZE);
    memcpy(LoRaWAN.JoinRequest.AppEUI, device.info.AppEUI, DEVICE_APP_EUI_SIZE);
    memcpy(LoRaWAN.JoinRequest.AppKey, device.info.AppKey, DEVICE_APP_KEY_SIZE);
    LoRaWAN.JoinRequest.DevNonce = device.info.DevNonce;

    device.payloadSize = LoRaWAN_JoinRequest(&LoRaWAN.JoinRequest, device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    uint32_t size = LoRaWAN_Binary_To_Base64(device.payload, device.payloadSize,
                                                EncoderB64Buffer, ENCODER_BASE64_BUFFER_MAX_SIZE);
    if(device.payloadSize != 0)
    {
        packet["rxpk"][0]["size"] = size;
        packet["rxpk"][0]["data"] = String(EncoderB64Buffer, size);
        return true;
    }
    return false;
}

bool EncoderClass::joinAccept(Device_data_t & device, String PHYPayload){
    LoRaWAN.JoinAccept.DevNonce = device.info.DevNonce;
    memcpy(LoRaWAN.JoinAccept.AppKey, device.info.AppKey, DEVICE_APP_KEY_SIZE);
    device.payloadSize = LoRaWAN_Base64_To_Binary(PHYPayload.c_str(), PHYPayload.length(), 
                                                    device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    bool status = LoRaWAN_JoinAccept(&LoRaWAN.JoinAccept, device.payload, device.payloadSize);
    if(status)
    {
        device.info.DevAddr = LoRaWAN.JoinAccept.DevAddr;
        memcpy(device.info.NwkSKey, LoRaWAN.JoinAccept.NwkSKey, DEVICE_NWK_SKEY_SIZE);
        memcpy(device.info.AppSKey, LoRaWAN.JoinAccept.AppSKey, DEVICE_APP_SKEY_SIZE);
    }
    return status;
}

bool EncoderClass::unconfirmedDataUp(Device_data_t & device, JSONVar & packet){

    LoRaWAN.MACPayload.FPort = device.info.FPort;
    LoRaWAN.MACPayload.FHDR.FCnt16 = device.info.FCnt;
    LoRaWAN.MACPayload.FHDR.DevAddr = device.info.DevAddr;
    LoRaWAN.MACPayload.FHDR.FCtrl.uplink.ACK = true;
    LoRaWAN.MACPayload.FHDR.FCtrl.uplink.ADR = true;
    memcpy(LoRaWAN.MACPayload.NwkSKey, device.info.NwkSKey, DEVICE_NWK_SKEY_SIZE);
    memcpy(LoRaWAN.MACPayload.AppSKey, device.info.AppSKey, DEVICE_APP_SKEY_SIZE);
    memcpy(LoRaWAN.MACPayload.payload, device.payload, device.payloadSize);
    LoRaWAN.MACPayload.payloadSize = device.payloadSize;

    device.payloadSize = LoRaWAN_UnconfirmedDataUp(&LoRaWAN.MACPayload, device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    uint32_t size = LoRaWAN_Binary_To_Base64(device.payload, device.payloadSize,
                                                EncoderB64Buffer, ENCODER_BASE64_BUFFER_MAX_SIZE);
    if(device.payloadSize != 0)
    {
        packet["rxpk"][0]["size"] = size;
        packet["rxpk"][0]["data"] = String(EncoderB64Buffer, size);
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


