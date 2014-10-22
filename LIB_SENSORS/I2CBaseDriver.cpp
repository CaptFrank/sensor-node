/*
 * I2CBaseDriver.cpp
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#include "I2CBaseDriver.h"

// -------------------------------------------------------------------------------
// Class Deconstructor

/**
 * The default deconstructor of the class
 */
I2C_Base_Driver::~I2C_Base_Driver(){

	// Close the I2C bus
	I2C_IF_Close();
}

// -------------------------------------------------------------------------------
// Setup

/**
 * This is the generic setup method for a sensor.
 */
void I2C_Base_Driver::setup(){

	// Setup the I2C bus - Standard speed
	I2C_IF_Open(I2C_MASTER_MODE_STD);
}

/**
 * Checks if the device is online
 *
 * @param device_address						- the address of the device to check.
 */
bool I2C_Base_Driver::check_presence(char device_address){

	// Command - PING
	char data[] = {0x00};

	// Create a request
	request_t* req = this->_create_request(device_address, data, sizeof(data), 0x00);

	// Send the ping
	this->write_bytes(req);

	if(!this->_request._rx_data->_valid) return 0;
	return 1;
}

// -------------------------------------------------------------------------------
// General Data I/O

/**
 * This is the default read method for a sensor which returns a
 * buffer type given a device address or pin.
 *
 * @param request								- the read request
 * @return buffer_t								- the buffer type with the data in it
 */
buffer_t* I2C_Base_Driver::read_byte(request_t* request){

	// Set the length to 1
	request->_rx_data->_length = 0x01;
	return this->read_bytes(request);
}

/**
 * This is the default write method for any sensor type. We give it a buffer type
 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
 * to send the data.
 *
 * @param request								- the write request
 */
void I2C_Base_Driver::write_byte(request_t* request){

	// Call the internal write bytes with 1 byte
	request->_tx_data->_length = 0x01; // 1 Byte to write
	this->write_bytes(request);
}

/**
 * This is the default read method for a sensor which returns a
 * buffer type given a device address or pin.
 *
 * @param request								- the write request
 * @return buffer_t								- the buffer type with the data in it
 */
buffer_t* I2C_Base_Driver::read_bytes(request_t* request){

	// We call the I2C API for a read
	if(!I2C_IF_Read(request->destination_device_address,
					request->_rx_data->_buffer,
					request->_rx_data->_length)){

		// Data corrupt --- Not good
		request->_rx_data->_valid = false;

		// Return the internal struct
		return request->_rx_data;
	}

	// Validate the data
	request->_rx_data->_valid = true;

	// Return the struct
	return request->_rx_data;

}

/**
 * This is the default write method for any sensor type. We give it a buffer type
 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
 * to send the data.
 *
 * @param request								- the write request
 */
void I2C_Base_Driver::write_bytes(request_t* request){

	// We call the I2C API
	if(!I2C_IF_Write(request->destination_device_address,
					 request->_tx_data->_buffer,
					 request->_tx_data->_length, STOP_BIT)){

		// Data corrupt --- Not good
		request->_tx_data->_valid = false;
	}

	// Validate the data
	request->_tx_data->_valid = true;

}

// -------------------------------------------------------------------------------
// Register I/O

/**
 * This is the default read register method for a sensor which returns a
 * buffer type given a device address or pin.
 *
 * @param request								- the read request
 * @return buffer_t								- the buffer type with the data in it
 */
buffer_t* I2C_Base_Driver::read_reg(request_t* request){

	// We write the byte to read
	if(!I2C_IF_Write(request->destination_device_address,
					 request->_tx_data->_buffer,
					 request->_tx_data->_length,
					 NO_STOP_BIT)){

		// Data corrupt --- Not good
		request->_rx_data->_valid = false;

		// Return the internal struct
		return request->_rx_data;
	}

	// Then we just read the bytes
	return this->read_bytes(request);
}

/**
 * This is the default write register method for any sensor type. We give it a buffer type
 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
 * to send the data.
 *
 * @param request								- the write request
 */
void I2C_Base_Driver::write_reg(request_t* request){

	// Call other method
	this->write_bytes(request);
}
