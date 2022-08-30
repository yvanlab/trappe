#include <FunctionalInterrupt.h>

#include "buttonControl.h"

ButtonControl::ButtonControl(uint8_t pinBtDown, uint8_t pinBtUp)
{
    m_pinButtonUp   = pinBtUp;
    m_pinButtonDown = pinBtDown;
    

}

void ButtonControl::beging() {
    DEBUGLOG("Begin");
    pinMode(m_pinButtonUp, INPUT_PULLDOWN);
    pinMode(m_pinButtonDown, INPUT_PULLDOWN);

    //attach interrupt
    /*attachInterrupt(m_pinButtonUp,  std::bind(&ButtonControl::buttonInterrupt, this), CHANGE);
    attachInterrupt(m_pinButtonDown,  std::bind(&ButtonControl::buttonInterrupt, this), CHANGE);*/

}



void IRAM_ATTR ButtonControl::buttonInterrupt() {
    
    if(m_delayHelper.isDone()) {
        bool btUp   = digitalRead(m_pinButtonUp);
        bool btDown = digitalRead(m_pinButtonDown);
        if (btUp&btDown) {
            DEBUGLOG("BUTTON [BOTH]");
            m_buttonStatus = BUTTON_PRESSED_SHORT;
            m_buttonPressed = BUTTON_BOTH;
        } else if (btDown) {
            DEBUGLOG("BUTTON [DOWN]");
            m_buttonStatus = BUTTON_PRESSED_SHORT;
            m_buttonPressed = BUTTON_DOWN;
        } else if (btUp) {
            DEBUGLOG("BUTTON [UP]");
            m_buttonStatus = BUTTON_PRESSED_SHORT;
            m_buttonPressed = BUTTON_UP;
        } else {
            DEBUGLOG("BUTTON [NOT PRESSED]");
            m_buttonStatus = BUTTON_NOT_PRESSED;
        }
         m_delayHelper.startDelay(250); 
    }
   

}


void ButtonControl::handle() {
   ;
}

bool  ButtonControl::isActionUnderProgress()
{
        return m_buttonStatus != BUTTON_NOT_PRESSED;
}

ButtonControl::BUTTON_PRESSED ButtonControl::getButtonPressed() {

    return m_buttonPressed;
    
}
    
ButtonControl::BUTTON_STATUS ButtonControl::getButtonStatus() {
   return m_buttonStatus;
}

