#include "web.h"
#include "system.h"
#include "common.h"
#include "log.h"
#include "forwarder.h"

WebClass Web;
AsyncWebServer Server(WEB_PORT);
char SSID[64];
char PASS[32];

void WebClass::initWeb(void){

    if(!System.readFile(WEB_PATH_SSID, this->ssid))
    {
        // Can't read file
        this->ssid = "";
    }
    if(!System.readFile(WEB_PATH_PASS, this->pass))
    {
        // Can't read file
        this->pass = "";
    }
}

void WebClass::initWiFiAP(void){
    SYSTEM_PRINT_LN("Setting AP (Access Point)");
    if (!WiFi.softAPConfig(WEB_DEFAULT_IP, WEB_DEFAULT_GW, WEB_DEFAULT_SN)){
        SYSTEM_PRINT_LN("AP Failed to configure");
    }
    if(WiFi.softAP(WEB_DEFAULT_SSID, WEB_DEFAULT_PASS))
    {
        SYSTEM_PRINT("AP IP address: ");SYSTEM_PRINT_LN(WiFi.softAPIP());
        this->server(WIFI_AP);
    }
}

void WebClass::initWiFiSTA(void){
    WiFi.mode(WIFI_STA);
    memcpy(SSID, this->ssid.c_str(), this->ssid.length()+1);
    memcpy(PASS, this->pass.c_str(), this->pass.length()+1);
    SYSTEM_PRINT_LN(SSID);
    SYSTEM_PRINT_LN(PASS);
    WiFi.begin(SSID, PASS);
    SYSTEM_PRINT_LN("Connecting to WiFi...");

    unsigned long currentMillis = millis();
    unsigned long previousMillis = currentMillis;

    while(WiFi.status() != WL_CONNECTED) {
        currentMillis = millis();
        if (currentMillis - previousMillis >= WEB_WIFI_TIMEOUT)
        {
            SYSTEM_PRINT_LN("WiFi failed to connect.");
            WiFi.disconnect(true);
            this->initWiFiAP();
            return;
        }
        delay(100);
    }
    SYSTEM_PRINT_LN("WiFi connected.");
    SYSTEM_PRINT_LN(WiFi.localIP());

    // Set disconnected event callback
    Web.disconnectedID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
        SYSTEM_PRINT("WiFi lost connection. Reason: ");
        SYSTEM_PRINT_LN(info.wifi_sta_disconnected.reason);
        WiFi.removeEvent(Web.disconnectedID);
        Web.initWiFiSTA();
    }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    this->server(WIFI_STA);
}

void WebClass::begin(void){

    this->initWeb();

    if(this->ssid=="" || this->pass==""){
        this->initWiFiAP();
    }else
    {
        this->initWiFiSTA();
    }

}

void WebClass::server(wifi_mode_t mode){
    if(mode == WIFI_AP)
    {
        this->serverWiFiConfig();
    }else if(mode == WIFI_STA)
    {
        this->serverGatewayConfig();
    }
    Server.serveStatic("/", FILE_SYSTEM, "/");
    Server.begin();
}

void WebClass::serverWiFiConfig(void){
    Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(FILE_SYSTEM, "/wifimanager.html", "text/html", false);
    });
    Server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
        int params = request->params();
        for(int i=0; i<params; i++){
            AsyncWebParameter* p = request->getParam(i);
            if(p->isPost())
            {
                SYSTEM_PRINT_LN(p->value());
                if (p->name() == WEB_PARAM_SSID) 
                {
                    String ssid = p->value();
                    System.writeFile(WEB_PATH_SSID, ssid);
                }
                else if(p->name() == WEB_PARAM_PASS) 
                {
                    String pass = p->value();
                    System.writeFile(WEB_PATH_PASS, pass);
                }
            }
        }
        request->send(WEB_HTTP_OK, "text/plain", "Done. Gateway will restart");
        delay(1000);
        System.restart();
    });
}

