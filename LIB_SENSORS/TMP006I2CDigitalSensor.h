/*
 * TMP006I2CDigitalSensor.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef TMP006I2CDIGITALSENSOR_H_
#define TMP006I2CDIGITALSENSOR_H_

// -------------------------------------------------------------------------------
// Includes

// Standards
extern "C" {

#include <math.h>
}

// The base I2C Device
#include "I2CBaseDriver.h"

// -------------------------------------------------------------------------------
// Defines

/**
 * These are some reference constants for the TMP006
 */

#define TMP006_B0				 -0.0000294   	//!< TMP006_B0
#define TMP006_B1				 -0.00000057  	//!< TMP006_B1
#define TMP006_B2				 0.00000000463	//!< TMP006_B2
#define TMP006_C2				 13.4         	//!< TMP006_C2
#define TMP006_A1				 0.00175      	//!< TMP006_A1
#define TMP006_A2				 -0.00001678  	//!< TMP006_A2
#define TMP006_S0				 6.4        	//!< TMP006_S0
#define TMP006_TREF				 298.15    		//!< TMP006_TREF


/**
 * These are the configurations for the TMP006 sensor
 */
enum TMP006_commands_t {

	TMP006_CONFIG			= 0x02,       		//!< TMP006_CONFIG
	TMP006_CFG_RESET		= 0x8000,   		//!< TMP006_CFG_RESET
	TMP006_CFG_MODEON		= 0x7000,   		//!< TMP006_CFG_MOEON
	TMP006_CFG_1SAMPLE		= 0x0000, 			//!< TMP006_CFG_1SAMPLE
	TMP006_CFG_2SAMPLE		= 0x0200, 			//!< TMP006_CFG_2SAMPLE
	TMP006_CFG_4SAMPLE		= 0x0400, 			//!< TMP006_CFG_4SAMPLE
	TMP006_CFG_8SAMPLE		= 0x0600, 			//!< TMP006_CFG_8SAMPLE
	TMP006_CFG_16SAMPLE		= 0x0800,			//!< TMP006_CFG_16SAMPLE
	TMP006_CFG_DRDYEN		= 0x0100,  			//!< TMP006_CFG_DRDYEN
	TMP006_CFG_DRDY			= 0x0080    		//!< TMP006_CFG_DRDY
};

/**
 * These are the available device addresses.
 */
enum TMP006_address_t {

	TMP006_I2CADDR			= 0x40,				//!< TMP006_I2CADDR
	TMP006_MANID			= 0xFE,  			//!< TMP006_MANID
	TMP006_DEVID			= 0xFF,  			//!< TMP006_DEVID
	TMP006_VOBJ				= 0x00,  			//!< TMP006_VOBJ
	TMP006_TAMP				= 0x01   			//!< TMP006_TAMP

};

/**
 * TMP006 Internals - Monitoring
 */
struct TMP006_driver_context_t{

	// Measurments
	double 			internal_temperature;
	double 			external_temperature;
};

/**
 * The measurments
 */
struct TMP006_measurements_t{

	unsigned int internal_temperature;
	unsigned int external_temperature;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the TMP006 sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 */
class TMP006_I2C_Digital_Sensor: public I2C_Base_Driver {

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
		TMP006_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
								  I2C_address device_address  = TMP006_I2CADDR);

		/**
		 * This is the begin engine method.
		 *
		 * @param samplerate							- the read samplerate
		 */
		bool begin(int samplerate = TMP006_CFG_16SAMPLE);

		/**
		 * This is the default run method
		 */
		static void run(void* object_pointer, void* _data){

			// Convert the object pointer
			TMP006_I2C_Digital_Sensor* ptr = (TMP006_I2C_Digital_Sensor*)object_pointer;
			TMP006_driver_context_t* data = (TMP006_driver_context_t*) _data;

			// Get the data
			ptr->read_external_temperature();
			ptr->read_internal_temperature();

			// memcpy the data
			memcpy(data, ptr->get_measurments(),
					sizeof(TMP006_driver_context_t));
		}

		/**
		 * Gets the measurments
		 *
		 * @return measurements							- the measurement struct
		 */
		TMP006_measurements_t* get_measurments(){
			return &this->_measurments;
		}

		/**
		 * This is the read internal temperature method.
		 *
		 * @return measurment							- the mesaured temperature
		 */
		double read_internal_temperature();

		/**
		 * This is the read external temperature method.
		 *
		 * @return measurment							- the mesaured temperature
		 */
		double read_external_temperature();

		/**
		 * This is the read internal raw temperature method.
		 *
		 * @return measurment							- the mesaured temperature
		 */
		int read_raw_internal_temperature();

		/**
		 * This is the read external raw temperature method.
		 *
		 * @return measurment							- the mesaured temperature
		 */
		int read_raw_voltage();

	// Private Context
	private:

		//! The device address
		I2C_address _device_address;

		//! Private access
		TMP006_measurements_t _measurments;

		/**
		 * This is the read integer method.
		 *
		 * @param register								- the register to read
		 * @return register								- the register value
		 */
		int _read_int(int reg);

		/**
		 * This is the write integrer method.
		 *
		 * @param reg									- the register to write to
		 * @param value									- the value to write
		 */
		void _write_int(int reg, int value);
};

#endif /* TMP006I2CDIGITALSENSOR_H_ */
