
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
	// Default constructor
	StateMachine(){};

	State* addState(const char *name);
	State* addState(const char *name, uint32_t min, uint32_t max);
	State* addState(const char *name, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr);
	State* addState(const char *name, uint32_t min = 0, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr);
	State* addState(const char *name, uint32_t min, uint32_t max, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr);

	void setCurrentState(State *newState, bool callOnEntering = true, bool callOnLeaving = true);

	void setInitialState(State* state) { m_currentState = state;}

	void start();

	void stop();

	const int GetStatesNumber();

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
