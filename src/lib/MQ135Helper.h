#ifndef MQ135Helper_h
#define MQ135Helper_h

#include "Arduino.h"

#include "baseHelper.h"

#include "MQ135.h"
#include "mesureHelper.h"

class MQ135Helper : public MQ135, MesureHelper
{
public:
    MQ135Helper(uint8_t pin) : MQ135(pin)
    {
        clear();
    };

    void addPPMSample()
    {
        addSample(getPPM());
    };

    uint16_t readPPM()
    {
        if (m_nbreMeasure == 0)
        {
            addSample((double)getPPM());
        }
        m_currentPPM  = (uint16_t)readMesure();
        clear();
        return m_currentPPM;
    };

    uint16_t getCurrentPPM() {
        return (uint16_t) m_currentPPM;
    }
    
protected:
    uint16_t m_currentPPM;

};
#endif
