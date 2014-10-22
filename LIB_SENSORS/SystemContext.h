/*
 * SystemContext.h
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#ifndef SYSTEMCONTEXT_H_
#define SYSTEMCONTEXT_H_

// -------------------------------------------------------------------------------
// Includes

#include "Sensors.h"
#include "SDSPIDigitalStorage.h"
#include "DS1307I2CDigitalSensor.h"

// -------------------------------------------------------------------------------
// Defines

// Forward definitions

// Timer
class Timer;

// Storage
class SD_SPI_Digital_Storage;
class Event_Logger;

// Network
class NTP_Client;
class MQTT_Client;

/**
 * The following are pointers to the objects that we will be using
 * throughout the entire runtime of the device.
 */
struct system_t {
	/**
	 * We define a structure of sensor pointers
	 */
	struct sensor_t {
		ADS1015_I2C_Digital_Sensor* 	_ads1015_sensor;
		BMP180_I2C_Digital_Sensor* 		_bmp180_sensor;
		SHT21_I2C_Digital_Sensor*		_sht21_sensor;
		TMP006_I2C_Digital_Sensor*		_tmp006_sensor;
		TMP102_I2C_Digital_Sensor*		_tmp102_sensor;
		DS1307_I2C_Digital_Sensor*		_ds1307_sensor;
	}_sensors;

	/**
	 * We define a structure of logger pointers
	 */
	struct loggers_t{

		Event_Logger* 					_event_logger;
	} _loggers;

	/**
	 * We define a structure of network clients pointers
	 */
	struct network_t{

		MQTT_Client* 					_mqtt_client;
		NTP_Client*						_ntp_client;
	} _network;

	/**
	 * We define a structure of storage pointers
	 */
	struct storage_t{

		//EEPROM_I2C_Digital_Storage* 	_eeprom;
		SD_SPI_Digital_Storage*			_sd;
	} _storage;

	//! We define a system timer pointer
	Timer* 								_timer;
};

// -------------------------------------------------------------------------------
// Data Structures

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the system context object. It is a simple container
 * class that holds the systems own data and settings.
 */
class System_Context {

	// Public Context
	public:

		// --------------------------------------------------------
		// General

		/**
		 * This is the default constructor for the class.
		 */
		System_Context(struct system_t* sys){
			this->_sys = sys;
			this->_overheat = false;
		}

		/**
		 * This sets the global update flag
		 *
		 * This flag only toggles when the get_data processes are ran
		 */
		void set_updated(){
			this->_updated = true;
		}

		/**
		 * This unsets the global update flag
		 *
		 * This flag only toggles when the log_data processes are ran
		 */
		void unsets_updated(){
			this->_updated = false;
		}

		/**
		 * This gets the global update flag
		 *
		 * We only return a copy of the flag... This is good protection.
		 */
		bool get_updated(){
			bool copy = this->_updated;
			return copy;
		}


		// --------------------------------------------------------
		// System Data

		// Time Data - System Monitoring
		DS1307_time_context_t 		_DS1307_time_context;

		// --------------------------------------------------------
		// Sensor Data

		// TMP102 Internals - Environment Monitoring
		struct TMP102_driver_context_t 		_TMP102_driver_context;

		// BMP180 Internals - Environment Monitoring
		struct BMP180_driver_context_t 		_BMP180_driver_context;

		// SHT21 Internals - Environment Monitoring
		struct SHT21_driver_context_t 		_SHT21_driver_context;

		// TMP006 Internals - Environment Monitoring
		struct TMP006_driver_context_t	 	_TMP006_driver_context;

		// LTC1867 Internals - Environment Monitoring
		struct ADS1015_driver_context_t		_ADS1015_driver_context;

		//! The system structure
		struct system_t* 					_sys;

		//! State update
		bool _state_update;

		//! Overheat flag
		bool _overheat;

	// Protected Context
	protected:

		//! The event logger
		Event_Logger* _logger;

	// Private Context
	private:

		//! The global update flag
		bool _updated;
};


#endif /* SYSTEMCONTEXT_H_ */
