/*
 * DS1629I2CDigitalSensor.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef DS1307I2CDIGITALSENSOR_H_
#define DS1307I2CDIGITALSENSOR_H_

// -------------------------------------------------------------------------------
// Includes

// The base I2C Device
#include "Sensors.h"

// -------------------------------------------------------------------------------
// Defines

#define YEAR_OFFSET					2000
#define SECONDS_PER_DAY 			86400L
#define SECONDS_FROM_1970_TO_2000 	946684800
#define SECONDS_FROM_1900_TO_2000	3156000000
#define MONTHS_IN_YEAR				12

/**
 * This is the global configuration of the device
 */
enum DS1307_config_t {

	DS1307_I2C_ADDR				= 0x68						//!< DS1307_I2C_ADDR
};

/**
 * These are the possible registers of the device
 */
enum DS1307_register_t {

	// Time
	DS1307_TIME_SEC_REG			= 0x00,                		//!< DS1307_TIME_SEC_REG
	DS1307_TIME_MIN_REG			= 0x01,                		//!< DS1307_TIME_MIN_REG
	DS1307_TIME_HOUR_REG		= 0x02,                		//!< DS1307_TIME_HOUR_REG
	DS1307_TIME_DAY_REG			= 0x03,                		//!< DS1307_TIME_DAY_REG

	// Date
	DS1307_DATE_REG				= 0x04,                   	//!< DS1307_DATE_REG
	DS1307_MONTH_REG			= 0x05,                   	//!< DS1307_MONTH_REG
	DS1307_YEAR_REG				= 0x06,                   	//!< DS1307_YEAR_REG

	// Control
	DS1307_START_STOP_REG		= DS1307_TIME_SEC_REG,		//!< DS1307_START_STOP_REG
	DS1307_CONTROL_REG			= 0x07,                 	//!< DS1307_CONTROL_REG

	// Ram
	DS1307_EXTRA_RAM_START		= 0x08,              		//!< DS1307_EXTRA_RAM_START
	DS1307_EXTRA_RAM_STOP		= 0x3F                		//!< DS1307_EXTRA_RAM_STOP
};

/**
 * These are the modes that are possible when setting up the SEC register.
 */
enum DS1307_TIME_SEC_REG_config_t {

	// Start / Stop -- In DS1307_TIME_SEC_REG
	DS1307_STOP_OSC				= 0x80,             		//!< DS1307_STOP_OSC
	DS1307_START_OSC			= (~DS1307_STOP_OSC)		//!< DS1307_START_OSC
};

/**
 * These are the modes that are possible when setting up the HOUR register.
 */
enum DS1307_TIME_HOUR_REG_config_t {

	// Time format select  -- In DS1307_TIME_HOUR_REG
	DS1307_TIME_12H				= 0x40,             		//!< DS1307_TIME_12H
	DS1307_TIME_24H				= (~DS1307_TIME_12H)		//!< DS1307_TIME_24H
};

/**
 * These are the possible modes associated with the CONTROL register.
 */
enum DS1307_CONTROL_REG_config_t {

	// Control select -- In DS1307_CONTROL_REG

	// Output Level
	DS1307_OUTPUT_ENABLE		= 0x80,						//!< DS1307_OUTPUT_ENABLE
	DS1307_OUTPUT_DISABLE		= (~DS1307_OUTPUT_ENABLE),	//!< DS1307_OUTPUT_DISABLE

	// OSC Enable
	DS1307_SQWE_ENABLE			= 0x10,                  	//!< DS1307_SQWE_ENABLE
	DS1307_SQWE_DISABLE			= (~DS1307_SQWE_ENABLE),	//!< DS1307_SQWE_DISABLE

