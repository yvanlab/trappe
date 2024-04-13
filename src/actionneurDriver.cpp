
#include "main.h"
#include "actionneurDriver.h"

ActionneurDriver::ActionneurDriver(uint8_t pinDirectionDown, uint8_t pinDirectionUp,uint8_t pinCommandTrappe)
{
    m_pinCommandeDirectionUp = pinDirectionUp;
    m_pinCommandeDirectionDown = pinDirectionDown;
    m_pinCommandTrappe = pinCommandTrappe;
}

void ActionneurDriver::begin(uint8_t upSensor)
{
    DEBUGLOG("Begin");
    pinMode(m_pinCommandeDirectionUp, OUTPUT);
    pinMode(m_pinCommandeDirectionDown, OUTPUT);
    pinMode(m_pinCommandTrappe, OUTPUT);
    // retrive previous status from file ??
/*    if (upSensor)
        m_status == STATUS_OPENED;
    else
        m_status == STATUS_CLOSED;*/
    m_status == STATUS_UNDETERMINED;    
    m_actioneur = ACTIONEUR_HORIZONTAL;
    stopActionneur();
}

bool ActionneurDriver::isActionUnderProgress()
{
    return (m_status == STATUS_CLOSING || m_status == STATUS_OPENING);
}

void ActionneurDriver::handle(float fIntensity, bool isUpSensorActivated)
{
    if (m_status == STATUS_CLOSED || m_status == STATUS_OPENED || m_status == STATUS_CLOSING_STOPPED || m_status == STATUS_OPENING_STOPPED)
    {
        return;
    }
    if (fIntensity > param.m_maxPowerAmp && m_delayIntensity.isDone())
    {
        DEBUGLOGF("Over intensity [%f] ==> STOP\n", fIntensity);
        stopActionneur();
        return;
    }

    if ((fIntensity < param.m_minPowerAmp) &&
    (m_status == STATUS_CLOSING || m_status == STATUS_OPENING) && 
    m_delayIntensity.isDone())
    {
        if (m_actioneur == ACTIONEUR_HORIZONTAL) {
            m_delayIntensity.startDelay(param.m_delayIntensity);
            // start trappe
            digitalWrite(m_pinCommandTrappe, true);
            m_actioneur = ACTIONEUR_TRAPPE;
            DEBUGLOGF("under intensity while opening [%f] ==> CYCLE HORIZONTAL FINISHED \n", fIntensity);
            intensityCtl.clean();
        } else {
            DEBUGLOGF("under intensity while closing or opening [%f] ==> CYCLE CLOSING/OPENING FINISHED \n", fIntensity);
            stopTechnicalActionneur();
            if (m_status == STATUS_CLOSING) {
                m_status = STATUS_CLOSED;    
            } else {
                m_status = STATUS_OPENED;
            }
        }
       
        
        return;
    }

}

void ActionneurDriver::stopTechnicalActionneur()
{
    digitalWrite(m_pinCommandeDirectionDown, false);
    digitalWrite(m_pinCommandeDirectionUp, false);
    digitalWrite(m_pinCommandTrappe, false);

    /*if (//m_delayHelper.isDone())
    {
        m_remainingDelay = 0;
        m_elapsedDelay = 0;
    }
    else
    {
        m_remainingDelay -= //m_delayHelper.getRemaining();
        m_elapsedDelay += //m_delayHelper.getElpased();
    }*/
    DEBUGLOGF("CMD [Stop] working time [%d]\n", m_workigTime / 1000);
    //m_delayHelper.startDelay(0);
}

void ActionneurDriver::stopActionneur()
{
    m_command = COMMAND_STOP;
    if (m_status == STATUS_CLOSING)
    {
        m_status = STATUS_CLOSING_STOPPED;
    }
    else if (m_status == STATUS_OPENING) 
    {
        m_status = STATUS_OPENING_STOPPED;
    }
    m_workigTime += millis() - m_workigStartTime;
    stopTechnicalActionneur();

    delay(500);
}

uint32_t ActionneurDriver::getElapse()
{
    return millis() - m_workigStartTime;
    // return m_elapsedDelay + //m_delayHelper.getElpased();
}

uint32_t ActionneurDriver::getRemaining()
{
    return 0;//m_remainingDelay - //m_delayHelper.getRemaining();
}


void ActionneurDriver::startActioneurOpen()
{
    
    if (m_status == STATUS_OPENING || m_status == STATUS_OPENED)
        return;
    if (m_status == STATUS_CLOSING)
    {
        stopActionneur();
        m_workigTime = 0;
    }
    else if (m_status == STATUS_CLOSED)
    {
        m_workigTime = 0;
    }
    m_command = COMMAND_OPEN;
    m_delayIntensity.startDelay(param.m_delayIntensity);
    digitalWrite(m_pinCommandeDirectionUp, true);
    digitalWrite(m_pinCommandeDirectionDown, false);
    digitalWrite(m_pinCommandTrappe, true); // active la trappe en meme temps
    m_status = STATUS_OPENING;
    m_actioneur = ACTIONEUR_TRAPPE_AND_HORIZONTAL;
    DEBUGLOGF("CMD Actionneur[Open]-ACTIONEUR_TRAPPE_AND_HORIZONTAL-delay courant [%d]\n",param.m_delayIntensity);
    m_workigStartTime = millis();
}

void ActionneurDriver::startActioneurClose()
{
    
    if (m_status == STATUS_CLOSING || m_status == STATUS_CLOSED)
        return;
    if (m_status == STATUS_OPENING)
    {
        stopActionneur();
        m_workigTime = 0;
    }
    else if (m_status == STATUS_OPENED)
    {
        m_workigTime = 0;
    }

    m_command = COMMAND_CLOSE;
    m_delayIntensity.startDelay(param.m_delayIntensity);
    digitalWrite(m_pinCommandeDirectionUp, false);
    digitalWrite(m_pinCommandeDirectionDown, true);
    digitalWrite(m_pinCommandTrappe, false); // trappe is deactivated : Only horizontal
    m_status = STATUS_CLOSING;
    m_actioneur = ACTIONEUR_HORIZONTAL;
    DEBUGLOGF("CMD Actionneur[Close]-ACTIONEUR_HORIZONTAL-delay courant [%d]\n",param.m_delayIntensity);
    m_workigStartTime = millis();
}
