
/*
* If you need to save some SRAM, store the state names in flash memory using F() macro.
* When needed you can use the getActiveStateName_P functions to get state name.
* This is useful for boards with limited SRAM like Arduino Uno;
* In addition, statically defined states, transitions and Actions should be used when possible.
*/
#include <AgileStateMachine.h>

#define PREV_BUTTON  	5
#define NEXT_BUTTON  	4
#define LED_BLINK 		LED_BUILTIN

// Let's use an array for change blink time
uint32_t blinkInterval = 1000;
uint32_t blinkTime[] = {0, 1000, 300, 150};

// Create new Finite State Machine
StateMachine myFSM;

// Blink led. Frequency depends of selected state
void blink() {	
	static bool level = LOW;
	static uint32_t bTime;
	if (millis() - bTime >= blinkInterval ) {
		bTime = millis();
		level = !level;		
		digitalWrite(LED_BLINK, level);
	}
}

/////////// STATE MACHINE FUNCTIONS //////////////////

// Define "on leaving" callback functions
void onLeaving(){
 	Serial.print(F("Leaving state "));
	Serial.println(myFSM.getActiveStateName_P());
}

// Define "on entering" callback functions (just a message in this example)
void onEntering(){
	Serial.print(F("Entered state "));
	Serial.println(myFSM.getActiveStateName_P());

	blinkInterval = blinkTime[myFSM.getCurrentState()->getIndex()];
	Serial.print(F("Blink time: "));
	Serial.println(blinkInterval);
}

// A simple callback function for transition trigger (just button state)
bool xNextButton() {
	/* 
	* Since we have defined a minimum run time for states, the button bounces  
	* has no effects on state machine mechanism functioning
	* (FSM can switch to next state only after min time has passed)	
	*/
	return (digitalRead(NEXT_BUTTON) == LOW);
}


// Definition of the model of the finite state machine and start execution
void setupStateMachine(){
	// Create some states and assign name and callback functions
	State* blink0 = myFSM.addState(F("Blink0"), 500, onEntering, onLeaving, nullptr);
	State* blink1 = myFSM.addState(F("Blink1"), 500, onEntering, onLeaving, nullptr);
	State* blink2 = myFSM.addState(F("Blink2"), 500, onEntering, onLeaving, nullptr);
	State* blink3 = myFSM.addState(F("Blink3"), 500, onEntering, onLeaving, nullptr);

	// Add transitions to target state and trigger condition (callback function or bool var)
	blink0->addTransition(blink1, xNextButton);		// xNextButton is a callback function
	blink1->addTransition(blink2, xNextButton);			
	blink2->addTransition(blink3, xNextButton);
	blink3->addTransition(blink0, 5000);			// This transition is on state timeout (5s)	

	// Start the Machine State
	myFSM.setInitialState(blink0);
	myFSM.start();
}


void setup() {
	pinMode(NEXT_BUTTON, INPUT_PULLUP);
	pinMode(PREV_BUTTON, INPUT_PULLUP);
	pinMode(LED_BLINK, OUTPUT);

	Serial.begin(115200);
	Serial.println(F("\n\nStarting State Machine...\n"));
	setupStateMachine();

	// Initial state
	Serial.print(F("Active state: "));
	Serial.println(myFSM.getActiveStateName_P());
	Serial.println();

	// Set initial blink time
	blinkInterval = blinkTime[myFSM.getCurrentState()->getIndex()];
}


void loop() {

	// Update State Machine	
	myFSM.execute();

	// If blinkInterval greater than 0, let's blink the led
	if (blinkInterval) {
		blink();
	}
	else {
		digitalWrite(LED_BLINK, LOW);
		delay(1);
	}

}
