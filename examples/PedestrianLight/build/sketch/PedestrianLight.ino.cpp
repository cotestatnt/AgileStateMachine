#include <Arduino.h>
#line 1 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
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
#line 24 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
void onEnter();
#line 29 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
void onExit();
#line 34 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
void onEnterCall();
#line 55 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
void setupStateMachine();
#line 82 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
void setup();
#line 94 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
void loop();
#line 24 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
void onEnter() {
	Serial.print(F("Enter on state: "));
	Serial.println(fsm.getActiveStateName());
}
// Define "on leaving" callback function (the same for all "light"  states)
void onExit() {
	Serial.print(F("Exit from state: "));
	Serial.println(fsm.getActiveStateName());
}
// Define "on enter" for CALL button state
void onEnterCall() {
	Serial.println(F("Call registered, please wait a little time."));
}

State stCall("Call semaphore", onEnter, onExit);
State stGreen("Green", onEnter, onExit);
State stRed("Red", onEnter, onExit);
State stYellow("Yellow", onEnter, onExit);

Transition greenTocall(stCall, inCallButton);
Transition callToYellow(stYellow, CALL_DELAY);
Transition yellowToRed(stRed, YELLOW_TIME);
Transition redToGreen(stGreen, RED_TIME);

Action setRedLight(Action::Type::N, outRed);
Action setGreenLight(Action::Type::S, outGreen);
Action resetGreenLight(Action::Type::R, outGreen);
Action setYellowLight(Action::Type::N, outYellow);


// Definition and modeling of the finite state machine
void setupStateMachine(){
	/* Create states and assign name and callback functions */
	fsm.addState(stCall);
	fsm.addState(stGreen);
	fsm.addState(stRed);
	fsm.addState(stYellow);


	stGreen.addTransition(greenTocall);
	stCall.addTransition(callToYellow);
	stYellow.addTransition(yellowToRed);
	stRed.addTransition(redToGreen);

	stRed.addAction(setRedLight);        // N -> while state is active red led is ON
	stGreen.addAction(setGreenLight);    // S -> SET green led on
	stYellow.addAction(resetGreenLight); // R -> RESET the green led
	stYellow.addAction(setYellowLight);  // N -> while state is active yellow led is ON

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
	Serial.println("Starting State Machine...");
	setupStateMachine();
}


void loop() {

	// Read inputs
	inCallButton = (digitalRead(BTN_CALL) == LOW);

	// Run State Machine	(true is state changed)
	if (fsm.execute()) {
		Serial.print(F("Active state: "));
		Serial.println(fsm.getActiveStateName());
		Serial.println();
	}

	// Set outputs
	digitalWrite(RED_LED, outRed);
	digitalWrite(GREEN_LED, outGreen);
	digitalWrite(YELLOW_LED, outYellow);
}



