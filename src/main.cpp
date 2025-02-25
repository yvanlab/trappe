#include <main.h>
portMUX_TYPE wtimerMux = portMUX_INITIALIZER_UNLOCKED;
QueueHandle_t xQueueCommand;

/* pinMode(m_pinButtonUp, INPUT_PULLDOWN);
   pinMode(m_pinButtonDown, INPUT_PULLDOWN);

   over intensity aux moins 2

   Mode forcer up or down

   mesure du temps passe


GPIO23	Status LED
GPIO32	Relay #1
GPIO33	Relay #2
GPIO25	Relay #3
GPIO26	Relay #4


*/
uint8_t pinLed = 5;
const uint8_t pinInyensity = 34;//PIN_INTENSITY;
const uint8_t pinButtonUp = 12;//PIN_BUTTON_UP;
const uint8_t pinButtonDown = 14;//PIN_BUTTON_DOWN;
const uint8_t pinUpSensorMain = 15;//PIN_BUTTON_DOWN;


const uint8_t pinCommandeUpDirection = 32;//PIN_COMMAND_UP;
const uint8_t pinCommandeDownDirection = 33;//PIN_COMMAND_DOWN;
const uint8_t pinCommandeTrappe = 25;//PIN_COMMAOuverure
//const uint8_t delayIntensity = 1000; // ms

const uint8_t pinCommandeUpSecond = 25;//PIN_COMMAND_UP;
const uint8_t pinCommandeDownSecond = 26;//PIN_COMMAND_DOWN;

using namespace MyDebug;


ConfigHelper config(pinLed);
Parameters param(pinLed);
WifiHelper myWifi(pinLed, &config);
IntensityControl intensityCtl(pinInyensity);
ActionneurDriver commandDriverMain(pinCommandeDownDirection, pinCommandeUpDirection, pinCommandeTrappe);
//ActionneurDriver commandDriverSecond(pinCommandeOuverured, pinCommandeUpSecond);
ButtonControl buttonCtl(pinButtonDown, pinButtonUp);
NetworkUI ui(pinLed, &param, &config);


#ifdef MCPOC_TELNET // Not in PRODUCTION

void status() {
	intensityCtl.clean();
	for (uint8_t i=0; i<11 ;i++ )
			intensityCtl.handle();
		DEBUGLOGF("m_actioneur [%d], m_command [%d]satus[%d], Courant[%f][%d] \n",commandDriverMain.m_actioneur,commandDriverMain.m_command, commandDriverMain.m_status, intensityCtl.m_fIntensityMeasure,analogRead(pinInyensity));

}

void processCmdRemoteDebug()
{
	String lastCmd = MyDebug::m_debugTelnet.getLastCommand();
	if (lastCmd == "st")
	{
		status();
	} else 	if (lastCmd == "ho")
	{
		DEBUGLOG("HO - horizontal OPEN");
		digitalWrite(pinCommandeUpDirection, true);
    	digitalWrite(pinCommandeDownDirection, false);
		status();
	}else 	if (lastCmd == "hc")
	{
		DEBUGLOG("HC - horizontal CLOSE");
		digitalWrite(pinCommandeUpDirection, false);
    	digitalWrite(pinCommandeDownDirection, true);
		status();
	}else if (lastCmd == "hn")
	{
		DEBUGLOG("HN - horizontal DEACTIVE");
		digitalWrite(pinCommandeUpDirection, false);
    	digitalWrite(pinCommandeDownDirection, false);
		status();
	}
	else if (lastCmd == "ta")
	{
		DEBUGLOG("TA - Trappe ACTIVE");
		digitalWrite(pinCommandeTrappe, true);
		status();
    }else 	if (lastCmd == "td")
	{
		DEBUGLOG("TA - Trappe deACTIVE");
		digitalWrite(pinCommandeTrappe, false);
		status();
    }
//
	else if (lastCmd == "fc")
	{
		DEBUGLOG("FC - force COSED status");
 		commandDriverMain.m_status = ActionneurDriver::STATUS_CLOSED;    
    	commandDriverMain.m_actioneur = ActionneurDriver::ACTIONEUR_HORIZONTAL;
		status();
    }else 	if (lastCmd == "fo")
	{
		DEBUGLOG("FO - force OPENED status");
 		commandDriverMain.m_status = ActionneurDriver::STATUS_OPENED;    
    	commandDriverMain.m_actioneur = ActionneurDriver::ACTIONEUR_TRAPPE_AND_HORIZONTAL;
		status();
    }

	else if (lastCmd == "restart")
	{
		ESP.restart();
	}
	else if (lastCmd == "reset")
	{
		WiFi.disconnect();
	}
}
#endif


