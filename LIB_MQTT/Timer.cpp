/*
 * Timer.cpp
 *
 *  Created on: Jun 18, 2014
 *      Author: Administrator
 */

#include "Timer.h"

// Public Context

/**
 * This is the overriden constructor for the class.
 *
 * @param settings							- the timer settings
 */
Timer::Timer(timer_settings_t* settings){

	// Go to setup
	this->setup(settings);
}

/**
 * This is the default setup routine. It is needed if the default
 * constructor is not overriden.
 *
 * @param settings							- the timer settings
 */
void Timer::setup(timer_settings_t* settings){

	// Copy the settings
	memcpy(&_settings, settings, sizeof(_settings));

	// Init the timer
	Timer_IF_Init(_settings.timer_peripheral,
				  _settings.timer_address,
				  _settings.timer_setting,
				  _settings.timer_bank,
				  _settings.timer_resolution);

	// Setup the interrupts
	Timer_IF_IntSetup(_settings.timer_address,
					  _settings.timer_bank,
					  default_timer_handler);
}

/**
 * This is the default start method for the timer object.
 */
void Timer::start(){

	// Turn on the timer
	Timer_IF_Start(_settings.timer_address,
				   _settings.timer_bank,
				   _settings.timer_scalar);
}

/**
 * This is the getter method to get the current elapsed time.
 *
 * @return int								- the elapsed time
 */
int Timer::get_ms(){

	// Get the ticks
	int ticks = _ticks;

	// Reset it
	this->reset_ms();

	// Return the ticks
	return ticks;
}

/**
 * This is the private method to reset the timer counter value.
 */
void Timer::reset_ms(){

	// Reset the ticks
	this->_ticks = 0x00;
}

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

timer_settings_t defaults = {PRCM_TIMERA1, TIMERA1_BASE, 0x000000ff,
							 0x00000022, 0,
							 PERIODIC_TEST_CYCLES * PERIODIC_TEST_LOOPS / 10};
#endif


