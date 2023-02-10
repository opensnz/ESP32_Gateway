#include "web.h"

WebClass Web;
AsyncWebServer Server(WEB_PORT);

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
    WiFi.begin(this->ssid.c_str(), this->pass.c_str());
    SYSTEM_PRINT_LN("Connecting to WiFi...");

    SYSTEM_PRINT_LN("Failed to connect.");

    // 
    SYSTEM_PRINT_LN("Connected.");
    this->server(WIFI_AP);
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
    SYSTEM_PRINT_LN("Web static files in /");
    Server.serveStatic("/", FILE_SYSTEM, "/");
    Server.begin();
}

void WebClass::serverWiFiConfig(void){
    Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        SYSTEM_PRINT_LN("/wifimanager.html");
        request->send(FILE_SYSTEM, "/wifimanager.html", "text/html", false, wifiConfig);
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
            }
            else if(p->name() == WEB_PARAM_PASS) 
            {

            }
        }
      }
      request->send(200, "text/plain", "Done. Gateway will restart");
      delay(3000);
      System.restart();
    });
}
void WebClass::serverGatewayConfig(void){
    Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(FILE_SYSTEM, "/index.html", "text/html", false, gatewayConfig);
    });
}



/*********************** Global Function Implementations ************************/


String wifiConfig(const String & var){
    return String();
}

String gatewayConfig(const String & var){
    return String();
}