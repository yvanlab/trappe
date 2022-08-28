

#ifndef UiHelper_h
#define UiHelper_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include <SPIFFS.h>
#include <Update.h>
//#include <ESP32HTTPUpdate.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>

extern "C"
{
#include "user_interface.h"
}
#endif

#ifdef OTA_FOR_ATOM
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

//#include <HourManager.h>
#include <baseHelper.h>
#include <configHelper.h>

#define FUNCTION_IN 1
#define FUNCTION_OUT 2

class UiHelper : public BaseHelper
{
public:
    UiHelper(uint8_t pinLed, ConfigHelper *m_configuration);
    wl_status_t begin();
     void handleServer();
    String getClassName() { return __PRETTY_FUNCTION__; }

    void restartESP();
    void displaySetup();
    void cssStyle();
    void setSetup();
    virtual String  getJSon();
    virtual void jsonSetup();
    void fileSetup();
    void dir();
    void open();
    void help();

    void clearMemory();
    void wifiReset();

#ifdef ESP32
    void initHttpUpdater();
    void httpUpdaterPage();
#endif
    bool loadFromSpiffs(String path);

#ifdef OTA_FOR_ATOM
    void OTAOnStart();
    void setOTAForAtom();
#endif

    void setCallback(std::function<void(uint8_t)> callBack) {
        m_callBack = callBack;
    }

    void callback(uint8_t param) {
        if (m_callBack) {
            m_callBack(param);
        }
    }

    
protected:
 
#ifdef ESP8266
    ESP8266WebServer *_server;
#endif
#ifdef ESP32
    WebServer *_server;
#endif

    // HourManager             *_hrManager;
#ifdef ESP8266
    ESP8266HTTPUpdateServer _httpUpdater;
#endif
#ifdef ESP32
    // ESP32HTTPUpdate _httpUpdater;
#endif

    ConfigHelper *m_configuration;
    std::function<void(uint8_t)> m_callBack = 0;
};

#endif
