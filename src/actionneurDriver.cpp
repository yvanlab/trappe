
#include "main.h"
#include "actionneurDriver.h"

ActionneurDriver::ActionneurDriver(uint8_t pinDown, uint8_t pinUp)
{
    m_pinCommandeUp = pinUp;
    m_pinCommandeDown = pinDown;
}

void ActionneurDriver::begin(uint8_t upSensor)
{
    DEBUGLOG("Begin");
    pinMode(m_pinCommandeUp, OUTPUT);
    pinMode(m_pinCommandeDown, OUTPUT);
    // retrive previous status from file ??
    if (upSensor)
        m_status == STATUS_OPENED;
    else
        m_status == STATUS_CLOSED;
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
    if (fIntensity > param.m_maxPowerAmp)
    {
        DEBUGLOGF("Over intensity [%f] ==> STOP\n", fIntensity);
        stopActionneur();
        return;
    }

    if (fIntensity < param.m_minPowerAmp /*&& m_status == STATUS_CLOSING*/ && m_delayIntensity.isDone())
    {
        DEBUGLOGF("under intensity while closing [%f] ==> STOP CLOSING\n", fIntensity);
        stopTechnicalActionneur();
        m_status = STATUS_CLOSED;
        return;
    }

    if (isUpSensorActivated && m_status == STATUS_OPENING)
    {
        DEBUGLOGF("up Sensor activated ==> STOP OPENING [%f]\n", fIntensity);
        stopTechnicalActionneur();
        m_status = STATUS_OPENED;
        return;
    }

    /*if (m_delayHelper.isDone()) {
        DEBUGLOG("Over delay");
        stopTechnicalActionneur();
        if (m_status == STATUS_CLOSING) m_status = STATUS_CLOSED;
        else if (m_status == STATUS_OPENING) m_status = STATUS_OPENED;
    }*/
}

void ActionneurDriver::stopTechnicalActionneur()
{
    digitalWrite(m_pinCommandeDown, false);
    digitalWrite(m_pinCommandeUp, false);
    if (m_delayHelper.isDone())
    {
        m_remainingDelay = 0;
        m_elapsedDelay = 0;
    }
    else
    {
        m_remainingDelay -= m_delayHelper.getRemaining();
        m_elapsedDelay += m_delayHelper.getElpased();
    }
    DEBUGLOGF("CMD [Stop] working time [%d] remaining[%d]\n", m_workigTime / 1000, m_remainingDelay / 1000);
    m_delayHelper.startDelay(0);
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
    // return m_elapsedDelay + m_delayHelper.getElpased();
}

uint32_t ActionneurDriver::getRemaining()
{
    return m_remainingDelay - m_delayHelper.getRemaining();
}

uint32_t ActionneurDriver::calculateDelay(uint32_t delay)
{
    // m_delayHelper.getRemaining();
    if (m_remainingDelay == 0)
    {
        m_remainingDelay = delay;
        m_elapsedDelay = 0;
        return m_remainingDelay;
    }
    if (m_status == STATUS_CLOSING_STOPPED)
    { // t has been stopped when it was closing
        if (m_command == COMMAND_CLOSE)
        { // we want to finish closing
            return m_remainingDelay;
        }
        else
        { // we want to open
            return delay - m_elapsedDelay;
        }
    }
    else if (m_status == STATUS_OPENING_STOPPED)
    {
        if (m_command == COMMAND_OPEN)
        { // we want to finish opening
            return m_remainingDelay;
        }
        else
        { // we want to close
            return delay - m_elapsedDelay;
        }
    }
    return delay;
}

/*void ActionneurDriver::stopActioneur() {
    DEBUGLOG("CMD Actionneur[UP]");

}*/

void ActionneurDriver::startActioneurOpen()
{
    DEBUGLOG("CMD Actionneur[Open]");
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
    // calculate new delay
    m_delayHelper.startDelay(calculateDelay(param.m_timeUpSec * 1000));
    digitalWrite(m_pinCommandeUp, true);
    digitalWrite(m_pinCommandeDown, false);
    m_status = STATUS_OPENING;
    m_workigStartTime = millis();
}

void ActionneurDriver::startActioneurClose()
{
    DEBUGLOG("CMD Actionneur[Close]");
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
    m_delayHelper.startDelay(calculateDelay(param.m_timeDownSec * 1000));
    m_delayIntensity.startDelay(500);
    digitalWrite(m_pinCommandeUp, false);
    digitalWrite(m_pinCommandeDown, true);
    m_status = STATUS_CLOSING;
    m_workigStartTime = millis();
}
