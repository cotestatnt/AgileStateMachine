#include <AgileStateMachine.h>

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

// State and Transition definitions
State stCall("Call semaphore", onEnter, onExit);
State stGreen("Green", onEnter, onExit);
State stRed("Red", onEnter, onExit);
State stYellow("Yellow", onEnter, onExit);

Transition toCall(stCall, inCallButton);
Transition toYellow(stYellow, CALL_DELAY);
Transition toRed(stRed, YELLOW_TIME);
Transition toGreen(stGreen, RED_TIME);

/*
* In this example Actions are used to set the semaphore lights. 
* The actions are executed only when the related state is active. 
* While red and yellow is active the Action of type N (Non-stored) is used, 
* but for the green state SET/RESET is needed because the transition is 
* greeen->call->yellow and the light must be kept on until the yellow state is enterd.
*/  
Action coilRed(stRed, Action::Type::N, outRed);          // N -> while state is active red led is ON
Action setGreen(stGreen, Action::Type::S, outGreen);     // S -> SET green led on
Action resetGreen(stYellow, Action::Type::R, outGreen);  // R -> RESET the green led
Action coilYellow(stYellow, Action::Type::N, outYellow); // N -> while state is active yellow led is ON


// Definition and modeling of the finite state machine
void setupStateMachine(){
	/* Add states to the FSM */
	fsm.addState(stGreen); 
  fsm.addState(stCall);
  fsm.addState(stRed);
  fsm.addState(stYellow);

  /* Add transitions to each state*/
  stGreen.addTransition(toCall);  
  stCall.addTransition(toYellow);
  stYellow.addTransition(toRed);
  stRed.addTransition(toGreen);

  /* Add actions to states*/
  stGreen.addAction(setGreen);
  stRed.addAction(coilRed);
  stYellow.addAction(resetGreen);
  stYellow.addAction(coilYellow);
  
	/* Start the FSM */
  // fsm.setInitialState(stGreen);
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
