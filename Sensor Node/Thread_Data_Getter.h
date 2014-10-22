/*
 * Thread_Data_Getter.h
 *
 *  Created on: Jun 30, 2014
 *      Author: Administrator
 */

#ifndef THREAD_DATA_GETTER_H_
#define THREAD_DATA_GETTER_H_

// -------------------------------------------------------------------------------
// Includes

// Include System Context
#include "SystemContext.h"
#include "SystemDefines.h"
// -------------------------------------------------------------------------------
// Defines

/**
 * The service type definition
 */
struct data_service_t {

	// The service id -- See above enum
	unsigned int id;

	// The service method
	void(*service_method)(void* context_object, void* storage_object);

	// The service context object
	void* context_object;

	// The Storage Pointer
	void* storage_object;
};

/**
 * These are the ids for each of the data services.
 */
enum data_service_id_t {

	SYS_DATA_SERVICE_ADS1015			= 1,			//!< SYS_DATA_SERVICE_ADS1015
	SYS_DATA_SERVICE_BMP180				= 2, 			//!< SYS_DATA_SERVICE_BMP180
	SYS_DATA_SERVICE_SHT21				= 3,  			//!< SYS_DATA_SERVICE_SHT21
	SYS_DATA_SERVICE_TMP006				= 4, 			//!< SYS_DATA_SERVICE_TMP006
	SYS_DATA_SERVICE_TMP102				= 5, 			//!< SYS_DATA_SERVICE_TMP102
	SYS_DATA_SERVICE_DS1307				= 6,  			//!< SYS_DATA_SERVICE_DS1307

	// Null
	SYS_DATA_SERVICE_NULL				= 7				//!< SYS_DATA_SERVICE_NULL
};

/**
 * Data services table
 */
data_service_id_t data_services_table[] = {

		SYS_DATA_SERVICE_ADS1015,
		SYS_DATA_SERVICE_BMP180,
		SYS_DATA_SERVICE_SHT21,
		SYS_DATA_SERVICE_TMP006,
		SYS_DATA_SERVICE_TMP102,
		SYS_DATA_SERVICE_DS1307,
		SYS_DATA_SERVICE_NULL
};

/**
 * This file holds all the service pointers that need to be accessed.
 */
struct data_service_t data_service_table [] = {

	//	ID							METHOD									OBJECT - Sensor Pointer						OBJECT - Storage Pointer
	SYS_DATA_SERVICE_ADS1015,		ADS1015_I2C_Digital_Sensor::run,		&_sys->_sys->_sensors._ads1015_sensor,		&_sys->_ADS1015_driver_context,
	SYS_DATA_SERVICE_BMP180,		BMP180_I2C_Digital_Sensor::run,			&_sys->_sys->_sensors._bmp180_sensor,		&_sys->_BMP180_driver_context,
	SYS_DATA_SERVICE_SHT21,			SHT21_I2C_Digital_Sensor::run,			&_sys->_sys->_sensors._sht21_sensor,		&_sys->_SHT21_driver_context,
	SYS_DATA_SERVICE_TMP006,		TMP006_I2C_Digital_Sensor::run,			&_sys->_sys->_sensors._tmp006_sensor,		&_sys->_TMP006_driver_context,
	SYS_DATA_SERVICE_TMP102,		TMP102_I2C_Digital_Sensor::run,			&_sys->_sys->_sensors._tmp102_sensor,		&_sys->_TMP102_driver_context,
	SYS_DATA_SERVICE_DS1307,		DS1307_I2C_Digital_Sensor::run,			&_sys->_sys->_sensors._ds1307_sensor,		&_sys->_DS1307_time_context,
	SYS_DATA_SERVICE_NULL,			0x00,									0x00,										0x00

};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is a data getter function. It is used to acquire data from
 * each sensor on the I2C Bus.
 */
void SYSTEM_DATA_GETTER_THREAD(System_Context* context){

	// We only update the data once it is old.
	if(context->get_updated()){
		return;
	}

	// We use the table constructed above to get the data from the sensors
	for(register char i = 0; static_cast<unsigned int>(SYS_DATA_SERVICE_NULL) != data_services_table[i]; i++){

		data_service_table[i].service_method(data_service_table[i].context_object, data_service_table[i].storage_object);

	}

	// Once this information is set, the updated flag is set high
	context->set_updated();
}

#endif /* THREAD_DATA_GETTER_H_ */
