/*
 * EventLogger.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Administrator
 */

#include "EventLogger.h"

// Public Context

/**
 * The constructor
 *
 * @param sd_storage			- the sd card engine ptr
 * @param context				- the system context
 */
Event_Logger::Event_Logger(SD_SPI_Digital_Storage* sd_storage, System_Context* context) :
												Logger(context){

	// Set internals
	this->_sd = sd_storage;
	this->_evt = 0;

	// Set data
	//memcpy(this->_patterns._data_header, DATA_LOG_HEADER_PATTERN, sizeof(DATA_LOG_HEADER_PATTERN));
	//memcpy(this->_patterns._sys_header, SYSTEM_LOG_HEADER_PATTERN, sizeof(SYSTEM_LOG_HEADER_PATTERN));
	memcpy(this->_patterns._data_log, DATA_LOG_ENTRY_PATTERN, sizeof(DATA_LOG_ENTRY_PATTERN));
	memcpy(this->_patterns._sys_log, SYSTEM_LOG_ENTRY_PATTERN, sizeof(SYSTEM_LOG_ENTRY_PATTERN));

	// Create the headers - add the date
	//DS1307_time_t* _temp = &this->_context->_DS1307_time_context.time;

	// Add the time and date to the headers
	/*sprintf(this->_patterns._data_header, DATA_LOG_HEADER_PATTERN, _temp->day,
										  _temp->month,
										  _temp->year,
										  _temp->hour,
										  _temp->minute,
										  _temp->second);

	sprintf(this->_patterns._sys_header,  SYSTEM_LOG_HEADER_PATTERN, _temp->day,
										  _temp->month,
										  _temp->year,
										  _temp->hour,
										  _temp->minute,
										  _temp->second);
	*/
}

/**
 * This is the default begin method
 *
 * @return success				- the setup success
 */
bool Event_Logger::begin(){

	// Start the SD engine
	/*if(this->_sd->begin()){

		// Write the headers

		// System
		if(this->_sd->write_record(SD_SYSTEM_FILE, this->_patterns._sys_header,
				sizeof(this->_patterns._sys_header))){

			// Data
			if(this->_sd->read_record(SD_DATA_FILE, this->_patterns._data_header,
					sizeof(this->_patterns._data_header))){
				return true;
			}
		}
	}else{
		return false;
	}
	return false;*/
	return true;
}

/**
 * This triggers a logging event
 *
 * @param evt
 * @return success				- the success
 */
