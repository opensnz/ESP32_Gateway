
#ifndef __TRANSCEIVER_H__
#define __TRANSCEIVER_H__

#include <Arduino.h>
#include <freertos/queue.h>
#include <SPI.h>
#include <LoRa.h>
#include "log.h"

#define halfWord(hi, lo)                    ((hi << 8) | lo)
#define bitRead(value, bit)                 (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)                  ((value) |= (1UL << (bit)))
#define bitClear(value, bit)                ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)      ((bitvalue) ? bitSet(value, bit) : \
                                            			  bitClear(value, bit))

#define LORA_FREQUENCY_DEFAULT  868000000U        // LoRa default RF frequency
#define LORA_CS_PIN             GPIO_NUM_18       // LoRa radio chip select
#define LORA_RESET_PIN          GPIO_NUM_14       // LoRa radio reset
#define LORA_IRQ_PIN            GPIO_NUM_26       // change for your board; must be a hardware interrupt pin

#define TRANSCEIVER_QUEUE_SIZE        10
#define TRANSCEIVER_DEV_EUI_SIZE      8
#define TRANSCEIVER_PAYLOAD_MAX_SIZE  256

typedef struct transceiver_data_t{
    uint8_t DevEUI[TRANSCEIVER_DEV_EUI_SIZE];
    uint16_t rssi;
    uint16_t snr;
    uint32_t payloadSize;
    uint8_t payload[TRANSCEIVER_PAYLOAD_MAX_SIZE];
} Transceiver_data_t;

 
class TransceiverClass {
private:
	uint32_t freq;
	
	void setup(void);
	void loop(void);
    

public:
	TransceiverClass(uint32_t freq);
	void main(void);
};


/******************* Exported Global Variables ************************/

extern QueueHandle_t qTransceiverToGateway;
extern TransceiverClass Transceiver;


/******************* Global Function Prototypes ************************/

void onReceiveLoRaNotification(int packetSize);
void printTransceiverData(Transceiver_data_t *tData);

#endif /* __TRANSCEIVER_H__ */