/*
 * Thread_System_Monitor.h
 *
 *  Created on: Jun 30, 2014
 *      Author: Administrator
 */

#ifndef THREAD_SYSTEM_MONITOR_H_
#define THREAD_SYSTEM_MONITOR_H_

// -------------------------------------------------------------------------------
// Includes

// Include System Context
#include "SystemContext.h"

// -------------------------------------------------------------------------------
// Defines

//! Memory placement identifiers
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

// -------------------------------------------------------------------------------
// Main code

/**
 * We reset teh device
 */
static void(*reset_device)(void) = 0;

/**
 * This is the system monitoring funstion handler, that uses the System
 * Context object and checks is everything is in order... Mainly it checks
 * for memory leaks and size of memeory left.
 */
inline void SYSTEM_MONITOR_THREAD(System_Context* context){

	// If we have an overheat event
	if(context->_overheat){
		reset_device();
	}
}


#endif /* THREAD_SYSTEM_MONITOR_H_ */
