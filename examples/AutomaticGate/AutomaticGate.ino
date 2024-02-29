#include "AgileStateMachine.h"

const byte SEC_FTC1 =  26;      // Safety photocell
const byte BTN_START = 27;      // Gate open request
const byte LED_OPEN =  14;     
const byte LED_CLOSE = 12;
const byte LED_FLASHER  = 13;

uint32_t openedDelay = 10000;   // Time the gate remain opened
uint32_t openTime   = 8000;     // Time needed to complete movement
uint32_t closeTime  = 8000;     // Time needed to complete movement
uint32_t waitTime   = 3000;     // Time to wait before restart after safety FTC

// The Finite State Machine
StateMachine fsm;

// Input/Output State Machine interface
bool inStartButton, inSafetyFTC, inResetGate;
bool outOpen, outClose, outFlashBlink;

State* stClosed;
State* stClosing;
State* stOpened;
State* stOpening;
State* stStopWait;

// External interrupt for safety photocell
void IRAM_ATTR SafetyPhotecellOn() {

	// Check photocell only while the motor is moving.
	if (outOpen || outClose) {
		inSafetyFTC = true;
	}
}

/////////// STATE MACHINE FUNCTIONS //////////////////

// This function will be executed before exit the current state
void onLeaving(){
 	Serial.print("Leaving state: ");
	Serial.println(fsm.getActiveStateName());     
}

// This function will be executed before enter next state
void onEntering(){
	Serial.print("Entered state: ");
	Serial.println(fsm.getActiveStateName());
}


// Definition and modeling of the finite state machine
void setupStateMachine(){
	/* Create states and assign name and callback functions */
	stClosed = fsm.addState("Closed", onEntering, nullptr, onLeaving);
	stClosing = fsm.addState("Closing", onEntering, nullptr, onLeaving);
	stOpened = fsm.addState("Opened", onEntering, nullptr, onLeaving);
	stOpening = fsm.addState("Opening", onEntering, nullptr, onLeaving);
	stStopWait = fsm.addState("Stop & Wait", waitTime, onEntering, nullptr, onLeaving);
  
	/* Define transitions to target state and trigger condition (callback function or bool var) */
	stClosed->addTransition(stOpening, inStartButton);		
	stOpening->addTransition(stOpened, openTime);		
	stOpened->addTransition(stClosing, openedDelay);		  
	stClosing->addTransition(stClosed, closeTime);		
	stClosing->addTransition(stStopWait, inSafetyFTC);		
	stStopWait->addTransition(stOpening, inResetGate);		

	/* Add actions for the states where needed */

	// Start blink flasher immediatly
	stOpening->addAction(Action::Type::S, outFlashBlink);
	stClosing->addAction(Action::Type::S, outFlashBlink);
	stOpened->addAction(Action::Type::R, outFlashBlink);
	stClosed->addAction(Action::Type::R, outFlashBlink);  

	// Start gate motor after some delay
	stOpening->addAction(Action::Type::D, outOpen, 2000);
	stClosing->addAction(Action::Type::D, outClose, 2000);

	// Safety handling (only on closing)
	stStopWait->addAction(Action::Type::R, outOpen);
	stStopWait->addAction(Action::Type::R, outClose);
	stStopWait->addAction(Action::Type::R, inSafetyFTC);
  
	/* Set initial state and start the Machine State */
	fsm.setInitialState(stClosed);
	fsm.start();
    Serial.print("Active state: ");
	Serial.println(fsm.getActiveStateName());
	Serial.println();
}


void setup() {
	pinMode(SEC_FTC1, INPUT_PULLUP);
	pinMode(BTN_START, INPUT_PULLUP);  
	pinMode(LED_OPEN, OUTPUT);
    pinMode(LED_CLOSE, OUTPUT);
    pinMode(LED_FLASHER, OUTPUT);

	Serial.begin(115200);
	Serial.println("Starting State Machine...\n");
	setupStateMachine();	

    // External interrupt for safety photocell
    attachInterrupt(digitalPinToInterrupt(SEC_FTC1), SafetyPhotecellOn, FALLING);  
}


void loop() {

    // Get machine state inputs
    inStartButton = (digitalRead(BTN_START) == LOW);
    inResetGate = (fsm.getCurrentState() == stStopWait) && inStartButton;

	// Run State Machine	(true is state changed)
	if (fsm.execute()) {
		Serial.println();
	}

    // Set outputs according to state
	digitalWrite(LED_OPEN, outOpen);
    digitalWrite(LED_CLOSE, outClose);

    // Blink flasher	
	if (outFlashBlink || fsm.getCurrentState() == stStopWait) {
    static bool level = LOW;
	  static uint32_t bTime;
		if (millis() - bTime >= 500 ) {
			level = !level;
			bTime = millis();
			digitalWrite(LED_FLASHER, level);
		}
	}
    else if (fsm.getCurrentState() == stOpened) {
        digitalWrite(LED_FLASHER, HIGH);
		delay(1);
    }
	else {
		digitalWrite(LED_FLASHER, LOW);
		delay(1);
	}

}


