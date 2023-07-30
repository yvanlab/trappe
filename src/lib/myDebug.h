

#ifndef Debug_h
#define Debug_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#ifdef MCPOC_TELNET
#include "Remotedebug.h"
#endif



#define DEBUG_SERIAL 0b00000001
#define DEBUG_TELNET 0b00000010
#define DEBUG_WEB 0b00001000


#ifdef MCPOC_TEST

#ifdef ESP32
#define DEBUGLOG(format) MyDebug::print( __FILE__, __LINE__, __FUNCTION__, format);
#define DEBUGLOGF(format, ...) MyDebug::printf( __FILE__, __LINE__, __FUNCTION__,format, __VA_ARGS__);
#else
#define DEBUGLOG(...)                                                                                           \
  DBG_PORT.printf("%s[%s][%d]\t", String(__FILE__).c_str(), String(__LINE__).c_str(), millis() - previousTime); \
  DBG_PORT.println(__VA_ARGS__);                                                                                \
  previousTime = millis();
#define DEBUGLOGF(...)                                                                                          \
  DBG_PORT.printf("%s[%s][%d]\t", String(__FILE__).c_str(), String(__LINE__).c_str(), millis() - previousTime); \
  DBG_PORT.printf(__VA_ARGS__);                                                                                 \
  previousTime = millis();
#endif

#else
#define DEBUGLOG(...)
#define DEBUGLOGF(...)
#endif


namespace MyDebug
{
 extern uint8_t m_debugOutput;
  #ifdef MCPOC_TELNET
extern RemoteDebug m_debugTelnet;
  #endif
extern uint32_t previousTime;
extern uint_fast32_t m_timeStampStatus;
  


  void begin(uint8_t output = 0, const char *moduleName = NULL);
  
  String buildLogStr (const char *file, int line, const char *function);

  void printGeneric(const char *file, int line, const char *function, String format);
  
  void print(const char *file, int line, const char *function, String format);

  void printf(const char *file, int line, const char *function,const char *format, ...);
  

  void handle();
 
   String getClassName(); 


  
};

#endif
