/*
 * TMP006I2CDigitalSensor.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#include "TMP006I2CDigitalSensor.h"

// Public Context

/**
 * This is the default constructor for the class.
 *
 * @param context								- the system context
 * @param master_address						- the master address
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
TMP006_I2C_Digital_Sensor::TMP006_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
						  I2C_address device_address) :
						  I2C_Base_Driver(context, master_address){

	// Set the device address
	this->_device_address = device_address;
}

/**
 * This is the begin engine method.
 *
 * @param samplerate							- the read samplerate
 */
bool TMP006_I2C_Digital_Sensor::begin(int samplerate){

	// Containers
	unsigned int mid, did;


	if(!this->check_presence((char)this->_device_address)) return false;

	// Write an int
	this->_write_int(TMP006_CONFIG, TMP006_CFG_MODEON | TMP006_CFG_DRDYEN | samplerate);

	// Read values
	mid = this->_read_int(TMP006_MANID);
	did = this->_read_int(TMP006_DEVID);

	if (mid != 0x5449) return false;
	if (did != 0x67) return false;
	return true;

}

/**
 * This is the read internal temperature method.
 *
 * @return measurment							- the mesaured temperature
 */
double TMP006_I2C_Digital_Sensor::read_internal_temperature(){

	// Get the raw value
	double Tdie = this->read_raw_internal_temperature();

	// Convert to Celsius
	Tdie *= 0.03125;
	Tdie *= 10; // Gets rid of the decimals
	this->_measurments.internal_temperature = (unsigned int)Tdie;
	return Tdie;
}

/**
 * This is the read external temperature method.
 *
 * @return measurment							- the mesaured temperature
 */
double TMP006_I2C_Digital_Sensor::read_external_temperature(){

	// Get data
	double Tdie = this->read_raw_internal_temperature();
	double Vobj = this->read_raw_voltage();

	// Convert
	Vobj *= 156.25;  // 156.25 nV per LSB
	Vobj /= 1000; // nV -> uV
	Vobj /= 1000; // uV -> mV
	Vobj /= 1000; // mV -> V
	Tdie *= 0.03125; // convert to celsius
	Tdie += 273.15; // convert to kelvin

	// Remove the bias
	double tdie_tref = Tdie - TMP006_TREF;
	double S;
	S = (1 + (TMP006_A1 * tdie_tref));
	S += (TMP006_A2 * (tdie_tref * tdie_tref));

	S *= TMP006_S0;
	S /= 10000000;
	S /= 10000000;

	double Vos;
	Vos = TMP006_B0 + (TMP006_B1 * tdie_tref);
	Vos += (TMP006_B2 * (tdie_tref * tdie_tref));

	double fVobj;
	fVobj = (Vobj - Vos) + (TMP006_C2 * ((Vobj-Vos) * (Vobj-Vos)));

	double Tobj;
	Tobj = sqrt(sqrt((Tdie * Tdie * Tdie * Tdie) + fVobj/S));

	Tobj -= 273.15; // Kelvin -> *C
	Tobj *= 10;
	this->_measurments.external_temperature = (unsigned int)Tobj;
	return Tobj;
}

/**
 * This is the read internal raw temperature method.
 *
 * @return measurment							- the mesaured temperature
 */
int TMP006_I2C_Digital_Sensor::read_raw_internal_temperature(){

	// Get the raw value
	int raw = this->_read_int(TMP006_TAMP);
	raw >>= 2;
	return raw;
}

/**
 * This is the read external raw temperature method.
 *
 * @return measurment							- the mesaured temperature
 */
int TMP006_I2C_Digital_Sensor::read_raw_voltage(){

	// Get the value
	int raw = this->_read_int(TMP006_VOBJ);
	return raw;

}

// Private Context

/**
 * This is the read integer method.
 *
 * @param register								- the register to read
 * @return register								- the register value
 */
int TMP006_I2C_Digital_Sensor::_read_int(int reg){

	// The return value
	unsigned int ret;

	// Create a command
	char data[2];
	data[0] = reg >> 8;
	data[1] = reg << 8;

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x02);

	// Send the request and read
	this->read_reg(req);

	// Get the value
	ret = (this->_request._rx_data->_buffer[0] << 8) | (this->_request._rx_data->_buffer[1]);
	return ret;
}

/**
 * This is the write integrer method.
 *
 * @param reg									- the register to write to
 * @param value									- the value to write
 */
void TMP006_I2C_Digital_Sensor::_write_int(int reg, int value){

	// Create a command
	char data[4];
	data[0] = reg >> 8;
	data[1] = reg << 8;
	data[2] = value << 8;
	data[3] = value >> 8;

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x00);

	// Send the request
	this->write_reg(req);
}
