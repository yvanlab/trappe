

#ifndef Parameters_h
#define Parameters_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <ArduinoJson.h>

enum MANAGEMENT_MODE
    {
        MODE_BUTTON = 0,
        MODE_TIME_BASED = 1,
        MODE_INTENSITY = 2,
        MODE_SENSOR = 3
    };

class Parameters
{
  public:
    Parameters(uint8_t pinLed);
    String toJson();
    String toString();
    virtual uint8_t begin();
    virtual uint8_t readData();
    virtual uint8_t writeData();
    //String getCla
    String getClassName() { return __PRETTY_FUNCTION__ ;}
//void sortPages();

	String toString(boolean bJson);

	uint8_t m_timeUpSec   = 60;
	uint8_t m_timeDownSec = 60;
	uint8_t m_maxPowerAmp = 10;
  uint16_t m_delayIntensity = 500;
  float   m_minPowerAmp = 1.5;
  
  MANAGEMENT_MODE m_managementMode = MODE_BUTTON;



};

#endif
