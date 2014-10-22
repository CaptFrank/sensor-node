/*
 * Timer.h
 *
 *  Created on: Jun 18, 2014
 *      Author: Administrator
 */

#ifndef iTIMER_H_
#define iTIMER_H_

// -------------------------------------------------------------------------------
// Includes


extern "C" {

#include <stdlib.h>
#include <string.h>

#include "timer_if.h"
}


// -------------------------------------------------------------------------------
// Defines

/**
 * This is the timer setting structure
 */
struct timer_settings_t {

	unsigned long timer_peripheral;
	unsigned long timer_address;
	unsigned long timer_bank;
	unsigned long timer_setting;
	unsigned long timer_resolution;
	unsigned long timer_scalar;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the generic timer class. It handles the setup and
 * the running of the timer.
 */
class Timer {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class
		 */
		Timer();

		/**
		 * This is the overriden constructor for the class.
		 *
		 * @param tick_handler						- the interrupt service routine
		 * @param settings							- the timer settings
		 */
		Timer(timer_settings_t* settings);

		/**
		 * This is the default setup routine. It is needed if the default
		 * constructor is not overriden.
		 *
		 * @param tick_handler						- the interrupt service routine
		 * @param settings							- the timer settings
		 */
		void setup(timer_settings_t* settings);

		/**
		 * This is the default start method for the timer object.
		 */
		void start();

		/**
		 * This is the getter method to get the current elapsed time.
		 *
		 * @return int								- the elapsed time
		 */
		int get_ms();

		/**
		 * This is the private method to reset the timer counter value.
		 */
		void reset_ms();

		//! The current time counter.
		static int _ticks;

		//! The timer settings
		static timer_settings_t _settings;

	// Private Context
	private:

		//! The callback
		void(*_tick_handler)(void);
};

#ifdef __cplusplus
extern "C"
{

/**
 * This is the default timer handler for created timers.
 *
 * @param value										- the timer value to be increased
 * @param timer										- the timer address
 */
void default_timer_handler(void){

	// Clear the interrupt flag
	Timer_IF_InterruptClear(Timer::_settings.timer_address);
	Timer::_ticks ++;
}

}
#endif
#endif /* TIMER_H_ */
