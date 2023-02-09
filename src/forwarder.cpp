#include "forwarder.h"

QueueHandle_t qGatewayToForwarder = NULL;
QueueHandle_t qForwarderToGateway = NULL;

ForwarderClass Forwarder = ForwarderClass(IPAddress(192, 168, 217, 62), FORWARDER_PORT_DEFAULT);

ForwarderClass::ForwarderClass(IPAddress host, uint16_t port){
    this->host = host;
    this->port = port;
}

void ForwarderClass::setup(void){

    qGatewayToForwarder = xQueueCreate(FORWARDER_QUEUE_SIZE, sizeof(Forwarder_data_t));
    qForwarderToGateway = xQueueCreate(FORWARDER_QUEUE_SIZE, sizeof(Forwarder_data_t));
    if (qGatewayToForwarder == NULL)
    {
        APP_LOGF_LN("Failed to create queue= %p", qGatewayToForwarder);
    }
    if (qForwarderToGateway == NULL)
    {
        APP_LOGF_LN("Failed to create queue= %p", qForwarderToGateway);
    }

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        APP_LOG_LN("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }

    if(udp.listen(IPAddress(0,0,0,0), this->port)) {
        APP_LOG_LN("UDP connected");
        udp.onPacket([](AsyncUDPPacket packet) {
            APP_LOG_LN("UDP Packet Received Notification");
            Forwarder.handle(packet.data(), packet.length());
        });
    }else{
        APP_LOG_LN("UDP not connected");
    }

    xTaskCreatePinnedToCore(periodicTaskEntry, "periodicTask",  2000, NULL, 1, &periodicTaskHandler, 1);            
  
}

void ForwarderClass::loop(void){
    Forwarder_data_t fData;
    BaseType_t status = pdFALSE;
    while(true)
    {
        if(qGatewayToForwarder == NULL)
        {
            APP_PRINT_LN("Waiting for Queue GatewayToForwarder Init");
            delay(100);
        }else
        {
            status = xQueueReceive(qGatewayToForwarder, &fData, portMAX_DELAY);
            if(status == pdTRUE)
            {
                uint8_t packet[PKT_MIN_SIZE + fData.packetSize];
                APP_PRINT_LN("\n##################################################");
                APP_LOG("PUSH DATA : ");
                this->handler.pushData(fData.packet, fData.packetSize, fData.DevEUI, packet);
                APP_PRINT_LN(String(packet, PKT_MIN_SIZE + fData.packetSize));
                APP_PRINT_LN("##################################################");
                this->udp.writeTo(packet, PKT_MIN_SIZE + fData.packetSize, this->host, this->port);
            }
        }
    }
    
}

void ForwarderClass::setHost(IPAddress host){
    this->host = host;
}

void ForwarderClass::setPort(uint16_t port){
    this->port = port;
}

IPAddress * ForwarderClass::getHost(){
    return &this->host;
}

uint16_t ForwarderClass::getPort(){
    return this->port;
}

AsyncUDP * ForwarderClass::getUDP(void){
    return &this->udp;
}

Handler * ForwarderClass::getHandler(void){
    return &this->handler;
}

void ForwarderClass::main(void){
    APP_PRINT_LN("Packet ForwarderClass setting...");
    this->setup();
    APP_PRINT_LN("Packet ForwarderClass running...");
    this->loop();
}

void ForwarderClass::handle(const uint8_t * data, uint32_t size){
    uint8_t packetType = data[3];
    switch (packetType)
    {
        case PKT_PUSH_DATA:
            APP_LOG_LN("PKT_PUSH_DATA");
            break;
        case PKT_PUSH_ACK:
            APP_LOG_LN("PKT_PUSH_ACK");
            break;
        case PKT_PULL_DATA:
            APP_LOG_LN("PKT_PULL_DATA");
            break;
        case PKT_PULL_ACK:
            APP_LOG_LN("PKT_PULL_ACK");
            break;
        case PKT_PULL_RESP:
            APP_LOG_LN("PKT_PULL_RESP");
            uint16_t tokenZ = this->handler.pullResp(data);
            uint8_t packet[PKT_MIN_SIZE + PKT_TX_ACK_DATA_SIZE];
            this->handler.txAck(tokenZ, packet);
            this->udp.writeTo(packet, PKT_MIN_SIZE + PKT_TX_ACK_DATA_SIZE, this->host, this->port);
            Forwarder_data_t fData;
            fData.packetSize = size - 4;
            for(int i=0; i<fData.packetSize; i++){
                fData.packet[i] = data[i+4];
            }
            /* Set DevEUI */
            xQueueSend(qForwarderToGateway, &fData, portMAX_DELAY);
            break;
    }
}



/********************** Periodic Task Entry *************************************/

void periodicTaskEntry(void * parameter){
    APP_PRINT_LN("periodicTaskEntry");
    uint8_t packet[PKT_MIN_SIZE];
    while(true)
    {
        APP_LOG_LN("PKT_PULL_DATA");
        APP_LOG_LN(*Forwarder.getHost());
        Forwarder.getHandler()->pullData(packet);
        APP_LOG("PKT : ");
        for(int i=0; i<PKT_MIN_SIZE; i++)
        {
            APP_PRINTF("%02X ", packet[i]);
        }
        APP_PRINT_LN();
        Forwarder.getUDP()->writeTo(packet, PKT_MIN_SIZE, *(Forwarder.getHost()), Forwarder.getPort());
        delay(PULL_DATA_FREQUENCY * 1000);
    }
}



/********************** Handler Class Implementation ****************************/


Handler::Handler(const char * gatewayEUI){
    this->hexStringToArray(gatewayEUI, this->gatewayEUI);
    this->tokenX = 0x0000;
    this->tokenY = 0x0000;
    this->tokenZ = 0x0000;
}

uint32_t Handler::hexStringToArray(const char * hexString, uint8_t *pArray) {
    char hex2Bytes[3]  = "FF";
    for (int i = 0; i < GATEWAY_EUI_SIZE; i += 1) {
        hex2Bytes[0] = hexString[2*i];
        hex2Bytes[1] = hexString[2*i+1];
        pArray[i] = (uint8_t) strtol(hex2Bytes, NULL, 16);
    }
    return GATEWAY_EUI_SIZE;
}

uint16_t Handler::randomU16() {
    return (uint16_t)random();
}


void Handler::setGatewayEUI(const char * gatewayEUI) {
    hexStringToArray(gatewayEUI, this->gatewayEUI);
}

bool Handler::validateTokenX(const uint8_t *data) {
    uint16_t token = halfWord(data[1], data[2]);
    return this->tokenX == token;
}

bool Handler::validateTokenY(const uint8_t *data) {
    uint16_t token = halfWord(data[1], data[2]);
    return this->tokenY == token;
}

uint16_t Handler::saveTokenZ(const uint8_t *data) {
    this->tokenZ = halfWord(data[1], data[2]);
    return this->tokenZ;
}

uint32_t  Handler::pushData(const uint8_t *data, uint32_t size,  const uint8_t * DevEUI, uint8_t *packet){
    this->tokenX = this->randomU16();
    packet[0] = PROTOCOL_VERSION;
    packet[1] = highByte(this->tokenX);
    packet[2] = lowByte(this->tokenX);
    packet[3] = PKT_PUSH_DATA;
    for(uint32_t i=4; i<PKT_MIN_SIZE; i++)
    {
        packet[i] = this->gatewayEUI[i-4];
    }
    for(uint32_t i=0; i<size; i++)
    {
        packet[PKT_MIN_SIZE + i] = data[i];
    }
    return (PKT_MIN_SIZE + size);
}

bool Handler::pushAck(const uint8_t *data){
    return this->validateTokenX(data);
}

uint32_t Handler::pullData(uint8_t *packet){
    this->tokenY = this->randomU16();
    packet[0] = PROTOCOL_VERSION;
    packet[1] = highByte(this->tokenY);
    packet[2] = lowByte(this->tokenY);
    packet[3] = PKT_PULL_DATA;
    for(uint32_t i=4; i<PKT_MIN_SIZE; i++)
    {
        packet[i] = this->gatewayEUI[i-4];
    }
    return PKT_MIN_SIZE;
}

bool Handler::pullAck(const uint8_t *data){
    return this->validateTokenY(data);
}

uint16_t Handler::pullResp(const uint8_t *data){
    return this->saveTokenZ(data);
}

uint32_t Handler::txAck(uint16_t tokenZ, uint8_t *packet){
    char data[] =  "{\"txpk_ack\":{\"error\":NONE}}";
    packet[0] = PROTOCOL_VERSION;
    packet[1] = highByte(tokenZ);
    packet[2] = lowByte(tokenZ);
    packet[3] = PKT_TX_ACK;
    for(uint32_t i=4; i<PKT_MIN_SIZE; i++)
    {
        packet[i] = this->gatewayEUI[i-4];
    }
    for(int i=0; i<PKT_TX_ACK_DATA_SIZE; i++)
    {
        packet[PKT_MIN_SIZE + i] = data[i];
    }
    return (PKT_MIN_SIZE + PKT_TX_ACK_DATA_SIZE);
}

uint32_t Handler::getDevEUI(uint8_t *pDevEUI){
    for(int i=0; i<FORWARDER_DEV_EUI_SIZE; i++)
    {
        pDevEUI[i] = this->gatewayEUI[i];
    }
    return FORWARDER_DEV_EUI_SIZE;
}


/*********************** Global Function Implementations ************************/

void printForwarderData(Forwarder_data_t *fData)
{
    APP_PRINT_LN("\n################ Forwarder Data ################");
    APP_LOG("DevEUI = ");
    for(int i=0; i<4; i++){
        APP_PRINTF("%02X", fData->DevEUI[i]);
    }
    APP_PRINT("\n");
    APP_LOG("Packet = ");
    for(int i=0; i<fData->packetSize; i++){
        APP_PRINTF("%02X", fData->packet[i]);
    }
    APP_PRINT_LN("\n##################################################");
}
