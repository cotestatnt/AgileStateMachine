#include "AgileStateMachine.h"
#include <Servo.h>

#define SIG_TRAIN_IN      3
#define SIG_TRAIN_OUT     2
#define SERVO_PIN         6
#define STOP_PASS         11
#define FREE_PASS         12
#define OUT_LIGHT_BELL    13

// After the train has passed, wait a little time to be sure
#define MOVE_TIME         1500   // Time needed for GATE moving (up and down)
#define WAIT_FREE_TIME    10000  // Wait time after train has gone (in case another train is arriving)
#define BLINK_TIME 250

#define CLOSE_POSITION  0
#define OPEN_POSITION   90

// Handle the gate position
Servo theGate;
uint16_t servoPos = OPEN_POSITION ;

// The Finite State Machine
StateMachine fsm;

// Input State Machine interface
bool inTrainArrive, inTrainGone;

// The finite state
State* stGateOpen;    // The rail crossing gate is opened (free entry)
State* stGateClose;   // The rail crossing gate is closed (stop entry)
State* stMoveUp;      // The rail crossing gate is moving up (free)
State* stMoveDown;    // The rail crossing gate is moving down (stop)
State* stWaitTrain;   // Wait if another train is passing before move up the gate

void runServoSlow() {
  static uint8_t actualPos;
  static uint32_t stepTime;

  if (millis() - stepTime > 10) {
    stepTime = millis();
    if (actualPos < servoPos)
      actualPos++;
    if (actualPos > servoPos)
      actualPos--;

    theGate.write(actualPos);
  }
}


/////////// STATE MACHINE FUNCTIONS //////////////////

// This function will be executed before enter next state
void onEnter() {
  if (fsm.getCurrentState() == stGateClose ) {
    Serial.println(F("The GATE is actually close"));
  }
  else if (fsm.getCurrentState() == stMoveUp ) {
    servoPos = OPEN_POSITION;
    Serial.println(F("The GATE is going to be opened"));
  }
  else if (fsm.getCurrentState() == stGateOpen ) {
    digitalWrite(STOP_PASS, LOW);
    digitalWrite(FREE_PASS, HIGH);
    digitalWrite(OUT_LIGHT_BELL, LOW);
    Serial.println(F("The GATE is actually open"));
  }
  else if (fsm.getCurrentState() == stMoveDown ) {
    servoPos = CLOSE_POSITION;
    digitalWrite(STOP_PASS, HIGH);
    digitalWrite(FREE_PASS, LOW);
    Serial.println(F("A new train is coming! Start closing the GATE."));
    Serial.println(F("The GATE is going to be closed"));
  }
  else if (fsm.getCurrentState() == stWaitTrain ) {
    delay(1);
    Serial.println(F("Train passed, but we have to wait a little time more"));
  }
}

// Blink and play the bell while gate is moving or closed
void bewareOfTrains() {
  static bool level = LOW;
  static uint32_t bTime;

  if (millis() - bTime >= BLINK_TIME ) {
    level = !level;
    bTime = millis();
    digitalWrite(OUT_LIGHT_BELL, level);
  }
}


// Definition and modeling of the finite state machine
void setupStateMachine() {
  /* Create states and assign name and callback functions */
  //                           name, minTime, onEnter cb, onRun cb, onExit cb
  stGateOpen   = fsm.addState("Gate OPEN", onEnter, nullptr, nullptr);
  stGateClose  = fsm.addState("Gate CLOSE", onEnter, bewareOfTrains, nullptr);
  stMoveDown   = fsm.addState("Move gate DOWN", onEnter, bewareOfTrains, nullptr);
  stMoveUp     = fsm.addState("Move gate UP", onEnter, bewareOfTrains, nullptr);
  stWaitTrain  = fsm.addState("Wait Train", onEnter, bewareOfTrains, nullptr);

  stGateOpen->addTransition(stMoveDown, inTrainArrive);
  stGateClose->addTransition(stWaitTrain, inTrainGone);
  stMoveDown->addTransition(stGateClose, MOVE_TIME);
  stMoveUp->addTransition(stGateOpen, MOVE_TIME);
  stWaitTrain->addTransition(stMoveUp, WAIT_FREE_TIME);

  /* Set initial state and start the Machine State */
  fsm.setInitialState(stGateOpen);
  fsm.start();
  Serial.print("Active state: ");
  Serial.println(fsm.getActiveStateName());
  Serial.println();
  
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting State Machine...\n");
  setupStateMachine();
  // Input/Output configuration
  pinMode(SIG_TRAIN_IN, INPUT_PULLUP);
  pinMode(SIG_TRAIN_OUT, INPUT_PULLUP);
  pinMode(OUT_LIGHT_BELL, OUTPUT);
  pinMode(STOP_PASS, OUTPUT);
  pinMode(FREE_PASS, OUTPUT);
  digitalWrite(FREE_PASS, HIGH);
  theGate.attach(SERVO_PIN);
}


void loop() {
  // Smoot run servo to taret position
  runServoSlow();

  // Update the input variables according to the signal inputs
  inTrainGone = digitalRead(SIG_TRAIN_OUT) == LOW;
  inTrainArrive = digitalRead(SIG_TRAIN_IN) == LOW;

  // Reset enter time so timeout became longer enough
  // to wait two ore more trains one after the other
  if (inTrainArrive && fsm.getCurrentState() == stWaitTrain) {
    stWaitTrain->resetEnterTime();

    static uint32_t pTime;
    if (millis() - pTime > 500) {
      pTime = millis();
      Serial.println(F("Another train is arriving, wait more time!"));
    }
  }

  // Run State Machine
  // Outputs will be handled inside onEnter callback function
  fsm.execute();
}


