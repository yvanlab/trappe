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
NetworkUI ui(pinLed, &param, &config);
IntensityControl intensityCtl(pinInyensity,delayIntensity);
ActionneurDriver commandDriver(pinCommandeDown,pinCommandeUp);
ButtonControl	 buttonCtl(pinButtonDown, pinCommandeDown);

void setup() {
    Serial.begin(115200); // delay(500);
	Serial.println("start debuging");
	DEBUGLOGF("Frq : %d \n", ESP.getCpuFreqMHz());
	DEBUGLOGF("Temp : %f \n", temperatureRead());

   /* Create a queue capable of containing 10 unsigned long values. */
    xQueueCommand = xQueueCreate( 5, sizeof( uint8_t ) );
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
	commandDriver.begin();
	buttonCtl.beging();

	mtTimer.begin(timerFrequence);
	mtTimer.setCustomMS(50);
}

void loop() {
	ButtonControl::BUTTON_PRESSED  btAction;
	if( xQueueReceive( xQueueCommand,
                         &( btAction ),
                         ( TickType_t ) 10 ) == pdPASS )
      {
         /* xRxedStructure now contains a copy of xMessage. */

	//if (buttonCtl.isActionUnderProgress()) {
		if (btAction == ButtonControl::BUTTON_DOWN) {
			commandDriver.startActioneurDOWN();
		} else if (btAction == ButtonControl::BUTTON_UP) {
			commandDriver.startActioneurUP();
		} else if (btAction == ButtonControl::BUTTON_BOTH) {
			commandDriver.stopActionneur();
		} else {
			DEBUGLOGF("CMD not known [%d]\n",btAction);
		}
	}
	if (commandDriver.isActionUnderProgress()) {
		intensityCtl.handle();
	}
	commandDriver.handle(intensityCtl.bIsOverTreshold);
	ui.handleServer();
 // put your main code here, to run repeatedly:
}