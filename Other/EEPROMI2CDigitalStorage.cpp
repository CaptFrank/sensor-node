/*
 * EEPROMI2CDigitalStorage.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#include "EEPROMI2CDigitalStorage.h"

// Public Context

// -------------------------------------------------------------------
// Handlers

/**
 * This is the default constructor for the class.
 *
 * @param context								- the sys	tem context
 * @param master_address						- the master address
 * @param device_map							- the device map
 * @param device_address						- the device address
 */
EEPROM_I2C_Digital_Storage::EEPROM_I2C_Digital_Storage(System_Context* context, I2C_address master_address,
						  	  	  	  	  	  	  	  I2C_address device_address  = EEPROM_I2C_ADDR) :
						  	  	  	  	  	  	  	  I2C_Base_Driver(context, master_address){

	// Assign internals
	this->_device_address = device_address;

	// Set context pointer
	this->_system_db->_sys->_storage._eeprom = this;
}

/**
 * This checks if the db already exists.
 *
 * @return exists								- the database exists.
 */
bool EEPROM_I2C_Digital_Storage::check_existence(){

	// Open the database to see what is good
	this->open(0x00);

	if(this->_DB_header.valid_check){
		return true;
	}
	return false;
}

/**
 * The default begin method
 */
bool EEPROM_I2C_Digital_Storage::begin(){

	// We check for existence
	if(!this->check_existence()){

		// If we don't have an existing db create one
		this->create(0x00, DB_TABLE_SIZE, DB_RECORD_SIZE);

		// We append the parameters
		this->append_record((char*)&this->_system_db->_system_state);
	}else{

		// The container
		void* ptr;

		// We read the state
		this->read_record(DB_RECORD, ptr);

		// Memcpy the record
		//memcpy(&this->_system_db->_system_state, ptr, sizeof(system_state_t));
	}
	return true;
}

/**
 * This creates a db
 *
 * @param head_ptr								- the head of the database
 * @param table_size							- the size of the tables
 * @param record_size							- the record sizes
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::create(unsigned long head_ptr, unsigned long table_size,
													unsigned int record_size){

	// Create a new db
	this->_DB_head_ptr = head_ptr;
	this->_DB_table_ptr = sizeof(DB_header_t) + this->_DB_head_ptr;
	this->_DB_header.number_records = 0;
	this->_DB_header.record_size = record_size;
	this->_DB_header.table_size = table_size;
	this->_DB_header.valid_check = true;

	// Write the header
	this->_db_write_head();
	return DB_OK;

}

/**
 * This opens a database already created
 *
 * @param head_ptr								- the head of the database
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::open(unsigned long head_ptr){

	// Read the header
	this->_DB_head_ptr = head_ptr;
	this->_db_read_head();
	return DB_OK;

}

/**
 * This writes a record
 *
 * @param record_number							- the record number to write
 * @param record								- the record itself
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::write_record(unsigned long record_number, void* record){

	// Create a command
	char data[] = {(this->_DB_table_ptr + ((record_number - 1) * this->_DB_header.record_size))};

	// We create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data),
			0x00);

	// Write the data
	this->write_bytes(req);
	return DB_OK;

}

/**
 * This reads a record
 *
 * @param record_number							- the record number
 * @param record								- the record container
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::read_record(unsigned long record_number, void* record){

	// Check for valid record number
	if (record_number < 1 || record_number > this->_DB_header.number_records)
		return DB_OUT_OF_RANGE;

	// Create a command
	char data[] = {(this->_DB_table_ptr + (record_number - 1) * this->_DB_header.record_size)};

	// Create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data),
						this->_DB_header.record_size);

	// Send the data
	this->read_reg(req);

	// Assign the internal buffer
	this->_record = this->_request._rx_data->_buffer;
	return DB_OK;
}

/**
 * This deletes a record
 *
 * @param record_number							- the record number
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::delete_record(unsigned long record_number){

	// Check for valid record number
	if (record_number < 0 || record_number > this->_DB_header.number_records)
		return  DB_OUT_OF_RANGE;

	// Delete the record
	void* rec = (char*)malloc(this->_DB_header.record_size);
	for (unsigned long i = record_number + 1; i <= this->_DB_header.number_records; i++){

		this->read_record(i, record);
		this->write_record(i - 1, record);
	}

	// Free memory
	free(rec);

	// Decrease the number of records and rewrite the header
	this->_DB_header.number_records--;
	this->_db_write_head();
	return DB_OK;

}

/**
 * This inserts a record within the db
 *
 * @param record_number							- the record number
 * @param record								- the record to insert
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::insert_record(unsigned long record_number, void* record){

	// Check if full
	if (this->count() == this->limit()) return DB_TABLE_FULL;

	// Check if out of range
	if (this->count() > 0 && (record_number < 0 || record_number > this->_DB_header.number_records))
		  return DB_OUT_OF_RANGE;

	// We check if we cna append
	if (this->count() == 0 && record_number == 1) return this->append_record(record);

	// Create a temp record
	void* buf = (char*)malloc(this->_DB_header.record_size);

	// We read and then write
	for (unsigned long i = this->_DB_header.number_records; i >= record_number; i--){

		this->read_record(i, buf);
		this->write_record(i + 1, buf);
	}

	// Free the buffer
	free(buf);

	this->read_record(record_number, record);
	this->_DB_header.number_records++;
	this->_db_write_head();

	return DB_OK;
}

/**
 * This updates a record information
 *
 * @param record_number							- the record number
 * @param record								- the record information
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::update_record(unsigned long record_number, void* record){

	// Check if command valid
	if (record_number < 0 || record_number > this->_DB_header.number_records)
		return DB_OUT_OF_RANGE;

	// Write it
	this->write_record(record_number, record);
	return DB_OK;

}

/**
 * This appends a record at the end of the db
 *
 * @param record								- the record to append
 * @return status								- the status
 */
