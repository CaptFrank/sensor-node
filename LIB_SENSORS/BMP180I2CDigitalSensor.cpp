/*
 * BMP180I2CDigitalSensor.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#include "BMP180I2CDigitalSensor.h"

// Public Context

// -----------------------------------------
// Setup methods

/**
 * This is the default constructor for the class.
 *
 * @param context								- the sys	tem context
 * @param master_address						- the master address
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
BMP180_I2C_Digital_Sensor::BMP180_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
						  	  	  	  	  	  	  	 I2C_address device_address) :
						  	  	  	  	  	  	  	 I2C_Base_Driver(context, master_address){

	// Set the internal device address
	this->_device_address = device_address;
}

/**
 * This is the default begin method for the class. it returns the status
 * of hte initialization.
 *
 * @return success								- the success of the steup method
 */
bool BMP180_I2C_Digital_Sensor::begin(){

	// Containers
	double c3, c4, b1;

	if(!this->check_presence((char)this->_device_address)) return false;

	// The BMP180 includes factory calibration data stored on the device.
	// Each device has different numbers, these must be retrieved and
	// used in the calculations when taking pressure measurements.

	// Retrieve calibration data from device:

	if ( (AC1 = 	_read_int(0xAA)) &&
		 (AC2 = 	_read_int(0xAC)) &&
		 (AC3 = 	_read_int(0xAE)) &&
		 (AC4 = 	_read_int(0xB0)) &&
		 (AC5 = 	_read_int(0xB2)) &&
		 (AC6 = 	_read_int(0xB4)) &&
		 (VB1 = 	_read_int(0xB6)) &&
		 (VB2 = 	_read_int(0xB8)) &&
		 (MB = 		_read_int(0xBA)) &&
		 (MC = 		_read_int(0xBC)) &&
		 (MD = 		_read_int(0xBE))){

		// All reads completed successfully!

		// If you need to check your math using known numbers,
		// you can uncomment one of these examples.
		// (The correct results are commented in the below functions.)

		// Example from Bosch datasheet
		// AC1 = 408; AC2 = -72; AC3 = -14383; AC4 = 32741; AC5 = 32757; AC6 = 23153;
		// B1 = 6190; B2 = 4; MB = -32768; MC = -8711; MD = 2868;

		// Example from http://wmrx00.sourceforge.net/Arduino/BMP180-Calcs.pdf
		// AC1 = 7911; AC2 = -934; AC3 = -14306; AC4 = 31567; AC5 = 25671; AC6 = 18974;
		// VB1 = 5498; VB2 = 46; MB = -32768; MC = -11075; MD = 2432;


		// Compute floating-point polynominals:

		c3 = 160.0 * pow(2,-15) * AC3;
		c4 = pow(10,-3) * pow(2,-15) * AC4;
		b1 = pow(160,2) * pow(2,-30) * VB1;
		c5 = (pow(2,-15) / 160) * AC5;
		c6 = AC6;
		mc = (pow(2,11) / pow(160,2)) * MC;
		md = MD / 160.0;
		x0 = AC1;
		x1 = 160.0 * pow(2,-13) * AC2;
		x2 = pow(160,2) * pow(2,-25) * VB2;
		y0 = c4 * pow(2,15);
		y1 = c4 * c3;
		y2 = c4 * b1;
		p0 = (3791.0 - 8.0) / 1600.0;
		p1 = 1.0 - 7357.0 * pow(2,-20);
		p2 = 3038.0 * 100.0 * pow(2,-36);

		this->_measurements.pressure_baseline = this->get_pressure();

		// Success!
		return(true);
	} else{
		// Error reading calibration data; bad component or connection?
		return(false);
	}
}

// -----------------------------------------
// Temperature methods

/**
 * This starts the temperature measurments. It returns the number of
 * ms to wait.
 *
 * @return ms									- the ms to wait
 */
char BMP180_I2C_Digital_Sensor::start_temperature(void){

	// Create a command
	char data[2];
	data[0] = BMP180_REG_CONTROL;
	data[1] = BMP180_COMMAND_TEMPERATURE;

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x00);

	// Issue the request
	this->write_reg(req);

	if (this->_request._tx_data->_valid) // good write?
		return(5); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return(0); // or return 0 if there was a problem communicating with the BMP
}

/**
 * This gets the temperature measurment. Returns 0 if the measurment has
 * failed.
 *
 * @return results								- the measured value
 */
