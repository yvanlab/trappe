

#include "myTimer.h"
//#include "context.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


MyTimer mtTimer(0);


#ifdef ESP32
extern portMUX_TYPE 		wtimerMux;// = portMUX_INITIALIZER_UNLOCKED;
#endif


MyTimer::MyTimer(unsigned char pinLed) /*: BaseManager(pinLed)*/{

}

/*String MyTimer::toString(boolean bJson = false){
  return "Period[" + String(period) + "]";
}*/

#ifdef ESP32
void IRAM_ATTR timerCallback() {
    //uint32 start = millis();
	portENTER_CRITICAL_ISR(&wtimerMux);
#endif
#ifdef ESP8266
	void timerCallback(void *pArg) {
#endif
    uint32_t LocalPeriod = 0;
     mtTimer.periodCPT++;

     //DEBUGLOG(mtTimer.periodCPT);
     LocalPeriod |= PERIOD_QUICK;
     if ((mtTimer.periodCPT % MOD_25MS) == 0)    {LocalPeriod |= PERIOD_25MS ;mtTimer.frequence ^= PERIOD_25MS;}
     if ((mtTimer.periodCPT % MOD_250MS) == 0)   {LocalPeriod |= PERIOD_250MS;mtTimer.frequence ^= PERIOD_250MS;}
     if ((mtTimer.periodCPT % MOD_1S) == 0)      {LocalPeriod |= PERIOD_1S   ;mtTimer.frequence ^= PERIOD_1S;}
     if ((mtTimer.periodCPT % MOD_1MN) == 0)     {LocalPeriod |= PERIOD_1MN  ;mtTimer.frequence ^= PERIOD_1MN;}
     if ((mtTimer.periodCPT % MOD_5MN) == 0)     {LocalPeriod |= PERIOD_5MN  ;mtTimer.frequence ^= PERIOD_5MN;}
     if ((mtTimer.periodCPT % MOD_30MN) == 0)    {LocalPeriod |= PERIOD_30MN ;mtTimer.frequence ^= PERIOD_30MN;}
     if ((mtTimer.periodCPT % MOD_1H) == 0)      {LocalPeriod |= PERIOD_1H   ;mtTimer.frequence ^= PERIOD_1H;}
     if ((mtTimer.periodCPT % mtTimer.MOD_custom) == 0)  {LocalPeriod |= PERIOD_CUSTOM;mtTimer.frequence ^= PERIOD_CUSTOM;}

     mtTimer.period [0] |= LocalPeriod;
     mtTimer.period [1] |= LocalPeriod;

#ifdef ESP32
    portEXIT_CRITICAL_ISR(&wtimerMux);
#endif


} // End of timerCallback

void MyTimer::begin(uint32_t freqMS){
DEBUGLOG("Begin");  
#ifdef ESP8266
  os_timer_setfn(&myTimer,timerCallback, NULL);
  os_timer_arm(&myTimer, freqMS, true);
#endif

#ifdef ESP32
  myTimer = timerBegin(MY_TIMER_NUM, 80, true);
  timerAttachInterrupt(myTimer, &timerCallback, true);
  timerAlarmWrite(myTimer, (uint64_t)timerFrequence*1000, true);
  timerAlarmEnable(myTimer);
#endif

}