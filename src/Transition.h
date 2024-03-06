#ifndef AGILE_TRANSITION_H
#define AGILE_TRANSITION_H
#pragma once
#include "Arduino.h"

class State;

using condition_cb = bool (*)();

class Transition
{
public:
	~Transition(){};

	Transition(State *out, bool &trigger) : m_outState(out), m_trigger_var(&trigger) {}
	Transition(State *out, condition_cb trigger) : m_outState(out), m_trigger_cb(trigger) {}
	Transition(State *out, uint32_t timeout) : m_outState(out), m_timeout(timeout) {}

	bool trigger(uint32_t enterTime)
	{

		// Trigger on callback function result
		if (m_trigger_cb != nullptr)
		{
			return m_trigger_cb();
		}

		// Trigger on bool variable
		else if (m_trigger_var != nullptr)
		{
			return *(m_trigger_var);
		}

		// Trigger on timeout
		else if (m_timeout > 0)
		{
			if (millis() - enterTime >= m_timeout)
			{
				return true;
			}
		}

		// No trigger
		return false;
	}

	State *getOutputState()
	{
		return m_outState;
	}

protected:
	State *m_outState;
	bool *m_trigger_var = nullptr;
	condition_cb m_trigger_cb = nullptr;
	uint32_t m_timeout = 0;
};

#endif