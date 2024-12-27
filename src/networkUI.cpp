
#include "main.h"
#include "UiHelper.h"
#include "networkUI.h"
#include "Arduino.h"



wl_status_t NetworkUI::begin() {
	DEBUGLOG("Begin");
	wl_status_t res = UiHelper::begin();

	//_server->on("/status", std::bind(&NetworkUI::dataJson, this));
	_server->on("/param", std::bind(&NetworkUI::displayParameters, this));
	_server->on("/setParam", std::bind(&NetworkUI::setParameters, this));
	_server->on("/jsonParam", std::bind(&NetworkUI::jsonParam, this));
	_server->on("/jsonOptions", std::bind(&NetworkUI::jsonOptions, this));

	_server->on("/saveParam", std::bind(&NetworkUI::saveParameters, this));
/*
	_server->on("/config", configPage);
	_server->on("/prog", progPage);
*/

	return res;
}

void  NetworkUI::jsonOptions() {
	callback(FUNCTION_IN);
	///digitalWrite(PIN_LED, LOW);
	//mpPages->stopTimer();
	DEBUGLOG("Optons");
	loadFromSpiffs("/options.json");
	////mpPages->startTimer();
	digitalWrite(PIN_LED, HIGH);
	callback(FUNCTION_OUT);
}


String  NetworkUI::jsonParam() {
	
	String tt("{\"module\":{");
	tt += m_configuration->toJson() + "},"; 
	tt += "\"setting\":{" + m_parameters->toJson() + "},";
	tt += "\"actionneur\":{" + commandDriverMain.toJson() + "},";
	tt += "\"buttons\":{" + buttonCtl.toJson() + "}";
	tt += "}";	
	_server->send(200, "text/html", tt);

// tt += "\"bmp\":{" + bmpMesure->toString(JSON_TEXT) + "},";
// tt += "\"LOG\":[" + wfManager->log(JSON_TEXT) + "," + _parameters->log(JSON_TEXT) + /*","+bmpMesure->log(JSON_TEXT)+*/ "]";
	return tt;
}


/*void NetworkUI::firmware()
{
	digitalWrite(PIN_LED, LOW);
	//mpPages->stopTimer();

	DEBUGLOG("My  firmware");
	wfManager->httpUpdaterPage();
	//_server->send_P ( 200, "text/html", HTML );

	////mpPages->startTimer();
	digitalWrite(PIN_LED, HIGH);
}*/

#ifdef OTA_FOR_ATOM
void NetworkUI::OTAOnStart()
{
	digitalWrite(PIN_LED, LOW);
	//mpPages->stopTimer();
	DEBUGLOG("My OTA");
	wfManager->OTAOnStart();
	digitalWrite(PIN_LED, HIGH);
}
#endif

void NetworkUI::saveParameters()
{
	callback(FUNCTION_IN);
	////mpPages->stopTimer();
	m_parameters->writeData();
	////mpPages->startTimer();
	//dataPage();
	callback(FUNCTION_OUT);
}


