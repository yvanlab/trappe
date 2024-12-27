

#include "ConfigHelper.h"
#include "SPIFFS.h"
#include <SD.h>
#include <ArduinoJson.h>
#include <Time.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

ConfigHelper::ConfigHelper(uint8_t pinLed) : BaseHelper(pinLed)
{
  m_version = 1;
  m_ssid = DEFAULT_SSID;
  m_password = DEFAULT_PASS;
  m_password = DEFAULT_PASS;
  m_routerIP = IPAddress(DEFAULT_ROUTEUR_IP);

  m_IP = IPAddress(DEFAULT_MODULE_IP);
  m_moduleName = DEFAULT_MODULE_NAME;
  m_mdns = DEFAULT_MODULE_MDNS;
  m_mdnsAP = DEFAULT_MODULE_MDNS_AP;

  m_privateKey = "";
  m_publicKey = "";
}

uint8_t ConfigHelper::begin()
{
  if (!SPIFFS.begin(true))
  {
    DEBUGLOG("SPIFFS Mount Failed")
    return 1;
  }
  else
    DEBUGLOG("SPIFFS Mount OK");
  return readData();
};

String ConfigHelper::toJson()
{
  String ss;
	
  ss = "\"version\":\"" + String(m_version) + "\",";
  ss += "\"uptime\":\"" + String(millis()/1000) + "\",";
  ss += "\"build_date\":\"" + String(__DATE__ " " __TIME__) + "\",";
  ss += "\"ssid\":\"" + m_ssid + "\",";
  ss += "\"password\":\"" + m_password + "\",";
  ss += "\"dynamicIP\":\"" + String(m_dynamicIP) + "\",";
  ss += "\"routerIP\":\"" + m_routerIP.toString() + "\",";
  ss += "\"IP\":\"" + m_IP.toString() + "\",";
  ss += "\"moduleName\":\"" + m_moduleName + "\",";
  ss += "\"mdns\":\"" + m_mdns + "\",";
  ss += "\"mdnsAP\":\"" + m_mdnsAP + "\",";
  ss += "\"publicKey\":\"" + m_publicKey + "\",";
  ss += "\"privateKey\":\"" + m_privateKey + "\"";

  return ss;
}

String ConfigHelper::toString()
{
  return "VER[" + String(m_version) + "] SSID[" + String(m_ssid) + "] PASS[" + m_password + "] privateKey[" + m_privateKey + "] publicKey[" + m_publicKey + "]";
}

uint8_t ConfigHelper::readData()
{
  switchOn();
  // Read configuartion file
  File file;
  char cfgFileName[] = "/config.json";
  if (SPIFFS.exists(cfgFileName))
  {
    DynamicJsonDocument doc(30000);
    file = SPIFFS.open(cfgFileName, FILE_READ);
    DeserializationError error = deserializeJson(doc, file);
    if (!error)
    {
      m_version = doc[F("version")];
      m_ssid = doc[F("ssid")].as<String>();
      m_dynamicIP = doc[F("dynamicIP")].as<int8_t>();
      m_routerIP.fromString(doc[F("routerIP")].as<String>());
      m_IP.fromString(doc[F("IP")].as<String>());
      m_moduleName = doc[F("moduleName")].as<String>();
      m_mdns = doc[F("mdns")].as<String>();
      m_mdnsAP = doc[F("mdnsAP")].as<String>();
      m_password = doc[F("password")].as<String>();
      m_privateKey = doc[F("privateKey")].as<String>();
      m_publicKey = doc[F("publicKey")].as<String>();
    }
    else
    {
      DEBUGLOGF("Config file SART READING [%s]", error.c_str());
    }
    file.close();
  }
  else
  {
    DEBUGLOG("Config file does not exist");
  }
  switchOff();
  return 0;
}

uint8_t ConfigHelper::writeData()
{
  switchOn();
  time_t tt = time(nullptr);

  String ss = "{\"datetime\":\"" + String(ctime(&tt)) + "\",";
  ss += toJson() + "}";
  DEBUGLOGF("set up : %s \n",ss.c_str());
  File cfgFile = SPIFFS.open("/config.json", FILE_WRITE);
  if (cfgFile.print(ss) == ss.length())
  {
    DEBUGLOG("Configuration file saved");
  }
  else
  {
    DEBUGLOG("Configuration PROBLEM");
  };
  cfgFile.close();

  switchOff();
  return 0;
}
