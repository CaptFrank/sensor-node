/*
 * LTC1867SPIDigitalSensor.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef LTC1867SPIDIGITALSENSOR_H_
#define LTC1867SPIDIGITALSENSOR_H_

// -------------------------------------------------------------------------------
// Includes

// Standards
#include <math.h>

// Mapped functions
#include "rom.h"
#include "rom_map.h"
#include "utils.h"


// The base I2C Device
#include "Sensor Drivers/Sensors.h"

// -------------------------------------------------------------------------------
// Defines

// Macros

#define LTC186X_CHAN_DIFF_0P_1N    (0)
#define LTC186X_CHAN_DIFF_2P_3N    (LTC186X_CONFIG_S0)
#define LTC186X_CHAN_DIFF_4P_5N    (LTC186X_CONFIG_S1)
#define LTC186X_CHAN_DIFF_6P_7N    (LTC186X_CONFIG_S1) | LTC186X_CONFIG_S0)
#define LTC186X_CHAN_DIFF_1P_0N    (LTC186X_CONFIG_ODD)
#define LTC186X_CHAN_DIFF_3P_2N    (LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S0)
#define LTC186X_CHAN_DIFF_5P_4N    (LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S1)
#define LTC186X_CHAN_DIFF_7P_6N    (LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S1) | LTC186X_CONFIG_S0)

#define LTC186X_CHAN_DIFF_0P_7COM  (LTC186X_CONFIG_COM) | LTC186X_CONFIG_SINGLE_END)
#define LTC186X_CHAN_DIFF_1P_7COM  (LTC186X_CONFIG_COM) | LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_ODD)
#define LTC186X_CHAN_DIFF_2P_7COM  (LTC186X_CONFIG_COM) | LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_S0)
#define LTC186X_CHAN_DIFF_3P_7COM  (LTC186X_CONFIG_COM) | LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S0)
#define LTC186X_CHAN_DIFF_4P_7COM  (LTC186X_CONFIG_COM) | LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_S1)
#define LTC186X_CHAN_DIFF_5P_7COM  (LTC186X_CONFIG_COM) | LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S1)
#define LTC186X_CHAN_DIFF_6P_7COM  (LTC186X_CONFIG_COM) | LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_S1)  | LTC186X_CONFIG_S0)

#define LTC186X_CHAN_SINGLE_0P     (LTC186X_CONFIG_SINGLE_END)
#define LTC186X_CHAN_SINGLE_1P     (LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_ODD)
#define LTC186X_CHAN_SINGLE_2P     (LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_S0)
#define LTC186X_CHAN_SINGLE_3P     (LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S0)
#define LTC186X_CHAN_SINGLE_4P     (LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_S1)
#define LTC186X_CHAN_SINGLE_5P     (LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S1)
#define LTC186X_CHAN_SINGLE_6P     (LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_S1)  | LTC186X_CONFIG_S0)
#define LTC186X_CHAN_SINGLE_7P     (LTC186X_CONFIG_SINGLE_END) | LTC186X_CONFIG_ODD) | LTC186X_CONFIG_S1) | LTC186X_CONFIG_S0)

#define NUM_DEVICES				5

/**
 * These are the various configuration registers needed to configure the
 * remote LTC device.
 */
enum LTC1867_configs_t {

	LTC186X_CONFIG_SINGLE_END 			= 7,		//!< LTC186X_CONFIG_SINGLE_END
	LTC186X_CONFIG_ODD        			= 6,		//!< LTC186X_CONFIG_ODD
	LTC186X_CONFIG_S1         			= 5,		//!< LTC186X_CONFIG_S1
	LTC186X_CONFIG_S0         			= 4,		//!< LTC186X_CONFIG_S0
	LTC186X_CONFIG_COM        			= 3,		//!< LTC186X_CONFIG_COM
	LTC186X_CONFIG_UNI        			= 2,		//!< LTC186X_CONFIG_UNI
	LTC186X_CONFIG_SLP        			= 1 		//!< LTC186X_CONFIG_SLP

};

/**
 * This enum is the register definitions for the LTC186X chip.
 */
enum LTC1867_registers_t {

	LTC186X_EEP_ADDR					= 0xFA,   	//!< LTC186X_EEP_ADDR

	LTC186X_EEP_DISABLE					= 0x00,		//!< LTC186X_EEP_DISABLE
	LTC186X_EEP_ADDR_00					= 0x50,		//!< LTC186X_EEP_ADDR_00
	LTC186X_EEP_ADDR_0Z					= 0x51,		//!< LTC186X_EEP_ADDR_0Z
	LTC186X_EEP_ADDR_Z0					= 0x52,		//!< LTC186X_EEP_ADDR_Z0
	LTC186X_EEP_ADDR_ZZ					= 0x53 		//!< LTC186X_EEP_ADDR_ZZ

};

