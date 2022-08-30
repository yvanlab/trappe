

#ifndef ActionneurDriver_h
#define ActionneurDriver_h

#include "main.h"

class ActionneurDriver
{
public:
    enum ACTIONNEUR_COMMAND
    {
        COMMAND_STOP = 0,
        COMMAND_UP = 1,
        COMMAND_DOWN = 2
    };

    enum ACTIONNEUR_STATUS
    {
        STATUS_OPENED = 0,
        STATUS_CLOSED = 1,
        STATUS_OPENING = 2,
        STATUS_CLOSING = 3,
        STATUS_OPENING_STOPPED = 4,
        STATUS_CLOSING_STOPPED = 5,
        STATUS_UNDETERMINED =6
    };
    

    ActionneurDriver(uint8_t pinDown, uint8_t pinUp);
    void begin();
    void handle(boolean isOverIntensity);
    
    void stopTechnicalActionneur();
    void stopActionneur();

    void startActioneurUP();
    void startActioneurDOWN();

    void changeDirection(ACTIONNEUR_COMMAND direction);
    
    bool isActionUnderProgress();
    
    String toJson() {
        String res;
        res =  "\"command\":\"" + String(m_command) + "\",";
        res +=  "\"status\":\"" + String(m_status) + "\"";
        return res;
    };

    String getClassName() { return __PRETTY_FUNCTION__ ;}
    uint32_t calculateDelay(uint32_t delay);
    
    
    ACTIONNEUR_COMMAND m_command = COMMAND_STOP;
    ACTIONNEUR_STATUS m_status   = STATUS_CLOSED;
    uint8_t  m_pinCommandeUp;
    uint8_t  m_pinCommandeDown;
    DelayHelper m_delayHelper;
    uint32_t m_remainingDelay = 0;
private:
};

#endif
