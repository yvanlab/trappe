

#ifndef IntensityControl_h
#define IntensityControl_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "main.h"

#else

#include "WProgram.h"

#endif

#include <delayHelper.h>
#include <BaseHelper.h>
#include "ACS712.h"


class IntensityControl : public ACS712
{
  public:
    IntensityControl(uint8_t pinInput,uint32_t delay_ms) : ACS712(ACS712_20A, pinInput) {
       m_delay_ms =delay_ms;;
    };
    
    uint8_t begin() {
        DEBUGLOG("Begin");
        calibrate();
        m_delayHelper.startDelay(m_delay_ms);
        return 0;
    };
    void handle(){
        if (m_delayHelper.isDone()) {
            float fIntensityMeasure = getCurrentDC();
            DEBUGLOGF("Mesurecourant [%f]\n",fIntensityMeasure);
            bIsOverTreshold = (fIntensityMeasure>param.m_maxPowerAmp) ? true : false; 
            m_delayHelper.startDelay(m_delay_ms);
        }
    };
    boolean isOverTreshold() {return bIsOverTreshold; };
    
    String getClassName() { return __PRETTY_FUNCTION__ ;}
    
    boolean bIsOverTreshold = false;
    uint32_t m_delay_ms;
    DelayHelper m_delayHelper;

};

#endif
