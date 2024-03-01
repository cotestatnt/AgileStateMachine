#include <AgileStateMachine.h>

#define PREV_BUTTON  	5
#define NEXT_BUTTON  	4
#define LED_BLINK 		LED_BUILTIN

// Let's use an array for change blink time
uint32_t blinkInterval = 1000;
uint32_t blinkTime[] = {0, 1000, 300, 150};

// Create new Finite State Machine
StateMachine myFSM;

// A variable for triggering transitions
bool xPrevButton = false;


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
 	Serial.print(F("Leaving state"));
	Serial.println(myFSM.getActiveStateName());
}

// Define "on entering" callback functions (just a message in this example)
void onEntering(){
	Serial.print(F("Entered state"));
	Serial.println(myFSM.getActiveStateName());

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
	State* blinkOff = myFSM.addState("BlinkOFF", 500, onEntering, nullptr, onLeaving);
	State* blink1 = myFSM.addState("Blink1", 1000, onEntering, nullptr, onLeaving);
	State* blink2 = myFSM.addState("Blink2", 2000, onEntering, nullptr, onLeaving);
	State* blink3 = myFSM.addState("Blink3", 3000, onEntering, nullptr, onLeaving);

	// Add transitions to target state and trigger condition (callback function or bool var)
	blink1->addTransition(blink2, xNextButton);			// xNextButton is a callback function
	blink1->addTransition(blinkOff, xPrevButton);		// xPrevButton is a bool variable
	blink2->addTransition(blink3, xNextButton);
	blink2->addTransition(blink1, xPrevButton);
	blink3->addTransition(blink2, xPrevButton);
	blinkOff->addTransition(blink1, 5000);				// This transition is on state timeout (5s)
	blinkOff->addTransition(blink1, xNextButton);

	// Start the Machine State
	myFSM.setInitialState(blinkOff);
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
	Serial.println(myFSM.getActiveStateName());
	Serial.println();

	// Set initial blink time
	blinkInterval = blinkTime[myFSM.getCurrentState()->getIndex()];
}


void loop() {

	// Read reset button input and update resetBlinky variable
	xPrevButton = ! digitalRead(PREV_BUTTON);

	// Update State Machine	(true is state changed)
	if (myFSM.execute()) {
		Serial.println();
	}

	// If blinkInterval greater than 0, let's blink the led
	if (blinkInterval > 0) {
		blink();
	}
	else {
		digitalWrite(LED_BLINK, LOW);
		delay(1);
	}

}
