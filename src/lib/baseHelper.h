

#ifndef BaseHelper_h
#define BaseHelper_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include "ylabconfig.h"
#include "myDebug.h"
/*
#ifdef MCPOC_TELNET
#include "RemoteDebug.h"
extern RemoteDebug Debug;
#endif

#define JSON_TEXT true
#define STD_TEXT false

#ifdef MCPOC_TELNET
#define DBG_PORT Debug
#else
#define DBG_PORT Serial
#endif


#ifdef MCPOC_TEST
static uint32_t previousTime = 0;

#ifdef ESP32
#define DEBUGLOG(...)                                                                                                                        \
  DBG_PORT.printf("[%d][%d]\t%s:[%d]%s\t", xPortGetCoreID(), (int)(millis() - previousTime), __FILE__, __LINE__,__FUNCTION__); \
  DBG_PORT.println(__VA_ARGS__);                                                                                                             \
  previousTime = millis();
#define DEBUGLOGF(...)                                                                                                                       \
  DBG_PORT.printf("[%d][%d]\t%s:[%d]%s\t", xPortGetCoreID(), (int)(millis() - previousTime), __FILE__, __LINE__,__FUNCTION__); \
  DBG_PORT.printf(__VA_ARGS__);                                                                                                              \
  previousTime = millis();
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
*/
class BaseHelper
{
public:
  BaseHelper() { ; };
  BaseHelper(uint8_t pinLed)
  {
    m_pinLed = pinLed;
    if (m_pinLed != 0)
      pinMode(m_pinLed, OUTPUT);
  };

  void setup(uint8_t pinLed)
  {
    m_pinLed = pinLed;
  };

  void switchOn()
  {
    if (m_pinLed != 0)
#ifdef ESP32
      digitalWrite(m_pinLed, HIGH);
#else
      digitalWrite(m_pinLed, LOW);
#endif
  };

  void switchOff()
  {
    if (m_pinLed != 0)
#ifdef ESP32
      digitalWrite(m_pinLed, LOW);
#else
      digitalWrite(m_pinLed, HIGH);
#endif
  };

  void setStatus(uint8_t codeStatus, String description)
  {
    setStatus(0, codeStatus, description);
  }

  void setStatus(unsigned long timeStampStatus, uint8_t codeStatus, String description)
  {
    m_timeStampStatus = timeStampStatus;
    m_codeStatus = codeStatus;
    m_description = description;
  }

  virtual String toString() { return ""; }
  virtual String toJson() { return ""; }

  String logJson(boolean bJson) { return  "{\"nom\":\"" + getClassName() + "\",\"code\":\"" + String(m_codeStatus) + "\",\"description\":\"" + m_description + "\"}"; };
  String logString(boolean bJson)  { return "{\"nom\":\"" + getClassName() + "\",\"code\":\"" + String(m_codeStatus) + "\",\"description\":\"" + m_description + "\"}"; };

  virtual String getClassName() { return __PRETTY_FUNCTION__; }
  uint8_t getStatusCode() { return m_codeStatus; };
  String getStatusDescription() { return m_description; };

private:
  uint8_t m_pinLed;
  uint8_t m_codeStatus;
  uint_fast32_t m_timeStampStatus;
  String m_description;
};

#endif
