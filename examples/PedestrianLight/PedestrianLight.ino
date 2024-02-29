#include "AgileStateMachine.h"

const byte BTN_CALL   = 2;
const byte GREEN_LED  = 12;
const byte YELLOW_LED = 11;
const byte RED_LED    = 10;

// Pedestrian traffic light -> green ligth ON until button pressed
const uint32_t YELLOW_TIME = 5000;
const uint32_t RED_TIME    = 10000;
const uint32_t CALL_DELAY  = 5000;

// The Finite State Machine
StateMachine fsm;

// Input/Output State Machine interface
bool inCallButton;
bool outRed, outGreen, outYellow;


/////////// STATE MACHINE FUNCTIONS //////////////////

// This function will be executed before exit the current state
void onEnter() {
	Serial.print(fsm.getActiveStateName());
	Serial.println(F(" light ON"));
}
// Define "on leaving" callback function (the same for all "light"  states)
void onExit() {
	Serial.print(fsm.getActiveStateName());
	Serial.println(F(" light OFF\n"));
}
// Define "on enter" for CALL button state
void onEnterCall() {
	Serial.println(F("Call registered, please wait a little time."));
}


// Definition and modeling of the finite state machine
void setupStateMachine(){
	/* Create states and assign name and callback functions */
	State* stCall = fsm.addState("Call semaphore", onEnter, nullptr, onExit);
	State* stGreen = fsm.addState("Green", onEnter, nullptr, onExit);
	State* stRed = fsm.addState("Red", onEnter, nullptr, onExit);
	State* stYellow = fsm.addState("Yellow", onEnter, nullptr, onExit);

	stGreen->addTransition(stCall, inCallButton);
	stCall->addTransition(stYellow, CALL_DELAY);
	stYellow->addTransition(stRed, YELLOW_TIME);
	stRed->addTransition(stGreen, RED_TIME);

	stRed->addAction(Action::Type::N, outRed);        // N -> while state is active red led is ON
	stGreen->addAction(Action::Type::S, outGreen);    // S -> SET green led on
	stYellow->addAction(Action::Type::R, outGreen);   // R -> RESET the green led
	stYellow->addAction(Action::Type::N, outYellow);  // N -> while state is active yellow led is ON


	/* Set initial state and start the Machine State */
	fsm.setInitialState(stGreen);
	fsm.start();
	Serial.print("Active state: ");
	Serial.println(fsm.getActiveStateName());
	Serial.println();
}


void setup() {
	pinMode(BTN_CALL, INPUT_PULLUP); 
	pinMode(GREEN_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);

	Serial.begin(115200);
	Serial.println("Starting State Machine...\n");
	setupStateMachine();	
}


void loop() {

	// Read inputs
	inCallButton = (digitalRead(BTN_CALL) == LOW);

	// Run State Machine	(true is state changed)
	if (fsm.execute()) {
		Serial.print(F("Active state: "));
		Serial.println(fsm.getActiveStateName());
	}

	// Set outputs
	digitalWrite(RED_LED, outRed);
	digitalWrite(GREEN_LED, outGreen);
	digitalWrite(YELLOW_LED, outYellow);
}


