/*
 * TMP102I2CDigitalSensor.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef TMP102I2CDIGITALSENSOR_H_
#define TMP102I2CDIGITALSENSOR_H_

// -------------------------------------------------------------------------------
// Includes

// The base I2C Device
#include "I2CBaseDriver.h"

// -------------------------------------------------------------------------------
// Defines

// Some constants that need to be configured
#define MAX_TEMP_ALERT			0x0320
#define MIN_TEMP_ALERT			0x0190
#define ALERT_MASK				(~0x20)

// Some macros to ease the job
#define CONFIG_REG(reg) 	((reg[0] << 8) | reg[1])
	//! Takes in a char* and returns an int

#define READ_ALERT(reg)		(reg & ALERT_MASK)
	//! Takes in an unsigned int and returns a bool

/**
 * These are the available registers to read and write from.
 */
enum TMP102_registers_t {

	TMP102_ADDRESS				= 0x92,						//!< TMP102_ADDRESS

	TMP102_TMPERATURE_REG		= 0x00,  					//!< TMP102_TMPERATURE_REG
	TMP102_CONFIGURATION_REG	= 0x01,						//!< TMP102_CONFIGURATION_REG
	TMP102_LOW_ALERT_REG		= 0x02,   					//!< TMP102_LOW_ALERT_REG
	TMP102_HIGH_ALERT_REG		= 0x03,   					//!< TMP102_HIGH_ALERT_REG

	TMP102_RESET				= 0x06						//!< TMP102_RESET

};

/**
 * These are the congifuration bits and the modes that are possible for the device (reg1).
 */
enum TMP102_config_reg1_t {

	// READ/WRITE
	TMP102_SHUTDOWN_BIT			= 0x01,         			//!< TMP102_SHUTDOWN_BIT
	TMP102_THERMOSTAT_BIT		= 0x02,         			//!< TMP102_THERMOSTAT_BIT
	TMP102_POLARITY_BIT			= 0x04,         			//!< TMP102_POLARITY_BIT
	TMP102_FAULT_MODE_BIT0		= 0x08,       				//!< TMP102_FAULT_MODE_BIT0
	TMP102_FAULT_MODE_BIT1		= 0x10,         			//!< TMP102_FAULT_MODE_BIT1
	TMP102_ONE_SHOT_BIT 		= 0x80,         			//!< TMP102_ONE_SHOT_BIT

	// READ ONLY
	TMP102_RESOLUTION_BIT0		= 0x20,         			//!< TMP102_RESOLUTION_BIT0
	TMP102_RESOLUTION_BIT1		= 0x40,         			//!< TMP102_RESOLUTION_BIT1

	// MODES

	// SHUTDOWN
	TMP102_SHUTDOWN 			= TMP102_SHUTDOWN_BIT,  	//!< TMP102_SHUTDOWN
	TMP102_WAKE					= (~TMP102_SHUTDOWN_BIT),   //!< TMP102_WAKE

	// THERMOSTAT
	TMP102_COMPARATOR_MODE		= (~TMP102_THERMOSTAT_BIT), //!< TMP102_COMPARATOR_MODE
	TMP102_INTERRUPT_MODE		= TMP102_THERMOSTAT_BIT,   	//!< TMP102_INTERRUPT_MODE

	// POLARITY
	TMP102_ALERT_ACTIVE_LOW		= (~TMP102_POLARITY_BIT),   //!< TMP102_ALERT_ACTIVE_LOW
	TMP102_ALERT_ACTIVE_HIGH	= TMP102_POLARITY_BIT,      //!< TMP102_ALERT_ACTIVE_HIGH

	// FAULT
	TMP102_1_FAULT				= (~TMP102_FAULT_MODE_BIT0 | ~TMP102_FAULT_MODE_BIT1),
															//!< TMP102_1_FAULT

	TMP102_2_FAULTS				= (TMP102_FAULT_MODE_BIT0 | ~TMP102_FAULT_MODE_BIT1),
															//!< TMP102_2_FAULTS

	TMP102_4_FAULTS				= (~TMP102_FAULT_MODE_BIT0 | TMP102_FAULT_MODE_BIT1),
															//!< TMP102_4_FAULTS

	TMP102_6_FAULTS				= (TMP102_FAULT_MODE_BIT0 | TMP102_FAULT_MODE_BIT1),
															//!< TMP102_6_FAULTS

	// ONE SHOT
	TMP102_ONE_SHOT_CONV		= TMP102_ONE_SHOT_BIT       //!< TMP102_ONE_SHOT_CONV
};

