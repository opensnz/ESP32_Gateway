#ifndef __GATEWAY_H__
#define __GATEWAY_H__

#include <Arduino.h>
#include <Arduino_JSON.h>

#define DEV_EUI_SIZE      8
#define APP_EUI_SIZE      8
#define APP_KEY_SIZE      16
#define DEV_ADDR_SIZE     4
#define NWK_SKEY_SIZE     16
#define APP_SKEY_SIZE     16
#define PAYLOAD_MAX_SIZE  256

#define DEVICE_DEVNONCE_DEFAULT   1
#define DEVICE_FCNT_DEFAULT       1
#define DEVICE_FPORT_DEFAULT      1

#define NTP_SERVER_1     "pool.ntp.org"
#define NTP_SERVER_2     "time.nist.gov"
#define GMT_OFFSET_SEC   0
#define DAY_OFFSET_SEC   0
#define GPS_START_TIMESTAMP 315964800 // January 6, 1980, 00:00:00 (seconds)


typedef struct device_data_t{
    uint8_t DevEUI[DEV_EUI_SIZE];
    uint8_t AppEUI[APP_EUI_SIZE];
    uint8_t AppKey[APP_KEY_SIZE];
    uint8_t DevAddr[DEV_ADDR_SIZE];
    uint8_t NwkSKey[NWK_SKEY_SIZE];
    uint8_t AppSKey[APP_SKEY_SIZE];
    uint8_t payload[PAYLOAD_MAX_SIZE];
    uint8_t PHYPayload[PAYLOAD_MAX_SIZE];
    uint16_t DevNonce = DEVICE_DEVNONCE_DEFAULT;
    uint16_t FCnt = DEVICE_FCNT_DEFAULT;
    uint8_t FPort = DEVICE_FPORT_DEFAULT;
} Device_data_t;


class GatewayClass {

private:
	
    Device_data_t device;

	void tSetup(void);
	void tLoop(void);

    void fSetup(void);
	void fLoop(void);
    
public:
	GatewayClass();
	void tMain(void);
    void fMain(void);

};

/**************** Exported Global Variables *******************/

extern GatewayClass Gateway;



/*************** Global Function Prototypes *******************/

int httpPOSTRequest(String serverName, String body, String & response);

uint32_t hexStringToArray(const char * hexString, uint8_t *pArray);

uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, char * hexString);

void timeAdjustmentNotification(struct timeval *t);

void genericPacket(JSONVar & packet);

time_t getTime(struct tm * timeinfo);

#endif /* __GATEWAY_H__ */