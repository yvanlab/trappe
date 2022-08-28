#ifndef MesureHelper_h
#define MesureHelper_h

#include "Arduino.h"

#include "baseHelper.h"

class MesureHelper
{
public:
    MesureHelper() {}

    void addSample(float value)
    {
        m_cumulValue += value;
        m_nbreMeasure++;
        if (value < m_minValue)
            m_minValue = value;
        if (value > m_maxValue)
            m_maxValue = value;
    }

    void clear()
    {
        m_cumulValue = 0;
        m_nbreMeasure = 0;
        m_minValue = 100000.0;
        m_maxValue = -100000.0;
    }

    double readMesure()
    {
        return m_cumulValue / m_nbreMeasure;
    }
    double readMaxMesure()
    {
        return m_maxValue;
    }

    double readMinMesure()
    {
        return m_minValue;
    }

protected:
    double m_minValue = +100000.0;
    double m_maxValue = -1 * 100000.0;

    double m_cumulValue = 0;
    uint16_t m_nbreMeasure = 0;
};
#endif
