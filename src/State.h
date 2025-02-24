#ifndef AGILE_STATE_H
#define AGILE_STATE_H
#pragma once

#include "Arduino.h"
#include "LinkedList.h"
#include "Action.h"
#include "Transition.h"

class Transition;
class Action;

using state_cb = void (*)();

class State
{
public:
    ~State() = default;

    template <typename T>
    State(T name, uint32_t min, uint32_t max, state_cb enter, state_cb exit, state_cb run)
        : m_stateName(reinterpret_cast<const char *>(name)),
          m_minTime(min),
          m_maxTime(max),
          m_onEntering(enter),
          m_onLeaving(exit),
          m_onRunning(run)
    {
    }

    // Delegazione ai costruttori principali
    template <typename T>
    State(T name)
        : State(name, 0, 0, nullptr, nullptr, nullptr) {}

    template <typename T>
    State(T name, uint32_t min, uint32_t max)
        : State(name, min, max, nullptr, nullptr, nullptr) {}

    template <typename T>
    State(T name, state_cb enter, state_cb exit, state_cb run)
        : State(name, 0, 0, enter, exit, run) {}

    template <typename T>
    State(T name, uint32_t min, state_cb enter, state_cb exit, state_cb run)
        : State(name, min, 0, enter, exit, run) {}

    void setTimeout(uint32_t preset);
    bool getTimeout();
    void resetEnterTime();
    uint32_t getEnterTime();
    void setStateMaxTime(uint32_t _time);
    void setStateMinTime(uint32_t _time);

    const char *getStateName() const
    {
        return m_stateName;
    }

    const __FlashStringHelper *getStateName_P() const
    {
        return reinterpret_cast<const __FlashStringHelper *>(m_stateName);
    }

    Transition *addTransition(State *out, bool &trigger);
    Transition *addTransition(State *out, condition_cb trigger);
    Transition *addTransition(State *out, uint32_t timeout);
    void addTransition(Transition &transition);

    Action *addAction(uint8_t type, bool &target, uint32_t _time = 0);
    void addAction(Action &action);

    void setIndex(uint8_t index);
    uint8_t getIndex() const;

protected:
    friend class StateMachine;
    friend class Transition;

    const char *m_stateName;
    uint32_t m_minTime = 0;
    uint32_t m_maxTime = 0;
    uint32_t m_enterTime;
    state_cb m_onEntering = nullptr;
    state_cb m_onLeaving = nullptr;
    state_cb m_onRunning = nullptr;

    uint8_t m_stateIndex = 0;
    bool m_timeout = false;
    LinkedList<Transition *> m_transitions;
    LinkedList<Action *> m_actions;

    State *runTransitions();
    void runActions();
    void clearActions();
    uint8_t getActions();
};

#endif
