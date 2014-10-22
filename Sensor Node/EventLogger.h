/*
 * EventLogger.h
 *
 *  Created on: Jun 26, 2014
 *      Author: Administrator
 */

#ifndef EVENTLOGGER_H_
#define EVENTLOGGER_H_

// -------------------------------------------------------------------------------
// Includes

// System context
#include "Logger.h"
#include "SystemContext.h"

// Include the SD engine
#include "SDSPIDigitalStorage.h"

// -------------------------------------------------------------------------------
// Defines

// We add the time and date
//#define DATA_LOG_HEADER_PATTERN	\
//		"~--------------- DATA ACQUIRED: [%d-%d-%d:%d:%d:%d] ---------------~\r\n"

// We add the time and date
//#define SYSTEM_LOG_HEADER_PATTERN \
//		"~--------------- SYSTEM LOG EVENTS: [%d-%d-%d:%d:%d:%d] ---------------~\r\n"

// We add the time, sensor id, params
#define DATA_LOG_ENTRY_PATTERN \
		"*[%d:%d:%d]: <%s> {%s}*\r\n"

// We add the time, event id, event
#define SYSTEM_LOG_ENTRY_PATTERN\
		"*[%d:%d:%d]: <%s> -> %s*\r\n"

#define TMP102_LOG_ENTRY  		"%d, %d, %d" 						// error, state, temp
#define TMP006_LOG_ENTRY		"%d, %d"							// in temp, ext temp
#define BMP180_LOG_ENTRY		"%d, %d, %d, %d, %d"				// pressure, pressure base, altitude, sealevel, temp
#define SHT21_LOG_ENTRY			"%d, %d"							// humidity, temp
#define ADS1015_LOG_ENTRY		"%d, %d, %d, %d"					// power, light, buff mic, mic
#define DS1307_LOG_ENTRY		"%d, %d, %d, %d, %d, %d, %d, %d"	// sec, min, hr, day, mon, yr, dow, state

/**
 * Defines the various logging events
 */
enum logging_event_t {

	// Generic event
	GENERIC_DATA_LOGGING_EVENT				= 0x10, 		//!< DATA_LOGGING_EVENT

	// Sensors
	DS1307_DATA_LOGGING_EVENT				= 0x11,			//!< DS1307_DATA_LOGGING_EVENT
	TMP102_DATA_LOGGING_EVENT				= 0x12,			//!< TMP102_DATA_LOGGING_EVENT
	TMP006_DATA_LOGGING_EVENT				= 0x13,			//!< TMP006_DATA_LOGGING_EVENT
	SHT21_DATA_LOGGING_EVENT				= 0x14,			//!< SHT21_DATA_LOGGING_EVENT
	ADS1015_DATA_LOGGING_EVENT				= 0x15,			//!< ADS1015_DATA_LOGGING_EVENT
	BMP180_DATA_LOGGING_EVENT				= 0x16,			//!< BMP180_DATA_LOGGING_EVENT

	// Generic system message
	SYSTEM_LOGGING_EVENT					= 0x01,			//!< SYSTEM_LOGGIN_EVENT
	STATE_LOGGING_EVENT						= 0x02,			//!< STATE_LOGGING_EVENT

	// Null logging event
	NULL_LOGGING_EVENT						= 0x00
};

/**
 * The possible logging ids
 */

// Sensors
#define LOGGING_DS1307_ID					"DS1307"  	//!< LOGGING_DS1307_ID
#define LOGGING_TMP102_ID					"TMP102"  	//!< LOGGING_TMP102_ID
#define LOGGING_TMP006_ID					"TMP006"  	//!< LOGGING_TMP006_ID
#define LOGGING_SHT21_ID					"SHT21"    	//!< LOGGING_SHT21_ID
#define LOGGING_ADS1015_ID					"ADS1015"	//!< LOGGING_ADS1015_ID
#define LOGGING_BMP180_ID					"BMP180"  	//!< LOGGING_BMP180_ID
#define LOGGING_GENERIC_ID					"GENERIC"	//!< LOGGING_GENERIC_ID

// System
#define LOGGING_ERROR_ID					"ERROR"    	//!< LOGGING_ERROR_ID
#define LOGGING_INFO_ID						"INFO"      //!< LOGGING_INFO_ID



/**
 * This defines a logging event type.
 */
struct logging_event_struct_t{

	logging_event_t type;

	char id[7];
	char* message;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the event logger class handler. It handles the formatting and logging
 * of events given the proper event type.
 */
class Event_Logger : public Logger{

	// Public Context
	public:

		/**
		 * The constructor
		 *
		 * @param sd_storage			- the sd card engine ptr
		 * @param context				- the system context
		 */
		Event_Logger(SD_SPI_Digital_Storage* sd_storage, System_Context* context);

		/**
		 * This is the default begin method
		 *
		 * @return success				- the setup success
		 */
		bool begin();

		/**
		 * This triggers a logging event
		 *
		 * @param evt					- the event
		 * @param object				- the event logger
		 * @return success				- the success
		 */
		static bool trigger_logging_event(logging_event_struct_t* evt, Event_Logger* object);

		//! The record that has been formatted
		char* _record;

	// Private Context
	private:

		//! Private SD engine
		SD_SPI_Digital_Storage* _sd;

		//! Private logging event type
		logging_event_struct_t* _evt;

		//! The pattern containers
		struct {

			//char _sys_header[sizeof(SYSTEM_LOG_ENTRY_PATTERN)];
			//char _data_header[sizeof(DATA_LOG_HEADER_PATTERN)];
			char _data_log[sizeof(DATA_LOG_ENTRY_PATTERN)];
			char _sys_log[sizeof(SYSTEM_LOG_ENTRY_PATTERN)];

		}_patterns;

		/**
		 * This processes and writes the logging event to the SD Card.
		 *
		 * @param evt					- the event
		 * @return success				- the success
		 */
		bool _process_logging_event(logging_event_struct_t* evt);

		/**
		 * Gets the formulated record.
		 */
		char* get_record(){
			return this->_record;
		}
};

#endif /* EVENTLOGGER_H_ */
