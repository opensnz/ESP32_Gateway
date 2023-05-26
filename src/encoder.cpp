/**
  ******************************************************************************
  * @file    encoder.cpp
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

#include "encoder.h"
#include "lorawan.h"
#include "common.h"
#include "system.h"
#include "log.h"


EncoderClass Encoder;

LoRaWAN_Packet_t UpStream;
LoRaWAN_Packet_t DownStream;

char EncoderB64Buffer[ENCODER_BASE64_BUFFER_MAX_SIZE];

EncoderClass::EncoderClass(){

}

uint32_t EncoderClass::parseUInt32LittleEndian(const uint8_t *bytes) {
    return (((uint32_t) bytes[0]) << 0u) | (((uint32_t) bytes[1]) << 8u) | (((uint32_t) bytes[2]) << 16u) | (((uint32_t) bytes[3]) << 24u);
}


void EncoderClass::upStream(Device_data_t & device){
    UpStream.MACPayload.FPort = device.info.FPort;
    UpStream.MACPayload.FHDR.FCnt16 = device.info.FCnt;
    UpStream.MACPayload.FHDR.DevAddr = device.info.DevAddr;
    UpStream.MACPayload.FHDR.FCtrl.uplink.ACK = true;
    UpStream.MACPayload.FHDR.FCtrl.uplink.ADR = false;
    UpStream.MACPayload.FHDR.FCtrl.uplink.FOptsLen = 0;
    UpStream.MACPayload.FHDR.FCtrl.uplink.ClassB = false;
    UpStream.MACPayload.FHDR.FCtrl.uplink.ADRACKReq = false;
    UpStream.MACPayload.payloadSize = device.payloadSize;
    memcpy(UpStream.MACPayload.NwkSKey, device.info.NwkSKey, DEVICE_NWK_SKEY_SIZE);
    memcpy(UpStream.MACPayload.AppSKey, device.info.AppSKey, DEVICE_APP_SKEY_SIZE);
    memcpy(UpStream.MACPayload.payload, device.payload, device.payloadSize);
}
void EncoderClass::downStream(Device_data_t & device){
    DownStream.MACPayload.FPort = 0;
    DownStream.MACPayload.FHDR.FCnt16 = 0;
    DownStream.MACPayload.FHDR.DevAddr = device.info.DevAddr;
    DownStream.MACPayload.FHDR.FCtrl.downlink.ACK = false;
    DownStream.MACPayload.FHDR.FCtrl.downlink.RFU = false;
    DownStream.MACPayload.FHDR.FCtrl.downlink.ADR = false;
    DownStream.MACPayload.FHDR.FCtrl.downlink.FOptsLen = 0;
    DownStream.MACPayload.FHDR.FCtrl.downlink.FPending = false;
    UpStream.MACPayload.payloadSize = 0;
    memcpy(DownStream.MACPayload.NwkSKey, device.info.NwkSKey, DEVICE_NWK_SKEY_SIZE);
    memcpy(DownStream.MACPayload.AppSKey, device.info.AppSKey, DEVICE_APP_SKEY_SIZE);
}

LoRaWAN_Packet_Type_t EncoderClass::packetType(String & PHYPayload)
{

    uint32_t size = LoRaWAN_Base64_To_Binary(PHYPayload.c_str(), PHYPayload.length(), 
                                            DownStream.MACPayload.payload, LORAWAN_MAX_PAYLOAD_LEN);
    if(size == 0)
    {
        // error
        return LoRaWAN_Packet_Type_t::LORAWAN_PROPRIETARY;
    }
    MHDR_MType_t type = LoRaWAN_MessageType(DownStream.MACPayload.payload, size);
    return (LoRaWAN_Packet_Type_t)(type);
}

bool EncoderClass::joinRequest(Device_data_t & device, JSONVar & packet){

    memcpy(UpStream.JoinRequest.DevEUI, device.info.DevEUI, DEVICE_DEV_EUI_SIZE);
    memcpy(UpStream.JoinRequest.AppEUI, device.info.AppEUI, DEVICE_APP_EUI_SIZE);
    memcpy(UpStream.JoinRequest.AppKey, device.info.AppKey, DEVICE_APP_KEY_SIZE);
    UpStream.JoinRequest.DevNonce = device.info.DevNonce;

    device.payloadSize = LoRaWAN_JoinRequest(&UpStream.JoinRequest, device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    uint32_t size = LoRaWAN_Binary_To_Base64(device.payload, device.payloadSize,
                                                EncoderB64Buffer, ENCODER_BASE64_BUFFER_MAX_SIZE);
    if(size == 0)
    {
        return false;
    }
    if(device.payloadSize != 0)
    {
        packet["rxpk"][0]["size"] = size;
        packet["rxpk"][0]["data"] = String(EncoderB64Buffer, size);
        return true;
    }
    return false;
}

bool EncoderClass::joinAccept(Device_data_t & device, String & PHYPayload){
    DownStream.JoinAccept.DevNonce = device.info.DevNonce;
    memcpy(DownStream.JoinAccept.AppKey, device.info.AppKey, DEVICE_APP_KEY_SIZE);
    device.payloadSize = LoRaWAN_Base64_To_Binary(PHYPayload.c_str(), PHYPayload.length(), 
                                                    device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    if(device.payloadSize == 0)
    {
        return false;
    }
    bool status = LoRaWAN_JoinAccept(&DownStream.JoinAccept, device.payload, device.payloadSize);
    if(status)
    {
        device.info.DevAddr = DownStream.JoinAccept.DevAddr;
        memcpy(device.info.NwkSKey, DownStream.JoinAccept.NwkSKey, DEVICE_NWK_SKEY_SIZE);
        memcpy(device.info.AppSKey, DownStream.JoinAccept.AppSKey, DEVICE_APP_SKEY_SIZE);
    }
    return status;
}

bool EncoderClass::unconfirmedDataUp(Device_data_t & device, JSONVar & packet){

    this->upStream(device);

    device.payloadSize = LoRaWAN_UnconfirmedDataUp(&UpStream.MACPayload, device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    uint32_t size = LoRaWAN_Binary_To_Base64(device.payload, device.payloadSize,
                                                EncoderB64Buffer, ENCODER_BASE64_BUFFER_MAX_SIZE);
    if(size == 0)
    {
        return false;
    }
    if(device.payloadSize != 0)
    {
        packet["rxpk"][0]["size"] = size;
        packet["rxpk"][0]["data"] = String(EncoderB64Buffer, size);
        return true;
    }
    return false;
}

bool EncoderClass::confirmedDataUp(Device_data_t & device, JSONVar & packet){

    this->upStream(device);

    device.payloadSize = LoRaWAN_ConfirmedDataUp(&UpStream.MACPayload, device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    uint32_t size = LoRaWAN_Binary_To_Base64(device.payload, device.payloadSize,
                                                EncoderB64Buffer, ENCODER_BASE64_BUFFER_MAX_SIZE);
    if(size == 0)
    {
        return false;
    }
    if(device.payloadSize != 0)
    {
        packet["rxpk"][0]["size"] = size;
        packet["rxpk"][0]["data"] = String(EncoderB64Buffer, size);
        return true;
    }
    return false;
}

bool EncoderClass::dataDown(Device_data_t & device, String & PHYPayload){
    
    device.payloadSize = LoRaWAN_Base64_To_Binary(PHYPayload.c_str(), PHYPayload.length(), 
                                                    device.payload, DEVICE_PAYLOAD_MAX_SIZE);
    if(device.payloadSize == 0)
    {
        return false;
    }
    device.info.DevAddr = this->parseUInt32LittleEndian(&device.payload[1]);
    if(!Device.getDeviceByDevAddr(device.info))
    {
        return false;
    }

    this->downStream(device);

    bool status = LoRaWAN_DataDown(&DownStream.MACPayload, device.payload, device.payloadSize);

    if(status)
    {
        device.payloadSize = DownStream.MACPayload.payloadSize;
        if(device.payloadSize > 0)
        {
            memcpy(device.payload, DownStream.MACPayload.payload, device.payloadSize);
        }
        return true;
    }

    return false;
}


/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/