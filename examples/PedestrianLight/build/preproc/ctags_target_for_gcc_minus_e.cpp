# 1 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
# 2 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 2

const byte BTN_CALL = 2;
const byte GREEN_LED = 12;
const byte YELLOW_LED = 11;
const byte RED_LED = 10;

// Pedestrian traffic light -> green ligth ON until button pressed
const uint32_t YELLOW_TIME = 5000;
const uint32_t RED_TIME = 10000;
const uint32_t CALL_DELAY = 5000;

// The Finite State Machine
StateMachine fsm;

// Input/Output State Machine interface
bool inCallButton;
bool outRed, outGreen, outYellow;


/////////// STATE MACHINE FUNCTIONS //////////////////

// This function will be executed before exit the current state
void onEnter() {
 Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 25 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
             (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 25 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
             "Enter on state: "
# 25 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
             ); &__c[0];}))
# 25 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
             )));
 Serial.println(fsm.getActiveStateName());
}
// Define "on leaving" callback function (the same for all "light"  states)
void onExit() {
 Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 30 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
             (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 30 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
             "Exit from state: "
# 30 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
             ); &__c[0];}))
# 30 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
             )));
 Serial.println(fsm.getActiveStateName());
}
// Define "on enter" for CALL button state
void onEnterCall() {
 Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 35 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
               (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 35 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
               "Call registered, please wait a little time."
# 35 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
               ); &__c[0];}))
# 35 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
               )));
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

 stRed.addAction(setRedLight); // N -> while state is active red led is ON
 stGreen.addAction(setGreenLight); // S -> SET green led on
 stYellow.addAction(resetGreenLight); // R -> RESET the green led
 stYellow.addAction(setYellowLight); // N -> while state is active yellow led is ON

 /* Set initial state and start the Machine State */
 fsm.setInitialState(stGreen);
 fsm.start();
 Serial.print("Active state: ");
 Serial.println(fsm.getActiveStateName());
 Serial.println();
}


void setup() {
 pinMode(BTN_CALL, 0x2);
 pinMode(GREEN_LED, 0x1);
 pinMode(YELLOW_LED, 0x1);
 pinMode(RED_LED, 0x1);

 Serial.begin(115200);
 Serial.println("Starting State Machine...");
 setupStateMachine();
}


void loop() {

 // Read inputs
 inCallButton = (digitalRead(BTN_CALL) == 0x0);

 // Run State Machine	(true is state changed)
 if (fsm.execute()) {
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 101 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 101 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
              "Active state: "
# 101 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino" 3
              ); &__c[0];}))
# 101 "C:\\Dati\\Cloud\\Dropbox\\ArduinoLibs\\AgileStateMachine\\examples\\PedestrianLight\\PedestrianLight.ino"
              )));
  Serial.println(fsm.getActiveStateName());
  Serial.println();
 }

 // Set outputs
 digitalWrite(RED_LED, outRed);
 digitalWrite(GREEN_LED, outGreen);
 digitalWrite(YELLOW_LED, outYellow);
}
