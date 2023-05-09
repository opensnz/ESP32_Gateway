/**
  ******************************************************************************
  * @file    device.h
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
#define DEVICE_DEVNONCE_DEFAULT  0
#define DEVICE_FCNT_DEFAULT      0
#define DEVICE_FCNT_MAX          65530
#define DEVICE_FPORT_DEFAULT     1

#define DEVICE_TOTAL             20
#define DEVICE_FILE_CONFIG       "/devices.txt"
#define DEVICE_FILE_INFO_EXT     ".info"
#define DEVICE_FILE_PACKET_EXT   ".packet"


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

typedef struct device_metadata_t{
    int rssi;
    float snr;
} __attribute__((packed)) Device_metadata_t;


typedef struct device_data_t{
    uint8_t payload[DEVICE_PAYLOAD_MAX_SIZE];
    uint8_t payloadSize = DEVICE_PAYLOAD_MAX_SIZE;
    Device_info_t info;
    Device_metadata_t metadata;
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
    bool getDeviceByDevAddr(Device_info_t & info);
};


/**************** Exported Global Variables *******************/

extern DeviceClass Device;


/*************** Global Function Prototypes *******************/


void printDeviceData(Device_data_t &device);

#endif /* __DEVICE_H__ */

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/