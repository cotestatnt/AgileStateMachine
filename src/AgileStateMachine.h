
/*
	Cotesta Tolentino, 2020.
	Released into the public domain.
*/
#ifndef AGILE_STATE_MACHINE_H
#define AGILE_STATE_MACHINE_H
#include "Arduino.h"
#include "LinkedList.h"
#include "State.h"


using state_cb = void (*)();

class StateMachine
{
public:
	// Default constructor/destructor
	StateMachine(){};
	~StateMachine(){};

	State* addState(const char *name, uint32_t min, uint32_t max, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr);

	State* addState(const char *name) {
		return addState(name, 0, 0, nullptr, nullptr, nullptr);
	}

	State* addState(const char *name, uint32_t min, uint32_t max) {
		return addState(name, min, max, nullptr, nullptr, nullptr);
	}

	State* addState(const char *name, uint32_t min, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr) {
		return addState(name, min, 0, enter, exit, run);
	}

	State* addState(const char *name, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr) {
		return addState(name, 0, 0, enter, exit, run);
	}

	void setCurrentState(State *newState, bool callOnEntering = true, bool callOnLeaving = true);

	void setInitialState(State* state) { m_currentState = state;}

	void start();

	void stop();

	int GetStatesNumber();

	const char *getActiveStateName();

	State* getCurrentState();

	bool execute();

	uint32_t getLastEnterTime();


private:
	friend class Action;
	friend class State;
	friend class Transition;

	bool     m_started		= false;
	State 	*m_currentState = nullptr;
	LinkedList<State *> m_states;
};

#endif
