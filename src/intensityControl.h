

#ifndef IntensityControl_h
#define IntensityControl_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "main.h"

#else

#include "WProgram.h"

#endif

#include "ACS712.h"

class IntensityControl : public ACS712
{
public:
    IntensityControl(uint8_t pinInput /*, uint32_t delay_ms*/) : ACS712(ACS712_20A, pinInput)
    {
        // m_delay_ms = delay_ms;
        ;
    };

    uint8_t begin()
    {
        DEBUGLOG("Begin");

        int tmp = calibrate();
        DEBUGLOGF("Calibrate [%d]\n", tmp);
        clean();
        // m_delayHelper.startDelay(m_delay_ms);
        return 0;
    };
    void handle()
    {
        // if (m_delayHelper.isDone()) {
        m_fCumulIntensityMeasure += getCurrentDC();
        m_nbMeasure++;
        // DEBUGLOGF("intensity [%f] [%f]\n",m_fCumulIntensityMeasure,m_fCumulIntensityMeasure/m_nbMeasure);

        if (m_nbMeasure >= 10)
        {
            m_fIntensityMeasure = m_fCumulIntensityMeasure / 10;
            m_nbMeasure = 0;
            m_fCumulIntensityMeasure = 0;

            /*if (m_fIntensityMeasure > param.m_maxPowerAmp)
            {
                m_bIsOverTreshold = true;
            }
            else
            {
                m_bIsOverTreshold = false;
            }*/
            if (m_delayMaxIntensity.isDone())
            {
                if (m_fIntensityMeasure > m_fMaxIntensityMeasure)
                    {
                        m_fMaxIntensityMeasure = m_fIntensityMeasure;
                    }
                // capture min intensity
                if (m_fIntensityMeasure < m_fMinIntensityMeasure)
                {
                    m_fMinIntensityMeasure = m_fIntensityMeasure;
                }
            }
        }
        // m_bIsOverTreshold = (m_fIntensityMeasure>param.m_maxPowerAmp) ? true : false;
        // m_delayHelper.startDelay(m_delay_ms);
        //}
    };
    // boolean isOverTreshold() { return m_bIsOverTreshold; };

    void clean()
    {
        // DEBUGLOGF("clean before-courant[%f]-calibration[%d]\n",m_fIntensityMeasure,zero);

        m_fIntensityMeasure = 0;
        m_fCumulIntensityMeasure = 0;
        m_nbMeasure = 0;
        m_fIntensityMeasure = 0;
        m_fMaxIntensityMeasure = 0;
        m_fMinIntensityMeasure = 1000;
        m_delayMaxIntensity.startDelay(param.m_delayIntensity);
        // int res = calibrate();
        DEBUGLOGF("clean AFTER -courant[%f]-calibration[%d]\n", m_fIntensityMeasure);
    }

    String getClassName() { return __PRETTY_FUNCTION__; }

    //
    boolean m_bIsOverTreshold = false;
    // boolean m_bPreviousIsOverTreshold = false;
    // uint32_t m_delay_ms;
    //  DelayHelper m_delayHelper;
    float m_fIntensityMeasure = 0;
    float m_fMaxIntensityMeasure = 0;
    float m_fMinIntensityMeasure = 0;

    float m_fCumulIntensityMeasure = 0;
    uint8_t m_nbMeasure = 0;

    DelayHelper m_delayMaxIntensity;
};

#endif
