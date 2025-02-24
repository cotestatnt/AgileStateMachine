#ifndef AGILE_TRANSITION_H
#define AGILE_TRANSITION_H
#pragma once
#include "Arduino.h"

class State;

using condition_cb = bool (*)();

class Transition
{
public:
    ~Transition() {}

    // Costruttore con riferimento a stato e variabile booleana
    Transition(State &out, bool &trigger) : m_outState(out), m_trigger_var(&trigger) {}

    // Costruttore con riferimento a stato e callback
    Transition(State &out, condition_cb trigger) : m_outState(out), m_trigger_cb(trigger) {}

    // Costruttore con riferimento a stato e timeout
    Transition(State &out, uint32_t timeout) : m_outState(out), m_timeout(timeout) {}

    // Costruttore con puntatore (versione originale, se vuoi mantenerla)
    Transition(State *out, bool &trigger) : m_outState(*out), m_trigger_var(&trigger) {}

    Transition(State *out, condition_cb trigger) : m_outState(*out), m_trigger_cb(trigger) {}

    Transition(State *out, uint32_t timeout) : m_outState(*out), m_timeout(timeout) {}

    bool trigger(uint32_t enterTime)
    {
        // Trigger su funzione callback
        if (m_trigger_cb != nullptr)
        {
            return m_trigger_cb();
        }

        // Trigger su variabile booleana
        else if (m_trigger_var != nullptr)
        {
            return *(m_trigger_var);
        }

        // Trigger su timeout
        else if (m_timeout > 0)
        {
            if (millis() - enterTime >= m_timeout)
            {
                return true;
            }
        }

        return false;
    }

    State * getOutputState()
    {
        return &m_outState;
    }

protected:
    State &m_outState; // Ora è un riferimento invece di un puntatore
    bool *m_trigger_var = nullptr;
    condition_cb m_trigger_cb = nullptr;
    uint32_t m_timeout = 0;
};

#endif
