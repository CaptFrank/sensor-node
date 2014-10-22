/*
 * TMP102I2CDigitalSensor.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#include "TMP102I2CDigitalSensor.h"

// Public Context

/**
 * This is the default constructor for the class.
 *
 * @param context								- the system context
 * @param master_address						- the master address
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
TMP102_I2C_Digital_Sensor::TMP102_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
						  	  	  	  	  	  	  	  I2C_address device_address) :
						  	  	  	  	  	  	  	  I2C_Base_Driver(context, master_address){

	// Setup the internals
	this->_device_address = device_address;
	this->_error = 0x00;
	this->_temperature = 0;
}

/**
 * This is the default setup method for the device
 *
 * @return success								- the setup success
 */
bool TMP102_I2C_Digital_Sensor::begin(){

	if(!this->check_presence((char)this->_device_address)) return false;

	// CONFIG REG
	// Create a command to write in the config reg
	char data_reg[] = {TMP102_CONFIGURATION_REG,

							// BYTE 1
				   	   	   	   TMP102_SHUTDOWN |
				   	   	   	   TMP102_INTERRUPT_MODE |
				   	   	   	   TMP102_ALERT_ACTIVE_HIGH |
				   	   	   	   TMP102_1_FAULT,

							// BYTE 2
				   	   	   	   TMP102_EXTEND_MEASUREMENTS |
				   	   	   	   TMP102_DEFAULT_RATE};

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data_reg,
											sizeof(data_reg), 0x00);
	// Write it
	this->write_bytes(req);

	// MAX TEMP
	// Create a command to write in the config reg
	char htemp_reg[] = {TMP102_HIGH_ALERT_REG,
								(char)(MAX_TEMP_ALERT >> 8), (char)(MAX_TEMP_ALERT << 8)};

	// Create a request
	req = this->_create_request((char)this->_device_address, htemp_reg, sizeof(htemp_reg), 0x00);

	// Write it
	this->write_bytes(req);

	// MIN TEMP
	// Create a command to write in the config reg
	char ltemp_reg[] = {TMP102_LOW_ALERT_REG,
								(char)(MIN_TEMP_ALERT >> 8), (char)(MIN_TEMP_ALERT << 8)};

	// Create a request
	req = this->_create_request((char)this->_device_address, ltemp_reg,
											sizeof(ltemp_reg), 0x00);
	// Write it
	this->write_bytes(req);

	// Set internals
	this->_device_configs._configs = CONFIG_REG(data_reg);
	this->_device_configs._high_alert = CONFIG_REG(htemp_reg);
	this->_device_configs._low_alert = CONFIG_REG(ltemp_reg);

	// We sleep the device
	this->_sleep();

	// For conformity
	return true;
}

/**
 * This reads a temperature from the device.
 *
 * @return temperature							- the temperature
 */
void TMP102_I2C_Digital_Sensor::read_temperature(){

	if(this->_device_configs._state == TMP102_STATE_SLEEP){

		// We use the one shot conv
		// Create a command to write in the config reg
		char config_cmd[] = {TMP102_CONFIGURATION_REG,
							this->_device_configs._configs | TMP102_ONE_SHOT_CONV};

		// Create a request
		request_t* req = this->_create_request((char)this->_device_address, config_cmd,
												sizeof(config_cmd), sizeof(int));
		// Write it
		this->write_bytes(req);
	}

	// Call the read
	this->_read_temperature();
}

// Private Context

/**
 * This reads in the temperature internally
 *
 * @return int									- the temperature
 */
void TMP102_I2C_Digital_Sensor::_read_temperature(){

	// Create a command to write
	char temp_cmd[] = {TMP102_TMPERATURE_REG};

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, temp_cmd,
											sizeof(temp_cmd), sizeof(int));
	// Write it
	this->read_reg(req);

	// Get the data
	int temp = (this->_request._rx_data->_buffer[0] << 4);
	temp |= (this->_request._rx_data->_buffer[1] >> 4);

	double temperature = temp;
	temperature *= 0.0625;

	this->_temperature = temperature;
}

/**
 * This reads in the error flag and faults from the device
 */
void TMP102_I2C_Digital_Sensor::_read_error_flag(){

	// Read the config register
	// Create a command to write in the config reg
	char config_cmd[] = {TMP102_CONFIGURATION_REG};

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, config_cmd,
											sizeof(config_cmd), sizeof(int));
	// Write it
	this->read_reg(req);

	unsigned int config_reg = ((this->_request._rx_data->_buffer[0] << 8) |
								this->_request._rx_data->_buffer[1]);

	this->_error |= READ_ALERT(config_reg);
}

/**
 * This puts the device to sleep
 */
void TMP102_I2C_Digital_Sensor::_sleep(){

	if(this->_device_configs._state == TMP102_STATE_AWAKE){

		// Get the internal configs and add sleep
		// Create a command to write in the config reg
		char sleep_cmd[] = {TMP102_CONFIGURATION_REG,
									((this->_device_configs._configs >> 8) | TMP102_SHUTDOWN),
									(this->_device_configs._configs << 8)};

		// Create a request
		request_t* req = this->_create_request(this->_device_address, sleep_cmd,
												sizeof(sleep_cmd), 0x00);
		// Write it
		this->write_bytes(req);

		// Set state
		this->_device_configs._state = TMP102_STATE_SLEEP;
	}
}

/**
 * This wakes the device up to function fully
 */
void TMP102_I2C_Digital_Sensor::_wake(){

	if(this->_device_configs._state == TMP102_STATE_SLEEP){

		// Get the internal configs and add sleep
		// Create a command to write in the config reg
		char sleep_cmd[] = {TMP102_CONFIGURATION_REG,
									((this->_device_configs._configs >> 8) & ~TMP102_SHUTDOWN),
									(this->_device_configs._configs << 8)};

		// Create a request
		request_t* req = this->_create_request(this->_device_address, sleep_cmd,
												sizeof(sleep_cmd), 0x00);
		// Write it
		this->write_bytes(req);

		// Set state
		this->_device_configs._state = TMP102_STATE_AWAKE;
	}
}
