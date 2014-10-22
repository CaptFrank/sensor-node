/*
 * DS1629I2CDigitalSensor.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#include "DS1307I2CDigitalSensor.h"

// -------------------------------------------------------------------------------
// Static Utilities

/**
 * Converts a time to a long variable
 *
 * @param days									- the numebr of days
 * @param h										- the number of hours
 * @param m										- the number of minutes
 * @param s										- the number of seconds
 */
static long time_to_long(unsigned int days, unsigned char h, unsigned char m, unsigned char s) {

	// Return the calculations
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

/**
 * Convert BIN format to BCD format.
 *
 * @param val									- the value to convert
 * @return char									- the bcd value
 */
static char bin_to_bcd (char val) {
	return val + 6 * (val / 10);
}

// -------------------------------------------------------------------------------
// DS1307 Date Time Object

/**
 * Override the constructor with a 32 bit time (Assuming its seconds from 1900)
 */
DS1307_Date_Time::DS1307_Date_Time(unsigned long t){

	// Get seconds
	t -= SECONDS_FROM_1900_TO_2000;
	this->_time.second = t % 60;

	// Get minutes
	t /= 60;
	this->_time.minute = t %60;

	// Get hours
	t /= 60;
	this->_time.hour = t % 24;

	// Get days
	unsigned int days = t / 24;
	unsigned char leap;

	// Get years
	for (this->_time.year = 0; ; ++ this->_time.year){
		leap = this->_time.year % 4 == 0;

		// Check if leap year
		if(days < 365 + leap){
			break;
		}
		days -= 365 + leap;
	}

	for(this->_time.month = 1; ; ++ this->_time.month){

		unsigned char days_per_month = _days_in_month[this->_time.month - 1];

		// Check if leap year
		if(leap && this->_time.month == 2){
			++ days_per_month;
		}

		// Check if days are bigger than the days from the table
		if(days < days_per_month){
			break;
		}
		days -= days_per_month;
	}
	this->_time.day = days + 1;
}


/**
 * This gets the unix time in terms of 1/1/1970
 *
 * @return ulong								- the unix time
 */
unsigned long DS1307_Date_Time::unix_time(){

	// Containers
	unsigned long t;

	// Get days
	unsigned int days = date_to_days(this->_time.year, this->_time.month, this->_time.day);

	// Get time
	t = time_to_long(days, this->_time.hour, this->_time.minute, this->_time.second);
	t += SECONDS_FROM_1970_TO_2000;
	return t;
}

// -------------------------------------------------------------------------------
// DS1307 Driver Object

// Public Context

/**
 * This is the default constructor for the class.
 *
 * @param context								- the system context
 * @param master_address						- the master address
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
DS1307_I2C_Digital_Sensor::DS1307_I2C_Digital_Sensor(System_Context* context, I2C_address master_address,
						  	  	  	  	  	  	  	  I2C_address device_address) :
						  	  	  	  	  	  	  	  I2C_Base_Driver(context, master_address){
	// Set address
	this->_device_address = device_address;
}

/**
 * This sets up the device
 */
bool DS1307_I2C_Digital_Sensor::begin(){

	// We check for presence
	if(this->check_presence(this->_device_address)){

		// Get the configs
		this->_get_control_reg();

		// We start the clock
		this->start_clock();
		this->_configs.state = DS1307_RUNNING;
	}
	return false;
}

/**
 * This stops the clock from incrementing time.
 */
void DS1307_I2C_Digital_Sensor::stop_clock(){

	// Check if its already asleep
	if(this->_configs.state == DS1307_RUNNING){

		// We create a command
		char data[] = {DS1307_TIME_SEC_REG, DS1307_STOP_OSC};

		// Create a request
		request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 0);

		// Write it
		this->write_reg(req);

		// Set the state
		this->_configs.state = DS1307_NOT_RUNNING;
	}
}

/**
 * This starts the clocks timer
 */
void DS1307_I2C_Digital_Sensor::start_clock(){

	// Check if its already running
	if(this->_configs.state == DS1307_NOT_RUNNING){

		// We create a command
		char data[] = {DS1307_TIME_SEC_REG, (char)DS1307_START_OSC};

		// Create a request
		request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 0);

		// Write it
		this->write_reg(req);

		// Set the state
		this->_configs.state = DS1307_RUNNING;
	}

}

/**
 * This adjusts the internal date time object
 *
 * @param date_time								- the date time object to update
 */
void DS1307_I2C_Digital_Sensor::adjust(const DS1307_Date_Time* date_time){

	// Create a command
	char data [] = {DS1307_TIME_SEC_REG,
					bin_to_bcd(date_time->second()),
					bin_to_bcd(date_time->minute()),
					bin_to_bcd(date_time->hour()),
					bin_to_bcd(0x00),
					bin_to_bcd(date_time->day()),
					bin_to_bcd(date_time->month()),
					bin_to_bcd(date_time->year() - 2000),
					0x00};

	// Create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 0x00);

	// Send it
	this->write_reg(req);
}

/**
 * Checks if the RTC is in fact running
 *
 * @return running								- the running state of the device
 */
bool DS1307_I2C_Digital_Sensor::is_running(){

	// Create a command
	char data[] = {DS1307_TIME_SEC_REG};

	// Create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 1);

	// Read
	this->read_reg(req);

	// Update the state
	char state = ~(this->_request._rx_data->_buffer[0] >> 7);
	this->_configs.state = (state == true) ?  DS1307_RUNNING : DS1307_NOT_RUNNING;

	return state;
}

/**
 * This returns a date time object with the current time
 *
 * @return Date_Time							- the current time
 */
DS1307_Date_Time* DS1307_I2C_Digital_Sensor::now(){

	// Gets the time
	this->_get_time();

	// Set the new data
	this->_set_time();

	// Return the new data pointer
	return &this->_datetime;
}


// Private Context

/**
 * This gets time and stores it internally.
 */
void DS1307_I2C_Digital_Sensor::_get_time(){

	// Create a command
	char data[] = {DS1307_TIME_SEC_REG};

	// We create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 7);

	// We read the data
	this->read_reg(req);
}

/**
 * This sets the time on the RTC
 */
void DS1307_I2C_Digital_Sensor::_set_time(){

	// We access the data
	for(register char i = 0;
				i < (sizeof(DS1307_time_t) - 1); // Account for the dow
				i ++){

		// We set the data in the struct
		((char*)(this->_datetime.get_date_time_struct()))[i] =
				this->_request._rx_data->_buffer[i];
	}
}

/**
 * This gets the control register internally
 */
void DS1307_I2C_Digital_Sensor::_get_control_reg(){

	// Create a command
	char data[1] = {DS1307_CONTROL_REG};

	// Create a req
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 1);

	// Read the response
	this->read_reg(req);

	// Set the configs
	this->_configs.config_reg.config_byte = this->_request._rx_data->_buffer[0];
}

/**
 * This sets the control register in the RTC
 *
 * @param settings 								- the device settings
 */
void DS1307_I2C_Digital_Sensor::_set_control_reg(char settings){

	// We assume that we have a config register internally
	// We create a command
	char data[] = {DS1307_CONTROL_REG,
				   this->_configs.config_reg.config_byte |
				   DS1307_SQWE_ENABLE |
				   DS1307_SQW_RATE_4Hz};

	// We create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 0);

	// Write the register
	this->write_reg(req);
}

