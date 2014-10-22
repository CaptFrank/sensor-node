/*
 * ADS1015I2CDigitalSensor.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Administrator
 */

#include "ADS1015I2CDigitalSensor.h"

/**
 * This is the default constructor for the class.
 *
 * @param context								- the system context
 * @param master_address						- the master address
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
ADS1015_I2C_Digital_Sensor::ADS1015_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
						  	  	  	  	  	  	  	I2C_address device_address) :
						  	  	  	  	  	  	  	I2C_Base_Driver(context, master_address){

	// Set the internals
	this->_device_address = device_address;
	this->_configs.conversion_delay =  ADS1015_CONVERSIONDELAY;
	this->_configs.bit_shift = 4;
	this->_configs.gain = GAIN_TWOTHIRDS;
}

/**
 * This sets up the device
 */
bool ADS1015_I2C_Digital_Sensor::begin(){

	// Check presence
	return this->check_presence(this->_device_address);
}

/**
 * This reads the adc single ended
 * @param channel								- the channel to read
 * @return unsigned int							- the value read
 */
unsigned int ADS1015_I2C_Digital_Sensor::read_single_ended(char channel){

	// Check
	if (channel > 3){
		return 0;
	}

	// Start with default values
	unsigned int config = 	ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
							ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
							ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
							ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
							ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
							ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

	// Set PGA/voltage range
	config |= this->_configs.gain;

	// Set single-ended input channel
	switch (channel) {

		case (0):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
			break;
		case (1):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
			break;
		case (2):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
			break;
		case (3):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
			break;
	}

	// Set 'start single-conversion' bit
	config |= ADS1015_REG_CONFIG_OS_SINGLE;

	// Create a comamnd
	char data[] = {ADS1015_REG_POINTER_CONFIG, (char)config >> 8, (char)config << 8};

	// Create a req
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 0);

	// Read the reg
	this->write_bytes(req);

	// Wait for the conversion to complete
	MAP_UtilsDelay(this->_configs.conversion_delay *100);

	// Read the conversion results
	// Shift 12-bit results right 4 bits for the ADS1015

	// Create a comamnd
	char data1[] = {ADS1015_REG_POINTER_CONVERT};

	// Create a req
	req = this->_create_request(this->_device_address, data1, sizeof(data1), 2);

	// Read the reg
	this->read_reg(req);

	// Get the data
	unsigned int res = (this->_request._rx_data->_buffer[0] << 8) | this->_request._rx_data->_buffer[1];

	// We shift the data
	res >>= this->_configs.bit_shift;

	return res;
}

/**
 * This reads the adc in a differential mode for channels 0 and 1
 *
 * @return int									- the value read
 */
int ADS1015_I2C_Digital_Sensor::read_differential_0_1(void){


	// NEVER GOING TO USE THIS
	return 0;
}

/**
 * This reads the adc in a differential mode for channels 2 and 3
 *
 * @return int									- the value read
 */
int ADS1015_I2C_Digital_Sensor::read_differential_2_3(void){

	// NEVER GOING TO USE THIS
	return 0;
}

/**
 * This starts the comparator for single ended reads
 *
 * @param channel								- the channel
 * @param threshold								- the threashold
 */
void ADS1015_I2C_Digital_Sensor::start_comparator_single_ended(char channel, int threshold){

	// Start with default values
	unsigned int config = ADS1015_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
					  	  ADS1015_REG_CONFIG_CLAT_LATCH   | // Latching mode
					  	  ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
					  	  ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
					  	  ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
					  	  ADS1015_REG_CONFIG_MODE_CONTIN  | // Continuous conversion mode
					  	  ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

	// Set PGA/voltage range
	config |= this->_configs.gain;

	// Set single-ended input channel
	switch (channel){

		case (0):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
			break;
		case (1):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
			break;
		case (2):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
			break;
		case (3):
			config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
			break;
	}

	// Set the high threshold register
	// Shift 12-bit results left 4 bits for the ADS1015
	// Create a comamnd
	char data[] = {ADS1015_REG_POINTER_HITHRESH, (char)threshold >> 8, (char)threshold << 8};

	// Create a req
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 0);

	// Read the reg
	this->write_bytes(req);


	// Write config register to the ADC
	char data1[] = {ADS1015_REG_POINTER_CONFIG, (char)config >> 8, (char)config << 8};

	// Create a req
	req = this->_create_request(this->_device_address, data1, sizeof(data1), 0);

	// Read the reg
	this->write_bytes(req);
}

/**
 * This gets the conversion results
 *
 * @return int									- the read value
 */
int ADS1015_I2C_Digital_Sensor::get_last_conversion_results(){

	// Create a comamnd
	char data[] = {ADS1015_REG_POINTER_CONVERT};

	// Create a req
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 2);

	// Read the reg
	this->read_reg(req);

	// Get the data
	unsigned int res = (this->_request._rx_data->_buffer[0] << 8) | this->_request._rx_data->_buffer[1];

	// We shift the data
	res >>= this->_configs.bit_shift;

	// Shift 12-bit results right 4 bits for the ADS1015,
	// making sure we keep the sign bit intact
	if (res > 0x07FF){
		// negative number - extend the sign to 16th bit
		res |= 0xF000;
	}

	return res;
}

/**
 * This sets a gain
 *
 * @param gain									- the gains
 */
void ADS1015_I2C_Digital_Sensor::setGain(ADS1015_gain_t gain){

	// Set the internal gain
	this->_configs.gain = gain;
}

/**
 * This gets a set gain
 *
 * @return
 */
ADS1015_gain_t ADS1015_I2C_Digital_Sensor::get_gain(void){

	// Return the gain
	return this->_configs.gain;
}
