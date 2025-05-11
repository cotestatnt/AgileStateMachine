#include <AgileStateMachine.h>

#define NEXT_BUTTON  	4
#define LED_BLINK 		LED_BUILTIN

// Let's use an array for change blink time
uint32_t blinkInterval = 1000;
uint32_t blinkTime[] = {0, 1000, 300, 150};

// Create new Finite State Machine
StateMachine myFSM;

void printStateInfo(State* state) {
	Serial.print(F("State '"));
	Serial.print(state->getStateName());
	Serial.print(F("' @: 0x"));
	Serial.println((uintptr_t)state, HEX);
}

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
	Serial.println(myFSM.getActiveStateName());
	Serial.println();
}

// Define "on entering" callback functions (just a message in this example)
void onEntering(){
	State* statePtr = myFSM.getCurrentState();
	printStateInfo(statePtr);	
	
	blinkInterval = blinkTime[myFSM.getCurrentState()->getIndex()];
	Serial.print(F("Blink time: "));
	Serial.println(blinkInterval);
}

bool xNextButton = false;

// State blink1 will be created dynamically
// State blink3 will be created dynamically

// Name, min time, on enter callback, on leave callback, run callback (not used)
State blink0("Blink0", 1000, onEntering, onLeaving, nullptr);
State blink2("Blink2", 1000, onEntering, onLeaving, nullptr);

// because the state still not exists, transition to blink1 and blink3
//  will be created dynamically in the setupStateMachine() function
Transition toBlink2(blink2, xNextButton);
Transition toBlink0(blink0, 5000); // This transition is on state timeout (5s)

// Definition of the model of the finite state machine and start execution
void setupStateMachine(){

	// State blink0 is created statically as global variable
    myFSM.addState(blink0);	
	printStateInfo(&blink0);	

	// State blink1 is created dynamically
	State* blink1 = myFSM.addState("Blink1", 1000, 0, onEntering, onLeaving, nullptr);
	printStateInfo(blink1);

	// State blink2 is created statically as global variable
	myFSM.addState(blink2);
	printStateInfo(&blink2);

	// State blink3 is created dynamically
	State* blink3 = myFSM.addState("Blink3", 1000, 0, onEntering, onLeaving, nullptr);
	printStateInfo(blink3);
	
	// Add transitions to the states
	// the transistion used are dinamically created
	blink0.addTransition(blink1, xNextButton);
	blink2.addTransition(blink3, xNextButton);

	// the transistion used are statically created as global variables
	blink1->addTransition(toBlink2);
	blink3->addTransition(toBlink0);

	// Start the Machine State
	myFSM.setInitialState(blink0);
	myFSM.start();
}


void setup() {
	pinMode(NEXT_BUTTON, INPUT_PULLUP);
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
	xNextButton = (digitalRead(NEXT_BUTTON) == LOW);

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
