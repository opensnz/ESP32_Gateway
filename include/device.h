#ifndef __DEVICE_H__
#define __DEVICE_H__

#define DEVICE_DEV_EUI_SIZE      8
#define DEVICE_APP_EUI_SIZE      8
#define DEVICE_APP_KEY_SIZE      16
#define DEVICE_DEV_ADDR_SIZE     4
#define DEVICE_NWK_SKEY_SIZE     16
#define DEVICE_APP_SKEY_SIZE     16
#define DEVICE_PAYLOAD_MAX_SIZE  252   // < 256

#define DEVICE_DEVNONCE_DEFAULT  15
#define DEVICE_FCNT_DEFAULT      1
#define DEVICE_FPORT_DEFAULT     1


typedef struct device_data_t{
    uint8_t DevEUI[DEVICE_DEV_EUI_SIZE];
    uint8_t AppEUI[DEVICE_APP_EUI_SIZE];
    uint8_t AppKey[DEVICE_APP_KEY_SIZE];
    uint8_t DevAddr[DEVICE_DEV_ADDR_SIZE];
    uint8_t NwkSKey[DEVICE_NWK_SKEY_SIZE];
    uint8_t AppSKey[DEVICE_APP_SKEY_SIZE];
    uint8_t payload[DEVICE_PAYLOAD_MAX_SIZE];
    uint8_t payloadSize = DEVICE_PAYLOAD_MAX_SIZE;
    uint8_t FPort = DEVICE_FPORT_DEFAULT;
    uint16_t FCnt = DEVICE_FCNT_DEFAULT;
    uint16_t DevNonce = DEVICE_DEVNONCE_DEFAULT;
} Device_data_t;


#endif /* __DEVICE_H__ */