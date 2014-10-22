/*
 * EEPROMI2CDigitalStorage.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef EEPROMI2CDIGITALSTORAGE_H_
#define EEPROMI2CDIGITALSTORAGE_H_

// -------------------------------------------------------------------------------
// Includes

// Standards
#include <math.h>

// The base I2C Device
#include "Sensor Drivers/Sensors.h"

// The table definitons
#include "System/SystemDefines.h"

// -------------------------------------------------------------------------------
// Defines

/**
 * The eeprom configs
 */
enum EEPROM_config_t{

	EEPROM_I2C_ADDR			= 0xA0						//!< EEPROM_I2C_ADDR
};

/**
 * The database status codes
 */
typedef enum DB_status_t {

	DB_OK,          									//!< DB_OK
	DB_OUT_OF_RANGE,									//!< DB_OUT_OF_RANGE
	DB_TABLE_FULL   									//!< DB_TABLE_FULL
};

//! A record type
typedef unsigned char* DB_record;

/**
 * DB Status structure
 */
struct DB_header_t {

	unsigned long number_records;
	unsigned int record_size;
	unsigned long table_size;
	bool valid_check;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the EEPROM sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 */
class EEPROM_I2C_Digital_Storage: public I2C_Base_Driver {

	// Public Context
	public:

		// -------------------------------------------------------------------
		// Handlers

		/**
		 * This is the default constructor for the class.
		 *
		 * @param context								- the sys	tem context
		 * @param master_address						- the master address
		 * @param device_address						- the device address
		 */
		EEPROM_I2C_Digital_Storage(System_Context* context, I2C_address master_address,
								  I2C_address device_address  = EEPROM_I2C_ADDR);

		/**
		 * The default begin method
		 */
		bool begin();

		/**
		 * This checks if the db already exists.
		 *
		 * @return exists								- the database exists.
		 */
		bool check_existence();

		/**
		 * This creates a db
		 *
		 * @param head_ptr								- the head of the database
		 * @param table_size							- the size of the tables
		 * @param record_size							- the record sizes
		 * @return status								- the status
		 */
		DB_status_t create(unsigned long head_ptr, unsigned long table_size,
															unsigned int record_size);

		/**
		 * This opens a database already created
		 *
		 * @param head_ptr								- the head of the database
		 * @return status								- the status
		 */
		DB_status_t open(unsigned long head_ptr);

		/**
		 * This writes a record
		 *
		 * @param record_number							- the record number to write
		 * @param record								- the record itself
		 * @return status								- the status
		 */
		DB_status_t write_record(unsigned long record_number, void* record);

		/**
		 * This reads a record
		 *
		 * @param record_number							- the record number
		 * @param record								- the record container
		 * @return status								- the status
		 */
		DB_status_t read_record(unsigned long record_number, void* record);

		/**
		 * This deletes a record
		 *
		 * @param record_number							- the record number
		 * @return status								- the status
		 */
		DB_status_t delete_record(unsigned long record_number);

		/**
		 * This inserts a record within the db
		 *
		 * @param record_number							- the record number
		 * @param record								- the record to insert
		 * @return status								- the status
		 */
		DB_status_t insert_record(unsigned long record_number, void* record);

		/**
		 * This updates a record information
		 *
		 * @param record_number							- the record number
		 * @param record								- the record information
		 * @return status								- the status
		 */
		DB_status_t update_record(unsigned long record_number, void* record);

		/**
		 * This appends a record at the end of the db
		 *
		 * @param record								- the record to append
		 * @return status								- the status
		 */
		DB_status_t append_record(void* record);

		/**
		 * Gets the limit
		 */
		unsigned long limit();

		/**
		 * Gets the record count
		 */
		unsigned long count();

		/**
		 * Resets the internal pointers
		 */
		void clear();

	// Private Context
	private:

		//! The device address
		I2C_address _device_address;

		//! Internal variables
	    unsigned long _DB_head_ptr;
	    unsigned long _DB_table_ptr;
	    struct DB_header_t _DB_header;

	    //! Intenal record container
	    void* _record;

		/**
		 * This writes the db header
		 */
		void _db_write_head();

		/**
		 * This reads teh db header
		 */
		void _db_read_head();
};

#endif /* EEPROMI2CDIGITALSTORAGE_H_ */
