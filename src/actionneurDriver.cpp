

#include "actionneurDriver.h"

ActionneurDriver::ActionneurDriver(uint8_t pinDown, uint8_t pinUp)
{
    m_pinCommandeUp   = pinUp;
    m_pinCommandeDown = pinDown;
    
    pinMode(m_pinCommandeUp, OUTPUT);
    pinMode(m_pinCommandeDown, OUTPUT);

    stopActionneur();

}

bool ActionneurDriver::isActionUnderProgress() {
    return ( m_status == STATUS_CLOSING || m_status == STATUS_OPENING) ;
}

void ActionneurDriver::handle(boolean isOverIntensity) {
    if (m_status == STATUS_CLOSED || m_status == STATUS_OPENED) return;
    if (isOverIntensity)  {
        stopActionneur();
        if (m_status == STATUS_CLOSING) m_status = STATUS_CLOSING_STOPPED; 
        else if (m_status == STATUS_OPENING) m_status = STATUS_OPENING_STOPPED;
        return;
    } 
    if (m_delayHelper.isDone()) {
        stopActionneur();
        if (m_status == STATUS_CLOSING) m_status = STATUS_CLOSED; 
        else if (m_status == STATUS_OPENING) m_status = STATUS_OPENED;
    }
}

void ActionneurDriver::stopActionneur()
{
    DEBUGLOG("CMD Actionneur[Stop]");
    m_command = COMMAND_STOP;
    digitalWrite(m_pinCommandeDown, false);
    digitalWrite(m_pinCommandeUp, false);
    m_remainingDelay =  m_delayHelper.getRemaining();
    delay(250);
}


uint8_t ActionneurDriver::calculateDelay(uint8_t delay) {
    m_delayHelper.getRemaining();
    if (m_remainingDelay == 0) return delay;
    if (m_status == STATUS_CLOSING_STOPPED) { //t has been stopped when it was closing
      if ( m_command == COMMAND_DOWN) { // we want to finish closing
        return m_remainingDelay;
      } else { // we want to open
        return param.m_timeUpSec -  m_remainingDelay;
      }
    }else if (m_status == STATUS_OPENING_STOPPED) {
        if ( m_command == COMMAND_UP) { // we want to finish opening
        return m_remainingDelay;
      } else { // we want to close
        return param.m_timeDownSec - m_remainingDelay;
      }
    }
    return delay;

}

void ActionneurDriver::startActioneurUP() {
    DEBUGLOG("CMD Actionneur[UP]");
    if (m_status == STATUS_OPENING) return;
    if (m_status == STATUS_CLOSING) {
        stopActionneur();
        m_status = STATUS_CLOSING_STOPPED;
    }
    m_command = COMMAND_UP;
    //calculate new delay
    m_delayHelper.startDelay(calculateDelay(param.m_timeUpSec));
    digitalWrite(m_pinCommandeUp, true);
    digitalWrite(m_pinCommandeDown, false);
}
    
void ActionneurDriver::startActioneurDOWN() {
    DEBUGLOG("CMD Actionneur[DOWN]");
    if (m_status == STATUS_CLOSING) return;
    if (m_status == STATUS_OPENING) {
        stopActionneur();
        m_status = STATUS_OPENING_STOPPED;
    }
    m_command = COMMAND_DOWN;
    m_delayHelper.startDelay(calculateDelay(param.m_timeUpSec));
    digitalWrite(m_pinCommandeUp, false);
    digitalWrite(m_pinCommandeDown, true);
}

