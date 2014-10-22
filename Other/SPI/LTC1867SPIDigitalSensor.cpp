/*
* LTC1867SPIDigitalSensor.cpp
*
*  Created on: Jun 20, 2014
*      Author: Administrator
*/

#include "LTC1867SPIDigitalSensor.h"

// Public Context

/**
 * This is the default constructor for the class.
 *
 * @param context								- the system context
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
LTC1867_SPI_Digital_Sensor::LTC1867_SPI_Digital_Sensor(System_Context* context,
						  	  	  	  	  	  	  	  SPI_cs_port_t device_port,
						  	  	  	  	  	  	  	  SPI_cs_pin_t device_pin,
						  	  	  	  	  	  	  	  LTC18xx_device_t device_type) :
						  	  	  	  	  	  	  	  SPI_Base_Driver(context, device_port,
						  	  	  	  	  	  	  			  	      device_pin){

	// Set the internal attributes
	this->_device_configs._device_type = device_type;
	this->_device_configs._init_status = LTC1867_LTC186X_ERR | LTC1867_EEPROM_ERR;
	this->_device_configs._current_configs = LTC186X_CHAN_DIFF_0P_1N;
	this->_device_channel_map = malloc(NUM_DEVICES);
	memcpy(this->_device_channel_map, device_map, NUM_DEVICES);
}

/**
 * This is the begin engine method.
 */
bool LTC1867_SPI_Digital_Sensor::begin(){

	char retval = 0;

	// Set default status
	this->_device_configs._init_status = LTC1867_SUCCESS;

	// Set the configs
	this->_device_configs._current_configs = LTC186X_CHAN_DIFF_0P_1N;

	// Create a command
	char data[2] = {this->_device_configs._current_configs, 0x00};

	// Create a request
	request_t* req = this->_create_request(SPI_DUMMY, data, sizeof(data), SPI_DUMMY);

	// Write
	this->write_bytes(req);
	this->sleep();

	return this->_device_configs._init_status;
}

/**
 * Reads a value from the remote device.
 *
 * @return unsigned int							- the value read
 */
unsigned int LTC1867_SPI_Digital_Sensor::read(){

	this->wake();

	// Create a command
	char data[2];
	data[0] = this->_device_configs._current_configs;
	data[1] = 0; // read 1 byte

	// Create a request
	request_t* req = this->_create_request(SPI_DUMMY, data, sizeof(data), SPI_DUMMY);

	// Write the request
	this->write_bytes(req);

	if(!this->_request._rx_data->_valid)
		return 0;

	// Compute from type
	if (DEVICE_LTC1863 == this->_device_configs._device_type){

		retval = this->_request._rx_data->_buffer[0];
		retval <<= 4;
		retval |= 0x0F & (this->_request._rx_data->_buffer[1] >> 4);
	}
	else if (DEVICE_LTC1867 == this->_device_configs._device_type){

		retval = this->_request._rx_data->_buffer[0];
		retval <<= 8;
		retval |= 0x0F & (this->_request._rx_data->_buffer[1]);
	}

	this->sleep();

	return(retval);
}

/**
 * Reads a bipolar value from the remote device.
 *
 * @return int									- the value read
 */
int LTC1867_SPI_Digital_Sensor::read_bipolar(){

	this->wake();

	// Get the data
	int retval = (int)this->read();

	// Check the device type
	if (DEVICE_LTC1863 == this->_device_configs._device_type
		&& (retval & 0x0800))
		retval |= 0xF000;

	this->sleep();

	return(retval);
}

/**
 * This reads a value and changes the channel of measurment
 *
 * @param device_channel						- the channel
 * @return unsigned int							- the read value
 */
unsigned int LTC1867_SPI_Digital_Sensor::read_and_change_channel(char device_channel,
																	char unipolar = UNIPOLAR){

	// Change channels
	this->change_channel(device_channel, unipolar);
	return this->read();
}

/**
 * This reads a bipolar value and changes the channel of measurment
 *
 * @param device_channel						- the channel
 * @return int									- the read value
 */
int LTC1867_SPI_Digital_Sensor::read_and_change_channel_bipolar(char device_channel,
																	char unipolar = UNIPOLAR){

	// Change the channel
	this->change_channel(device_channel, unipolar);
	return this->read_bipolar();
}


/**
 * This changes the channel of measurment
 *
 * @param device_channel						- the new channel association
 */
void LTC1867_SPI_Digital_Sensor::change_channel(char device_channel, char unipolar = UNIPOLAR){

	this->wake();

	// Change the channel
	this->_internal_channel_change(device_channel, unipolar);

	// Create a command
	char data[2];
	data[0] = this->_device_configs._current_configs;
	data[1] = 0;

	// Create a request
	request_t* req = this->_create_request(SPI_DUMMY, data, sizeof(data), SPI_DUMMY);

	// do teh exchange
	this->write_bytes(req);

	this->sleep();
}

/**
 * This sleeps the device
 */
void LTC1867_SPI_Digital_Sensor::sleep(){

	// Update the config
	this->_device_configs._current_configs |= LTC186X_CONFIG_SLP;

	// Create a command
	char data[2];
	data[0] = this->_device_configs._current_configs;
	data[1] = 0; // read 1 byte

	// Create a request
	request_t* req = this->_create_request(SPI_DUMMY, data, sizeof(data), SPI_DUMMY);

	// We write the bytes
	this->write_bytes(req);
}

/**
 * This wakes the device
 */
void LTC1867_SPI_Digital_Sensor::wake(){

	// Get the current configs
	char was_sleeping = this->_device_configs._current_configs & LTC186X_CONFIG_SLP;

	// Update the configs
	this->_device_configs._current_configs &= ~LTC186X_CONFIG_SLP;

	// Create a command
	char data[2];
	data[0] = this->_device_configs._current_configs;
	data[1] = 0; // read 1 byte

	// Create a request
	request_t* req = this->_create_request(SPI_DUMMY, data, sizeof(data), SPI_DUMMY);

	// Write the buffer and read 1 byte
	this->write_bytes(req);

	// Wake-up time if we were really sleeping
	if (was_sleeping)
		MAP_UtilsDelay(700);

}

// Private Context

/**
 * Change the channel
 *
 * @param device_channel						- the channel to change
 */
void LTC1867_SPI_Digital_Sensor::_internal_channel_change(char device_channel, char unipolar){

	// Configure the internal configs
	this->_device_configs._current_configs = unipolar ? LTC186X_CONFIG_UNI : 0;
	this->_device_configs._current_configs |= (device_channel &
												(LTC186X_CONFIG_SINGLE_END |
												LTC186X_CONFIG_ODD |
												LTC186X_CONFIG_S1 |
												LTC186X_CONFIG_S0 |
												LTC186X_CONFIG_COM));
}

