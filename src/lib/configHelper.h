

#ifndef ConfigHelper_h
#define ConfigHelper_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif


#include "baseHelper.h"
//#include <EEPROM.h>

class ConfigHelper : public BaseHelper
{
  public:
    uint8_t m_version = 0;
    String m_ssid ;
    String m_password;
    IPAddress m_IP;
    String m_moduleName;
    String m_mdns;
    String m_mdnsAP;

    boolean m_dynamicIP = false;
    IPAddress m_routerIP;
    String m_privateKey;
    String m_publicKey;

    ConfigHelper(uint8_t pinLed);
    String toJson();
    String toString();
    virtual uint8_t begin();
    virtual uint8_t readData();
    virtual uint8_t writeData();
   

    //String getCla
    String getClassName() { return __PRETTY_FUNCTION__ ;}
 



};

#endif