double BMP180_I2C_Digital_Sensor::get_temperature(){

	double result;
	double tu, a;

	// Create a command
	char data[1];
	data[0] = BMP180_REG_RESULT;

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x02);

	// Read the reg
	this->read_reg(req);

	if (this->_request._rx_data->_valid){
		tu = (this->_request._rx_data->_buffer[0] * 256.0) +
				this->_request._rx_data->_buffer[1];

		//example from Bosch datasheet
		//tu = 27898;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf
		//tu = 0x69EC;

		a = c5 * (tu - c6);
		result = a + (mc / (a + md));

	}
	this->_measurements.temperature = result;
	return(result);

}

// -----------------------------------------
// Pressure methods

/**
 * This starts the pressure measurements.
 *
 * @param oversampling							- the oversampling setting
 * @return success								- the success of the setting
 */
char BMP180_I2C_Digital_Sensor::start_pressure(char oversampling){

	// Container
	unsigned char delay;

	// Create a command
	char data[2];
	data[0] = BMP180_REG_CONTROL;

	switch (oversampling){

		case 0:
			data[1] = BMP180_COMMAND_PRESSURE0;
			delay = 5;
		break;
		case 1:
			data[1] = BMP180_COMMAND_PRESSURE1;
			delay = 8;
		break;
		case 2:
			data[1] = BMP180_COMMAND_PRESSURE2;
			delay = 14;
		break;
		case 3:
			data[1] = BMP180_COMMAND_PRESSURE3;
			delay = 26;
		break;
		default:
			data[1] = BMP180_COMMAND_PRESSURE0;
			delay = 5;
		break;
	}

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x00);

	this->write_reg(req);
	return(delay); // return the delay in ms (rounded up) to wait before retrieving data
}

/**
 * This gets the pressure (mbar) value given the temperature value
 *
 * @return	pressure							- the calculated pressure value
 */
double BMP180_I2C_Digital_Sensor::get_pressure(){

	// Container
	double result = 0.00;
	double pu, s, x, y, z;

	// Create a command
	char data[1];
	data[0] = BMP180_REG_RESULT;

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x03);

	// Send the request
	this->read_reg(req);

	if(this->_request._rx_data->_valid){

		pu = (this->_request._rx_data->_buffer[0] * 256.0) +
				this->_request._rx_data->_buffer[1] +
				(this->_request._rx_data->_buffer[2]/256.0);

		//example from Bosch datasheet
		//pu = 23843;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf, pu = 0x982FC0;
		//pu = (0x98 * 256.0) + 0x2F + (0xC0/256.0);

		s = this->_measurements.temperature - 25.0;
		x = (x2 * pow(s,2)) + (x1 * s) + x0;
		y = (y2 * pow(s,2)) + (y1 * s) + y0;
		z = (pu - x) / y;
		result = (p2 * pow(z,2)) + (p1 * z) + p0;
	}

	this->_measurements.pressure = result;
	return(result);
}

// -----------------------------------------
// Calculations methods

/**
 * This converts the absolute pressure to sea level pressure.
 *
 * @param pressure								- pressure value
 * @param altitude								- altitude value
 *
 * @return sea-level pressure					- the sea-level pressure
 */
double BMP180_I2C_Digital_Sensor::sea_level(double pressure, double altitude){

	// Calculate
	return(pressure/pow(1-(altitude/44330.0),5.255));
}

/**
 * This converts the pressure measurements to an altitude.
 *
 * @param pressure								- the calculated pressure
 * @param initial_pressure						- the initial pressure
 * @return altitude								- the calculated altitude (m)
 */
double BMP180_I2C_Digital_Sensor::altitude(double pressure, double initial_pressure){

	// Calculate
	return(44330.0*(1-pow(pressure/initial_pressure,1/5.255)));
}

// Private Context

/**
 * This is the read integer method.
 *
 * @param register								- the register to read
 * @return register								- the register value
 */
int BMP180_I2C_Digital_Sensor::_read_int(int reg){

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
void BMP180_I2C_Digital_Sensor::_write_int(int reg, int value){

	// Create a command
	char data[4];
	data[0] = reg >> 8;
	data[1] = reg << 8;
	data[2] = value >> 8;
	data[3] = value << 8;

	// Create a request
	request_t* req = this->_create_request((char)this->_device_address, data, sizeof(data), 0x00);

	// Send the request
	this->write_reg(req);
}