/**
 * These are the various states that the device can result.
 */
enum LTC1867_states_t {

	LTC1867_SUCCESS						= 0,    	//!< LTC1867SUCCESS
	LTC1867_LTC186X_ERR					= 1,		//!< LTC1867_LTC186X_ERR
	LTC1867_EEPROM_ERR					= 2  		//!< LTC1867_EEPROM_ERR
};

/**
 * These are the device types that this driver supports.
 */
enum LTC18xx_device_t {

	DEVICE_LTC1863						= 0,		//!< DEVICE_LTC1863
	DEVICE_LTC1867						= 1, 		//!< DEVICE_LTC1867

	// The device CS pin... GPIO30
	DEVICE_CS							= 30		//!< DEVICE_CS
};

enum LTC1867_readings_t {

	BIPOLAR								= 0,
	UNIPOLAR							= 1
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the LTC1867 sensor driver and handler. We use this to access the data
 * registers from the SPI device. We also use it to control the device and its
 * applications. This class extends the @see SPI_Base_Driver
 */
class LTC1867_SPI_Digital_Sensor : public SPI_Base_Driver {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class.
		 *
		 * @param context								- the system context
		 * @param device_map							- the device map
		 * @param device_address						- the device address
		 */
		LTC1867_SPI_Digital_Sensor(System_Context* context,
								  SPI_cs_port_t device_port,
								  SPI_cs_pin_t device_cs  = DEVICE_CS,
								  LTC18xx_device_t device_type);

		/**
		 * This is the begin engine method.
		 */
		bool begin();

		/**
		 * The default run method.
		 */
		static void run(void* object_pointer){

//			// Convert object ptr to our object
//			LTC1867_SPI_Digital_Sensor* ptr = (LTC1867_SPI_Digital_Sensor*)object_ptr;
//
//			//  Get the data
//			for(register int i = 0; i < sizeof(NUM_DEVICES); i ++){
//				ptr->change_channel(ptr->_device_channel_map[i], UNIPOLAR);
//				((unsigned int*)&ptr->_measurments)[i] = ptr->read();
//			}
//
//			// Copy the data over
//			memcpy(&ptr->_system_db->_LTC1867_driver_context,
//					&ptr->_measurments, sizeof(ptr->_measurments));
//
		}

		/**
		 * Reads a value from the remote device.
		 *
		 * @return unsigned int							- the value read
		 */
		unsigned int read();

		/**
		 * Reads a bipolar value from the remote device.
		 *
		 * @return int									- the value read
		 */
		int read_bipolar();

		/**
		 * This reads a value and changes the channel of measurment
		 *
		 * @param device_channel						- the channel
		 * @return unsigned int							- the read value
		 */
		unsigned int read_and_change_channel(char device_channel);

		/**
		 * This reads a value and changes the channel of measurement.
		 *
		 * @param device_channel						- the measurment is unipolar?
		 * @param device_channel						- the channel
		 * @return unsigned int							- the read value
		 */
		unsigned int read_and_change_channel(char device_channel,
											 char unipolar);

		/**
		 * This reads a bipolar value and changes the channel of measurment
		 *
		 * @param device_channel						- the channel
		 * @return int									- the read value
		 */
		int read_and_change_channel_bipolar(char device_channel);

		/**
		 * This reads a bipolar value and changes the channel of measurement.
		 *
		 * @param device_channel						- the measurment is unipolar?
		 * @param device_channel						- the channel
		 * @return int									- the read value
		 */
		int read_and_change_channel_bipolar(char device_channel,
											 char unipolar);

		/**
		 * This changes the channel of measurment
		 *
		 * @param device_channel						- the new channel association
		 */
		void change_channel(char device_channel);

		/**
		 * This changes the channel of measurment
		 *
		 * @param device_channel						- the new channel association
		 * @param unipolar								- the unipolar flag
		 */
		void change_channel(char device_channel, char unipolar);

		/**
		 * This sleeps the device
		 */
		void sleep();

		/**
		 * This wakes the device
		 */
		void wake();

	// Private Context
	private:

		//! The device configs structure.
		struct {

			char _init_status;
			char _current_configs;
			char _device_type;
			char _device_cs;

		} _device_configs;

		//! Measurments
		struct {

			unsigned int battery_monitor;
			unsigned int power_sense;
			unsigned int light;
			unsigned int buffered_mic;
			unsigned int raw_mic;

		}_measurments;

		//! Device channel map
		char* _device_channel_map;

		/**
		 * Change the channel
		 *
		 * @param device_channel						- the channel to change
		 * @param unipolar								- the polarity
		 */
		void _internal_channel_change(char device_channel, char unipolar);

};

#endif /* LTC1867SPIDIGITALSENSOR_H_ */
