
#ifndef main_h
#define main_h

#include <Arduino.h>
#include <baseHelper.h>
#include <configHelper.h>
#include <uiHelper.h>


#include <SPIFFS.h>
#include <SD.h>
#include <FS.h>
#include "TimeLib.h"
#include "lib/baseHelper.h"
#include "myTimer.h"
#include "configHelper.h"
#include "wifiHelper.h"
#include "uiHelper.h"
#include "delayHelper.h"

#include "parameters.h"


extern Parameters            param;
extern QueueHandle_t         xQueueCommand;

#include "intensityControl.h"
extern IntensityControl      intensityCtl;

#include "actionneurDriver.h"
extern ActionneurDriver      commandDriver;
#include "buttonControl.h"
extern ButtonControl	       buttonCtl;

#include "networkUI.h"

#define PROC_ONE 0
#define PROC_TWO 1


#define PIN_LED 2


#ifdef MCPOC_TELNET
//extern RemoteDebug          Debug;
#endif

#ifdef ESP32
extern portMUX_TYPE 		wtimerMux;// = portMUX_INITIALIZER_UNLOCKED;
#endif

#ifdef ESP32
extern "C"
{
  uint8_t temprature_sens_read();
}
#endif
#ifdef ESP8266
uint8_t temprature_sens_read()
{
  return 0;
}
#endif



#endif
