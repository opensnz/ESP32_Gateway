/**
  ******************************************************************************
  * @file    forwarder.cpp
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

#include "forwarder.h"
#include "common.h"
#include "system.h"
#include "rgb.h"

QueueHandle_t qGatewayToForwarder = NULL;
QueueHandle_t qForwarderToGateway = NULL;
TaskHandle_t hForwarder   = NULL;

ForwarderClass Forwarder;

ForwarderClass::ForwarderClass(){}

ForwarderClass::ForwarderClass(const char * host, uint16_t port){
    this->host.fromString(host);
    this->port = port;
}

bool ForwarderClass::loadConfig(void){
    String content;
    JSONVar config;
    if(System.readFile(FORWARDER_GATEWAY_FILE, content))
    {
        config = JSON.parse(content);
        this->host.fromString((const char *)config["host"]);
        this->port = (uint16_t)config["port"];
        this->handler.setGatewayID((const char *)config["id"]);
        this->handler.gatewayLati = (double)config["lat"];
        this->handler.gatewayLong = (double)config["lon"];
        this->handler.gatewayAlti = (uint32_t)config["alt"];
        this->handler.aliveInterval = (uint32_t)config["alive"];
        this->handler.statInterval  = (uint32_t)config["stat"];
        return true;
    }
    return false;
}

void ForwarderClass::setup(void){
    while(!this->loadConfig())
    {
        SYSTEM_LOGF_LN("Loading Config Info failed");
        delay(100);
    }
    qGatewayToForwarder = xQueueCreate(FORWARDER_QUEUE_SIZE, sizeof(Forwarder_data_t));
    qForwarderToGateway = xQueueCreate(FORWARDER_QUEUE_SIZE, sizeof(Forwarder_data_t));
    if (qGatewayToForwarder == NULL)
    {
        SYSTEM_LOGF_LN("Failed to create queue= %p", qGatewayToForwarder);
    }
    if (qForwarderToGateway == NULL)
    {
        SYSTEM_LOGF_LN("Failed to create queue= %p", qForwarderToGateway);
    }

    if(udp.listen(IPAddress(0,0,0,0), this->port)) {
        SYSTEM_LOG("UDP listenning on port ");
        SYSTEM_PRINT_LN(this->port);
        udp.onPacket([](AsyncUDPPacket packet) {
            SYSTEM_LOG_LN("UDP Packet Receipt Notification");
            Forwarder.handle(packet.data(), packet.length());
        });
    }else{
        SYSTEM_LOG("UDP not listenning on port ");
        SYSTEM_PRINT_LN(this->port);
    }

    xTaskCreatePinnedToCore(periodicTaskEntry, "periodicTask",  10000, NULL, 1, &periodicTaskHandler, 1);            
  
}

void ForwarderClass::loop(void){
    Forwarder_data_t fData;
    BaseType_t status = pdFALSE;
    while(true)
    {
        if(qGatewayToForwarder == NULL)
        {
            SYSTEM_PRINT_LN("Waiting for Queue GatewayToForwarder Init");
            delay(100);
            continue;
        }
        status = xQueueReceive(qGatewayToForwarder, &fData, portMAX_DELAY);
        if(status == pdFALSE)
        {
            continue;
        }
        uint8_t packet[PKT_MIN_SIZE + fData.packetSize];
        SYSTEM_PRINT_LN("##################################################");
        SYSTEM_LOG("PUSH DATA : ");
        this->handler.pushData(fData.packet, fData.packetSize, packet);
        SYSTEM_PRINT_LN(String(packet, PKT_MIN_SIZE + fData.packetSize));
        SYSTEM_PRINT_LN("##################################################");
        this->udp.writeTo(packet, PKT_MIN_SIZE + fData.packetSize, this->host, this->port);  
    }
    
}

void ForwarderClass::setHost(const char * host){
    this->host.fromString(host);
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
    SYSTEM_PRINT_LN("Packet Forwarder setting...");
    this->setup();
    SYSTEM_PRINT_LN("Packet Forwarder running...");
    this->loop();
}

void ForwarderClass::handle(const uint8_t * data, uint32_t size){
    uint8_t packetType = data[3];
    switch (packetType)
    {
        case PKT_PUSH_DATA:
            SYSTEM_LOG_LN("PKT_PUSH_DATA");
            break;
        case PKT_PUSH_ACK:
            SYSTEM_LOG_LN("PKT_PUSH_ACK");
            break;
        case PKT_PULL_DATA:
            SYSTEM_LOG_LN("PKT_PULL_DATA");
            break;
        case PKT_PULL_ACK:
            SYSTEM_LOG_LN("PKT_PULL_ACK");
            this->isServerOnline = true;
            if(!RGB.isGreenColor())
            {
                RGB.selectColor(false, true, false);
            }
            break;
        case PKT_PULL_RESP:
            SYSTEM_LOG_LN("PKT_PULL_RESP");
            uint16_t tokenZ = this->handler.pullResp(data);
            uint8_t packet[PKT_MIN_SIZE + PKT_TX_ACK_DATA_SIZE];
            this->handler.txAck(tokenZ, packet);
            this->udp.writeTo(packet, PKT_MIN_SIZE + PKT_TX_ACK_DATA_SIZE, this->host, this->port);
            Forwarder_data_t fData;
            fData.packetSize = size - 4;
            for(int i=0; i<fData.packetSize; i++)
            {
                fData.packet[i] = data[i+4];
            }
            if(qForwarderToGateway != NULL)
            {
                xQueueSend(qForwarderToGateway, &fData, portMAX_DELAY);
            }
            break;
    }
}



/********************** Periodic Task Entry *************************************/