	// SQW Rate Select
	DS1307_SQW_RATE_1Hz			= 0x00,                 	//!< DS1307_SQW_RATE_1Hz
	DS1307_SQW_RATE_4Hz			= 0x01,                 	//!< DS1307_SQW_RATE_4Hz
	DS1307_SQW_RATE_8Hz			= 0x02,                 	//!< DS1307_SQW_RATE_8Hz
	DS1307_SQW_RATE_32Hz		= 0x03                  	//!< DS1307_SQW_RATE_32Hz
};

/**
 * The months of the year
 */
enum month_t {

	JANUARY,  												//!< JANUARY
	FEBRUARY, 												//!< FEBRUARY
	MARCH,    												//!< MARCH
	APRIL,    												//!< APRIL
	MAY,      												//!< MAY
	JUNE,     												//!< JUNE
	JULY,     												//!< JULY
	AUGUST,   												//!< AUGUST
	SEPTEMBER,												//!< SEPTEMBER
	OCTOBER,  												//!< OCTOBER
	NOVEMBER, 												//!< NOVEMBER
	DECEMBER  												//!< DECEMBER
};

/**
 * Define the state of the device
 */
enum DS1307_state_t {

	DS1307_RUNNING,   										//!< RUNNING
	DS1307_NOT_RUNNING										//!< NOT_RUNNING
};

/**
 * The time format
 */
enum DS1307_time_format_t {

	DS1307_12HR,											//!< DS1307_12HR
	DS1307_24HR 											//!< DS1307_24HR
};

/**
 * Defining the config bits to ease writing
 */
union DS1307_configs_t{

	struct config_bits_t {

		char output_select 	: 1;
		char reserved_1		: 1;
		char reserved_2		: 1;
		char sqw_enable		: 1;
		char reserved_3		: 1;
		char reserved_4		: 1;
		char rate_select_1	: 1;
		char rate_select_2	: 1;
	}_bits;

	char config_byte;
};

/**
 * This is the time structure pointer.
 */
struct DS1307_time_t {

	unsigned char 	second;
	unsigned char 	minute;
	unsigned char 	hour;
	unsigned char 	_reserved;

	unsigned char	day;
	unsigned char 	month;
	unsigned int 	year;

	unsigned char	dow;
};

static char _days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

// -------------------------------------------------------------------------------
// Main code

// DS1307 Date Time Object

/**
 * This is a container object class. It holds the current date and time. No real
 * logic is implemented within it as the real date and the real time are set
 * via the NTP queries at startup.
 */
class DS1307_Date_Time {

	// Public Context
	public:

		/**
		 * This is the default constructor of the class.
		 */
		DS1307_Date_Time(){

			this->days_in_month = _days_in_month;
		}

		/**
		 * Override the constructor with a 32 bit time
		 */
		DS1307_Date_Time(unsigned long time);

		/**
		 * This is the override of the default constructor with a set
		 * time structure.
		 *
		 * @param time_struct							- the acquired time struct
		 */
		DS1307_Date_Time(DS1307_time_t* time_struct){

			// Copy the time struct
			memcpy(&this->_time, (void*)time_struct, sizeof(this->_time));
		}


		/**
		 * This is the year getter.
		 *
		 * @return uint									- the year
		 */
		unsigned int year() const {
			return this->_time.year + YEAR_OFFSET;
		}

		/**
		 * This is the month getter.
		 *
		 * @return uint									- the month
		 */
		unsigned char month() const {
			return this->_time.month;
		}

		/**
		 * This is the day getter.
		 *
		 * @return uint									- the year
		 */
		unsigned char day() const {
			return this->_time.day;
		}

		/**
		 * This is the hour getter.
		 *
		 * @return uint									- the hour
		 */
		unsigned char hour() const {
			return this->_time.hour;
		}

		/**
		 * This is the minute getter.
		 *
		 * @return uint									- the minute
		 */
		unsigned char minute() const {
			return this->_time.minute;
		}

		/**
		 * This is the second getter.
		 *
		 * @return uint									- the second
		 */
		unsigned char second() const {
			return this->_time.second;
		}

