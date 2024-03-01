#include <AgileStateMachine.h>

#if defined(ESP32)
#define PREV_BUTTON  	5
#define NEXT_BUTTON  	4
#define LED 			RGB_BUILTIN
#else
#define PREV_BUTTON  	5
#define NEXT_BUTTON  	4
#define LED 			LED_BUILTIN
#endif


// Let's use an array for change blink time
uint32_t blinkInterval = 1000;
uint32_t blinkTime[] = {0, 1000, 300, 150};

// Create new Finite State Machine
StateMachine myFSM;

// A variable for triggering transitions
bool xPrevButton = false;

bool xAction1Out = false;
bool xActionDelayed = false;


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

// A callback function for triggerin transition
// set argument target true only on rising edge
bool xNextButton() {
	static bool risingEdge = false;    	// store last button state
	static uint32_t pushTime;			// for button debouncing

	if (!digitalRead(NEXT_BUTTON) && !risingEdge) {
		risingEdge = true;
		if( millis() - pushTime > 200){
			pushTime = millis();
			return true;
		}
	}
	else if ( millis() - pushTime > 200 && risingEdge) {
		risingEdge = false;
	}
	return false;
}


// Setup the State Machine properties
void setupStateMachine(){
	// Create some states and assign name and callback functions
	State* blinkOff = myFSM.addState("BlinkOFF", 500, onEntering, nullptr, onLeaving);
	State* blink1 = myFSM.addState("Blink1", 500, onEntering, nullptr, onLeaving);
	State* blink2 = myFSM.addState("Blink2", 500, onEntering, nullptr, onLeaving);
	State* blink3 = myFSM.addState("Blink3", 500, onEntering, nullptr, onLeaving);

	// Add transitions to target state and trigger condition (callback function or bool var)
	blink1->addTransition(blink2, xNextButton);			// xNextButton is a callback function
	blink1->addTransition(blinkOff, xPrevButton);		// xPrevButton is a bool variable
	blink2->addTransition(blink3, xNextButton);
	blink2->addTransition(blink1, xPrevButton);
	blink3->addTransition(blink2, xPrevButton);
	blinkOff->addTransition(blink1, 5000);				// This transition is on state timeout (5s)

	blink2->addAction(Action::Type::S, xAction1Out);
	blink3->addAction(Action::Type::R, xAction1Out);
	blink1->addAction(Action::Type::R, xAction1Out);
	blink3->addAction(Action::Type::L, xActionDelayed, 3000);

	// Start the Machine State
	myFSM.setInitialState(blinkOff);
	myFSM.start();
}



void setup() {
	pinMode(NEXT_BUTTON, INPUT_PULLUP);
	pinMode(PREV_BUTTON, INPUT_PULLUP);
	pinMode(LED, OUTPUT);

	Serial.begin(115200);
	Serial.println(F("\n\nStarting State Machine...\n"));
	setupStateMachine();

	// Initial state
	Serial.print(F("Active state: "));
	Serial.println(myFSM.getActiveStateName());
	Serial.println();
	blinkInterval = blinkTime[myFSM.getCurrentState()->getIndex()];
}


void loop() {

	// Blink led. Frequency depends of selected state
	static bool level = LOW;
	static uint32_t bTime;
	if (blinkInterval > 0) {
		if (millis() - bTime >= blinkInterval ) {
			level = !level;
			bTime = millis();
			digitalWrite(LED, level);
		}
	}
	else {
		digitalWrite(LED, LOW);
		delay(1);
	}

	// Read reset button input and update resetBlinky variable
	xPrevButton = ! digitalRead(PREV_BUTTON);

	// Update State Machine	(true is state changed)
	if (myFSM.execute()) {
		Serial.println();
	}

	// Check FSM actions
	if (xAction1Out) {
		static uint32_t aTime;
		if (millis() - aTime > 1000) {
			aTime = millis();
			Serial.println(F("xAction1Out == true"));
		}
	}

	if (xActionDelayed) {
		static uint32_t aTime;
		if (millis() - aTime > 1000) {
			aTime = millis();
			Serial.println(F("xActionDelayed == true"));
		}
	}

}


