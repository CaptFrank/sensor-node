/*
 * SHT21I2CDigitalSensor.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#include "SHT21I2CDigitalSensor.h"

// Public Context

/**
 * This is the default constructor for the class.
 *
 * @param context								- the system context
 * @param master_address						- the master address
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
SHT21_I2C_Digital_Sensor::SHT21_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
						  I2C_address device_address) :
						  I2C_Base_Driver(context, master_address){

	// Set the device address
	this->_device_address = device_address;;
}

/**
 * This sets up the device
 */
bool SHT21_I2C_Digital_Sensor::begin(){

	if(!this->check_presence((char)this->_device_address)) return false;

	// We read the sensor
	// Read temperature
	this->get_temperature();

	// Read himudity
	this->get_humidity();

	if((this->_measurements.humidity != 0) &&
			this->_measurements.temperature != 0) return true;
	return false;
}


/**
 * This gets the measured humidity value
 *
 * @return double								- the humidity value
 */
void SHT21_I2C_Digital_Sensor::get_humidity(void){

	// Get and convert
	this->_measurements.humidity = (unsigned int)((-6.0 + 125.0 / 65536.0 *
				(double)(this->_read_sensor(SHT21_RELHUMIDITY_HOLD)))*10);
}

/**
 * This gets the temperature measured
 *
 * @return double								- the temperature value
 */
void SHT21_I2C_Digital_Sensor::get_temperature(void){

	// Get and convert
	this->_measurements.temperature = (unsigned int)((-46.85 + 175.72 / 65536.0 *
				(double)(this->_read_sensor(SHT21_TEMPERATURE_HOLD)))*10);
}

// Private Context

/**
 * This reads a sensor by issuing a command
 *
 * @param command								- the read command
 * @return int									- the raw sensor value
 */
unsigned int SHT21_I2C_Digital_Sensor::_read_sensor(char command){

	// Result container
	unsigned int result;

	// Create a command
	char data[1];
	data[0] = command;

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x02);

	// Get the value
	result = ((this->_request._rx_data->_buffer[0] << 8) +
			 (this->_request._rx_data->_buffer[1])) & ~0x0003;

	return result;

}
