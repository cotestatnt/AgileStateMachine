#include "AgileStateMachine.h"

void StateMachine::addState(State &state) {
	state.setIndex(m_states.size());
	m_states.append(&state);
	m_currentState = &state;
}


void StateMachine::start() {
	m_started = true;
}


void StateMachine::stop() {
	m_started = false;
}


bool StateMachine::execute() {

	if (!m_started) {
		return false;
	}

	for (State *state = m_states.first(); state != nullptr; state = m_states.next()) {

		if (state == m_currentState) {

			if (m_currentState->m_minTime > 0) {
				if (millis() - m_currentState->m_enterTime < m_currentState->m_minTime)	{
					continue;
				}
			}

			// Check triggers for current state
			State *m_nextState = state->runTransitions();

			// One of the transitions has triggered, set the new state
			if (m_nextState != nullptr) {

				// Clear the actions before exit actual state
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


State* StateMachine::getCurrentState() {
	return m_currentState;
}


int StateMachine::GetStatesNumber() {
	return m_states.size();
}


uint32_t StateMachine::getLastEnterTime() {
	return m_currentState->getEnterTime();
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
