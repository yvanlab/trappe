

#ifndef ButtonControl_h
#define ButtonControl_h

#include "main.h"

class ButtonControl
{
public:
    enum BUTTON_PRESSED
    {
        BUTTON_UP   = 0,
        BUTTON_DOWN = 1,
        BUTTON_BOTH = 2      
    };

    enum BUTTON_STATUS
    {
        BUTTON_NOT_PRESSED      = 0,
        BUTTON_PRESSED_SHORT    = 1,
        BUTTON_PRESSED_LONG     = 2,    
        BUTTON_DOUBLE_PRESSED   = 3  
    };
    

    ButtonControl(uint8_t pinBtDown, uint8_t pinBtUp);

    void handle();
    bool isActionUnderProgress();
    void beging();
    
    BUTTON_PRESSED getButtonPressed();
    BUTTON_STATUS  getButtonStatus();
    void buttonInterrupt();
    
    
    String toJson() {
        String res;
        res =  "\"button\":\"" + String(m_buttonPressed) + "\",";
        res += "\"status\":\"" + String(m_buttonStatus) + "\",";
        return res;
    };

    String getClassName() { return __PRETTY_FUNCTION__ ;}
    
    
    
    BUTTON_PRESSED m_buttonPressed = BUTTON_UP;
    BUTTON_STATUS  m_buttonStatus  = BUTTON_NOT_PRESSED;
    uint8_t  m_pinButtonUp;
    uint8_t  m_pinButtonDown;
    DelayHelper m_delayHelper;
private:
};

#endif
