

#ifndef WifiHelper_h
#define WifiHelper_h

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

#ifdef MCPOC_TELNET
#include "RemoteDebug.h"
extern RemoteDebug Debug;
#endif

class WifiHelper : public BaseHelper
{
public:
    WifiHelper(uint8_t pinLed, ConfigHelper *m_configuration);

    wl_status_t begin();

    String toString();

    // unsigned char connectSSID();
    wl_status_t connectSSID(String &sid, String &pass, IPAddress ip, String &MDNS_LABEL);
    uint8_t connectAP(String &MDNS_LABEL);

    String getClassName() { return __PRETTY_FUNCTION__ ;}


private:
    wl_status_t connecting();
    wl_status_t connecting(String &ssid, String &pass);

    ConfigHelper *m_configuration;
};

#endif
