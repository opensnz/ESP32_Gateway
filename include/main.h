#include <Arduino.h>


/*************** Task Entry Prototypes *******************/

void TGatewayTaskEntry(void * parameter);

void FGatewayTaskEntry(void * parameter);

void TransceiverTaskEntry(void * parameter);

void ForwarderTaskEntry(void * parameter);



/*************** Function Prototypes *******************/

void httpPOSTRequest(String serverName, String body);

uint32_t hexStringToArray(const char * hexString, uint8_t *pArray);

uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, char * hexString);