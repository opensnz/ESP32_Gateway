#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <Arduino.h>

#define DEVICE_DEV_EUI_SIZE      8
#define DEVICE_APP_EUI_SIZE      8
#define DEVICE_APP_KEY_SIZE      16
#define DEVICE_DEV_ADDR_SIZE     4
#define DEVICE_NWK_SKEY_SIZE     16
#define DEVICE_APP_SKEY_SIZE     16
#define DEVICE_PAYLOAD_MAX_SIZE  252   // < 256

#define DEVICE_DEVNONCE_DEFAULT  32
#define DEVICE_FCNT_DEFAULT      257
#define DEVICE_FCNT_MAX          65530
#define DEVICE_FPORT_DEFAULT     1

#define DEVICE_TOTAL             5
#define DEVICE_FILE_CONFIG       "/devices.txt"
#define DEVICE_FILE_FCNT_EXT     ".fcnt"
#define DEVICE_FILE_DEVNONCE_EXT ".devnonce"
#define DEVICE_FILE_KEYS_EXT     ".keys"
#define DEVICE_FILE_PACKET_EXT   ".packet"

#define DEVICE_TEST_PAYLOAD_HEX  "68100000000000000081319012002C601300002C150515002C330303033556353700043600303016111119200012004216"
#define DEVICE_TEST_PAYLOAD_SIZE 98

typedef struct device_data_t{
    uint8_t DevEUI[DEVICE_DEV_EUI_SIZE];
    uint8_t AppEUI[DEVICE_APP_EUI_SIZE];
    uint8_t AppKey[DEVICE_APP_KEY_SIZE];
    uint8_t DevAddr[DEVICE_DEV_ADDR_SIZE];
    uint8_t NwkSKey[DEVICE_NWK_SKEY_SIZE];
    uint8_t AppSKey[DEVICE_APP_SKEY_SIZE];
    uint8_t payload[DEVICE_PAYLOAD_MAX_SIZE];
    uint16_t DevNonce = DEVICE_DEVNONCE_DEFAULT;
    uint16_t FCnt = DEVICE_FCNT_DEFAULT;
    uint8_t payloadSize = DEVICE_PAYLOAD_MAX_SIZE;
    uint8_t FPort = DEVICE_FPORT_DEFAULT;
} Device_data_t;

class DeviceClass {

public:
    bool isDeviceConfigured(Device_data_t & device);
    bool addDevice(Device_data_t & device);
    bool loadDevice(Device_data_t & device);
    bool saveFCnt(Device_data_t & device);
    bool saveDevNonce(Device_data_t & device);
    bool saveKeys(Device_data_t & device);
    bool savePacket(Device_data_t & device, String & packet);
    bool loadPacket(Device_data_t & device, String & packet);
    bool removePacket(Device_data_t & device);
};


/**************** Exported Global Variables *******************/

extern DeviceClass Device;


/*************** Global Function Prototypes *******************/


void printDeviceData(Device_data_t &device);

#endif /* __DEVICE_H__ */