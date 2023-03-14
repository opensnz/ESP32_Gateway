#include <Arduino.h>
#include <freertos/queue.h>

/*************** Task Entry Prototypes *******************/

void TGatewayTaskEntry(void * parameter);

void FGatewayTaskEntry(void * parameter);

void TransceiverTaskEntry(void * parameter);

void ForwarderTaskEntry(void * parameter);