void NetworkUI::setParameters()
{
	bool changed = false;


	DEBUGLOG("SetData");
#ifdef MCPOC_TEST
	for (uint8_t i = 0; i < _server->args(); i++)
	{
		DEBUGLOGF("[%s,%s]\n", _server->argName(i).c_str(), _server->arg(i).c_str());
	}
	DEBUGLOG("");
#endif

	String str;
	ButtonControl::BUTTON_GLOBAL_STATUS tmp;
	if ((str = _server->arg("timeUpSec")) != NULL)
	{
		m_parameters->m_timeUpSec = (uint8_t)atoi(str.c_str());
	}
	else if ((str = _server->arg("timeDownSec")) != NULL)
	{
		m_parameters->m_timeDownSec = (uint8_t)atoi(str.c_str());;
	}
	//Max Power
	else if ((str = _server->arg("maxPowerAmp")) != NULL)
	{
		m_parameters->m_maxPowerAmp = (float)atof(str.c_str());;
	} 
	else if ((str = _server->arg("maxAmpDownHori")) != NULL)
	{
		m_parameters->m_maxAmpDownHori = (float)atof(str.c_str());;
	} 
	else if ((str = _server->arg("maxAmpDownVert")) != NULL)
	{
		m_parameters->m_maxAmpDownVert = (float)atof(str.c_str());;
	} 
	else if ((str = _server->arg("maxAmpUpHori")) != NULL)
	{
		m_parameters->m_maxAmpUpHori = (float)atof(str.c_str());;
	} 
	else if ((str = _server->arg("maxAmpUpVert")) != NULL)
	{
		m_parameters->m_maxAmpUpVert = (float)atof(str.c_str());;
	} 
	else if ((str = _server->arg("minPowerAmp")) != NULL)
	{
		m_parameters->m_minPowerAmp = (float)atof(str.c_str());;
	} 
	else if ((str = _server->arg("delayIntensity")) != NULL)
	{
		m_parameters->m_delayIntensity = (uint16_t)atoi(str.c_str());
	} 
	else if ((str = _server->arg("managementMode")) != NULL)
	{
		m_parameters->m_managementMode = (MANAGEMENT_MODE)atoi(str.c_str());;
	} 
	else if (_server->hasArg("btUp"))
	{
		tmp =  ButtonControl::BUTTON_OPEN_PRESSED;
		DEBUGLOG("SEND UP");
		if( xQueueSendToBack( xQueueCommand,
                             &tmp,
                             ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
			DEBUGLOG("Failed to por CMD UP");
        }
	}else if (_server->hasArg("btDown"))
	{
		tmp = ButtonControl::BUTTON_CLOSE_PRESSED;
		if( xQueueSendToBack( xQueueCommand,
                             &tmp,
                             ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
			DEBUGLOG("Failed to por CMD UP");
        }
	}else if (_server->hasArg("btStop"))
	{
		tmp = ButtonControl::BUTTON_OPEN_AND_CLOSE_PRESSED;
		if( xQueueSendToBack( xQueueCommand,
                             &tmp,
                             ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
			DEBUGLOG("Failed to por CMD BOTH");
        }
	}else if (_server->hasArg("btDownLong")) {
		tmp = ButtonControl::BUTTON_CLOSE_PRESSED_LONG;
		if( xQueueSendToBack( xQueueCommand,
                             &tmp,
                             ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
			DEBUGLOG("Failed to por CMD BOTH");
        }
	}else if (_server->hasArg("btUpLong")) {
		tmp = ButtonControl::BUTTON_OPEN_PRESSED_LONG;
		if( xQueueSendToBack( xQueueCommand,
                             &tmp,
                             ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
			DEBUGLOG("Failed to por CMD BOTH");
        }
	}else if (_server->hasArg("btStopDownLong")) {
		tmp = ButtonControl::BUTTON_CLOSE_RELEASED_FROM_PRESSED_LONG;
		if( xQueueSendToBack( xQueueCommand,
                             &tmp,
                             ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
			DEBUGLOG("Failed to por CMD BOTH");
        }
	}else if (_server->hasArg("btStopUpLong")) {
		tmp = ButtonControl::BUTTON_OPEN_RELEASED_FROM_PRESSED_LONG;
		if( xQueueSendToBack( xQueueCommand,
                             &tmp,
                             ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
			DEBUGLOG("Failed to por CMD BOTH");
        }
	}

	_server->send(200, "text/html", "ok");
}	

void NetworkUI::displayParameters()
{

	digitalWrite(PIN_LED, LOW);
	////mpPages->stopTimer();

	DEBUGLOG("dataPage");
	delay(1000);
	loadFromSpiffs("/index.html");
	//_server->send_P ( 200, "text/html", HTML );

	//phPresence->forceStatus(true);
	////mpPages->startTimer();
	digitalWrite(PIN_LED, HIGH);

}




