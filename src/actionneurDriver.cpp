
#include "main.h"
#include "actionneurDriver.h"

ActionneurDriver::ActionneurDriver(uint8_t pinDown, uint8_t pinUp)
{
    m_pinCommandeUp   = pinUp;
    m_pinCommandeDown = pinDown;
}

void ActionneurDriver::begin() {
    DEBUGLOG("Begin");
    pinMode(m_pinCommandeUp, OUTPUT);
    pinMode(m_pinCommandeDown, OUTPUT);
    // retrive previous status from file ??
    m_status == STATUS_CLOSED;
    stopActionneur();
    
}

bool ActionneurDriver::isActionUnderProgress() {
    return ( m_status == STATUS_CLOSING || m_status == STATUS_OPENING) ;
}

void ActionneurDriver::handle(boolean isOverIntensity) {
    if (m_status == STATUS_CLOSED || m_status == STATUS_OPENED || m_status == STATUS_CLOSING_STOPPED || m_status == STATUS_OPENING_STOPPED ) return;
    if (isOverIntensity)  {
        DEBUGLOG("Over intensity");
        stopActionneur();
        return;
    } 
    if (m_delayHelper.isDone()) {
        DEBUGLOG("Over delay");
        stopTechnicalActionneur();
        if (m_status == STATUS_CLOSING) m_status = STATUS_CLOSED; 
        else if (m_status == STATUS_OPENING) m_status = STATUS_OPENED;
    }
}

void ActionneurDriver::stopTechnicalActionneur() {
    digitalWrite(m_pinCommandeDown, true);
    digitalWrite(m_pinCommandeUp, true);
    m_remainingDelay =  m_delayHelper.getRemaining();
    m_delayHelper.startDelay(0);

}

void ActionneurDriver::stopActionneur()
{
    m_command = COMMAND_STOP;
    if (m_status == STATUS_CLOSING) {
        m_status = STATUS_CLOSING_STOPPED;
    } 
    else if (m_status == STATUS_OPENING) {
        m_status = STATUS_OPENING_STOPPED;
    }
    stopTechnicalActionneur();
    DEBUGLOGF("CMD [Stop] [%d]\n",m_remainingDelay);
    delay(500);
}




uint32_t ActionneurDriver::calculateDelay(uint32_t delay) {
    m_delayHelper.getRemaining();
    if (m_remainingDelay == 0) return delay;
    if (m_status == STATUS_CLOSING_STOPPED) { //t has been stopped when it was closing
      if ( m_command == COMMAND_CLOSE) { // we want to finish closing
        return m_remainingDelay;
      } else { // we want to open
        return delay -  m_remainingDelay;
      }
    }else if (m_status == STATUS_OPENING_STOPPED) {
        if ( m_command == COMMAND_OPEN) { // we want to finish opening
        return m_remainingDelay;
      } else { // we want to close
        return delay - m_remainingDelay;
      }
    }
    return delay;

}

/*void ActionneurDriver::stopActioneur() {
    DEBUGLOG("CMD Actionneur[UP]");

}*/

void ActionneurDriver::startActioneurOpen() {
    DEBUGLOG("CMD Actionneur[Open]");
    if (m_status == STATUS_OPENING || m_status == STATUS_OPENED) return;
    if (m_status == STATUS_CLOSING)
        stopActionneur();

    m_command = COMMAND_OPEN;
    //calculate new delay
    m_delayHelper.startDelay(calculateDelay(param.m_timeUpSec*1000));
    digitalWrite(m_pinCommandeUp, true);
    digitalWrite(m_pinCommandeDown, false);
    m_status = STATUS_OPENING;
}
    
void ActionneurDriver::startActioneurClose() {
    DEBUGLOG("CMD Actionneur[Close]");
    if (m_status == STATUS_CLOSING || m_status == STATUS_CLOSED) return;
    if (m_status == STATUS_OPENING) {
        stopActionneur();
    }
    m_command = COMMAND_CLOSE;
    m_delayHelper.startDelay(calculateDelay(param.m_timeDownSec*1000));
    digitalWrite(m_pinCommandeUp, false);
    digitalWrite(m_pinCommandeDown, true);
    m_status = STATUS_CLOSING;
}