void periodicTaskEntry(void * parameter){
    SYSTEM_PRINT_LN("periodicTaskEntry");
    time_t timestamp;
    String timeGMT;
    uint32_t length;
    uint8_t packet[PKT_MAX_SIZE];


    while(true)
    {
        while(WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
        }
        
        timestamp = RTC.getEpoch();

        if((timestamp - Forwarder.getHandler()->statTimestamp) >= Forwarder.getHandler()->statInterval)
        {
            Forwarder.getHandler()->statTimestamp = (uint32_t)timestamp;
            length = Forwarder.getHandler()->pushStat(packet);
            SYSTEM_PRINT_LN("##################################################");
            SYSTEM_LOG("PKT_PUSH_DATA : ");
            SYSTEM_PRINT_LN(String(packet, length));
            SYSTEM_PRINT_LN("##################################################");
            
            Forwarder.getUDP()->writeTo(packet, length, *(Forwarder.getHost()), Forwarder.getPort());
        }
        
        if(Forwarder.getHandler()->pktTxNb == UINT32_MAX - 1)
        {
            Forwarder.getHandler()->loraRxNb = 0;
            Forwarder.getHandler()->loraTxNb = 0;
            Forwarder.getHandler()->pktAckNb = 0;
            Forwarder.getHandler()->pktRxNb = 0;
            Forwarder.getHandler()->pktTxNb = 0;
        }

        SYSTEM_LOG_LN("PKT_PULL_DATA");
        Forwarder.getHandler()->pullData(packet);
        Forwarder.getUDP()->writeTo(packet, PKT_MIN_SIZE, *(Forwarder.getHost()), Forwarder.getPort());
        Forwarder.isServerOnline = false;
        delay(Forwarder.getHandler()->aliveInterval * S_TO_MS_FACTOR);
        if(!Forwarder.isServerOnline)
        {
            if(WiFi.status() == WL_CONNECTED)
            {
                RGB.selectColor(false, false, true);
            }
        }
    }
}



/********************** Handler Class Implementation ****************************/


Handler::Handler(){
    this->tokenX = 0x0000;
    this->tokenY = 0x0000;
    this->tokenZ = 0x0000;
}

void Handler::setGatewayID(String id){

    hexStringToArray(id, this->gatewayEUI);

}

uint32_t Handler::getGatewayID(uint8_t *pGatewayEUI){
    for(int i=0; i<FORWARDER_GATEWAY_EUI_SIZE; i++)
    {
        pGatewayEUI[i] = this->gatewayEUI[i];
    }
    return FORWARDER_GATEWAY_EUI_SIZE;
}

uint16_t Handler::randomU16() {
    return (uint16_t)random();
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

uint32_t  Handler::pushData(const uint8_t *data, uint32_t size, uint8_t *packet){
    this->tokenX = this->randomU16();
    this->loraRxNb++;
    this->pktTxNb++;
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
    this->pktAckNb++;
    this->pktRxNb++;
    return this->validateTokenX(data);
}

uint32_t Handler::pullData(uint8_t *packet){
    this->tokenY = this->randomU16();
    this->pktTxNb++;
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
    this->pktAckNb++;
    this->pktRxNb++;
    return this->validateTokenY(data);
}

uint16_t Handler::pullResp(const uint8_t *data){
    this->pktRxNb++;
    return this->saveTokenZ(data);
}

uint32_t Handler::txAck(uint16_t tokenZ, uint8_t *packet){
    char data[] =  "{\"txpk_ack\":{\"error\":\"NONE\"}}";
    this->pktTxNb++;
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

uint32_t Handler::pushStat(uint8_t *packet)
{
    JSONVar data;
    uint32_t packetSize;
    float ackr = 0.00;
    if(this->pktTxNb > 0)
    {
        ackr = (100.0 * this->pktAckNb)/this->pktTxNb;
    }
    data["stat"]["time"] = RTC.getTime("%Y-%m-%d %H:%M:%S GMT");
    data["stat"]["lati"] = this->gatewayLati;
    data["stat"]["long"] = this->gatewayLong;
    data["stat"]["alti"] = this->gatewayAlti;
    data["stat"]["rxnb"] = this->loraRxNb;
    data["stat"]["rxok"] = this->loraRxNb;
    data["stat"]["rxfw"] = this->loraTxNb;
    data["stat"]["ackr"] = ackr;
    data["stat"]["dwnb"] = this->pktRxNb;
    data["stat"]["txnb"] = this->pktTxNb;
    String jsonData = JSON.stringify(data);
    packetSize = this->pushData((uint8_t *)jsonData.c_str(), jsonData.length(), packet);
    this->loraRxNb--;
    return packetSize;
}


/*********************** Global Function Implementations ************************/

void printForwarderData(Forwarder_data_t *fData)
{
    SYSTEM_PRINT_LN("\n################ Forwarder Data ################");
    SYSTEM_LOG("Packet = ");
    for(int i=0; i<fData->packetSize; i++){
        SYSTEM_PRINTF("%02X", fData->packet[i]);
    }
    SYSTEM_PRINT_LN("\n##################################################");
}


/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/