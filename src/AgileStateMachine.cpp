#include "AgileStateMachine.h"


State* StateMachine::addState(const char *name)
{
	return addState(name, 0, 0, nullptr, nullptr, nullptr);
}

State* StateMachine::addState(const char *name, uint32_t min, uint32_t max) {
	return addState(name, min, max, nullptr, nullptr, nullptr);
}

State* StateMachine::addState(const char *name, uint32_t min, state_cb enter, state_cb run, state_cb exit)
{
	return addState(name, min, 0, enter, run, exit);
}

State* StateMachine::addState(const char *name, state_cb onEntering, state_cb onState, state_cb onLeaving)
{
	return addState(name, 0, 0, onEntering, onState, onLeaving);
}

State* StateMachine::addState(const char *name, uint32_t min, uint32_t max, state_cb enter, state_cb run, state_cb exit)
{
	State *state = new State(name, min, max, enter, run, exit);

	state->setIndex(m_states.size());
	m_states.add(state);
	m_currentState = state;

	return state;
}

void StateMachine::start() {
	State *state = nullptr;
	for(int i = 0; i < m_states.size(); i++){
		state = m_states.get(i);
		state->clearTransitions();
	}
	m_started = true;
}


void StateMachine::stop() {
	m_started = false;
}


bool StateMachine::execute() {

	if (!m_started) {
		return false;
	}

	State *state = nullptr;
	for(int i = 0; i < m_states.size(); i++){
		state = m_states.get(i);
		// delayMicroseconds(10);

		if (state == m_currentState) {

			if (m_currentState->m_minTime > 0) {
				if (millis() - m_currentState->m_enterTime < m_currentState->m_minTime)	{
					return false;
				}
			}

			// Check triggers for current state
			m_nextState = state->runTransitions();

			// One of the transitions has triggered, set the new state
			if (m_nextState != nullptr) {

				// Clear the actions (N, D and L) before exit actual state
				if (m_currentState->getActions()){
					m_currentState->clearActions();
				}

				// Call current state OnLeaving() callback function
				if (m_currentState->m_onLeaving != nullptr) {
					m_currentState->m_onLeaving();
				}

				// Set new state
				m_currentState = m_nextState;
				m_currentState->m_enterTime = millis();
				m_currentState->m_timeout = false;

				// Call actual state OnEntering() callback function
				if (m_currentState->m_onEntering != nullptr) {
					m_currentState->m_onEntering();
				}

				return true;
			}
		}
	}

	// Run callback function while FSM remain in actual state
	if (m_currentState->m_onRunning != nullptr) {
		m_currentState->m_onRunning();
	}

	// Run actions for current state (ALL types if defined)
	if (m_currentState->getActions()){
		m_currentState->runActions();
	}

	return false;
}

State* StateMachine::getNextState() {
	return m_nextState;
}

State* StateMachine::getCurrentState() {
	return m_currentState;
}

const char * StateMachine::getActiveStateName() {
	return m_currentState->getStateName();
}

const int StateMachine::GetStatesNumber() {
	return m_states.size();
}


uint32_t StateMachine::getLastEnterTime() {
	return m_currentState->getEnteringTime();
}

void StateMachine::setCurrentState(State *newState, bool callOnEntering, bool callOnLeaving) {
	// Guarantee that will run OnLeaving()
	if (m_currentState->m_onLeaving != nullptr && callOnLeaving)
		m_currentState->m_onLeaving();

	// Update current machine state
	m_currentState = newState;

	// Guarantee that will run OnEntering()
	if (m_currentState->m_onEntering != nullptr && callOnEntering)
		m_currentState->m_onEntering();

	// Update Enter Time
	m_currentState->m_enterTime = millis();
	m_currentState->m_timeout = false;
}
