

#ifndef ActionneurDriver_h
#define ActionneurDriver_h



class ActionneurDriver
{
public:
    enum ACTIONNEUR_COMMAND
    {
        COMMAND_STOP = 0,
        COMMAND_OPEN = 1,
        COMMAND_CLOSE = 2
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
    
    enum ACTIONNEUR_SELECTED
    {
        ACTIONEUR_TRAPPE = 0,
        ACTIONEUR_HORIZONTAL = 1,
        ACTIONEUR_TRAPPE_AND_HORIZONTAL = 2
    };

    ActionneurDriver(uint8_t pinDirectionDown, uint8_t pinDirectionUp,uint8_t pinCommandTrappe);
    void begin(uint8_t upSensor) ;
    void handle(float fIntensity, bool isUpSensorActivated);
    
    void stopTechnicalActionneur();
    void stopActionneur();

    void startActioneurOpen();
    void startActioneurClose();

    void changeDirection(ACTIONNEUR_COMMAND direction);
    
    bool isActionUnderProgress();
    bool isActionClosing() {return m_status == STATUS_CLOSING;};
    bool isActionOpening() {return m_status == STATUS_OPENING;};

    String toJson() {
        String res;
        res =  "\"command\":\"" + String(m_command) + "\",";
        res += "\"workigTime\":\"" + String(m_workigTime) + "\",";
        res += "\"status\":\"" + String(m_status) + "\"";
        return res;
    };

    String getClassName() { return __PRETTY_FUNCTION__ ;}
    //uint32_t calculateDelay(uint32_t delay);
    uint32_t getElapse();
    uint32_t getRemaining();
    
    
    ACTIONNEUR_COMMAND m_command = COMMAND_STOP;
    ACTIONNEUR_STATUS m_status   = STATUS_CLOSED;
    ACTIONNEUR_SELECTED m_actioneur = ACTIONEUR_HORIZONTAL;

    uint8_t  m_pinCommandeDirectionUp;
    uint8_t  m_pinCommandeDirectionDown;
    uint8_t  m_pinCommandTrappe;
    
    //DelayHelper m_delayHelper;
    //uint32_t m_remainingDelay = 0;
    //uint32_t m_elapsedDelay = 0;
    
    uint32_t m_workigStartTime = 0;
    uint32_t m_workigTime = 0;



    DelayHelper m_delayIntensity;
private:
};

#endif
