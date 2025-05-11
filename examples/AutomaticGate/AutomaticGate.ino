#include <AgileStateMachine.h>

const byte BTN_START = 2; // Gate open request
const byte SEC_FTC1 = 3;  // Safety photocell
const byte LED_OPEN = 11;
const byte LED_CLOSE = 12;
const byte LED_FLASHER = 13;

uint32_t openedDelay = 10000; // Time the gate remain opened
uint32_t openTime = 8000;     // Time needed to complete movement
uint32_t closeTime = 8000;    // Time needed to complete movement
uint32_t waitTime = 3000;     // Time to wait before restart after safety FTC

// The Finite State Machine
StateMachine fsm;

// Input/Output State Machine interface
bool inStartButton, inSafetyFTC, inResetGate;
bool outOpen, outClose, outFlashBlink;

/////////// STATE MACHINE FUNCTIONS //////////////////

// This function will be executed before exit the current state
void onLeaving() {
  Serial.print("Leaving state: ");
  Serial.println(fsm.getActiveStateName());
}

// This function will be executed before enter next state
void onEntering() {
  Serial.print("Entered state: ");
  Serial.println(fsm.getActiveStateName());
}

// State machine states
State stClosed("Closed", onEntering, onLeaving, nullptr);
State stClosing("Closing", onEntering, onLeaving, nullptr);
State stOpened("Opened", onEntering, onLeaving, nullptr);
State stOpening("Opening", onEntering, onLeaving, nullptr);
State stStopWait("Stop & Wait", waitTime, onEntering, onLeaving, nullptr);

// Transitions between states
Transition toOpening(stOpening, inStartButton);
Transition toOpened(stOpened, openTime);
Transition toClosing(stClosing, openedDelay);
Transition toClosed(stClosed, closeTime);
Transition toStopWait(stStopWait, inSafetyFTC);
Transition toOpeningWait(stOpening, inResetGate);

// Action declarations
// Start blink flasher immediatly
Action actFlashOpening(stOpening, Action::Type::S, outFlashBlink);
Action actFlashClosing(stClosing, Action::Type::S, outFlashBlink);
Action actFlashOpened(stOpened, Action::Type::R, outFlashBlink);
Action actFlashClosed(stClosed, Action::Type::R, outFlashBlink);
// Start gate motor after some delay
Action actGateOpenDelayed(stOpening, Action::Type::D, outOpen, 2000);
Action actGateCloseDelayed(stClosing, Action::Type::D, outClose, 2000);
// Safety handling (only on closing)
Action actStopWaitResetOpen(stStopWait, Action::Type::R, outOpen);
Action actStopWaitResetClose(stStopWait, Action::Type::R, outClose);
Action actStopWaitResetSafety(stStopWait, Action::Type::R, inSafetyFTC);

// External interrupt for safety photocell
void SafetyPhotecellOn() {
  // Check photocell only while the motor is moving.
  if (outOpen || outClose){
    inSafetyFTC = true;
  }
}

// Definition and modeling of the finite state machine
void setupStateMachine() {
  /* Create states and assign name and callback functions */
  fsm.addState(stClosed);
  fsm.addState(stClosing);
  fsm.addState(stOpened);
  fsm.addState(stOpening);
  fsm.addState(stStopWait);

  /* Configure transitions to target state and trigger condition (callback function or bool var) */
  stClosed.addTransition(toOpening);
  stOpening.addTransition(toOpened);
  stOpened.addTransition(toClosing);
  stClosing.addTransition(toClosed);
  stClosing.addTransition(toStopWait);
  stStopWait.addTransition(toOpeningWait);

  /* Configure actions to be executed on each state */
  stOpening.addAction(actFlashOpening);
  stClosing.addAction(actFlashClosing);
  stOpened.addAction(actFlashOpened);
  stClosed.addAction(actFlashClosed);
  stOpening.addAction(actGateOpenDelayed);
  stClosing.addAction(actGateCloseDelayed);
  stStopWait.addAction(actStopWaitResetOpen);
  stStopWait.addAction(actStopWaitResetClose);
  stStopWait.addAction(actStopWaitResetSafety);

  /* Set initial state and start the Machine State */
  fsm.setInitialState(stClosed);
  fsm.start();
  Serial.print("Active state: ");
  Serial.println(fsm.getActiveStateName());
  Serial.println();
}

void setup() {
  pinMode(SEC_FTC1, INPUT_PULLUP);
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(LED_OPEN, OUTPUT);
  pinMode(LED_CLOSE, OUTPUT);
  pinMode(LED_FLASHER, OUTPUT);

  Serial.begin(115200);
  Serial.println("Starting State Machine...\n");
  setupStateMachine();

  // External interrupt for safety photocell
  attachInterrupt(digitalPinToInterrupt(SEC_FTC1), SafetyPhotecellOn, FALLING);
}

void loop() {

  // Get machine state inputs
  inStartButton = (digitalRead(BTN_START) == LOW);
  inResetGate = fsm.isCurrentState(stStopWait) && inStartButton;

  // Run State Machine	(true is state changed)
  if (fsm.execute()) {
    Serial.println();
  }

  // Set outputs according to state
  digitalWrite(LED_OPEN, outOpen);
  digitalWrite(LED_CLOSE, outClose);

  // Blink flasher
  if (outFlashBlink || fsm.isCurrentState(stStopWait)) {
    static bool level = LOW;
    static uint32_t bTime;
    if (millis() - bTime >= 500){
      level = !level;
      bTime = millis();
      digitalWrite(LED_FLASHER, level);
    }
  }
  else if (fsm.isCurrentState(stOpened)) {
    digitalWrite(LED_FLASHER, HIGH);
    delay(1);
  }
  else {
    digitalWrite(LED_FLASHER, LOW);
    delay(1);
  }
}
