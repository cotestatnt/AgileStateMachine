/*
* In this example, we use a servo motor to simulate a gate that opens and closes
* when a train arrives or leaves. The gate is controlled by a servo motor, and
* the state machine manages the gate's states: Open, Closed, Moving Up, Moving Down,
* and Waiting for the next train.
* The state machine transitions between these states based on the signals from  
* the train sensors. The gate opens when a train leaves and closes when a train arrives.
* The gate also has a waiting state to ensure it doesn't open and close too quickly
* when multiple trains are arriving or leaving in quick succession.
* The code uses the AgileStateMachine library to manage the state machine and 
* transitions between states. The servo motor is controlled using the Servo library.
* The code also includes a blinking light and bell sound to alert people when a train is approaching.
*/
#include <AgileStateMachine.h>
#include <Servo.h>

// Define the gpio pins for the signals and outputs
#define SIG_TRAIN_IN      3
#define SIG_TRAIN_OUT     2
#define SERVO_PIN         6
#define STOP_PASS         11
#define FREE_PASS         12
#define OUT_LIGHT_BELL    13

// Define the timeouts and delays
#define MOVE_TIME         1500   // Time needed for GATE moving (up and down)
#define WAIT_FREE_TIME    10000  // Wait time after train has gone (in case another train is arriving)
#define BLINK_TIME        250    // Blink time for light and bell

// Servo positions
#define CLOSE_POSITION  0
#define OPEN_POSITION   90

// Handle the gate position
Servo theGate;
uint16_t servoPos = OPEN_POSITION ;

// The Finite State Machine
StateMachine fsm;

// Input State Machine interface signals
bool inTrainArrive, inTrainGone;

// Function to run the servo motor slowly to the target position
void runServoSlow() {
  static uint8_t currentPos = theGate.read();
  static uint32_t lastUpdate = 0;

  if (millis() - lastUpdate >= 15) {
    lastUpdate = millis();
    if (theGate.read() != servoPos) {
      currentPos += (currentPos < servoPos) ? 1 : -1;
      theGate.write(currentPos);
    }
  }
}


/////////// STATE MACHINE FUNCTIONS //////////////////

// Callback functions to be called when entering Closed state
void onEnterGateClosed() {
  Serial.println(F("The GATE is actually closed"));
}
// Callback functions to be called when entering Opened state
void onEnterGateOpened() {
  digitalWrite(STOP_PASS, LOW);
  digitalWrite(FREE_PASS, HIGH);
  digitalWrite(OUT_LIGHT_BELL, LOW);
  Serial.println(F("The GATE is actually opened"));
}
// Callback functions to be called when entering MoveUp state
void onEnterMoveUp() {
  servoPos = OPEN_POSITION;
  Serial.println(F("The GATE is going to be opened"));
}
// Callback functions to be called when entering MoveDown state
void onEnterMoveDown() {
  servoPos = CLOSE_POSITION;
  digitalWrite(STOP_PASS, HIGH);
  digitalWrite(FREE_PASS, LOW);
  Serial.println(F("A new train is coming! Start closing the GATE."));
  Serial.println(F("The GATE is going to be closed"));
}
// Callback functions to be called when entering Wait state
void onEnterWaitTrain() {
  Serial.println(F("Train passed, but we have to wait a little time more"));
}

// Callback functions to be called while states are running 
// in order to blink light and play the bell.
void bewareOfTrains() {
  static bool level = LOW;
  static uint32_t bTime;

  if (millis() - bTime >= BLINK_TIME ) {
    level = !level;
    bTime = millis();
    digitalWrite(OUT_LIGHT_BELL, level);
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Static states definition as global variables 
State stGateOpened("Gate OPEN", onEnterGateOpened);
State stGateClosed("Gate CLOSE", onEnterGateClosed, nullptr, bewareOfTrains);
State stMoveDown("Move gate DOWN", onEnterMoveDown, nullptr, bewareOfTrains);
State stMoveUp("Move gate UP", onEnterMoveUp, nullptr, bewareOfTrains);
State stWaitTrain("Wait Train", onEnterWaitTrain, nullptr, bewareOfTrains);

// Definition and modeling of the finite state machine
void setupStateMachine() {
  /* Create states and assign name and callback functions */
  fsm.addState(stGateOpened);
  fsm.addState(stGateClosed);
  fsm.addState(stMoveDown);
  fsm.addState(stMoveUp);
  fsm.addState(stWaitTrain);

  /* Add transitions between states */
  stGateOpened.addTransition(stMoveDown, inTrainArrive);
  stGateClosed.addTransition(stWaitTrain, inTrainGone);
  stMoveDown.addTransition(stGateClosed, MOVE_TIME);
  stMoveUp.addTransition(stGateOpened, MOVE_TIME);
  stWaitTrain.addTransition(stMoveUp, WAIT_FREE_TIME);

  /* Set initial state and start the Machine State */
  fsm.setInitialState(stGateOpened);
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

  // Servo configuration
  theGate.attach(SERVO_PIN);
}


void loop() {
  // Smoot run servo to target position
  runServoSlow();

  // Update the input variables according to the signal inputs
  inTrainGone = (digitalRead(SIG_TRAIN_OUT) == LOW);
  inTrainArrive = (digitalRead(SIG_TRAIN_IN) == LOW);

  // Reset enter time so timeout became longer enough
  // to wait two ore more trains one after the other
  if (inTrainArrive && fsm.isCurrentState(stWaitTrain)) {
    stWaitTrain.resetEnterTime();

    static uint32_t pTime;
    if (millis() - pTime > 500) {
      pTime = millis();
      Serial.println(F("Another train is arriving, wait more time!"));
    }
  }

  // Run State Machine
  // Outputs will be handled inside onEnter callback functions
  fsm.execute();
}


