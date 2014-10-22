/*
 * Timer.h
 *
 *  Created on: Jun 18, 2014
 *      Author: Administrator
 */

#ifndef TIMER_H_
#define TIMER_H_

// -------------------------------------------------------------------------------
// Includes
#include <stdlib.h>
#include <string.h>

#include "hw_types.h"
#include "interrupt.h"
#include "hw_ints.h"
#include "pin.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "timer.h"
#include "utils.h"
#include "timer_if.h"

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

// Timer settings
extern timer_settings_t defaults;

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
		 * This is the overriden constructor for the class.
		 *
		 * @param tick_handler						- the interrupt service routine
		 * @param settings							- the timer settings
		 */
		Timer(timer_settings_t* settings = &defaults);

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
extern "C" {
extern void default_timer_handler(void);
}
#endif /* TIMER_H_ */
