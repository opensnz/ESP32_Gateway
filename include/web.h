#ifndef __WEB_H__
#define __WEB_H__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Arduino_JSON.h>
#include "device.h"


#define WEB_PORT        80

#define WEB_PARAM_SSID  "ssid"
#define WEB_PARAM_PASS  "pass"
#define WEB_PATH_SSID   "/ssid.txt"
#define WEB_PATH_PASS   "/pass.txt"

#define WEB_DEFAULT_IP   IPAddress(192, 168, 1, 1)
#define WEB_DEFAULT_GW   IPAddress(192, 168, 1, 255)
#define WEB_DEFAULT_SN   IPAddress(255, 255, 255, 0)

#define WEB_DEFAULT_SSID "OPENSNZ_GATEWAY"
#define WEB_DEFAULT_PASS "opensnztech"

#define WEB_WIFI_TIMEOUT 10000 // 10 seconds
#define WEB_HTTP_OK                    200
#define WEB_HTTP_BAD_REQUEST           400
#define WEB_HTTP_INTERNAL_SERVER_ERROR 500

#define WEB_PARAM_DEVEUI "deveui"
#define WEB_PARAM_APPEUI "appeui"
#define WEB_PARAM_APPKEY "appkey"

#define WEB_PARAM_GWEUI  "gweui"
#define WEB_PARAM_FWHOST "host"
#define WEB_PARAM_FWPORT "port"

class WebClass
{
private:
    String ssid;
    String pass;
    wifi_event_id_t disconnectedID;

    void initWeb(void);
    void initWiFiAP(void);
    void initWiFiSTA(void);
    void serverWiFiConfig(void);
    void serverGatewayConfig(void);
    void server(wifi_mode_t mode);

public:
    void begin(void);
    bool serverGetDevices(JSONVar & body);
    bool serverAddDevice(JSONVar & body);
    bool serverDeleteDevice(JSONVar & body);
};

/******************* Exported Global Variables ************************/

extern WebClass Web;


/******************* Global Function Prototypes ************************/

String gatewayConfig(const String & var);

#endif /* __WEB_H__ */