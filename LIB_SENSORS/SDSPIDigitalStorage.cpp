/*
 * SDSPIDigitalStorage.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#include "SDSPIDigitalStorage.h"

// Public Context

/**
 * The default constructor
 */
SD_SPI_Digital_Storage::SD_SPI_Digital_Storage(System_Context* context){

	// Init the SD card controller
	MAP_PinDirModeSet(PIN_07,PIN_DIR_MODE_OUT);
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST,PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_SDHOST);
    MAP_SDHostInit(SDHOST_BASE);
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST), 15000000);

    // Set the system context
    this->_system_context = context;

    // Set the file names
    sprintf(this->_configs._data_filename, DATA_FILE, this->_system_context->_DS1307_time_context.time.day,
    									   this->_system_context->_DS1307_time_context.time.month,
    									   this->_system_context->_DS1307_time_context.time.year);

    sprintf(this->_configs._log_filename, LOG_FILE, this->_system_context->_DS1307_time_context.time.day,
        									   this->_system_context->_DS1307_time_context.time.month,
        									   this->_system_context->_DS1307_time_context.time.year);
}

/**
 * This initiates the class driver
 *
 * @return success						- returns the success
 */
bool SD_SPI_Digital_Storage::begin(){

	// Containers
	FRESULT res;

	// Mount the card
	f_mount(0, &this->_configs._filesystem);

	Message("Opening root directory.................... ");
	// Open the root directory
	res = f_opendir(&this->_configs.directory, "/");

	// Check for success
	if(res == FR_OK){

		Message("[ok]\n\n\r");

		// Create new file... If not already there.

		Message("Creating new files.................... ");
		// Data
		res = f_open(&this->_configs.file, this->_configs._data_filename, FA_OPEN_EXISTING |
																		  FA_CREATE_NEW |
																		  FA_WRITE);
		f_close(&this->_configs.file);


		// Log
		res = f_open(&this->_configs.file, this->_configs._log_filename, FA_OPEN_EXISTING |
																		  FA_CREATE_NEW |
																		  FA_WRITE);
		f_close(&this->_configs.file);

		// Check
		if(res == FR_OK){

			Message("[ok]\n\n\r");
			return true;
		}else{

			// Error
			Message("[Failed]\n\n\r");
			return false;
		}
	}else{

		// Error
		Message("[Failed]\n\n\r");
		return false;
	}
}

/**
 * This writes a record
 *
 * @param file							- the file
 * @param record						- the record
 * @param length						- the length
 * @return success						- the write success
 */
bool SD_SPI_Digital_Storage::write_record(SD_file_t file, char* record, int length){

	// Container
	WORD written;

	// Switch on the file
	if(this->_change_file(file, FA_OPEN_EXISTING | FA_WRITE)){

		f_write(&this->_configs.file, (void*)record, length, &written);
	}
	f_close(&this->_configs.file);
	return true;
}

/**
 * This reads a file
 *
 * @param directory						- the directory
 * @param file							- the file
 * @return char*						- the file
 * @param length						- the length to read
 */
char* SD_SPI_Digital_Storage::read_record(SD_file_t file, char* file_buffer, int length){

	// Container
	WORD written;
	file_buffer = (char*)malloc(length);

	// Switch on the file
	if(this->_change_file(file, FA_OPEN_EXISTING | FA_READ)){

		f_write(&this->_configs.file, (void*)file_buffer, length, &written);
	}
	f_close(&this->_configs.file);

	// Return the buffer.
	return file_buffer;
}

// Private Context

/**
 * This changes the file pointer
 *
 * @param file							- the new file pointer
 * @param flag							- the read or write flag
 */
bool SD_SPI_Digital_Storage::_change_file(SD_file_t file, char flag){

	// Result
	FRESULT res;
	char* file_name;

	// Switch on the directory
	switch(file){

		// Data directory
		case SD_DATA_FILE:

			file_name = this->_configs._data_filename;
			break;

		// Log directory
		case SD_SYSTEM_FILE:

			file_name = this->_configs._log_filename;
			break;

		// Invalid
		default:
			break;
	}

	// Open
	res = f_open(&this->_configs.file, (char*)file_name, flag);

	// Check
	if(res == FR_OK){
		return true;
	}
	return false;
}
