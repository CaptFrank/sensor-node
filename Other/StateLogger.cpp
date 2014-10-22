/*
 * StateLogger.cpp
 *
 *  Created on: Jun 27, 2014
 *      Author: Administrator
 */

#include "StateLogger.h"

// Public Context

/**
 * This is the default constructor for the class.
 *
 * @param eeprom_storage						- the pointer to the eeprom driver
 * @param context								- the system context
 */
State_Logger::State_Logger(EEPROM_I2C_Digital_Storage* eeprom_storage, System_Context* contex) :
																	Logger(context){

	//! Access to the eeprom
	this->_eeprom = eeprom_storage;
}

/**
 * This is the default begin method
 *
 * @return success				- the setup success
 */
bool State_Logger::begin(){

	// We begin the eeprom engine and read/ write the record
	return this->_eeprom->begin();
}

/**
 * This saves the system state.
 * In most cases this stores the password and username
 * of both the WLAN access point and the MQTT connection and server.
 *
 */
void State_Logger::save_sys_state(State_Logger* state){

	// We write the record
	state->_eeprom->write_record(DB_RECORD, this->_system_state);
}

/**
 * This reads the system state from the EEPROM DB
 */
void State_Logger::read_sys_state(){

	// We read the only record there
	this->_eeprom->read_record(DB_RECORD, &this->_system_state);
}
