#include "AgileStateMachine.h"
#include "Arduino.h"

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


bool StateMachine::updateStateIndex(State *state, uint8_t index)
{
	if (state != nullptr) {
		state->setIndex(index);
		return true;
	}
	// If the state is null, return false
	return false;
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