//TODO
void WebClass::serverGatewayConfig(void){
    Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(FILE_SYSTEM, "/index.html", "text/html", false, gatewayConfig);
    });

    Server.on("/system", HTTP_GET, 
        [](AsyncWebServerRequest *request)
        {
            String system = "{}";
            if(System.getSystemInfo(system))
            {
                request->send(WEB_HTTP_OK, "application/json", system);
            }else
            {
                request->send(WEB_HTTP_INTERNAL_SERVER_ERROR);
            }
        }
    );

    Server.on("/gateway", HTTP_POST, 
        [](AsyncWebServerRequest *request){}, NULL, 
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            SYSTEM_PRINT_LN("################ Body ##############");
            JSONVar body = JSON.parse(String(data, len));
            SYSTEM_PRINT_LN(body);
            request->send(WEB_HTTP_OK);
        }
    );

    Server.on("/device/all", HTTP_GET, [](AsyncWebServerRequest *request){
        JSONVar devices;
        if(Web.serverGetDevices(devices))
        {
            request->send(WEB_HTTP_OK, "application/json", JSON.stringify(devices));
        }else
        {
            request->send(WEB_HTTP_INTERNAL_SERVER_ERROR);
        }
    });
    Server.on("/device/add", HTTP_POST,
        [](AsyncWebServerRequest *request){}, NULL, 
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            JSONVar body = JSON.parse(String(data, len));
            if(!body.hasOwnProperty("DevEUI"))
            {
                return request->send(WEB_HTTP_BAD_REQUEST, "application/json", 
                        JSON.stringify("{\"error\" : \"DevEUI required\"}"));
            }
            if(!body.hasOwnProperty("AppEUI"))
            {
                return request->send(WEB_HTTP_BAD_REQUEST, "application/json", 
                        JSON.stringify("{\"error\" : \"AppEUI required\"}"));
            }
            if(!body.hasOwnProperty("AppKey"))
            {
                return request->send(WEB_HTTP_BAD_REQUEST, "application/json", 
                        JSON.stringify("{\"error\" : \"AppKey required\"}"));
            }
            if(Web.serverAddDevice(body))
            {
                request->send(WEB_HTTP_OK);
            }else
            {
                request->send(WEB_HTTP_INTERNAL_SERVER_ERROR);
            }
        }
    );


    Server.on("/device/delete", HTTP_POST,
        [](AsyncWebServerRequest *request){}, NULL, 
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            JSONVar body = JSON.parse(String(data, len));
            if(!body.hasOwnProperty("DevEUI"))
            {
                return request->send(WEB_HTTP_BAD_REQUEST, "application/json", 
                        JSON.stringify("{\"error\" : \"DevEUI required\"}"));
            }
            if(Web.serverDeleteDevice(body))
            {
                request->send(WEB_HTTP_OK);
            }else
            {
                request->send(WEB_HTTP_INTERNAL_SERVER_ERROR);
            }
        }
    );
}


bool WebClass::serverGetDevices(JSONVar & body)
{
    String path, content;
    if(!System.readFile(DEVICE_FILE_CONFIG, content))
    {
        return false;
    }
    JSONVar config = JSON.parse(content);
    uint8_t length = (uint8_t)config.length();
    for(uint8_t i=0; i<length; i++)
    {
        path = String("/") + (const char *)config[i] + DEVICE_FILE_INFO_EXT;
        Device_info_t info;
        if(!System.readFile(path, (uint8_t *)(&info), sizeof(Device_info_t)))
        {
            return false;
        }
        String data;
        arrayToHexString(info.DevEUI, DEVICE_DEV_EUI_SIZE, data);
        body[i]["DevEUI"] = data;
        arrayToHexString(info.AppEUI, DEVICE_APP_EUI_SIZE, data);
        body[i]["AppEUI"] = data;
        arrayToHexString(info.AppKey, DEVICE_APP_KEY_SIZE, data);
        body[i]["AppKey"] = data;
        arrayToHexString(info.NwkSKey, DEVICE_NWK_SKEY_SIZE, data);
        body[i]["NwkSKey"] = data;
        arrayToHexString(info.AppSKey, DEVICE_APP_SKEY_SIZE, data);
        body[i]["AppSKey"] = data;
        body[i]["DevNonce"] = info.DevNonce;
        body[i]["DevAddr"] = info.DevAddr;
        body[i]["FPort"] = info.FPort;
        body[i]["FCnt"] = info.FCnt;
    }
    return true;
}
bool WebClass::serverAddDevice(JSONVar & body)
{
    Device_info_t info;
    if(strlen((const char *)body["DevEUI"]) != 2*DEVICE_DEV_EUI_SIZE)
    {
        return false;
    }
    hexStringToArray((const char *)body["DevEUI"], info.DevEUI);
    if(strlen((const char *)body["AppEUI"]) != 2*DEVICE_APP_EUI_SIZE)
    {
        return false;
    }
    hexStringToArray((const char *)body["AppEUI"], info.AppEUI);
    if(strlen((const char *)body["AppKey"]) != 2*DEVICE_APP_KEY_SIZE)
    {
        return false;
    }
    hexStringToArray((const char *)body["AppKey"], info.AppKey);
    return Device.addDevice(info);
}

bool WebClass::serverDeleteDevice(JSONVar & body)
{
    Device_info_t info;
    if(strlen((const char *)body["DevEUI"]) != 2*DEVICE_DEV_EUI_SIZE)
    {
        return false;
    }
    hexStringToArray((const char *)body["DevEUI"], info.DevEUI);
    return Device.removeDevice(info);
}

/*********************** Global Function Implementations ************************/


String gatewayConfig(const String & var){
    return String();
}