#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include "device.h"


#define ENCODER_SERVER                  "http://192.168.10.99:5050/"
#define ENCODER_JOIN_REQUEST            "JoinRequest"
#define ENCODER_JOIN_ACCEPT             "JoinAccept"
#define ENCODER_UNCONFIRMED_DATA_UP     "UnconfirmedDataUp"
#define ENCODER_UNCONFIRMED_DATA_DOWN   "UnconfirmedDataDown"
#define ENCODER_CONFIRMED_DATA_UP       "ConfirmedDataUp"
#define ENCODER_CONFIRMED_DATA_DOWN     "ConfirmedDataDown"
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

public:
    EncoderClass();
    LoRaWAN_Packet_Type_t packetType(String PHYPayload);
    bool joinRequest(Device_data_t & device, JSONVar & packet);
    bool joinAccept(Device_data_t & device, String PHYPayload);
    bool unconfirmedDataUp(Device_data_t & device, JSONVar & packet);
    bool unconfirmedDataDown(Device_data_t & device, JSONVar & packet);
    bool confirmedDataUp(Device_data_t & device, JSONVar & packet);
    bool confirmedDataDown(Device_data_t & device, JSONVar & packet);
};


/**************** Exported Global Variables *******************/

extern EncoderClass Encoder;
extern char EncoderB64Buffer[ENCODER_BASE64_BUFFER_MAX_SIZE];


#endif /* __ENCODER_H__ */