bool Event_Logger::trigger_logging_event(logging_event_struct_t* evt, Event_Logger* object){

	char* data;

	// Reset the tag
	memset(evt->id, 0, sizeof(evt->id));


	// We switch on the event type
	switch(evt->type){

		// System logging
		// 	- Here the ID must be set by the user
		case SYSTEM_LOGGING_EVENT:
			return object->_process_logging_event(evt);

		// Data logging
		//	- This is just a random message
		//  	Np processing needed
		case GENERIC_DATA_LOGGING_EVENT:

			memcpy(evt->id, LOGGING_GENERIC_ID, sizeof(evt->id));
			goto SENSOR_LOGGING;

		// RTC
		case DS1307_DATA_LOGGING_EVENT:
			memcpy(evt->id, LOGGING_DS1307_ID, sizeof(evt->id));

			// Get pointer
			DS1307_time_context_t* context1 = &object->_context->_DS1307_time_context;

			// Allocate the data
			data = (char*)malloc(sizeof(DS1307_LOG_ENTRY));
			memcpy(data, DS1307_LOG_ENTRY, sizeof(data));
			sprintf(data, DS1307_LOG_ENTRY, context1->time.second,
						  context1->time.minute,
						  context1->time.hour,
						  context1->time.day,
						  context1->time.month,
						  context1->time.year,
						  context1->time.dow,
						  (char)context1->state);

			goto SENSOR_LOGGING;

		// TEMP
		case TMP102_DATA_LOGGING_EVENT:
			memcpy(evt->id, LOGGING_TMP102_ID, sizeof(evt->id));

			// Get pointer
			TMP102_driver_context_t* context2 = &object->_context->_TMP102_driver_context;

			// Allocate the data
			data = (char*)malloc(sizeof(TMP102_LOG_ENTRY));
			memcpy(data, TMP102_LOG_ENTRY, sizeof(data));
			sprintf(data, TMP102_LOG_ENTRY, context2->state.error,
						  context2->state.state,
						  context2->temperature);

			goto SENSOR_LOGGING;

		// TEMP
		case TMP006_DATA_LOGGING_EVENT:
			memcpy(evt->id, LOGGING_TMP006_ID, sizeof(evt->id));

			goto SENSOR_LOGGING;

		// TEMP + HUMIDITY
		case SHT21_DATA_LOGGING_EVENT:
			memcpy(evt->id, LOGGING_SHT21_ID, sizeof(evt->id));

			// Get pointer
			SHT21_driver_context_t* context3 = &object->_context->_SHT21_driver_context;

			// Allocate the data
			data = (char*)malloc(sizeof(SHT21_LOG_ENTRY));
			memcpy(data, SHT21_LOG_ENTRY, sizeof(data));
			sprintf(data, SHT21_LOG_ENTRY,context3->humidity,
						  context3->temperature);

			goto SENSOR_LOGGING;

		// ANALOG VALUES
		case ADS1015_DATA_LOGGING_EVENT:
			memcpy(evt->id, LOGGING_ADS1015_ID, sizeof(evt->id));

			// Get pointer
			ADS1015_driver_context_t* context4 = &object->_context->_ADS1015_driver_context;

			// Allocate the data
			data = (char*)malloc(sizeof(ADS1015_LOG_ENTRY));
			memcpy(data, ADS1015_LOG_ENTRY, sizeof(data));
			sprintf(data, ADS1015_LOG_ENTRY, context4->power_sense,
						  context4->light,
						  context4->buffered_mic,
						  context4->raw_mic);

			goto SENSOR_LOGGING;

		// PRESSURE + TEMP + ALTITUDE
		case BMP180_DATA_LOGGING_EVENT:
			memcpy(evt->id, LOGGING_BMP180_ID, sizeof(evt->id));

			// Get pointer
			BMP180_driver_context_t* context5 = &object->_context->_BMP180_driver_context;

			// Allocate the data
			data = (char*)malloc(sizeof(BMP180_LOG_ENTRY));
			memcpy(data, BMP180_LOG_ENTRY, sizeof(data));
			sprintf(data, BMP180_LOG_ENTRY, context5->pressure,
						  context5->pressure_baseline,
						  context5->altitude,
						  context5->sea_level,
						  context5->temperature);

		// Here we jump the the logging method
		SENSOR_LOGGING:

			// Assign the message
			evt->message = data;

			// Process
			bool status = object->_process_logging_event(evt);

			// free message
			free(data);

			// Return
			return status;

		//Not a valid logging event
		default:
			break;
	}
	return false;
}

// Private Context

/**
 * This processes and writes the logging event to the SD Card.
 *
 * @param evt					- the event
 * @return success				- the success
 */
bool Event_Logger::_process_logging_event(logging_event_struct_t* evt){

	// Get the time pointer
	DS1307_time_t* _temp = &this->_context->_DS1307_time_context.time;

	// If the evt is a data logging event
	if(evt->type & 0x10){

		// Create the string to write
		// Create the headers - add the date
		char* record = this->_patterns._data_log;

		// Add the time and date to the headers
		sprintf(record, DATA_LOG_ENTRY_PATTERN, _temp->hour,
						 _temp->minute,
						 _temp->second,
						 evt->id,
						 evt->message);

		// Write the string to sd
		/*if(this->_sd->write_record(SD_DATA_FILE, record, strlen(record))){
			return true;
		}return false;*/
		return true;

	// System logging
	}else{

		// Create the string to write
		// Create the headers - add the date
		char* record = this->_patterns._sys_log;

		// Add the time and date to the headers
		sprintf(record, SYSTEM_LOG_ENTRY_PATTERN, _temp->hour,
						 _temp->minute,
						 _temp->second,
						 evt->id,
						 evt->message);

		// Set the internal record
		this->_record = record;

		// Write the string to sd
		/*if(this->_sd->write_record(SD_SYSTEM_FILE, record, strlen(record))){
			return true;
		}return false;*/
		return true;
	}
}
