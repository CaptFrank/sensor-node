/*
 * SHT21I2CDigitalSensor.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef SHT21I2CDIGITALSENSOR_H_
#define SHT21I2CDIGITALSENSOR_H_

// -------------------------------------------------------------------------------
// Includes

// The base I2C Device
#include "I2CBaseDriver.h"

// -------------------------------------------------------------------------------
// Defines

/**
 * This is the device configuration enum
 */
enum SHT21_configs_t{

	SHT21_I2CADDR 						= 0x40	//!< SHT21_ADDR
};

/**
 * This is the available commands for the device
 */
enum SHT21_commands_t {

	SHT21_TEMPERATURE_HOLD			= 0xE3, //!< SHT21_TEMPERATURE_HOLD
	SHT21_RELHUMIDITY_HOLD			= 0xE5, //!< SHT21_RELHUMIDITY_HOLD
	SHT21_TEMPERATURE_NOHOLD		= 0xF3,	//!< SHT21_TEMPERATURE_NOHOLD
	SHT21_RELHUMIDITY_NOHOLD		= 0xF5 	//!< SHT21_RELHUMIDITY_NOHOLD
};

/**
 * SHT21 Internals - Monitoring
 */
struct SHT21_driver_context_t{

	// Measurments
	unsigned int 			humidity;
	unsigned int 			temperature;
};

/**
 * The measured data struct
 */
struct SHT21_measurements_t {

	unsigned int 			humidity;
	unsigned int 			temperature;
};

// -------------------------------------------------------------------------------
// Main code


/**
 * This is the SHT21 sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 */
class SHT21_I2C_Digital_Sensor: public I2C_Base_Driver {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class.
		 *
		 * @param context								- the system context
		 * @param master_address						- the master address
		 * @param device_map							- the device map
		 * @param device_address						- the device address
		 */
		SHT21_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
				  	  	  	  	  I2C_address device_address  = SHT21_I2CADDR);

		/**
		 * This sets up the device
		 */
		bool begin();

		/**
		 * The default run method
		 */
		static void run(void* object_pointer, void* _data){

			// Convert our pointer
			SHT21_I2C_Digital_Sensor* ptr = (SHT21_I2C_Digital_Sensor*)object_pointer;
			SHT21_driver_context_t* data = (SHT21_driver_context_t*) _data;

			// Get the data and store it
			ptr->get_humidity();
			ptr->get_temperature();

			// Store the data in the global context
			memcpy(data, ptr->get_measurments(), sizeof(SHT21_driver_context_t));
		}

		/**
		 * This gets the measurements.
		 *
		 * @return measurements						 	- the measured data
		 */
		SHT21_measurements_t* get_measurments(){
			return &this->_measurements;
		}

		/**
		 * This gets the measured humidity value
		 *
		 * @return double								- the humidity value
		 */
		void get_humidity(void);

		/**
		 * This gets the temperature measured
		 *
		 * @return double								- the temperature value
		 */
		void get_temperature(void);

	// Private Context
	private:

		//! The device address
		I2C_address _device_address;

		//! Private values
		SHT21_measurements_t _measurements;

		/**
		 * This reads a sensor by issuing a command
		 *
		 * @param command								- the read command
		 * @return int									- the raw sensor value
		 */
		unsigned int _read_sensor(char command);
};

#endif /* SHT21I2CDIGITALSENSOR_H_ */
