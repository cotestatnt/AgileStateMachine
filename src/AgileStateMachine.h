
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

	// Run the state machine
	bool execute();

	/**
     * Add a static/global state to the state machine
     * Ideal for statically defined states
     * 
     * @param state Reference to the statically defined state
     * @return Pointer to the added state (same address as passed)
     */
    State* addState(State& state) {
		// Set the index of the state and add to list
		state.setIndex(m_states.size());
		State* statePtr = m_states.addReference(state);

		// If it's the first state, set it as current
		if (m_states.size() == 1) {  
			m_currentState = statePtr;
		}
        return statePtr;
    }

	/**
     * Create dynamically a state
     * The StateMachine takes ownership of the memory
     * 
     * @param state Pointer to the state allocated with new
     * @return The same pointer passed as parameter
     */
	template <typename T>
    State* addState(	T name, 
						uint32_t min, 
						uint32_t max, 
						state_cb enter = nullptr, 
						state_cb exit = nullptr, 
						state_cb run = nullptr) 
	{
		State* state = new State(name, min, max, enter, exit, run);
		state->setIndex(m_states.size());
		// If it's the first state, set it as current		
		if (m_states.size() == 1) {
			m_currentState = state;
		}
		// Add the state to the list and return it
        return m_states.addOwned(state);
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

	
	/**
     * Add a copy of a state to the state machine
     * Ideal for locally defined states (e.g., in a function)
     * The StateMachine will manage the memory
     * 
     * @param state Reference to the state to copy
     * @return Pointer to the copied state (address internal to the list)
     */
    State* addState(const State& state) {
		State* statePtr = m_states.addCopy(state);

		// If it's the first state, set it as current
		if (m_states.size() == 1) {  
			m_currentState = statePtr;
		}
		// Set the index of the state
		updateStateIndex(statePtr, m_states.size());
        return statePtr;
    }

	// Force to the specific state the State Machine
	void setCurrentState(State *newState, bool callOnEntering = true, bool callOnLeaving = true);

	// Sets the initial state.
	void setInitialState(State *state) { m_currentState = state; }
	void setInitialState(State &state) { m_currentState = &state; }
	
	// Start the State Machine
	inline void StateMachine::start() {
		m_started = true;
	}

	// Stop the State Machine
	inline void StateMachine::stop() {
		m_started = false;
	}

	// Returns the numbers of states added to State Machine
	inline int getStatesNumber() {
		return m_states.size();
	}

	// Returns the name of the currently active state as const char*
	inline const char *getActiveStateName()
	{
		return m_currentState->getStateName();
	}

	// Returns the name of the currently active state as pointer to flash string helper - F() macro
	inline const __FlashStringHelper *getActiveStateName_P()
	{
		return m_currentState->getStateName_P();
	}

	// Return information about the current state of the database. This is a pointer to the pager state
	inline State *getCurrentState() {
		return m_currentState;
	}

	// Return the current state of the database. This is a pointer to the pager state
	inline bool isCurrentState(State &state)
	{
		return m_currentState == &state;
	}

	// Return the last enter time in nanoseconds
	inline uint32_t getLastEnterTime(){
		return m_currentState->getEnterTime();
	}
	



private:
	friend class Action;
	friend class State;
	friend class Transition;

	bool m_started = false;
	State *m_currentState = nullptr;
	LinkedList<State> m_states;

	bool updateStateIndex(State *state, uint8_t index);
};

#endif





