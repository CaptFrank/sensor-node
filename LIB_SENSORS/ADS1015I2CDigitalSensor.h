/*
 * ADS1015I2CDigitalSensor.h
 *
 *  Created on: Jun 26, 2014
 *      Author: Administrator
 */

#ifndef ADS1015I2CDIGITALSENSOR_H_
#define ADS1015I2CDIGITALSENSOR_H_

// -------------------------------------------------------------------------------
// Includes

// The base I2C Device
#include "I2CBaseDriver.h"

// -------------------------------------------------------------------------------
// Defines

#define NUMBER_OF_SENSOR					4

/**
 * The address
 */
enum ADS1015_address_config_t {

	ADS1015_I2C_ADDR						= 0x48			//!< ADS1015_I2C_ADDR
};

/**
 * The conversion delays
 */
enum ADS1015_conversion_delay_t {

	ADS1015_CONVERSIONDELAY         		= (1),  		//!< ADS1015_CONVERSIONDELAY
   	ADS1115_CONVERSIONDELAY         		= (8)			//!< ADS1115_CONVERSIONDELAY
};

/**
 * The valid registers
 */
enum ADS1015_register_t {

	ADS1015_REG_POINTER_MASK        		= (0x03),		//!< ADS1015_REG_POINTER_MASK
	ADS1015_REG_POINTER_CONVERT     		= (0x00),		//!< ADS1015_REG_POINTER_CONVERT
	ADS1015_REG_POINTER_CONFIG      		= (0x01),		//!< ADS1015_REG_POINTER_CONFIG
	ADS1015_REG_POINTER_LOWTHRESH   		= (0x02),		//!< ADS1015_REG_POINTER_LOWTHRESH
	ADS1015_REG_POINTER_HITHRESH    		= (0x03) 		//!< ADS1015_REG_POINTER_HITHRESH
};

enum ADS1015_config_t {

	ADS1015_REG_CONFIG_OS_MASK      		= (0x8000),
    ADS1015_REG_CONFIG_OS_SINGLE    		= (0x8000),  	// Write: Set to start a single-conversion
    ADS1015_REG_CONFIG_OS_BUSY      		= (0x0000),  	// Read: Bit = 0 when conversion is in progress
    ADS1015_REG_CONFIG_OS_NOTBUSY   		= (0x8000),  	// Read: Bit = 1 when device is not performing a conversion

    ADS1015_REG_CONFIG_MUX_MASK     		= (0x7000),
    ADS1015_REG_CONFIG_MUX_DIFF_0_1 		= (0x0000),  	// Differential P = AIN0, N = AIN1 = (default)
    ADS1015_REG_CONFIG_MUX_DIFF_0_3 		= (0x1000),  	// Differential P = AIN0, N = AIN3
    ADS1015_REG_CONFIG_MUX_DIFF_1_3 		= (0x2000),  	// Differential P = AIN1, N = AIN3
    ADS1015_REG_CONFIG_MUX_DIFF_2_3 		= (0x3000),  	// Differential P = AIN2, N = AIN3
    ADS1015_REG_CONFIG_MUX_SINGLE_0 		= (0x4000),  	// Single-ended AIN0
    ADS1015_REG_CONFIG_MUX_SINGLE_1 		= (0x5000),  	// Single-ended AIN1
    ADS1015_REG_CONFIG_MUX_SINGLE_2 		= (0x6000), 	// Single-ended AIN2
    ADS1015_REG_CONFIG_MUX_SINGLE_3 		= (0x7000), 	// Single-ended AIN3

    ADS1015_REG_CONFIG_PGA_MASK     		= (0x0E00),
    ADS1015_REG_CONFIG_PGA_6_144V   		= (0x0000),  	// +/-6.144V range = Gain 2/3
    ADS1015_REG_CONFIG_PGA_4_096V   		= (0x0200),  	// +/-4.096V range = Gain 1
    ADS1015_REG_CONFIG_PGA_2_048V   		= (0x0400),  	// +/-2.048V range = Gain 2 = (default)
    ADS1015_REG_CONFIG_PGA_1_024V   		= (0x0600),  	// +/-1.024V range = Gain 4
    ADS1015_REG_CONFIG_PGA_0_512V   		= (0x0800),  	// +/-0.512V range = Gain 8
    ADS1015_REG_CONFIG_PGA_0_256V   		= (0x0A00),  	// +/-0.256V range = Gain 16

    ADS1015_REG_CONFIG_MODE_MASK    		= (0x0100),
    ADS1015_REG_CONFIG_MODE_CONTIN  		= (0x0000),  	// Continuous conversion mode
    ADS1015_REG_CONFIG_MODE_SINGLE  		= (0x0100),  	// Power-down single-shot mode = (default)

    ADS1015_REG_CONFIG_DR_MASK      		= (0x00E0),
    ADS1015_REG_CONFIG_DR_128SPS    		= (0x0000),  	// 128 samples per second
    ADS1015_REG_CONFIG_DR_250SPS    		= (0x0020),  	// 250 samples per second
    ADS1015_REG_CONFIG_DR_490SPS    		= (0x0040),  	// 490 samples per second
    ADS1015_REG_CONFIG_DR_920SPS    		= (0x0060),  	// 920 samples per second
    ADS1015_REG_CONFIG_DR_1600SPS   		= (0x0080),  	// 1600 samples per second = (default)
    ADS1015_REG_CONFIG_DR_2400SPS   		= (0x00A0),  	// 2400 samples per second
    ADS1015_REG_CONFIG_DR_3300SPS   		= (0x00C0),  	// 3300 samples per second

