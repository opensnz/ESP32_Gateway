
#ifndef __FORWARDER_H__
#define __FORWARDER_H__

#include <Arduino.h>
#include <freertos/queue.h>
#include <map>
#include <WiFi.h>
#include <AsyncUDP.h>
#include "device.h"
#include "log.h"

#define halfWord(hi, lo)                    ((hi << 8) | lo)
#define bitRead(value, bit)                 (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)                  ((value) |= (1UL << (bit)))
#define bitClear(value, bit)                ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)      ((bitvalue) ? bitSet(value, bit) : \
                                                          bitClear(value, bit))

// Constants for Semtech UDP protocol
#define PROTOCOL_VERSION  0x02
#define PKT_MIN_SIZE      12
#define PKT_PUSH_DATA     0x00
#define PKT_PUSH_ACK      0x01
#define PKT_PULL_DATA     0x02
#define PKT_PULL_RESP     0x03
#define PKT_PULL_ACK      0x04
#define PKT_TX_ACK        0x05

#define PKT_MAX_SIZE            1024
#define PKT_TX_ACK_DATA_SIZE    27
#define PKT_PULL_DATA_FREQUENCY 30 // every 30 seconds

#define FORWARDER_GATEWAY_EUI_SIZE        8
#define FORWARDER_GATEWAY_FILE            "/gateway.json"
#define FORWARDER_HOST_DEFAULT            IPAddress(192, 168, 10, 67)
#define FORWARDER_PORT_DEFAULT            1700

#define FORWARDER_QUEUE_SIZE    DEVICE_TOTAL


typedef struct forwarder_data_t{
    uint32_t packetSize;
    uint8_t packet[PKT_MAX_SIZE];
} Forwarder_data_t;


class Handler {
private:
    uint8_t  gatewayEUI[FORWARDER_GATEWAY_EUI_SIZE];
    uint16_t tokenX;
    uint16_t tokenY;
    uint16_t tokenZ;
    
    uint16_t randomU16();

public:

    float gatewayLong;
    float gatewayLati;
    uint32_t gatewayAlti;
    uint32_t statTimestamp;
    uint32_t statInterval;
    uint32_t aliveInterval;
    uint32_t loraRxNb;
    uint32_t loraTxNb;
    uint32_t pktRxNb;
    uint32_t pktTxNb;
    uint32_t pktAckNb;

    Handler();

    void setGatewayID(String id);
    
    uint32_t getGatewayID(uint8_t * pGatewayEUI);

    bool validateTokenX(const uint8_t *data);

    bool validateTokenY(const uint8_t *data);

    uint16_t saveTokenZ(const uint8_t *data);

    uint32_t pushData(const uint8_t *data, uint32_t size, uint8_t *packet);

    bool pushAck(const uint8_t *data);

    uint32_t pullData(uint8_t *packet);

    bool pullAck(const uint8_t *data);

    uint16_t pullResp(const uint8_t *data);

    uint32_t txAck(uint16_t tokenZ, uint8_t *packet);

    uint32_t pushStat(uint8_t *packet);
};



class ForwarderClass {
private:
    IPAddress host;
    uint16_t port;
    Handler handler = Handler();
    AsyncUDP udp;
    TaskHandle_t periodicTaskHandler = NULL;

    void setup(void);
    void loop(void);

public:
    ForwarderClass(IPAddress host, uint16_t port = FORWARDER_PORT_DEFAULT);
    bool loadConfig(void);
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
extern TaskHandle_t hForwarder;
extern ForwarderClass Forwarder;




/*********************** Periodic Task Entry ************************/

void periodicTaskEntry(void * parameter);


/*********************** Global Function Prototypes ************************/

void printForwarderData(Forwarder_data_t *fData);

#endif /* __FORWARDER_H__ */