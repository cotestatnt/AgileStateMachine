#include "AgileStateMachine.h"

#define START_BUTTON  4
#define STOP_BUTTON  	5
#define OUT_MOTOR 		13

// Create new Finite State Machine
StateMachine myFSM;

// Input/Output State Machine interface
bool inStart, inStop;
bool outMotor;


/////////// STATE MACHINE FUNCTIONS //////////////////

// Define "on leaving" callback functions
void onLeaving() {
  Serial.print(F("Leaving state: "));
  Serial.println(myFSM.getActiveStateName());
}

// Define "on entering" callback functions (just a message in this example)
void onEntering() {
  Serial.print(F("Entered state: "));
  Serial.println(myFSM.getActiveStateName());
}


// Definition of the model of the finite state machine and start execution
void setupStateMachine() {
  // Create some states and assign name, min time and callback functions
  State* stIdle  = myFSM.addState("IDLE", onEntering, nullptr, onLeaving);
  State* stRun = myFSM.addState("RUN", 5000, onEntering, nullptr, onLeaving);
  State* stStop  = myFSM.addState("STOP", 1000, onEntering, nullptr, onLeaving);
  

  // Add transitions to target state and trigger condition 
  stIdle->addTransition(stRun, inStart);		
  stRun->addTransition(stStop, inStop);		
  stStop->addTransition(stIdle, 1000);	// Wait some time to be shure motor is stopped
 
	// Add actions to state
	stRun->addAction(Action::Type::S, outMotor);	
	stStop->addAction(Action::Type::R, outMotor);		

  // Start the Machine State
  myFSM.setInitialState(stIdle);
  myFSM.start();
}


void setup() {
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(STOP_BUTTON, INPUT_PULLUP);
  pinMode(OUT_MOTOR, OUTPUT);

  Serial.begin(115200);
  Serial.println(F("\n\nStarting State Machine...\n"));
  setupStateMachine();

  // Initial state
  Serial.print(F("Active state: "));
  Serial.println(myFSM.getActiveStateName());
  Serial.println();
}


void loop() {

  // Read reset button input and update resetBlinky variable
  inStart = (digitalRead(START_BUTTON) == LOW) && !outMotor;
	inStop = (digitalRead(STOP_BUTTON) == LOW) && outMotor;

  // Update State Machine	(true is state changed)
  if (myFSM.execute()) {
    Serial.println();
  }

  digitalWrite(OUT_MOTOR, outMotor);
}
