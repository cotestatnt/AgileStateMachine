# AgileStateMachine
Arduino/C++ library to simply and efficiently model and run a finite state machine.

___
### Introduction

States represent the different situations in which the machine can be at any time. The transitions connect two states, and are associated with a trigger condition which carries out the change of state (if you take a look at the examples included, you will find some different way for triggering a transition).

To update the machine, you must call the `execute()` function in your `loop()`, which checks for transitions that have the current state as input and associated conditions.

If any of the transitions associated with the current state satisfy the trigger condition, the machine goes into the next state.

```cpp
/* The Finite State Machine */
StateMachine fsm;

```

![SFC_esempio](https://user-images.githubusercontent.com/27758688/125982036-0eab0bb2-ed13-4101-af5c-6e49e82908fd.png)

### State definition and Callback functions
Each of the states of the machine can be binded with a callback function that will be executed when the state is activated (on enter), when it is left (on exit) and while it is running (on run). 

For each state it is also possible to define an optional maximum and a minimum duration time.

If a callback function is not needed, simply use value `nullptr` or `NULL` instead of the name of function.

```cpp
stExampleState->addTransition(stAnotherState, aCallbackFunction);    // A callback function will be used as trigger
stAnotherState->addTransition(stAnotherState, aBoolVariable);        // A bool variable will be used as trigger
stAnotherState->addTransition(stGateClose, aTime);                   // An unsigned long variable will be used as timeout trigger

/* Set initial state and start the Machine State */
fsm.setInitialState(stExampleState);
fsm.start();
```

### Transition definition and trigger
To connect two states, you need to define the transition. The trigger of transition can be performed with a `void function(bool &)` or a `bool` variable. 
Also the timeout of state itself can be used for triggering to next state. 

```cpp
stExampleState->addTransition(stAnotherState, aCallbackFunction);    // A callback function will be used as trigger
stAnotherState->addTransition(stAnotherState, aBoolVariable);        // A bool variable will be used as trigger
stAnotherState->addTransition(stGateClose, aTime);                   // An unsigned long variable will be used as timeout trigger
```

You can also check timeout for a specific state with the method `bool getTimeout();`

``` cpp
if(fsm.getCurrentState()->getTimeout) {....}
```

### Action definition
For each state you can define also a set of qualified action, that will be executed when state is active causing effect to the target bool variable

```cpp
bool targetVar1, targetVar2;
stExampleState->addAction(Action::Type::N, targetVar1);
stAnotherState->addAction(Action::Type::R, targetVar2);
```

The library actually support this action qualifiers:

| Action qualifier | Description | Short explanation on the effect | Duration |
| :---: | :---: | :--- | :---: |
| N | Non-stored | Action is active (target = TRUE) as long as the state input is active | NO |
| R | Reset | Reset to FALSE the value of target variable | NO |
| S | Set (or Stored) | Set to TRUE the value of target variable | NO |
| L | time Limited | target = TRUE until the end of the set time or until the state is deactivated  | YES |
| D | time Delayed | target = TRUE  after the set time has elapsed until the state is deactivated  | YES |

### Examples

Take a look at the examples provided in the [examples folder](https://github.com/cotestatnt/YA_FSM/tree/master/examples).
Start from the simplest [Blinky.ino](https://github.com/cotestatnt/YA_FSM/blob/master/examples/Blinky) or [PedestrianLight.ino](https://github.com/cotestatnt/YA_FSM/tree/master/examples/PedestrianLight)


or a more advanced like classic algorithm for opening an automatic gate (simplified) [AutomaticGate](https://github.com/cotestatnt/YA_FSM/blob/master/examples/AutomaticGate) or rail crossing [RailCRossing.ino](https://github.com/cotestatnt/YA_FSM/blob/master/examples/RailCrossing)

<div style="content: flex">
<img src="https://user-images.githubusercontent.com/27758688/125971825-ed89b51d-6441-474b-86a5-063bc1f96770.png" width="480">
<img src="https://user-images.githubusercontent.com/27758688/125979422-31ad3912-eb93-406f-9b41-6f32c30b02ba.png" width="450">
</div>


### Constructor

```c++
// Create new Finite State Machine
StateMachine fsm;

```
### General methods
```c++

// Get current state index
uint8_t GetState() const;

// Get pointer to current state object
FSM_State* CurrentState();

// Get pointer to a specific state object passing the index
FSM_State*  GetStateAt(uint8_t index);

// Get active state name
const char* ActiveStateName();

// Get the number of defined finite states
const int GetNumStates()

// Set machine to state at index. Will call every function as expected unless told otherwise
// If is the same state as the current, it should call onEntering and onLeaving and Refresh Timeout
void SetState (uint8_t index, bool callOnEntering = true, bool callOnLeaving = true)
  
// Set or modify a state timeout (preset = milliseconds)
void SetTimeout(uint8_t index, uint32_t preset);

// Check if a state is timeouted
bool GetTimeout(uint8_t index);		// deprecated
bool Timeout(uint8_t index);		// More clear method name
stateMachine.CurrentState()->timeout	// Access directly to the value stored in FSM_State struct

// Get the time (milliseconds) when state was activated
uint32_t GetEnteringTime(uint8_t index) 
	
// Update state machine. Run in loop()
bool Update();

// Set up a transition and trigger input callback function (or bool variable)
void AddTransition(uint8_t inputState, uint8_t outputState, condition_cb condition);
void AddTransition(uint8_t inputState, uint8_t outputState, bool condition);

// Set up a timed transition
void AddTransition(uint8_t inputState, uint8_t outputState);
void AddTimedTransition(uint8_t inputState, uint8_t outputState);

// Set up and action for a specific state (supported qualifiers N, S, R, D, L
// More actions can be added to the same state (actaully limited by #define MAX_ACTIONS 64)
uint8_t AddAction(uint8_t inputState, uint8_t type, bool &target, uint32_t _time=0);

// Set up a state with properties and callback function
uint8_t AddState(const char* name, uint32_t maxTime, uint32_t minTime,	action_cb onEntering, action_cb onState, action_cb onLeaving);
uint8_t AddState(const char* name, uint32_t maxTime, action_cb onEntering, action_cb onState, action_cb onLeaving);
	
------- OLD VERSION  -------
// Configure input and output and run actions of a state
void SetOnEntering(uint8_t index, action_cb action);
void SetOnLeaving(uint8_t index, action_cb action);
void SetOnState(uint8_t index, action_cb action, uint32_t setTimeout = 0)   // 0 disabled

void ClearOnEntering(uint8_t index);
void ClearOnLeaving(uint8_t index);
void ClearOnState(uint8_t index);
```

### Supported boards
The library works virtually with every boards supported by Arduino framework (no hardware dependency)

+ 1.0.0 Initial version

