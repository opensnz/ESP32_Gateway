#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <Arduino.h>

#define DEVICE_DEV_EUI_SIZE      8
#define DEVICE_APP_EUI_SIZE      8
#define DEVICE_APP_KEY_SIZE      16
#define DEVICE_NWK_SKEY_SIZE     16
#define DEVICE_APP_SKEY_SIZE     16
#define DEVICE_PAYLOAD_MAX_SIZE  252   // < 256

#define DEVICE_DEV_ADDR_DEFAULT  0
#define DEVICE_DEVNONCE_DEFAULT  34
#define DEVICE_FCNT_DEFAULT      0
#define DEVICE_FCNT_MAX          65530
#define DEVICE_FPORT_DEFAULT     1

#define DEVICE_TOTAL             20
#define DEVICE_FILE_CONFIG       "/devices.txt"
#define DEVICE_FILE_INFO_EXT     ".info"
#define DEVICE_FILE_PACKET_EXT   ".packet"

#define DEVICE_TEST_PAYLOAD_HEX  "68100000000000000081319012002C601300002C150515002C330303033556353700043600303016111119200012004216"
#define DEVICE_TEST_PAYLOAD_SIZE 98

typedef struct device_info_t{
    uint8_t DevEUI[DEVICE_DEV_EUI_SIZE];
    uint8_t AppEUI[DEVICE_APP_EUI_SIZE];
    uint8_t AppKey[DEVICE_APP_KEY_SIZE];
    uint8_t NwkSKey[DEVICE_NWK_SKEY_SIZE];
    uint8_t AppSKey[DEVICE_APP_SKEY_SIZE];
    uint32_t DevAddr = DEVICE_DEV_ADDR_DEFAULT;
    uint8_t FPort = DEVICE_FPORT_DEFAULT;
    uint16_t DevNonce = DEVICE_DEVNONCE_DEFAULT;
    uint16_t FCnt = DEVICE_FCNT_DEFAULT;
} __attribute__((packed)) Device_info_t;

#define DEVICE_FILE_INFO_SIZE      sizeof(Device_info_t)
#define DEVICE_FILE_DEVNONCE_POS   (DEVICE_FILE_INFO_SIZE - 4)
#define DEVICE_FILE_FCNT_POS       (DEVICE_FILE_INFO_SIZE - 2)

typedef struct device_data_t{
    uint8_t payload[DEVICE_PAYLOAD_MAX_SIZE];
    uint8_t payloadSize = DEVICE_PAYLOAD_MAX_SIZE;
    Device_info_t info;
} __attribute__((packed)) Device_data_t;

class DeviceClass {

public:
    bool isDeviceConfigured(Device_info_t & info);
    bool addDevice(Device_info_t & info);
    bool removeDevice(Device_info_t & info);
    bool loadDevice(Device_info_t & info);
    bool saveDevice(Device_info_t & info);
    bool saveFCnt(Device_info_t & info);
    bool saveDevNonce(Device_info_t & info);
};


/**************** Exported Global Variables *******************/

extern DeviceClass Device;


/*************** Global Function Prototypes *******************/


void printDeviceData(Device_data_t &device);

#endif /* __DEVICE_H__ */