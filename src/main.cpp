#include <main.h>
portMUX_TYPE wtimerMux = portMUX_INITIALIZER_UNLOCKED;
QueueHandle_t xQueueCommand;

 
uint8_t pinLed = 5;
const uint8_t pinInyensity 	= PIN_INTENSITY;
const uint8_t pinButtonUp 	= PIN_BUTTON_UP;
const uint8_t pinButtonDown = PIN_BUTTON_DOWN;
const uint8_t pinCommandeUp	= PIN_COMMAND_UP;
const uint8_t pinCommandeDown = PIN_COMMAND_DOWN;

const uint8_t delayIntensity = 250; //ms

ConfigHelper config(pinLed);
Parameters param(pinLed);
WifiHelper myWifi(pinLed, &config);
IntensityControl intensityCtl(pinInyensity,delayIntensity);
ActionneurDriver commandDriver(pinCommandeDown,pinCommandeUp);
ButtonControl	 buttonCtl(pinButtonDown, pinButtonUp);
NetworkUI ui(pinLed, &param, &config);

void setup() {
    Serial.begin(115200); // delay(500);
	Serial.println("start debuging");
	DEBUGLOGF("Frq : %d \n", ESP.getCpuFreqMHz());
	DEBUGLOGF("Temp : %f \n", temperatureRead());

   /* Create a queue capable of containing 10 unsigned long values. */
    xQueueCommand = xQueueCreate( 5, sizeof( ButtonControl::BUTTON_GLOBAL_STATUS ) );
	if( xQueueCommand == NULL )
    {
        DEBUGLOGF("Queue not created : %d \n", xQueueCommand);
    }


	config.begin();
	param.begin();
	myWifi.begin();
	DEBUGLOG(myWifi.toString());
	ui.begin();
	//ui.setCallback(manageDisplay);
	intensityCtl.begin();
	commandDriver.begin();
	buttonCtl.beging();

	mtTimer.begin(timerFrequence);
	mtTimer.setCustomMS(50);
}

//DelayHelper m_tempoLongMode;
ButtonControl::BUTTON_GLOBAL_STATUS  btAction;
ButtonControl::BUTTON_GLOBAL_STATUS  btPreviousAction;
void loop() {

	if( xQueueReceive( xQueueCommand,
                         &( btAction ),
                         ( TickType_t ) 10 ) == pdPASS )
      {
         /*if (btAction == ButtonControl::BUTTON_CLOSE_PRESSED_LONG || btAction == ButtonControl::BUTTON_OPEN_PRESSED_LONG ) {
			m_tempoLongMode.startDelay(120*1000);//2mn

		 }*/

	//if (buttonCtl.isActionUnderProgress()) {
		if ((btAction == ButtonControl::BUTTON_CLOSE_PRESSED) || (btAction == ButtonControl::BUTTON_CLOSE_PRESSED_LONG)) {
			if (commandDriver.isActionClosing()) {
				commandDriver.stopActionneur();
			} else {
				commandDriver.startActioneurClose();
			}
		} else if ((btAction == ButtonControl::BUTTON_OPEN_PRESSED) || (btAction == ButtonControl::BUTTON_OPEN_PRESSED_LONG)) {
			if (commandDriver.isActionOpening()) {
				commandDriver.stopActionneur();
			} else {
				commandDriver.startActioneurOpen();
			}
			
		} else if (btAction == ButtonControl::BUTTON_OPEN_AND_CLOSE_PRESSED) {
			commandDriver.stopActionneur();
			
		} else if ( (btAction == ButtonControl::BUTTON_CLOSE_RELEASED_FROM_PRESSED_LONG) || (btAction == ButtonControl::BUTTON_OPEN_RELEASED_FROM_PRESSED_LONG)) {
			commandDriver.stopActionneur();
		} else {
			DEBUGLOGF("CMD not known [%d]\n",btAction);
		}
	}
	if (commandDriver.isActionUnderProgress()) {
		intensityCtl.handle();
	}
	commandDriver.handle(intensityCtl.bIsOverTreshold);
	buttonCtl.handle();
	ui.handleServer();
 // put your main code here, to run repeatedly:
}