DB_status_t EEPROM_I2C_Digital_Storage::append_record(void* record){

	// Check for space
	if (this->_DB_header.number_records + 1 > limit()) return DB_TABLE_FULL;

	// Update the data
	this->_DB_header.number_records++;
	this->write_record(this->_DB_header.number_records, record);
	this->_db_write_head();

	return DB_OK;

}

/**
 * Gets the limit
 */
unsigned long EEPROM_I2C_Digital_Storage::limit(){

	// Return the limit
	return (this->_DB_header.table_size - this->_DB_table_ptr) / this->_DB_header.record_size;
}

/**
 * Gets the record count
 */
unsigned long EEPROM_I2C_Digital_Storage::count(){

	// Return the number of records in the db
	return this->_DB_header.number_records;
}

/**
 * Resets the internal pointers
 */
void EEPROM_I2C_Digital_Storage::clear(){

	// We read the header
	this->_db_read_head();

	// We create a new entry
	this->create(this->_DB_head_ptr, this->_DB_header.table_size,
									 this->_DB_header.record_size);
}

// Private Context

/**
 * This writes the db header
 */
void EEPROM_I2C_Digital_Storage::_db_write_head(){

	// Create a command
	char data[sizeof(DB_header_t)];
	memcpy(data, this->_DB_header, sizeof(DB_header_t));

	// Create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data), 0x00);

	// Write the header
	this->write_bytes(req);
}

/**
 * This reads teh db header
 */
void EEPROM_I2C_Digital_Storage::_db_read_head(){

	// Create a command
	char data[] = {this->_DB_head_ptr};

	// Create a request
	request_t* req = this->_create_request(this->_device_address, data, sizeof(data),
						sizeof(DB_header_t));

	// Read the registers
	this->read_reg(req);

	// map the header
	memcpy(this->_DB_header, this->_request._rx_data->_buffer, sizeof(DB_header_t));
}
