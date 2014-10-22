/*
 * Structures.h
 *
 *  Created on: Jul 3, 2014
 *      Author: Administrator
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_


/**
 * The following are pointers to the objects that we will be using
 * throughout the entire runtime of the device.
 */
typedef struct system_t {
	/**
	 * We define a structure of sensor pointers
	 */
	struct sensor_t {
		ADS1015_I2C_Digital_Sensor* 	_ads1015_sensor;
		BMP180_I2C_Digital_Sensor* 		_bmp180_sensor;
		SHT21_I2C_Digital_Sensor*		_sht21_sensor;
		TMP006_I2C_Digital_Sensor*		_tmp006_sensor;
		TMP102_I2C_Digital_Sensor*		_tmp102_sensor;
		DS1307_I2C_Digital_Sensor*		_ds1307_sensor;
	}_sensors;

	/**
	 * We define a structure of logger pointers
	 */
	struct loggers_t{

		Event_Logger* 					_event_logger;

	} _loggers;

	/**
	 * We define a structure of network clients pointers
	 */
	struct network_t{

		MQTT_Client* 					_mqtt_client;
		NTP_Client*						_ntp_client;
	} _network;

	/**
	 * We define a structure of storage pointers
	 */
	struct storage_t{

		//EEPROM_I2C_Digital_Storage* 	_eeprom;
		SD_SPI_Digital_Storage*			_sd;
	} _storage;

	//! We define a system timer pointer
	Timer* 								_timer;
} system_t;

// -------------------------------------------------------------------------------
// Data Structures

/**
 * TMP102 Internals - Monitoring
 */
struct TMP102_driver_context_t {

	// State information
	struct {
		bool 			error;
		TMP102_state_t 	state;
	} state;

	// Measurement
	unsigned int  		temperature;
};

/**
 * BMP180 Internals - Monitoring
 */
struct BMP180_driver_context_t {

	// Measurments
	unsigned int  		pressure;
	unsigned int  		pressure_baseline;
	unsigned int  		altitude;
	unsigned int  		sea_level;
	unsigned int  		temperature;
};

/**
 * SHT21 Internals - Monitoring
 */
struct SHT21_driver_context_t {

	// Measurments
	unsigned int  		humidity;
	unsigned int  		temperature;
};

/**
 * TMP006 Internals - Monitoring
 */
struct TMP006_driver_context_t {

	// Measurments
	unsigned int  		internal_temperature;
	unsigned int  		external_temperature;
};

/**
 * ADS1015 Internals - Monitoring
 */
struct ADS1015_driver_context_t{

	unsigned int 		power_sense;
	unsigned int 		light;
	unsigned int 		buffered_mic;
	unsigned int 		raw_mic;

};

/**
 * This is the DS1307 RTC data construct.
 */
struct DS1307_time_context_t {

	// Time
	DS1307_time_t 	time;

	// Device State
	char 	state;
};


#endif /* STRUCTURES_H_ */
