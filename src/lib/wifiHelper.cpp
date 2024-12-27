
#include "wifiHelper.h"

//#include "httpUpdater.h"

//#include <WiFiClient.h>
//#include "context.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



WifiHelper::WifiHelper(uint8_t pinLed, ConfigHelper *smManager) : BaseHelper(pinLed)
{
  m_configuration = smManager;
}

wl_status_t WifiHelper::begin()
{
  wl_status_t connection = connectSSID(m_configuration->m_ssid, m_configuration->m_password, m_configuration->m_IP, m_configuration->m_mdns);
  if (connection == WL_CONNECTED)
  {
    Serial.println("connected");
    setStatus(WiFi.status(), "ssid");
    //_hrManager->begin("pool.ntp.org", 1, true);
    configTime(3600, 3600, "europe.pool.ntp.org", "pool.ntp.org", "north-america.pool.ntp.org");
#ifdef MCPOC_TEST
    WiFi.softAP(m_configuration->m_mdns.c_str(), MODULE_AP_WPA);
#else
    WiFi.softAP(MODULE_MDNS, MODULE_AP_WPA, 1, true);
#endif
  }
  else
  {
    Serial.println("Not connected");
    connectAP(m_configuration->m_mdns);
    setStatus(WiFi.status(), "ap");
    WiFi.softAP(m_configuration->m_mdns.c_str(), MODULE_AP_WPA);
  }

  if (!MDNS.begin(m_configuration->m_mdns.c_str()))
  {
    Serial.println("Problem with mDNS");
  }
  // MDNS.addService("http", "tcp", 80);


#ifdef ESP8266
  WiFi.hostname(MODULE_NAME);
#endif
#ifdef ESP32
  WiFi.setHostname(m_configuration->m_moduleName.c_str());
#endif



  return connection;
}

String WifiHelper::toString()
{
  return "Mode[" + String(WiFi.getMode()) +
         "] SSID[" + WiFi.SSID() +
         "] LocalIP[" + WiFi.localIP().toString() +
         "] softIP[" + WiFi.softAPIP().toString() +
         "] routerIP[" + WiFi.gatewayIP().toString() +
         "] status[" + String(WiFi.status(), DEC) + "]";
}

wl_status_t WifiHelper::connecting()
{
  String empty1, empty2;
  return connecting(empty1, empty2);
}

wl_status_t WifiHelper::connecting(String &ssid, String &pass)
{
  if (ssid.length() == 0)
  {
    if (!WiFi.SSID())
      return WL_NO_SSID_AVAIL;
    Serial.println("Quick");
#ifdef ESP8266
    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
#endif
#ifdef ESP32
    esp_wifi_disconnect();
#endif
    WiFi.begin();
  }
  else
  {
    Serial.println("Long");
    WiFi.begin(ssid.c_str(), pass.c_str());
  }
  uint8_t inbTest = 0;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && inbTest < 10)
  {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
    inbTest++;
  }
  return WiFi.status();
}

wl_status_t WifiHelper::connectSSID(String &ssid, String &pass, IPAddress ip, String &MDNS_LABEL)
{
  if (ssid.isEmpty() || pass.isEmpty())
    return WL_NO_SSID_AVAIL;
  if (!m_configuration->m_dynamicIP ){
    // IPAddress gateway(192, 168, 1, 254); // IP address of the router
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns(8, 8, 4, 4);

    WiFi.config(ip, m_configuration->m_routerIP, subnet, dns);
   } 
  // WiFi.config(ip, gateway, subnet);
  Serial.println("");
  switchOn();
  if (connecting() != WL_CONNECTED)
    connecting(ssid, pass);
  switchOff();
  return WiFi.status();
}

uint8_t WifiHelper::connectAP(String &MDNS_LABEL)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  return WiFi.status();
}
