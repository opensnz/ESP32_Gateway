
#ifndef __LORAWAN__
#define __LORAWAN__

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "lw_base64.h"

#define LORAWAN_MAX_FOPTS_LEN       (15)
#define LORAWAN_MAX_PAYLOAD_LEN     (224)


// LoRaWAN MAC header (MHDR)
typedef enum {
    MTYPE_JOIN_REQUEST = 0,
    MTYPE_JOIN_ACCEPT,
    MTYPE_UNCONFIRMED_DATA_UP,
    MTYPE_UNCONFIRMED_DATA_DOWN,
    MTYPE_CONFIRMED_DATA_UP,
    MTYPE_CONFIRMED_DATA_DOWN,
    MTYPE_REJOIN_REQUEST,
    MTYPE_PROPRIETARY = 0b111
} MHDR_MType_t;

typedef enum {
    LORAWAN_R1 = 0,
} MHDR_LoRaWAN_MajorVersion_t;

// part of FHDR_FCtrl_t
typedef struct {
    uint8_t ADR :1;         // Adaptive data rate control bit
    uint8_t RFU :1;         // Reserved for Future Use
    uint8_t ACK :1;
    uint8_t FPending :1;
    uint8_t FOptsLen :4;
} FHDR_FCtrl_downlink_t;

// part of FHDR_FCtrl_t
typedef struct {
    uint8_t ADR :1;         // Adaptive data rate control bit
    uint8_t ADRACKReq :1;
    uint8_t ACK :1;
    uint8_t ClassB :1;
    uint8_t FOptsLen :4;
} FHDR_FCtrl_uplink_t;

// part of FHDR_t
typedef union {
    FHDR_FCtrl_downlink_t downlink;
    FHDR_FCtrl_uplink_t uplink;
} FHDR_FCtrl_t;

// part of MACPayload_t
typedef struct {
    uint32_t DevAddr;
    FHDR_FCtrl_t FCtrl;
    uint16_t FCnt16; // only LSB of 32 bit frame Counter
    uint8_t FOpts[LORAWAN_MAX_FOPTS_LEN];
} FHDR_t;

// part of LoRaWAN_Packet_t (union)
typedef struct {
    FHDR_t FHDR;
    uint8_t NwkSKey[16];
    uint8_t AppSKey[16];
    uint8_t FPort; 	
    uint8_t payloadSize;
    uint8_t payload[LORAWAN_MAX_PAYLOAD_LEN];        
} MACPayload_t;

// part of LoRaWAN_Packet_t (union)
typedef struct {
    uint8_t AppEUI[8]; 
    uint8_t DevEUI[8];
    uint8_t AppKey[16];
    uint16_t DevNonce; 
} JoinRequest_t;

// part of JoinAccept_t
typedef struct {
    uint8_t Rx2DR :4;
    uint8_t Rx1DRoffset :3;
    uint8_t OptNeg :1;
} DLsettings_t;

// part of JoinAccept_t
typedef struct {
    uint8_t FreqCH4[3];
    uint8_t FreqCH5[3];
    uint8_t FreqCH6[3];
    uint8_t FreqCH7[3];
    uint8_t FreqCH8[3];
    // + RFU (1 Byte)
} CFlist_t;

// part of LoRaWAN_Packet_t (union)
typedef struct {
    uint32_t AppNonce;               // 24 bit (3 Byte), server nonce
    uint32_t NetID;                  // 24 bit (3 Byte), network identifier
    uint32_t DevAddr;                // 32 bit (4 Byte), end-device address
    DLsettings_t DLsettings;         // 8 bit (1 Byte), providing some of the downlink parameter
    uint8_t RxDelay;                 // 8 bit (1 Byte), the delay between TX and RX
    CFlist_t CFlist;                 // 16 byte, optional list of network parameters (e.g. frequencies for EU868)
    bool hasCFlist;
    uint16_t DevNonce; 
    uint8_t AppKey[16];
    uint8_t NwkSKey[16];
    uint8_t AppSKey[16];
} JoinAccept_t;

typedef union {
    JoinRequest_t JoinRequest;
    JoinAccept_t JoinAccept;
    MACPayload_t MACPayload;
} LoRaWAN_Packet_t;


extern LoRaWAN_Packet_t LoRaWAN;


uint8_t LoRaWAN_JoinRequest(JoinRequest_t * packet, uint8_t* buffer, uint8_t bufferSize);
bool LoRaWAN_JoinAccept(JoinAccept_t * packet, uint8_t* buffer, uint8_t bufferSize);
uint8_t LoRaWAN_UnconfirmedDataUp(MACPayload_t * packet, uint8_t* buffer, uint8_t bufferSize);
uint8_t LoRaWAN_ConfirmedDataUp(MACPayload_t * packet, uint8_t* buffer, uint8_t bufferSize);
uint8_t LoRaWAN_DataDown(MACPayload_t * packet, uint8_t* buffer, uint8_t bufferSize);

uint32_t LoRaWAN_Base64_To_Binary(const char * in, int size, uint8_t * out, int max_len);
uint32_t LoRaWAN_Binary_To_Base64(const uint8_t * in, int size, char * out, int max_len); 

#ifdef __cplusplus
	}
#endif

#endif // __LORAWAN__