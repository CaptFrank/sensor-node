/*
 * Thread_MQTT_Engine.h
 *
 *  Created on: Jun 30, 2014
 *      Author: Administrator
 */

#ifndef THREAD_MQTT_ENGINE_H_
#define THREAD_MQTT_ENGINE_H_

// -------------------------------------------------------------------------------
// Includes

// Include System Context
#include "SystemContext.h"

// -------------------------------------------------------------------------------
// Defines

// N/A

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the MQTT engine handler. It is the handler that given
 * a context object, will take care of any polling and live packets
 * on the MQTT protocol base.
 */
inline void SYSTEM_MQTT_ENGINE_THREAD(System_Context* context){

	// We check for any mqtt responses
	context->_sys->_network._mqtt_client->MQTT_poll();

	// We send out a live response
	context->_sys->_network._mqtt_client->MQTT_live();
}

#endif /* THREAD_MQTT_ENGINE_H_ */
