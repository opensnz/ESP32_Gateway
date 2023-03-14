
#ifndef __TRANSCEIVER_H__
#define __TRANSCEIVER_H__

#include <Arduino.h>
#include <freertos/queue.h>
#include <SPI.h>
#include <LoRa.h>
#include "device.h"
#include "log.h"

#define halfWord(hi, lo)                    ((hi << 8) | lo)
#define bitRead(value, bit)                 (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)                  ((value) |= (1UL << (bit)))
#define bitClear(value, bit)                ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)      ((bitvalue) ? bitSet(value, bit) : \
                                            			  bitClear(value, bit))

#define LORA_FREQUENCY_DEFAULT  868E6        	  // LoRa default RF frequency
#define LORA_CS_PIN             GPIO_NUM_18       // LoRa radio chip select
#define LORA_RESET_PIN          GPIO_NUM_14       // LoRa radio reset
#define LORA_IRQ_PIN            GPIO_NUM_26       // change for your board; must be a hardware interrupt pin

#define TRANSCEIVER_QUEUE_SIZE        DEVICE_TOTAL
#define TRANSCEIVER_DEV_EUI_SIZE      8
#define TRANSCEIVER_PAYLOAD_MAX_SIZE  256

typedef struct transceiver_data_t{
    uint8_t DevEUI[DEVICE_DEV_EUI_SIZE];
    int rssi;
    float snr;
    uint32_t payloadSize;
    uint8_t payload[DEVICE_PAYLOAD_MAX_SIZE];
} Transceiver_data_t;

 
class TransceiverClass {
private:
	uint32_t freq;
	
	void setup(void);
	void loop(void);
	bool isBase64(const char * data);
    

public:
	TransceiverClass(uint32_t freq);
	void main(void);
};


/******************* Exported Global Variables ************************/

extern TaskHandle_t  hTransceiver;
extern QueueHandle_t qTransceiverToGateway;
extern TransceiverClass Transceiver;


/******************* Global Function Prototypes ************************/

void onReceiveLoRaNotification(int packetSize);
void printTransceiverData(Transceiver_data_t *tData);

#endif /* __TRANSCEIVER_H__ */