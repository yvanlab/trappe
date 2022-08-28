

#ifndef Parameters_h
#define Parameters_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <ArduinoJson.h>



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


};

#endif
