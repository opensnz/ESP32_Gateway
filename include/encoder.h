/**
  ******************************************************************************
  * @file    encoder.h
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

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "device.h"


#define ENCODER_BASE64_BUFFER_MAX_SIZE  (300)


typedef enum {
    LORAWAN_JOIN_REQUEST = 0,
    LORAWAN_JOIN_ACCEPT,
    LORAWAN_UNCONFIRMED_DATA_UP,
    LORAWAN_UNCONFIRMED_DATA_DOWN,
    LORAWAN_CONFIRMED_DATA_UP,
    LORAWAN_CONFIRMED_DATA_DOWN,
    LORAWAN_REJOIN_REQUEST,
    LORAWAN_PROPRIETARY = 0b111
} LoRaWAN_Packet_Type_t;

class EncoderClass {

private:
  uint32_t parseUInt32LittleEndian(const uint8_t *bytes);
  void upStream(Device_data_t & device);
  void downStream(Device_data_t & device);

public:
    EncoderClass();
    LoRaWAN_Packet_Type_t packetType(String & PHYPayload);
    bool joinRequest(Device_data_t & device, JSONVar & packet);
    bool joinAccept(Device_data_t & device, String & PHYPayload);
    bool unconfirmedDataUp(Device_data_t & device, JSONVar & packet);
    bool confirmedDataUp(Device_data_t & device, JSONVar & packet);
    bool dataDown(Device_data_t & device, String & PHYPayload);
};


/**************** Exported Global Variables *******************/

extern EncoderClass Encoder;
extern char EncoderB64Buffer[ENCODER_BASE64_BUFFER_MAX_SIZE];


#endif /* __ENCODER_H__ */

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/