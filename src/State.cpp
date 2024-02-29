#include "State.h"


Transition* State::addTransition(State *out, bool &trigger) {
    Transition* tr = new Transition(out, trigger);
    m_transitions.add(tr);
    return tr;
}

Transition* State::addTransition(State *out, condition_cb trigger) {
    Transition* tr = new Transition(out, trigger);
    m_transitions.add(tr);
    return tr;
}
Transition* State::addTransition(State *out, uint32_t timeout) {
    Transition* tr = new Transition(out, timeout);
    m_transitions.add(tr);
    return tr;
}

Action* State::addAction(uint8_t type, bool &target, uint32_t _time ) {
    Action* action = new Action(this, type, &target, _time);
    m_actions.add(action);
    return action;
}

void State::clearTransitions() {
    Transition *transition = nullptr;
	for(int i = 0; i < m_transitions.size(); i++) {
		transition = m_transitions.get(i);
        transition->reset();
    }
}

State* State::runTransitions() {
    Transition *transition = nullptr;
	for(int i = 0; i < m_transitions.size(); i++) {
		transition = m_transitions.get(i);
        // Pass m_enterTime to activate transition on timeout (if defined)
        if (transition->trigger(m_enterTime)) {
            transition->reset();
            return transition->getOutputState();
        }
    }
    return nullptr;
}

void State::runActions() {
    Action *action = nullptr;
	for(int i = 0; i < m_actions.size(); i++){
		action = m_actions.get(i);
        action->execute();
    }
}


void State::clearActions() {
    Action *action = nullptr;
	for(int i = 0; i < m_actions.size(); i++){
		action = m_actions.get(i);
        action->clear();
    }
}

uint8_t State::getActions() {
    return m_actions.size();
}


void State::setIndex(uint8_t index) {
	m_stateIndex = index;
}

uint8_t State::getIndex() {
	return m_stateIndex;
}

void State::setTimeout(uint32_t _time) {
	if (_time) {
        m_maxTime = _time;
    }
}

bool State::getTimeout() {
	return m_timeout;
}

uint32_t State::getEnteringTime() {
	return m_enterTime;
}

const char* State::getStateName() {
	return m_stateName;
}

void State::setStateMaxTime(uint32_t _time) {
	m_maxTime = _time;
}

void State::setStateMinTime(uint32_t _time) {
	m_minTime = _time;
}