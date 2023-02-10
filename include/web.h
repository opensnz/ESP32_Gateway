#ifndef __WEB_H__
#define __WEB_H__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "system.h"
#include "log.h"

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

class WebClass
{
private:
    String ssid;
    String pass;

    void initWeb(void);
    void initWiFiAP(void);
    void initWiFiSTA(void);
    void serverWiFiConfig(void);
    void serverGatewayConfig(void);
    void server(wifi_mode_t mode);

public:
    void begin(void);
};

/******************* Exported Global Variables ************************/

extern WebClass Web;


/******************* Global Function Prototypes ************************/

String wifiConfig(const String & var);
String gatewayConfig(const String & var);

#endif /* __WEB_H__ */