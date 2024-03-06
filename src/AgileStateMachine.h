
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

/// @brief
class StateMachine
{
public:
	// Default constructor/destructor
	StateMachine(){};
	~StateMachine(){};

	// Add a new state to the list of states
	template <typename T>
	State *addState(T name, uint32_t min, uint32_t max, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr)
	{
		State *state = new State(name, min, max, enter, exit, run);
		state->setIndex(m_states.size());
		m_states.append(state);
		m_currentState = state;
		return state;
	}

	template <typename T>
	State *addState(T name)
	{
		return addState(name, 0, 0, nullptr, nullptr, nullptr);
	}

	template <typename T>
	State *addState(T name, uint32_t min, uint32_t max)
	{
		return addState(name, min, max, nullptr, nullptr, nullptr);
	}

	template <typename T>
	State *addState(T name, uint32_t min, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr)
	{
		return addState(name, min, 0, enter, exit, run);
	}

	template <typename T>
	State *addState(T name, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr)
	{
		return addState(name, 0, 0, enter, exit, run);
	}

	void addState(State &state);

	// Force to the specific state the State Machine
	void setCurrentState(State *newState, bool callOnEntering = true, bool callOnLeaving = true);

	// Sets the initial state.
	void setInitialState(State *state) { m_currentState = state; }

	// Start the State Machine
	void start();

	// Stop the State Machine
	void stop();

	// Returns the numbers of states added to State Machine
	int GetStatesNumber();

	// Returns the name of the currently active state as const char*
	const char *getActiveStateName()
	{
		return m_currentState->getStateName();
	}

	// Returns the name of the currently active state as pointer to flash string helper - F() macro
	const __FlashStringHelper *getActiveStateName_P()
	{
		return m_currentState->getStateName_P();
	}

	// Return information about the current state of the database. This is a pointer to the pager state
	State *getCurrentState();

	// Run the state machine
	bool execute();

	// Return the last enter time in nanoseconds
	uint32_t getLastEnterTime();

private:
	friend class Action;
	friend class State;
	friend class Transition;

	bool m_started = false;
	State *m_currentState = nullptr;
	LinkedList<State *> m_states;
};

#endif
