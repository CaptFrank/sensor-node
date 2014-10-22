/*
 * I2CBaseDriver.h
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#ifndef I2CBASEDRIVER_H_
#define I2CBASEDRIVER_H_

// -------------------------------------------------------------------------------
// Includes

// Standards
extern "C" {

#include <stdlib.h>
#include <string.h>
}

// The I2C interface
#include "i2c_if.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"

// The system context
#include "SensorDriver.h"

// -------------------------------------------------------------------------------
// Defines

#define STOP_BIT				1
#define NO_STOP_BIT				0

//! Define the I2C address map & address types
typedef char* I2C_device_list_t;
typedef char  I2C_address;

// -------------------------------------------------------------------------------
// Main code

class Sensor_Driver;
class System_Context;

/**
 * This is the I2C interface engine class handler. It hanldes all I2C communications
 * as well as handling of the I2C bus setting up. It extends the @see Sensor_Driver class.
 */
class I2C_Base_Driver : public Sensor_Driver {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class. Nothing
		 * happens here appart from setting the internal reference to the
		 * @see System_Context.
		 *
		 * @param context								- the device system context
		 * @param master_address						- the master address of the I2C device
		 * @param device_map							- the device map (addresses)
		 */
		I2C_Base_Driver(System_Context* context, I2C_address master_address){

			// Set the context
			this->_system_db = context;

			// Set the master address
			this->_master_address = master_address;

			// Set internal buffers
			this->_request._tx_data = &this->_buffers._tx_buffer;
			this->_request._rx_data = &this->_buffers._rx_buffer;

		}

		/**
		 * The default deconstructor of the class
		 */
		~I2C_Base_Driver();

		// Method overrides
		// Setup

		/**
		 * This is the generic setup method for a sensor.
		 */
		void setup();

		/**
		 * Checks if the device is online
		 *
		 * @param device_address						- the address of the device to check.
		 */
		bool check_presence(char device_address);

		// Global Data I/O

		/**
		 * This is the default read method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the read request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		virtual buffer_t* read_byte(request_t* request);

		/**
		 * This is the default write method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		virtual void write_byte(request_t* request);

		/**
		 * This is the default read method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the write request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		virtual buffer_t* read_bytes(request_t* request);

		/**
		 * This is the default write method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		virtual void write_bytes(request_t* request);

		// Register I/O

		/**
		 * This is the default read register method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the read request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		virtual buffer_t* read_reg(request_t* request);

		/**
		 * This is the default write register method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		virtual void write_reg(request_t* request);

	// Private Context
	private:

		//! The master address
		I2C_address _master_address;

};

#endif /* I2CBASEDRIVER_H_ */
