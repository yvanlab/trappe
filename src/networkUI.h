#ifndef networkUI_h
#define networkUI_h

#include "Arduino.h"


#include <main.h>

class NetworkUI : public UiHelper
{
public:
    NetworkUI(uint8_t pinLed, Parameters *parameters, ConfigHelper *m_configuration) : UiHelper(pinLed, m_configuration) {m_parameters = parameters;};
    
    wl_status_t begin();



    virtual String  jsonParam();
    void jsonOptions();
    void displayParameters();        
    void setParameters();
    void saveParameters();

    //void dataJson();


    //void firmware();
    void OTAOnStart();
    void clearEEPROM();
    void configPage();
    void progPage();

    protected:
    Parameters *m_parameters;
};

#endif
