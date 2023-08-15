




#include <ArduinoJson.h>

#include "main.h"


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Parameters::Parameters(unsigned char pinLed)
{
 
}

uint8_t Parameters::begin()
{
  DEBUGLOG("Begin");
  return readData();
};



String Parameters::toJson()
	{
		String ss;
		ss = "\"timeUpSec\":\"" + String(m_timeUpSec) + "\",";
		ss += "\"timeDownSec\":\"" + String(m_timeDownSec) + "\",";
		ss += "\"maxPowerAmp\":\"" + String(m_maxPowerAmp) + "\",";
    ss += "\"minPowerAmp\":\"" + String(m_minPowerAmp) + "\",";
    ss += "\"delayIntensity\":\"" + String(m_delayIntensity) + "\",";
    ss += "\"managementMode\":\"" + String(m_managementMode) + "\"";
		return ss;
	}

String Parameters::toString()
{
  return "VER[";
}



uint8_t Parameters::readData()
{
  
  char cfgFileName[] =  "/param.json";
  if (SPIFFS.exists(cfgFileName)){
    DynamicJsonDocument doc(30000);
    fs::File file = SPIFFS.open(cfgFileName, FILE_READ);
    DeserializationError error = deserializeJson(doc, file);
    if (!error) {
      m_timeUpSec = doc[F("timeUpSec")];
      m_timeDownSec = doc[F("timeDownSec")];
      m_maxPowerAmp = doc[F("maxPowerAmp")];
      m_minPowerAmp = doc[F("minPowerAmp")];
      m_delayIntensity = doc[F("delayIntensity")];
      m_managementMode = doc[F("managementMode")];
    } else {
      DEBUGLOGF("Config file SART READING [%s]",error.c_str());
    }
    file.close();
  } else {
      DEBUGLOG("Config file does not exist");
  }
  return 0;
}

uint8_t Parameters::writeData()
{
  time_t tt = time(nullptr);
 
  String ss = "{\"datetime\":\"" + String(ctime(&tt)) + "\",";
  ss += toJson()+"}";
  File cfgFile = SPIFFS.open("/param.json", FILE_WRITE);
  if (cfgFile.print(ss) == ss.length() ) {
    DEBUGLOG("Parameters file saved");
  } else {
    DEBUGLOG("Parameters PROBLEM");
  };
  cfgFile.close();
  
  return 0;
}
