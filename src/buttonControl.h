

#ifndef ButtonControl_h
#define ButtonControl_h



class ButtonControl
{
public:
    enum BUTTON_GLOBAL_STATUS
    {
        BUTTON_RELEASED                 = 0,  
        BUTTON_OPEN_PRESSED             = 1,
        BUTTON_OPEN_PRESSED_LONG        = 2,
        BUTTON_OPEN_RELEASED_FROM_PRESSED_LONG = 3,
        BUTTON_CLOSE_PRESSED            = 4,
        BUTTON_CLOSE_PRESSED_LONG       = 5,
        BUTTON_CLOSE_RELEASED_FROM_PRESSED_LONG = 6,
        BUTTON_OPEN_AND_CLOSE_PRESSED   = 7,
        BUTTON_OPEN_AND_CLOSE_PRESSED_LONG = 8       
    };


    enum BUTTON_FUNCTION
    {
        BUTTON_OPEN   = 0,
        BUTTON_CLOSE = 1,
        BUTTON_BOTH = 2      
    };
        

    enum BUTTON_STATUS
    {
        BUTTON_NOT_PRESSED      = 0,
        BUTTON_PRESSED          = 1,
        BUTTON_PRESSED_SHORT    = 2,
        BUTTON_PRESSED_LONG     = 3
    };
    
    struct MyButton {
        BUTTON_STATUS currentStatus;
        BUTTON_STATUS previousStatus;
        BUTTON_FUNCTION function;
        uint8_t pin;
    } ;

    
    
    ButtonControl(uint8_t pinBtDown, uint8_t pinBtUp);

    void handle();
    bool isActionUnderProgress();
    void beging();
    
   /* BUTTON_FUNCTION getButtonPressed();
    BUTTON_STATUS  getButtonStatus();*/
    void IRAM_ATTR buttonInterrupt();
    
    
    String toJson() {
        String res;
        res =  "\"global_status\":\"" + String(m_buttonsStatus) + "\",";
        res +=  "\"btpOpen\":{";
        res +=           "\"current_status\":\"" + String(m_button[BUTTON_OPEN].currentStatus)+ "\","; 
        res +=           "\"previous_status\":\"" + String(m_button[BUTTON_OPEN].previousStatus)+ "\"},"; 
        res +=  "\"btpClose\":{";
        res +=           "\"current_status\":\"" + String(m_button[BUTTON_CLOSE].currentStatus)+ "\","; 
        res +=           "\"previous_status\":\"" + String(m_button[BUTTON_CLOSE].previousStatus)+ "\"}"; 

        return res;
    };

    String getClassName() { return __PRETTY_FUNCTION__ ;} 
    
    BUTTON_GLOBAL_STATUS m_buttonsStatus  = BUTTON_RELEASED;
   // BUTTON_STATUS  m_buttonStatus  = BUTTON_NOT_PRESSED;

    
    MyButton m_button[2];
    DelayHelper m_delayHelper;
private:
};

#endif