    ADS1015_REG_CONFIG_CMODE_MASK   		= (0x0010),
    ADS1015_REG_CONFIG_CMODE_TRAD   		= (0x0000),  	// Traditional comparator with hysteresis = (default)
    ADS1015_REG_CONFIG_CMODE_WINDOW 		= (0x0010),  	// Window comparator

    ADS1015_REG_CONFIG_CPOL_MASK    		= (0x0008),
    ADS1015_REG_CONFIG_CPOL_ACTVLOW 		= (0x0000),  	// ALERT/RDY pin is low when active = (default)
    ADS1015_REG_CONFIG_CPOL_ACTVHI  		= (0x0008),  	// ALERT/RDY pin is high when active

    ADS1015_REG_CONFIG_CLAT_MASK   			= (0x0004),  	// Determines if ALERT/RDY pin latches once asserted
    ADS1015_REG_CONFIG_CLAT_NONLAT  		= (0x0000),  	// Non-latching comparator = (default)
    ADS1015_REG_CONFIG_CLAT_LATCH   		= (0x0004),  	// Latching comparator

    ADS1015_REG_CONFIG_CQUE_MASK    		= (0x0003),
    ADS1015_REG_CONFIG_CQUE_1CONV   		= (0x0000),  	// Assert ALERT/RDY after one conversions
    ADS1015_REG_CONFIG_CQUE_2CONV   		= (0x0001),  	// Assert ALERT/RDY after two conversions
    ADS1015_REG_CONFIG_CQUE_4CONV   		= (0x0002),  	// Assert ALERT/RDY after four conversions
    ADS1015_REG_CONFIG_CQUE_NONE    		= (0x0003)  	// Disable the comparator and put ALERT/RDY in high state = (default)
};

/**
 * The various gains
 */
enum ADS1015_gain_t {

	GAIN_TWOTHIRDS    						= ADS1015_REG_CONFIG_PGA_6_144V,	//!< GAIN_TWOTHIRDS
	GAIN_ONE          						= ADS1015_REG_CONFIG_PGA_4_096V,	//!< GAIN_ONE
	GAIN_TWO          						= ADS1015_REG_CONFIG_PGA_2_048V,	//!< GAIN_TWO
	GAIN_FOUR         						= ADS1015_REG_CONFIG_PGA_1_024V,	//!< GAIN_FOUR
	GAIN_EIGHT        						= ADS1015_REG_CONFIG_PGA_0_512V,	//!< GAIN_EIGHT
	GAIN_SIXTEEN      						= ADS1015_REG_CONFIG_PGA_0_256V 	//!< GAIN_SIXTEEN
};

/**
 * ADS1015 Internals - Monitoring
 */
struct ADS1015_driver_context_t{

	unsigned int 	power_sense;
	unsigned int 	light;
	unsigned int 	buffered_mic;
	unsigned int 	raw_mic;

};

/**
 * The sensor data
 */
struct ADS1015_measurments_t{

	unsigned int power_sense;
	unsigned int light;
	unsigned int buffered_mic;
	unsigned int raw_mic;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the ADS1015 sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 */
class ADS1015_I2C_Digital_Sensor : public I2C_Base_Driver {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class.
		 *
		 * @param context								- the system context
		 * @param master_address						- the master address
		 * @param device_map							- the device map
		 * @param device_address						- the device address
		 */
		ADS1015_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
								  I2C_address device_address  = ADS1015_I2C_ADDR);

		/**
		 * This sets up the device
		 */
		bool begin();

		/**
		 * The default run method
		 */
		static void run(void* object_pointer, void* _data){

			// Get the proper pointer
			ADS1015_I2C_Digital_Sensor* ptr = (ADS1015_I2C_Digital_Sensor*) object_pointer;
			ADS1015_measurments_t* data = (ADS1015_measurments_t*) _data;

			// Get the data
			for(register int i = 0; i < NUMBER_OF_SENSOR; i++){
				((unsigned int*)(&ptr->_measurments))[i] = ptr->read_single_ended(i);
			}

			// memcpy the data
			memcpy(data, &ptr->_measurments, sizeof(ptr->_measurments));
		}

		/**
		 * This reads the adc single ended
		 * @param channel								- the channel to read
		 * @return unsigned int							- the value read
		 */
		unsigned int read_single_ended(char channel);

		/**
		 * This reads the adc in a differential mode for channels 0 and 1
		 *
		 * @return int									- the value read
		 */
		int read_differential_0_1(void);

		/**
		 * This reads the adc in a differential mode for channels 2 and 3
		 *
		 * @return int									- the value read
		 */
		int read_differential_2_3(void);

		/**
		 * This starts the comparator for single ended reads
		 *
		 * @param channel								- the channel
		 * @param threshold								- the threashold
		 */
		void start_comparator_single_ended(char channel, int threshold);

		/**
		 * This gets the conversion results
		 *
		 * @return int									- the read value
		 */
		int get_last_conversion_results();

		/**
		 * This sets a gain
		 *
		 * @param gain									- the gains
		 */
		void setGain(ADS1015_gain_t gain);

		/**
		 * This gets a set gain
		 *
		 * @return
		 */
		ADS1015_gain_t get_gain(void);

	// Private Context
	private:

		//! The device address
		char _device_address;

		//! Device configs
		struct {

			char conversion_delay;
			char bit_shift;
			ADS1015_gain_t gain;
		}_configs;

		//! Measurments
		ADS1015_measurments_t _measurments;
};

#endif /* ADS1015I2CDIGITALSENSOR_H_ */
