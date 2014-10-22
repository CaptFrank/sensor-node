/*
 * Thread_Data_Logger.h
 *
 *  Created on: Jun 30, 2014
 *      Author: Administrator
 */

#ifndef THREAD_DATA_LOGGER_H_
#define THREAD_DATA_LOGGER_H_

// -------------------------------------------------------------------------------
// Includes

// Include System Context
#include "SystemContext.h"
#include "SystemDefines.h"

// Loggers
//#include "EventLogger.h"

// -------------------------------------------------------------------------------
// Defines

/**
 * The service type definition
 */
struct logger_service_t{

	// The service id -- See above enum
	unsigned int id;

	// The event id
	logging_event_t evt_id;

	// The service method
	bool(*service_method)(logging_event_struct_t* evt, Event_Logger* object);

	// The service context object
	void* context_object;
};

/**
 * These are the ids for each of the data services.
 */
enum logger_service_id_t {

	// Data logging
	SYS_LOGGER_SERVICE_ADS1015,			//!< SYS_DATA_SERVICE_ADS1015
	SYS_LOGGER_SERVICE_BMP180, 			//!< SYS_DATA_SERVICE_BMP180
	SYS_LOGGER_SERVICE_SHT21,  			//!< SYS_DATA_SERVICE_SHT21
	SYS_LOGGER_SERVICE_TMP006, 			//!< SYS_DATA_SERVICE_TMP006
	SYS_LOGGER_SERVICE_TMP102, 			//!< SYS_DATA_SERVICE_TMP102
	SYS_LOGGER_SERVICE_DS1307,  		//!< SYS_DATA_SERVICE_DS1307

	// State logging
	SYS_LOGGER_SERVICE_STATE,			//!< SYS_LOGGER_SERVICE_STATE

	// NULL
	SYS_LOGGER_SERVICE_NULL				//!< SYS_DATA_SERVICE_NULL
};

/**
 * The array containning the events to log
 */
logger_service_id_t logging_services[] = {

		// Data logging
		SYS_LOGGER_SERVICE_ADS1015,			//!< SYS_DATA_SERVICE_ADS1015
		SYS_LOGGER_SERVICE_BMP180, 			//!< SYS_DATA_SERVICE_BMP180
		SYS_LOGGER_SERVICE_SHT21,  			//!< SYS_DATA_SERVICE_SHT21
		SYS_LOGGER_SERVICE_TMP006, 			//!< SYS_DATA_SERVICE_TMP006
		SYS_LOGGER_SERVICE_TMP102, 			//!< SYS_DATA_SERVICE_TMP102
		SYS_LOGGER_SERVICE_DS1307,  		//!< SYS_DATA_SERVICE_DS1307
		SYS_LOGGER_SERVICE_NULL				//!< SYS_DATA_SERVICE_NULL
};

/**
 * The array containning the events to log
 */
logging_event_t events[] = {

		ADS1015_DATA_LOGGING_EVENT,
		BMP180_DATA_LOGGING_EVENT,
		SHT21_DATA_LOGGING_EVENT,
		TMP006_DATA_LOGGING_EVENT,
		TMP102_DATA_LOGGING_EVENT,
		DS1307_DATA_LOGGING_EVENT,
		NULL_LOGGING_EVENT
};

// -------------------------------------------------------------------------------
// Main code

inline void SYSTEM_DATA_LOGGER_THREAD(System_Context* context){

	// We only log the data if the data is new.
	if(!context->get_updated()){
		return;
	}

	// Cycle through the service table
	for(register char i = 0; static_cast<unsigned int>(SYS_LOGGER_SERVICE_NULL) != logging_services[i]; i++){

		// If we need to update the state
		/*if((logger_service_table[i].id == SYS_LOGGER_SERVICE_STATE)
				&& context->_state_update){

			// Log
			//logger_service_table[i].service_method(logger_service_table[i].evt_id,
			//									logger_service_table[i].context_object);

		}else { // All other logger services*/

		// Create a logging type
		logging_event_struct_t _log;
		_log.type = events[i]; // Set the event type to log

		// log to the sd
		Event_Logger::trigger_logging_event(&_log, _sys->_sys->_loggers._event_logger);

		// Publish the message to the MQTT Server.... - The record is saved in the class
		context->_sys->_network._mqtt_client->MQTT_publish((char*)MQTT_PUB_TOPIC_1,
				_sys->_sys->_loggers._event_logger->_record);
		/*}*/
	}

	// Unset the logger flag
	context->unsets_updated();
}


#endif /* THREAD_DATA_LOGGER_H_ */