/**
 * These are the configuration bits and modes for the device (reg2).
 */
enum TMP102_config_reg2_t{

	// READ/WRITE
	TMP102_EXTEND_BIT			= 0x10,                     //!< TMP102_EXTEND_BIT
	TMP102_CONVRATE_BIT1		= 0x30,                     //!< TMP102_CONVRATE_BIT1
	TMP102_CONVRATE_BIT0		= 0x40,                     //!< TMP102_CONVRATE_BIT0

	// READ ONLY
	TMP102_ALERT_BIT			= 0x20,                     //!< TMP102_ALERT_BIT

	// MODES

	// MEASURMENT
	TMP102_EXTEND_MEASUREMENTS 	= TMP102_EXTEND_BIT,       	//!< TMP102_EXTEND_MEASUREMENTS

	// CONVERSION RATE
	TMP102_CONVERT_025Hz		= (~TMP102_CONVRATE_BIT0 | ~TMP102_CONVRATE_BIT1),
															//!< TMP102_CONVERT_025Hz

	TMP102_CONVERT_1Hz			= (TMP102_CONVRATE_BIT0 | ~TMP102_CONVRATE_BIT1),
															//!< TMP102_CONVERT_1Hz

	TMP102_CONVERT_4Hz			= (~TMP102_CONVRATE_BIT0 | TMP102_CONVRATE_BIT1),
															//!< TMP102_CONVERT_4Hz

	TMP102_CONVERT_8Hz			= (TMP102_CONVRATE_BIT0 | TMP102_CONVRATE_BIT1),
															//!< TMP102_CONVERT_8Hz

	TMP102_DEFAULT_RATE			= TMP102_CONVERT_4Hz       //!< TMP102_DEFAULT_RATE

};

/**
 * These are the states that are possible
 */
enum TMP102_state_t {

	TMP102_STATE_SLEEP,										//!< TMP102_STATE_SLEEP
	TMP102_STATE_AWAKE 										//!< TMP102_STATE_AWAKE

};


/**
 * TMP102 Internals - Monitoring
 */
struct TMP102_driver_context_t{

	// State information
	struct {
		bool 			error;
		TMP102_state_t 	state;
	} state;

	// Measurement
	double 			temperature;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the TMP102 sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 */
class TMP102_I2C_Digital_Sensor : public I2C_Base_Driver {

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
		TMP102_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
								  I2C_address device_address  = TMP102_ADDRESS);

		/**
		 * This is the default setup method for the device
		 *
		 * @return success								- the setup success
		 */
		bool begin();

		/**
		 * This is the default run method
		 */
		static void run(void* object_pointer, void* _data){

			// Convert our pointer
			TMP102_I2C_Digital_Sensor* ptr = (TMP102_I2C_Digital_Sensor*) object_pointer;
			TMP102_driver_context_t* data = (TMP102_driver_context_t*) _data;

			// Get the data
			ptr->read_temperature();

			// Set the new data
			data->state.error = ptr->get_error();
			data->state.state = ptr->get_state();
			data->temperature = (unsigned int)(ptr->get_temperature() * 10); // Gets rid of the decimals
		}

		/**
		 * This reads a temperature from the device.
		 *
		 * @return temperature							- the temperature
		 */
		void read_temperature();

		/**
		 * This gets the internal variable
		 *
		 * @return error								- the error flag
		 */
		bool get_error(){
			return this->_error;
		}

		/**
		 * Gets the internal state flag
		 *
		 * @return state								- the state
		 */
		TMP102_state_t get_state(){
			return this->_device_configs._state;
		}

		/**
		 * Gets the internal temperature
		 *
		 * @return temperature							- the temperature
		 */
		double get_temperature(){
			return this->_temperature;
		}

	// Private Context
	private:

		//! The address of the device
		I2C_address _device_address;

		//! Internal storage of the temperature
		double _temperature;

		//! Stores weather their is an error or alert.
		bool _error;

		//! Device configs
		struct {

			unsigned int _configs;
			unsigned int _low_alert;
			unsigned int _high_alert;
			TMP102_state_t _state;

		}_device_configs;

		/**
		 * This reads in the temperature internally
		 *
		 * @return int									- the temperature
		 */
		void _read_temperature();

		/**
		 * This reads in the error flag and faults from the device
		 */
		void _read_error_flag();

		/**
		 * This puts the device to sleep
		 */
		void _sleep();

		/**
		 * This wakes the device up to function fully
		 */
		void _wake();

		/**
		 * This is the power-up reset routine
		 */
		void _reset();

};

#endif /* TMP102I2CDIGITALSENSOR_H_ */
