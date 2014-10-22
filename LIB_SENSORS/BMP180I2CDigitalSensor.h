/*
 * BMP180I2CDigitalSensor.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef BMP180I2CDIGITALSENSOR_H_
#define BMP180I2CDIGITALSENSOR_H_

// -------------------------------------------------------------------------------
// Includes

// Standards
extern "C" {

#include <math.h>
}

// The base I2C Device
#include "I2CBaseDriver.h"

// -------------------------------------------------------------------------------
// Defines

/**
 * These are the registers that are used within the context of this class.
 */
enum BMP180_registers_t {

	// Address
	BMP180_I2CADDR					= 0x77,   		//!< BMP180_I2CADDR

	// Registers
	BMP180_REG_CONTROL				= 0xF4,			//!< BMP180_REG_CONTROL
	BMP180_REG_RESULT				= 0xF6  		//!< BMP180_REG_RESULT
};

/**
 * These are the various commands that are possible.
 */
enum BMP180_commands_t {

	// Commands
	BMP180_COMMAND_TEMPERATURE		= 0x2E,			//!< BMP180_COMMAND_TEMPERATURE
	BMP180_COMMAND_PRESSURE0		= 0x34,  		//!< BMP180_COMMAND_PRESSURE0
	BMP180_COMMAND_PRESSURE1		= 0x74,  		//!< BMP180_COMMAND_PRESSURE1
	BMP180_COMMAND_PRESSURE2		= 0xB4,  		//!< BMP180_COMMAND_PRESSURE2
	BMP180_COMMAND_PRESSURE3		= 0xF4   		//!< BMP180_COMMAND_PRESSURE3

};

/**
 * BMP180 Internals - Monitoring
 */
struct BMP180_driver_context_t{

	// Measurments
	unsigned int 			pressure;
	unsigned int 			pressure_baseline;
	unsigned int 			altitude;
	unsigned int 			sea_level;
	unsigned int 			temperature;
};

/**
 * The BMP180 sensor data struct
 */
struct BMP180_measurments_t{
	unsigned int 			pressure;
	unsigned int 			pressure_baseline;
	unsigned int 			altitude;
	unsigned int 			sea_level;
	unsigned int 			temperature;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the BMP180 sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 */
class BMP180_I2C_Digital_Sensor : public I2C_Base_Driver {

	// Public Context
	public:

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
		BMP180_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
								  I2C_address device_address  = BMP180_I2CADDR);

		/**
		 * This is the default begin method for the class. it returns the status
		 * of hte initialization.
		 *
		 * @return success								- the success of the steup method
		 */
		bool begin();

		/**
		 * The default run method
		 */
		static void run(void* object_pointer, void* _data){

			// Convert our pointer
			BMP180_I2C_Digital_Sensor* ptr = (BMP180_I2C_Digital_Sensor*)object_pointer;
			BMP180_measurments_t* data = (BMP180_measurments_t*) _data;

			// Get the data

			// Temperature
			char delay = ptr->start_temperature();
			MAP_UtilsDelay(delay *10);
			ptr->_measurements.temperature = (unsigned int)(ptr->get_temperature()*10);

			// Pressure
			delay = ptr->start_pressure(3);
			MAP_UtilsDelay(delay *10);
			ptr->_measurements.pressure = (unsigned int)(ptr->get_pressure()*10);

			// Altitude and sea level
			ptr->_measurements.altitude = (unsigned int)(ptr->altitude(ptr->_measurements.pressure,
					ptr->_measurements.pressure_baseline)*10);
			ptr->_measurements.sea_level = (unsigned int)(ptr->sea_level(ptr->_measurements.pressure,
					ptr->_measurements.altitude)*10);

			// Memcopy the data
			memcpy(data, &ptr->_measurements, sizeof(ptr->_measurements));
		}

		// -----------------------------------------
		// Temperature methods

		/**
		 * This starts the temperature measurments. It returns the number of
		 * ms to wait.
		 *
		 * @return ms									- the ms to wait
		 */
		char start_temperature(void);

		/**
		 * This gets the temperature measurment. Returns 0 if the measurment has
		 * failed.
		 *
		 * @return results								- the measured value
		 */
		double get_temperature();

		// -----------------------------------------
		// Pressure methods

		/**
		 * This starts the pressure measurements.
		 *
		 * @param oversampling							- the oversampling setting
		 * @return delay								- the delay
		 */
		char start_pressure(char oversampling);

		/**
		 * This gets the pressure (mbar) value given the temperature value
		 *
		 * @return	pressure							- the calculated pressure value
		 */
		double get_pressure();

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
		double sea_level(double pressure, double altitude);

		/**
		 * This converts the pressure measurements to an altitude.
		 *
		 * @param pressure								- the calculated pressure
		 * @param initial_pressure						- the initial pressure
		 * @return altitude								- the calculated altitude (m)
		 */
		double altitude(double pressure, double initial_pressure);

	// Private Context
	private:

		//! The device address
		I2C_address _device_address;

		/**
		 * This is the read integer method.
		 *
		 * @param register								- the register to read
		 * @return register								- the register value
		 */
		int _read_int(int reg);

		/**
		 * This is the write integrer method.
		 *
		 * @param reg									- the register to write to
		 * @param value									- the value to write
		 */
		void _write_int(int reg, int value);

		//! Internal register values
		int AC1, AC2, AC3, VB1, VB2, MB, MC, MD;
		unsigned int AC4, AC5, AC6;
		double c5, c6, mc, md, x0, x1, x2, y0, y1, y2, p0, p1, p2;

		//! The internal measurements
		BMP180_measurments_t _measurements;
};

#endif /* BMP180I2CDIGITALSENSOR_H_ */
