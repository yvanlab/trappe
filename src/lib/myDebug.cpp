
#include "MyDebug.h"

using namespace MyDebug;

namespace MyDebug
{
    uint8_t m_debugOutput;
#ifdef MCPOC_TELNET
    RemoteDebug m_debugTelnet;
#endif
    uint32_t previousTime;
    uint_fast32_t m_timeStampStatus;

    void begin(uint8_t output /* = 0*/, const char *moduleName /*= NULL*/)
    {
        if (output == 0)
        {
            m_debugOutput = DEBUG_SERIAL;
        }
        else
        {
            m_debugOutput = output;
        }

#ifdef MCPOC_TELNET
        m_debugOutput |= DEBUG_TELNET;
        if (moduleName != NULL)
        {
            m_debugTelnet.begin(moduleName);
        }
#endif
        previousTime = 0;
        // Serial.printf("m_debugOutput  INIT [%d]\n", m_debugOutput);
        //DEBUGLOGF("DEBUG LOGF m_debugOutput  INIT [%d]\n", m_debugOutput);
    }

    String buildLogStr(const char *file, int line, const char *function)
    {
        char tampon[250];
        snprintf(tampon, 65, "[%d][%5d] %25s:%-3d %-20s ", xPortGetCoreID(), (int)(millis() - previousTime), file, line, function);
        /*String res(tampon);
        if (res.length()<100) {
          res.concat(' ',100-res.length());
       snprintf(tampon,100,' ');

       }*/
        return String(tampon);
    }

    void print(const char *file, int line, const char *function, String format)
    {
        printGeneric(file, line, function, format + "\n");
    }

    void printGeneric(const char *file, int line, const char *function, String format)
    {
        // Serial.printf(" Print m_debugOutput [%d]\n", m_debugOutput);
        //Serial.printf("m_debugOutput PRINR[%x][%d]\n", &m_debugOutput, m_debugOutput);
        if (m_debugOutput & DEBUG_SERIAL)
        {
            Serial.print(buildLogStr(file, line, function));
            Serial.print(format);
        }
#ifdef MCPOC_TELNET
        if (m_debugOutput & DEBUG_TELNET)
        {
            m_debugTelnet.print(buildLogStr(file, line, function));
            m_debugTelnet.print(format);
        }
#endif
        previousTime = millis();
    };

    void printf(const char *file, int line, const char *function, const char *format, ...)
    {
        // Serial.printf(" PrintF output [%d]\n", m_debugOutput);
        va_list args;
        va_start(args, format);
        char acTmp[500]; // place holder for sprintf output
        vsprintf(acTmp, format, args);
        va_end(args);
        printGeneric(file, line, function, acTmp);

        /* if (m_debugOutput & DEBUG_SERIAL)
         {
           Serial.print(buildLogStr(file, line, function));
           Serial.print(acTmp);
         }
     #ifdef MCPOC_TELNET
         if (m_debugOutput & DEBUG_TELNET)
         {
           m_debugTelnet.print(buildLogStr(file, line, function));
           m_debugTelnet.print(acTmp);
         }
     #endif

         previousTime = millis();*/
    };

    void handle()
    {
#ifdef MCPOC_TELNET
        m_debugTelnet.handle();
#endif
    }

    String getClassName() { return __PRETTY_FUNCTION__; }

};
