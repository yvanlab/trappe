
#ifndef DelayHelper_h
#define DelayHelper_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

class DelayHelper
{
public:
	DelayHelper() {
		m_endMS = 0;
	};

	void startDelay(uint32_t  delayMS) {
		m_startMS 	= millis();
		m_endMS 	= m_startMS+delayMS;
	}

	uint32_t getRemaining(){
		if (isDone()) return 0;
		return m_endMS - millis();
	}

	uint32_t getElpased(){
		if (isDone()) return m_endMS-m_startMS;
		return millis()-m_startMS;
	}

	bool isDone() {return millis() > m_endMS;}

protected:
	uint32_t		m_endMS=0;
	uint32_t		m_startMS=0;
	

};

#endif