		/**
		 * This is the dow getter.
		 *
		 * @return uint									- the dow
		 */
		unsigned char dow(){

			unsigned int day = date_to_days(this->_time.year,
											this->_time.month,
											this->_time.day);
			day = (day + 6) % 7;
			this->_time.dow = day;
			return day;
		}

		/**
		 * This gets the unix time in terms of 1/1/1970
		 *
		 * @return ulong								- the unix time
		 */
		unsigned long unix_time();

		/**
		 * Return the internal time struct
		 *
		 * @return DS1307_time_t						- time struct
		 */
		DS1307_time_t* get_date_time_struct(){
			return &this->_time;
		}

		//! The internal days in the month
		char* days_in_month;




		/**
		 * Converts a date to a number of days since 2000
		 * @param y										- the year
		 * @param m										- the month
		 * @param d										- the day
		 * @return uint									- the number of days
		 */
		static unsigned int date_to_days(unsigned int y, unsigned char m, unsigned char d) {

			// Check if bigger than 2000
		    if (y >= 2000)
		        y -= 2000;

		    // Get the day
		    unsigned int days = d;

		    // Add the number of days in each month
		    for (unsigned char i = 1; i < m; ++i)
		        days += _days_in_month[i - 1];

		    // Leap year
		    if (m > 2 && y % 4 == 0)
		        ++days;

		    // Return the calculation
		    return days + 365 * y + (y + 3) / 4 - 1;
		}

	// Protected Context
	protected:

		//! The internal time structure
		DS1307_time_t _time;
};

/**
 * This is the DS1307 RTC data construct.
 */
struct DS1307_time_context_t{

	// Time
	DS1307_time_t 	time;

	// Device State
	char 	state;
};

// -------------------------------------------------------------------------------
// DS1307 Driver

/**
 * This is the DS1629 sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 */
class DS1307_I2C_Digital_Sensor: public I2C_Base_Driver {

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
		DS1307_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
				  	  	  	  	  I2C_address device_address  = DS1307_I2C_ADDR);

		/**
		 * This sets up the device
		 */
		bool begin();

		/**
		 * The default run method
		 */
		static void run(void* _driver, void* _data){

			// Get the pointer
			DS1307_I2C_Digital_Sensor* driver = (DS1307_I2C_Digital_Sensor*) _driver;
			DS1307_time_context_t* data = (DS1307_time_context_t*) _data;

			// Get the time and the status of the RTC
			DS1307_Date_Time* time = driver->now();

			// Set the data within the system context
			memcpy(&data->time, time, sizeof(DS1307_time_t));

			// Set state
			data->state = driver->_configs.state;
		}

		/**
		 * This stops the clock from incrementing time.
		 */
		void stop_clock();

		/**
		 * This starts the clocks timer
		 */
		void start_clock();

		/**
		 * This adjusts the internal date time object
		 *
		 * @param date_time								- the date time object to update
		 */
		void adjust(const DS1307_Date_Time* date_time);

		/**
		 * Checks if the RTC is in fact running
		 *
		 * @return running								- the running state of the device
		 */
		bool is_running();

		/**
		 * This returns a date time object with the current time
		 *
		 * @return Date_Time							- the current time
		 */
		DS1307_Date_Time* now();


	// Private Context
	private:

		//! The internal device address
		char _device_address;

		//! The device configs
		struct {

			DS1307_configs_t config_reg;
			DS1307_state_t state;

		}_configs;

		//! Internal access to the date and time
		DS1307_Date_Time _datetime;

		/**
		 * This gets time and stores it internally.
		 */
		void _get_time();

		/**
		 * This sets the time on the RTC
		 */
		void _set_time();

		/**
		 * This gets the control register internally
		 */
		void _get_control_reg();

		/**
		 * This sets the control register in the RTC
		 *
		 * @param settings 								- the device settings
		 */
		void _set_control_reg(char settings);
};

#endif /* DS1629I2CDIGITALSENSOR_H_ */
