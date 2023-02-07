
#ifndef __TRANSCEIVER_H__
#define __TRANSCEIVER_H__

#include <Arduino.h>
#include <freertos/queue.h>
#include <SPI.h>
#include <LoRa.h>

#define halfWord(hi, lo)                    ((hi << 8) | lo)
#define bitRead(value, bit)                 (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)                  ((value) |= (1UL << (bit)))
#define bitClear(value, bit)                ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)      ((bitvalue) ? bitSet(value, bit) : \
                                            			  bitClear(value, bit))

#define LORA_FREQUENCY_DEFAULT  868000000U         // LoRa default RF frequency
#define LORA_CS_PIN             GPIO_NUM_18       // LoRa radio chip select
#define LORA_RESET_PIN          GPIO_NUM_14       // LoRa radio reset
#define LORA_IRQ_PIN            GPIO_NUM_26       // change for your board; must be a hardware interrupt pin

#define TRANSCEIVER_QUEUE_SIZE  10


typedef struct transceiver_data_t{
    uint8_t DevEUI[4];
    uint16_t rssi;
    uint16_t snr;
    uint32_t payloadSize;
    uint8_t payload[256];
} Transceiver_data_t;

 

void onReceiveLoRa(int packetSize);
void printTransceiverData(Transceiver_data_t *tData);


class TransceiverClass {
private:
	uint32_t freq;
	
	void setup(void);
	void loop(void);
    

public:
	TransceiverClass(uint32_t freq);
	void main(void);
};

extern QueueHandle_t qTransceiverToGateway;
extern TransceiverClass Transceiver;

#endif /* __TRANSCEIVER_H__ */