#include "State.h"


Transition* State::addTransition(State *out, bool &trigger) {
    Transition* tr = new Transition(out, trigger);
    m_transitions.append(tr);
    return tr;
}

Transition* State::addTransition(State *out, condition_cb trigger) {
    Transition* tr = new Transition(out, trigger);
    m_transitions.append(tr);
    return tr;
}
Transition* State::addTransition(State *out, uint32_t timeout) {
    Transition* tr = new Transition(out, timeout);
    m_transitions.append(tr);
    return tr;
}

Action* State::addAction(uint8_t type, bool &target, uint32_t _time ) {
    Action* action = new Action(this, type, &target, _time);
    m_actions.append(action);
    return action;
}


State* State::runTransitions() {
    for (Transition *tr = m_transitions.first(); tr != nullptr; tr = m_transitions.next()) {
        // Pass m_enterTime to activate transition on timeout (if defined)
        if (tr->trigger(m_enterTime)) {
            return tr->getOutputState();
        }
    }
    return nullptr;
}

void State::runActions() {
    for (Action *action = m_actions.first(); action != nullptr; action = m_actions.next()) {
        action->execute();
    }
}


void State::clearActions() {
    for (Action *action = m_actions.first(); action != nullptr; action = m_actions.next()) {
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

void State::resetEnterTime() {
  m_enterTime = millis();
}

uint32_t State::getEnterTime() {
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