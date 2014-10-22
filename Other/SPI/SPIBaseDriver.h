/*
 * SPIBaseDriver.h
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#ifndef SPIBASEDRIVER_H_
#define SPIBASEDRIVER_H_

// -------------------------------------------------------------------------------
// Includes

// Standards
#include <stdlib.h>
#include <string.h>

// The SPI interface
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "gpio_if.h"

// The system context
#include "../SensorDriver.h"

// -------------------------------------------------------------------------------
// Defines

//! Define the SPI CS States
#define STOP_SPI			1
#define START_SPI			0
#define SUCCESS				0
#define SPI_DUMMY			0x00

//! Define the SPI address map & address types
typedef char* SPI_device_list_t;
typedef char* SPI_cs_pin_t;
typedef unsigned long SPI_cs_port_t;

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the SPI interface engine class handler. It hanldes all SPI communications
 * as well as handling of the SPI bus setting up. It extends the @see Sensor_Driver class.
 */
class SPI_Base_Driver: public Sensor_Driver {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class. Nothing
		 * happens here appart from setting the internal reference to the
		 * @see System_Context.
		 *
		 * @param context								- the device system context
		 * @param device_map							- the device map (addresses)
		 */
		SPI_Base_Driver(System_Context* context,
						SPI_cs_port_t cs_port,
						SPI_cs_pin_t cs_pin){


			// Set the context
			this->_system_db = context;

			// Set the GPIO port
			this->_port = cs_port;

			// Set the pin
			this->_pin = cs_pin;

			// Set internal buffers
			this->_request._tx_data = &this->_buffers._tx_buffer;
			this->_request._rx_data = &this->_buffers._rx_buffer;
		}

		/**
		 * The default deconstructor of the class
		 */
		~SPI_Base_Driver()

		// Method overrides
		// Setup

		/**
		 * This is the generic setup method for a sensor.
		 */
		void setup();

		// Global Data I/O

		/**
		 * This is the default read method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the read request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		buffer_t* read_byte(request_t* request);

		/**
		 * This is the default write method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		void write_byte(request_t* request);

		/**
		 * This is the default read method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the write request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		buffer_t* read_bytes(request_t* request);

		/**
		 * This is the default write method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		void write_bytes(request_t* request);

		// Register I/O

		/**
		 * This is the default read register method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the read request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		buffer_t* read_reg(request_t* request);

		/**
		 * This is the default write register method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		void write_reg(request_t* request);

	// Private Context
	private:

		//! The device CS pin port
		SPI_cs_port_t _port;

		//! The device CS pin
		SPI_cs_pin_t _pin;

};

#endif /* SPIBASEDRIVER_H_ */
