#include <FunctionalInterrupt.h>

#include "main.h"
#include "buttonControl.h"

ButtonControl::ButtonControl(uint8_t pinBtDown, uint8_t pinBtUp)
{
    /* m_pinButtonUp   = pinBtUp;
    m_pinButtonDown = pinBtDown;*/
    m_button[BUTTON_OPEN] = new MyButton(BUTTON_OPEN, pinBtUp,"Open");
    m_button[BUTTON_CLOSE] = new MyButton(BUTTON_CLOSE, pinBtDown, "Close");

    /*m_button[BUTTON_OPEN].function = BUTTON_OPEN;
    m_button[BUTTON_OPEN].pin = pinBtUp;
    m_button[BUTTON_OPEN].currentStatus = BUTTON_NOT_PRESSED;
    m_button[BUTTON_OPEN].previousStatus = BUTTON_NOT_PRESSED;


    m_button[BUTTON_CLOSE]->function = BUTTON_CLOSE;
    m_button[BUTTON_CLOSE]->pin = pinBtDown;
    m_button[BUTTON_CLOSE]->currentStatus = BUTTON_NOT_PRESSED;
    m_button[BUTTON_CLOSE]->previousStatus = BUTTON_NOT_PRESSED;
*/
}

void ButtonControl::beging()
{
    DEBUGLOG("Begin");
    // attach interrupt
    /*attachInterrupt(m_pinButtonUp,  std::bind(&ButtonControl::buttonInterrupt, this), CHANGE);
    attachInterrupt(m_pinButtonDown,  std::bind(&ButtonControl::buttonInterrupt, this), CHANGE);*/
}

void IRAM_ATTR ButtonControl::buttonInterrupt()
{
    bool bStatusChanged = false;

    for (uint8_t i = 0; i < 2; i++)
    {
        if (m_button[i]->m_delayHelper.isDone())
        {
            // DEBUGLOGF("BUTTON [%d][%d][%d]\n",m_button[i]->function,m_button[i]->pin, digitalRead(m_button[i]->pin) );
            if (digitalRead(m_button[i]->pin))
            {
                if (m_button[i]->previousStatus == MyButton::BUTTON_PRESSED)
                    m_button[i]->currentStatus = MyButton::BUTTON_PRESSED_LONG;
                else if (m_button[i]->previousStatus == MyButton::BUTTON_NOT_PRESSED)
                    m_button[i]->currentStatus = MyButton::BUTTON_PRESSED;
            }
            else
            {
                if (m_button[i]->previousStatus == MyButton::BUTTON_PRESSED)
                    m_button[i]->currentStatus = MyButton::BUTTON_PRESSED_SHORT;
                else
                    m_button[i]->currentStatus = MyButton::BUTTON_NOT_PRESSED;
            }
            if (m_button[i]->previousStatus != m_button[i]->currentStatus)
            {
                DEBUGLOGF("BUTTON [%s][%d] current[%s] previous[%s]\n", m_button[i]->name, m_button[i]->pin, MyButton::getStatusString(m_button[i]->currentStatus), MyButton::getStatusString(m_button[i]->previousStatus));
                // m_button[i]->previousStatus = m_button[i]->currentStatus;
                bStatusChanged = true;
                m_button[i]->m_delayHelper.startDelay(500);
            };
        }
    }
    if (bStatusChanged)
    {
        if (m_buttonsStatus == BUTTON_OPEN_AND_CLOSE_PRESSED) {
            m_buttonsStatus = BUTTON_RELEASED;
        }
        else if ((m_button[BUTTON_OPEN]->currentStatus == MyButton::BUTTON_PRESSED_SHORT || m_button[BUTTON_OPEN]->currentStatus == MyButton::BUTTON_PRESSED_LONG) 
        && (m_button[BUTTON_CLOSE]->currentStatus == MyButton::BUTTON_PRESSED_SHORT || m_button[BUTTON_CLOSE]->currentStatus == MyButton::BUTTON_PRESSED_LONG))
        {
            DEBUGLOG("BUTTON [BOTH]");
            m_buttonsStatus = BUTTON_OPEN_AND_CLOSE_PRESSED;
        }
        else if (m_button[BUTTON_OPEN]->currentStatus == MyButton::BUTTON_PRESSED_SHORT)
        {
            m_buttonsStatus = BUTTON_OPEN_PRESSED;
        }
        else if (m_button[BUTTON_CLOSE]->currentStatus == MyButton::BUTTON_PRESSED_SHORT)
        {
            m_buttonsStatus = BUTTON_CLOSE_PRESSED;
        }
        else if (m_button[BUTTON_OPEN]->currentStatus == MyButton::BUTTON_PRESSED_LONG)
        {
            m_buttonsStatus = BUTTON_OPEN_PRESSED_LONG;
        }
        else if (m_button[BUTTON_CLOSE]->currentStatus == MyButton::BUTTON_PRESSED_LONG)
        {
            m_buttonsStatus = BUTTON_CLOSE_PRESSED_LONG;
        }
        else if (m_button[BUTTON_OPEN]->currentStatus == MyButton::BUTTON_NOT_PRESSED && m_button[BUTTON_OPEN]->previousStatus == MyButton::BUTTON_PRESSED_LONG)
        {
            m_buttonsStatus = BUTTON_OPEN_RELEASED_FROM_PRESSED_LONG;
        }
        else if (m_button[BUTTON_CLOSE]->currentStatus == MyButton::BUTTON_NOT_PRESSED && m_button[BUTTON_CLOSE]->previousStatus == MyButton::BUTTON_PRESSED_LONG)
        {
            m_buttonsStatus = BUTTON_CLOSE_RELEASED_FROM_PRESSED_LONG;
        }
        else
            m_buttonsStatus = BUTTON_RELEASED;

        m_button[BUTTON_OPEN]->previousStatus = m_button[BUTTON_OPEN]->currentStatus;
        m_button[BUTTON_CLOSE]->previousStatus = m_button[BUTTON_CLOSE]->currentStatus;

        if (xQueueSendToBack(xQueueCommand,
                             &m_buttonsStatus,
                             (TickType_t)10) != pdPASS)
        {
            DEBUGLOG("Failed sending button");
        }
    }
}

void ButtonControl::handle()
{
    buttonInterrupt();
}

/*
bool  ButtonControl::isActionUnderProgress()
{
        return m_buttonStatus != BUTTON_NOT_PRESSED;
}

ButtonControl::BUTTON_PRESSED ButtonControl::getButtonPressed() {

    return m_buttonPressed;

}

ButtonControl::BUTTON_STATUS ButtonControl::getButtonStatus() {
   return m_buttonStatus;
}*/