void setup()
{
	Serial.begin(115200); // delay(500);
	Serial.println("start debuging");

#ifdef MCPOC_TELNET
	MyDebug::m_debugTelnet.setResetCmdEnabled(true); // Enable the reset command

	MyDebug::m_debugTelnet.showProfiler(true); // Profiler (Good to measure times, to optimize codes)

	MyDebug::m_debugTelnet.showColors(true); // Color
	String helpCmd = "??\nrestart\nreset\n";
	helpCmd += "ho - horizontal OPEN\nHC - horizontal CLOSE\nHN - horizontal NEUTRE\n";
	helpCmd += "ta - Trappe ACTIVE\nTD - Trappe deACTIVE\n";
	helpCmd += "fc - FORCE CLOSED STATUS\nfoc - FORCE OPENED STATUS\n";
	helpCmd += "st - actioneur [], command []satus[], Courant[][] \n";

	MyDebug::m_debugTelnet.setHelpProjectsCmds(helpCmd);
	MyDebug::m_debugTelnet.setCallBackProjectCmds(&processCmdRemoteDebug);
#endif
	DEBUGLOGF("Frq : %d \n", ESP.getCpuFreqMHz());
	DEBUGLOGF("Temp : %f \n", temperatureRead());

	/* Create a queue capable of containing 10 unsigned long values. */
	xQueueCommand = xQueueCreate(5, sizeof(ButtonControl::BUTTON_GLOBAL_STATUS));
	if (xQueueCommand == NULL)
	{
		DEBUGLOGF("Queue not created : %d \n", xQueueCommand);
	}
	pinMode(pinUpSensorMain, INPUT_PULLDOWN);

	config.begin();
	param.begin();
	myWifi.begin();
	MyDebug::begin(DEBUG_SERIAL | DEBUG_TELNET, config.m_moduleName.c_str());
	DEBUGLOG(myWifi.toString());
	ui.begin();
	// ui.setCallback(manageDisplay);
	intensityCtl.begin();
	commandDriverMain.begin(digitalRead(pinUpSensorMain));
	//commandDriverSecond.begin(digitalRead(pinUpSensorMain));
	buttonCtl.beging();

	mtTimer.begin(timerFrequence);
	mtTimer.setCustomMS(50);
}

void manageTimeBased(ButtonControl::BUTTON_GLOBAL_STATUS btAction)
{
	if ((btAction == ButtonControl::BUTTON_CLOSE_PRESSED) || (btAction == ButtonControl::BUTTON_CLOSE_PRESSED_LONG))
	{
		if (commandDriverMain.isActionClosing())
		{
			commandDriverMain.stopActionneur();
		}
		else
		{
			intensityCtl.clean();
			commandDriverMain.startActioneurClose();
		}
	}
	else if ((btAction == ButtonControl::BUTTON_OPEN_PRESSED) || (btAction == ButtonControl::BUTTON_OPEN_PRESSED_LONG))
	{
		if (commandDriverMain.isActionOpening())
		{
			commandDriverMain.stopActionneur();
		}
		else
		{
			intensityCtl.clean();
			commandDriverMain.startActioneurOpen();
		}
	}
	else if (btAction == ButtonControl::BUTTON_OPEN_AND_CLOSE_PRESSED)
	{
		commandDriverMain.stopActionneur();
	}
	else if ((btAction == ButtonControl::BUTTON_CLOSE_RELEASED_FROM_PRESSED_LONG) || (btAction == ButtonControl::BUTTON_OPEN_RELEASED_FROM_PRESSED_LONG))
	{
		commandDriverMain.stopActionneur();
	}
	else
	{
		DEBUGLOGF("CMD not known ON TIME BASED[%d]\n", btAction);
	}
}

void manageButtonBased(ButtonControl::BUTTON_GLOBAL_STATUS btAction)
{
	if ((btAction == ButtonControl::BUTTON_CLOSE_PRESSED_LONG))
	{
			intensityCtl.clean();
			commandDriverMain.startActioneurClose();
	}
	else if ( (btAction == ButtonControl::BUTTON_OPEN_PRESSED_LONG))
	{
			intensityCtl.clean();
			commandDriverMain.startActioneurOpen();
	}
	else if ((btAction == ButtonControl::BUTTON_CLOSE_RELEASED_FROM_PRESSED_LONG) || (btAction == ButtonControl::BUTTON_OPEN_RELEASED_FROM_PRESSED_LONG))
	{
		intensityCtl.clean();
		commandDriverMain.stopActionneur();
	}
	else
	{
		DEBUGLOGF("CMD not knownON BUTTON BASED [%d]\n", btAction);
	}
}


// DelayHelper m_tempoLongMode;
ButtonControl::BUTTON_GLOBAL_STATUS btAction;
//ButtonControl::BUTTON_GLOBAL_STATUS btPreviousAction;
void loop()
{

	if (xQueueReceive(xQueueCommand,
					  &(btAction),
					  (TickType_t)10) == pdPASS)
	{
		if (param.m_managementMode == MODE_TIME_BASED)
			manageTimeBased(btAction);
		else if (param.m_managementMode == MODE_BUTTON)
			manageButtonBased(btAction);
		else
			DEBUGLOGF("Mode [%d]] not implemented\n",param.m_managementMode);
	};

	if (commandDriverMain.isActionUnderProgress())
	{
		intensityCtl.handle();
		if (mtTimer.is1SPeriod())
		{
			DEBUGLOGF("courant mesure/instant[%f/%f], Max/Min [%f/%f], Max courant accepted[%f]", 
				intensityCtl.m_fIntensityMeasure, 
				analogRead(pinInyensity), 
				intensityCtl.m_fMaxIntensityMeasure,
				intensityCtl.m_fMinIntensityMeasure);
			DEBUGLOGF("m_actioneur cc[%d], elapse[%d] \n"  ,commandDriverMain.m_maxCurrentAccepted,  commandDriverMain.getElapse() / 1000);
		}
	}
	commandDriverMain.handle(intensityCtl.m_fIntensityMeasure);

	buttonCtl.handle();
	ui.handleServer();
	MyDebug::handle();
	mtTimer.clearPeriod();
}