/**
  ******************************************************************************
  * @file    transceiver.h
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

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/