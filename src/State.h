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
		~State(){};

	   	State(const char *name, uint32_t min, uint32_t max,
			state_cb enter, state_cb exit, state_cb run )
			:
			m_stateName(name),
			m_minTime(min),
			m_maxTime(max),
			m_onEntering(enter),
			m_onLeaving(exit),
			m_onRunning(run) {}

		State(const char *name) {
			State(name, 0, 0, nullptr, nullptr, nullptr);
		}

		State(const char *name, uint32_t min, uint32_t max) {
			State(name, min, max, nullptr, nullptr, nullptr);
		}

		State(const char *name, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr) {
			State(name, 0, 0, enter, exit, run);
		}

		State(const char *name, uint32_t min = 0, state_cb enter = nullptr, state_cb exit = nullptr, state_cb run = nullptr) {
			State(name, 0, min, enter, exit, run);
		}

		void 		 setIndex(uint8_t index);
		uint8_t		 getIndex() ;
		void 		 setTimeout(uint32_t preset);
		bool 		 getTimeout() ;

		void 	     resetEnterTime();
		uint32_t 	 getEnterTime();
		const char*  getStateName();

		void 		 setStateMaxTime(uint32_t _time);
		void 		 setStateMinTime(uint32_t _time);

		Transition*  addTransition(State *out, bool &trigger);
		Transition*  addTransition(State *out, condition_cb trigger);
		Transition*  addTransition(State *out, uint32_t timeout);
		State* 		 runTransitions();

		Action* 	 addAction(uint8_t type, bool &target, uint32_t _time = 0);
		uint8_t 	 getActions();
		void 		 runActions();
		void 		 clearActions();

	protected:
		friend class StateMachine;
		friend class Transition;

		const char 	   *m_stateName;
		uint32_t 		m_minTime = 0;   // 0 -> No min time
		uint32_t 		m_maxTime = 0;   // 0 -> No timeout
		uint32_t 		m_enterTime;
		state_cb 		m_onEntering;
		state_cb 		m_onLeaving;
		state_cb 		m_onRunning;

		uint8_t         m_stateIndex = 0;
		bool 			m_timeout = false;
		LinkedList<Transition*> m_transitions;
		LinkedList<Action*> m_actions;
};



#endif