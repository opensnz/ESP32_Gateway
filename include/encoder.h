#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include "device.h"
#include "system.h"
#include "log.h"

#define ENCODER_SERVER                  "http://192.168.217.188:5050/"
#define ENCODER_JOIN_REQUEST            "JoinRequest"
#define ENCODER_JOIN_ACCEPT             "JoinAccept"
#define ENCODER_UNCONFIRMED_DATA_UP     "UnconfirmedDataUp"
#define ENCODER_UNCONFIRMED_DATA_DOWN   "UnconfirmedDataDown"
#define ENCODER_CONFIRMED_DATA_UP       "ConfirmedDataUp"
#define ENCODER_CONFIRMED_DATA_DOWN     "ConfirmedDataDown"


class EncoderClass {

private:
    uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, char * hexString);
    uint32_t hexStringToArray(const char * hexString, uint8_t *pArray);
    t_http_codes httpPOSTRequest(String serverName, String body, String & response);

public:
    EncoderClass();
    bool joinRequest(Device_data_t & device, JSONVar & packet);
    bool joinAccept(Device_data_t & device, String PHYPayload);
    bool unconfirmedDataUp(Device_data_t & device, JSONVar & packet);
    bool unconfirmedDataDown(Device_data_t & device, JSONVar & packet);
    bool confirmedDataUp(Device_data_t & device, JSONVar & packet);
    bool confirmedDataDown(Device_data_t & device, JSONVar & packet);
};


/**************** Exported Global Variables *******************/

extern EncoderClass Encoder;



#endif /* __ENCODER_H__ */