# 1 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\AutomaticGate\\AutomaticGate.ino"
# 2 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\AutomaticGate\\AutomaticGate.ino" 2

const byte SEC_FTC1 = 26; // Safety photocell
const byte BTN_START = 27; // Gate open request
const byte LED_OPEN = 14;
const byte LED_CLOSE = 12;
const byte LED_FLASHER = 13;

uint32_t openedTime = 10000; // Time the gate remain opened
uint32_t openTime = 10000; // Time needed to complete movement
uint32_t closeTime = 10000; // Time needed to complete movement
uint32_t waitTime = 5000; // Time to wait before restart after safety FTC

// Input/Output State Machine interface
bool inStartButton, inSafetyFTC;
bool outOpen, outClose, outFlashBlink;

// External interrupt for safety photocell

void __attribute__((section(".iram1" "." "0"))) SafetyPhotecellOn() {
 // Check photocell only while the motor is moving.
 if (outOpen || outClose) {
  inSafetyFTC = true;
 }
}
# 35 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\AutomaticGate\\AutomaticGate.ino"
void doBlink(const uint8_t pin, uint32_t interval) {
 static bool level = 0x0;
 static uint32_t bTime;

 if (!interval) {
  digitalWrite(pin, 0x0);
  return;
 }

 if (millis() - bTime >= interval) {
  bTime = millis();
  level = !level;
  return digitalWrite(pin, level);
 }
}

/////////// STATE MACHINE FUNCTIONS //////////////////
// The Finite State Machine definition
StateMachine fsm;

// This function will be executed before exit the current state
void onLeave(){
  Serial0.print("Leaving state: ");
 Serial0.println(fsm.getActiveStateName());
}

// This function will be executed before enter next state
void onEnter(){
 Serial0.print("Entered state: ");
 Serial0.println(fsm.getActiveStateName());
}

void onEnterWait() {
 onEnter();
 inSafetyFTC = false;
 outOpen = false;
 outClose = false;
}

// Possible finite states declaration
State stClosed("Closed", onEnter, onLeave);
State stClosing("Closing", onEnter, onLeave);
State stOpened("Opened", onEnter, onLeave);
State stOpening("Opening", onEnter, onLeave);
State stStopWait("Stop & Wait", onEnterWait, onLeave);

// Transitions between states
Transition closedToOpening(stOpening, inStartButton); // Start with button
Transition openingToOpened(stOpened, openTime); // Keep open output active for openTime
Transition openedToClosing(stClosing, openedTime); // Wait some time before close
Transition closigToClosed(stClosed, closeTime); // Keep cloase output active for closeTime
Transition closigToWait(stStopWait, inSafetyFTC); // If safety FTC triggered, go to wait state
Transition waitToOpening(stOpening, waitTime); // If safety, after some time open again the gate

// Action linked to each state
Action setBlinking(Action::Type::S, outFlashBlink); // Set action
Action clearBlinking(Action::Type::R, outFlashBlink); // Reset action
Action openGate(Action::Type::D, outOpen, 2000); // Delayed set action
Action closeGate(Action::Type::D, outClose, 2000);
Action clearOpen(Action::Type::R, outOpen);
Action clearClose(Action::Type::R, outClose);

// Definition and modeling of the finite state machine
void setupStateMachine(){
 /* Create states and assign name and callback functions */
 fsm.addState(stClosed);
 fsm.addState(stOpening);
 fsm.addState(stOpened);
 fsm.addState(stClosing);
 fsm.addState(stStopWait);

 /* Define transitions to target state and trigger condition (callback function or bool var) */
 stClosed.addTransition(closedToOpening);
 stOpening.addTransition(openingToOpened);
 stOpened.addTransition(openedToClosing);
 stClosing.addTransition(closigToClosed);
 stClosing.addTransition(closigToWait);
 stStopWait.addTransition(waitToOpening);

 /* Add actions for the states where needed */

 // Start blink flasher immediatly
 stOpening.addAction(setBlinking);
 stClosing.addAction(setBlinking);
 stClosed.addAction(clearBlinking);

 // Start gate motor after some delay
 stOpening.addAction(openGate);
 stClosing.addAction(closeGate);

 // Safety handling (only on closing)
 stStopWait.addAction(clearOpen);
 stStopWait.addAction(clearClose);

 /* Set initial state and start the Machine State */
 fsm.setInitialState(stClosed);
 fsm.start();
    Serial0.print("Active state: ");
 Serial0.println(fsm.getActiveStateName());
 Serial0.println();
}


void setup() {
 pinMode(SEC_FTC1, 0x05);
 pinMode(BTN_START, 0x05);
 pinMode(LED_OPEN, 0x03);
    pinMode(LED_CLOSE, 0x03);
    pinMode(LED_FLASHER, 0x03);

 Serial0.begin(115200);
 Serial0.println("Starting State Machine...\n");
 setupStateMachine();

    // External interrupt for safety photocell
    attachInterrupt(((((uint8_t)(SEC_FTC1)) < 40 /* All GPIOs*/) ? (SEC_FTC1) : -1), SafetyPhotecellOn, 0x02);
}


void loop() {
 static uint32_t t0 = millis();

    // Get machine state input
    inStartButton = (digitalRead(BTN_START) == 0x0);

 // Reset timer to have clear debug info about timings
 while (digitalRead(BTN_START) == 0x0) {
  t0 = millis();
 }

 // Run State Machine (true is state changed)
 if (fsm.execute()) {
  Serial0.print(millis() - t0);
  Serial0.print("+ Active state: ");
  Serial0.println(fsm.getActiveStateName());
  Serial0.println();
  t0 = millis();
 }

    // Set outputs according to state
 digitalWrite(LED_OPEN, outOpen);
    digitalWrite(LED_CLOSE, outClose);

 // Keep flasher on during opened wait time (don't blink)
 if (fsm.getCurrentState() == &stOpened) {
  digitalWrite(LED_FLASHER, 0x1);
 }
 // Blink flasher if necessary
 else {
  doBlink(LED_FLASHER, outFlashBlink ? 500 : 0);
 }

 // This is only to increse performance of the simulator
 delay(1);
}
