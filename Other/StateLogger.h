/*
 * StateLogger.h
 *
 *  Created on: Jun 27, 2014
 *      Author: Administrator
 */

#ifndef STATELOGGER_H_
#define STATELOGGER_H_

// -------------------------------------------------------------------------------
// Includes

// System context
#include "System/Utilities/Logger.h"

// Include the EEPROM engine
#include "System Drivers/Storage/EEPROMI2CDigitalStorage.h"

// -------------------------------------------------------------------------------
// Defines


// -------------------------------------------------------------------------------
// Main code

/**
 * This is the state logger class handler. It handles the formatting and logging
 * of state given.
 */
class State_Logger : public Logger{

	// Public Context
	public:

		/**
		 * This is the default constructor for the class.
		 *
		 * @param eeprom_storage						- the pointer to the eeprom driver
		 * @param context								- the system context
		 */
		State_Logger(EEPROM_I2C_Digital_Storage* eeprom_storage, System_Context* context);

		/**
		 * This checks if the db already exists.
		 *
		 * @return exists								- the database exists.
		 */
		bool check_existence();

		/**
		 * This is the default begin method
		 *
		 * @return success								- the setup success
		 */
		bool begin();

		/**
		 * This saves the system state.
		 * In most cases this stores the password and username
		 * of both the WLAN access point and the MQTT connection and server.
		 *
		 */
		static void save_sys_state(logging_event_struct_t* structure, State_Logger* state);

		/**
		 * This reads the system state from the EEPROM DB
		 */
		void read_sys_state();

	// Private Context
	private:

		//! Internal access to eeprom
		EEPROM_I2C_Digital_Storage* _eeprom;
};

#endif /* STATELOGGER_H_ */
