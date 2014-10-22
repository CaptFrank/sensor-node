/*
 * SensorDriver.h
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#ifndef SENSORDRIVER_H_
#define SENSORDRIVER_H_

// -------------------------------------------------------------------------------
// Includes

// Strings
extern "C" {

#include <string.h>
#include <stdlib.h>
}

// -------------------------------------------------------------------------------
// Defines

#define MAX_BUFFER_SIZE				255

/**
 * This is the buffer structure used to read and write data to and
 * from the divers digital busses used.
 */
typedef struct buffer_t {

	// The buffer of data
	unsigned char _buffer[MAX_BUFFER_SIZE];

	// The buffer langth
	char _length;

	// The validation flag
	bool _valid;
} buffer_t;

/**
 * This is the read/write request structure.
 */
typedef struct request_t {

	// The remote device address / pin
	unsigned char 	destination_device_address;

	// The remote device memory address
	unsigned int 	destination_memory_address;

	// The buffer to send/receive data to/from
	buffer_t*		_rx_data;
	buffer_t*		_tx_data;

	// Only used by the SPI bus
	unsigned int 	pin_port;
} request_t;

// -------------------------------------------------------------------------------
// Main code

class System_Context;

/**
 * This is the interface for all sensor drivers that are implemented within
 * the system. It provides a standard method layout that needs to be implemented
 * in each sensor class.
 */
class Sensor_Driver {

	// Public Context
	public:

		// Setup

		/**
		 * This is the generic setup method for a sensor.
		 */
		virtual void setup() = 0;

		// Run

		/**
		 * The entry point for each driver
		 */
		static void run(void* object_pointer);

		// Global Data I/O

		/**
		 * This is the default read method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the read request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		virtual buffer_t* read_byte(request_t* request) = 0;

		/**
		 * This is the default write method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		virtual void write_byte(request_t* request) = 0;

		/**
		 * This is the default read method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the write request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		virtual buffer_t* read_bytes(request_t* request) = 0;

		/**
		 * This is the default write method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		virtual void write_bytes(request_t* request) = 0;

		// Register I/O

		/**
		 * This is the default read register method for a sensor which returns a
		 * buffer type given a device address or pin.
		 *
		 * @param request								- the read request
		 * @return buffer_t								- the buffer type with the data in it
		 */
		virtual buffer_t* read_reg(request_t* request) = 0;

		/**
		 * This is the default write register method for any sensor type. We give it a buffer type
		 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
		 * to send the data.
		 *
		 * @param request								- the write request
		 */
		virtual void write_reg(request_t* request) = 0;

		//! System context
		System_Context* _system_db;

		//! Internal reference to the request type
		request_t _request;

		//! Buffer definitions
		struct {

			buffer_t _rx_buffer;
			buffer_t _tx_buffer;
		}_buffers;

		/**
		 * This private method creates a request either read or write to use within the
		 * driver context.
		 *
		 * @param dest_addr								- the device address
		 * @param rx_buffer								- the rx buffer
		 * @param tx_buffer								- the tx buffer
		 */
		request_t* _create_request(char dest_addr, char* tx_buffer, char tx_length,
							 char rx_length){

			// Set the values
			this->_request.destination_device_address = dest_addr;
			this->_request._rx_data->_length = rx_length;
			memcpy(this->_request._tx_data->_buffer, tx_buffer, tx_length);
			this->_buffers._tx_buffer._length = tx_length;
			return &this->_request;
		}
};



#endif /* SENSORDRIVER_H_ */
