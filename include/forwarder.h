
#ifndef __FORWARDER_H__
#define __FORWARDER_H__

#include <Arduino.h>
#include <freertos/queue.h>
#include <map>
#include <WiFi.h>
#include <AsyncUDP.h>
#include "log.h"

#define halfWord(hi, lo)                    ((hi << 8) | lo)
#define bitRead(value, bit)                 (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)                  ((value) |= (1UL << (bit)))
#define bitClear(value, bit)                ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)      ((bitvalue) ? bitSet(value, bit) : \
                                            			  bitClear(value, bit))

// Constants for the Semtech UDP protocol
#define PROTOCOL_VERSION  0x02
#define PKT_MIN_SIZE      12
#define PKT_PUSH_DATA     0x00
#define PKT_PUSH_ACK      0x01
#define PKT_PULL_DATA     0x02
#define PKT_PULL_RESP     0x03
#define PKT_PULL_ACK      0x04
#define PKT_TX_ACK        0x05


#define PKT_TX_ACK_DATA_SIZE    27
#define PKT_PULL_DATA_FREQUENCY 30 // every 30 seconds

#define GATEWAY_EUI_SIZE        8
#define GATEWAY_EUI_DEFAULT     "52db527edd69b3df"
#define FORWARDER_PORT_DEFAULT  1700
#define FORWARDER_QUEUE_SIZE    10

#define FORWARDER_DEV_EUI_SIZE      8
#define FORWARDER_PKT_MAX_SIZE      1024

typedef struct forwarder_data_t{
    uint8_t DevEUI[FORWARDER_DEV_EUI_SIZE];
	uint32_t packetSize;
    uint8_t packet[FORWARDER_PKT_MAX_SIZE];
} Forwarder_data_t;


class Handler {
private:
    uint8_t  gatewayEUI[GATEWAY_EUI_SIZE];
    std::map<uint16_t, uint8_t[FORWARDER_DEV_EUI_SIZE]> mapping;
    uint16_t tokenX;
    uint16_t tokenY;
    uint16_t tokenZ;

    uint32_t hexStringToArray(const char * hexString, uint8_t * pArray);

    uint16_t randomU16();

public:
	Handler(const char * gatewayEUI);

    void setGatewayEUI(const char * gatewayEUI);

    bool validateTokenX(const uint8_t *data);

    bool validateTokenY(const uint8_t *data);

    uint16_t saveTokenZ(const uint8_t *data);

	uint32_t pushData(const uint8_t *data, uint32_t size, const uint8_t * DevEUI,  uint8_t *packet);

	bool pushAck(const uint8_t *data);

	uint32_t pullData(uint8_t *packet);

	bool pullAck(const uint8_t *data);

	uint16_t pullResp(const uint8_t *data);

	uint32_t txAck(uint16_t tokenZ, uint8_t *packet);

	uint32_t getDevEUI(uint8_t * pDevEUI);

};



class ForwarderClass {
private:
	IPAddress host;
	uint16_t port;
	Handler handler = Handler(GATEWAY_EUI_DEFAULT);
	AsyncUDP udp;
	TaskHandle_t periodicTaskHandler = NULL;
	
	

	void setup(void);
	void loop(void);

public:
	ForwarderClass(IPAddress host, uint16_t port = FORWARDER_PORT_DEFAULT);
    void handle(const uint8_t * data, uint32_t size);
	void setHost(IPAddress host);
	void setPort(uint16_t port);
	IPAddress * getHost();
	uint16_t getPort();
	AsyncUDP * getUDP(void);
	Handler * getHandler(void);
	void main(void);


};

/********************* Exported Global Variables **********************/

extern QueueHandle_t qGatewayToForwarder;
extern QueueHandle_t qForwarderToGateway;
extern ForwarderClass Forwarder;


/*********************** Periodic Task Entry ************************/

void periodicTaskEntry(void * parameter);


/*********************** Global Function Prototypes ************************/

void printForwarderData(Forwarder_data_t *fData);

#endif /* __FORWARDER_H__ */