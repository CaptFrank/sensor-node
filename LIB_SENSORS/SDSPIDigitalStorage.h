/*
 * SDSPIDigitalStorage.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef SDSPIDIGITALSTORAGE_H_
#define SDSPIDIGITALSTORAGE_H_

// -------------------------------------------------------------------------------
// Includes

// General

extern "C" {

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ff.h>

// Drivers
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "pin.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "sdhost.h"
#include "utils.h"
#include "interrupt.h"
#include "uart_if.h"
}

#include "SystemContext.h"

// -------------------------------------------------------------------------------
// Defines

// Files
#define LOG_FILE				"Log-%d-%d-%d.txt"
#define DATA_FILE				"Data-%d-%d-%d.txt"
/**
 * The valid files for a day
 */
enum SD_file_t {

	SD_DATA_FILE, 				//!< DATA_DIRECTORY
	SD_SYSTEM_FILE				//!< SYSTEM_DIRECTORY
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the handler for the SD card. It implements the Fat File system
 * and handles all the commands sending and receiving from the SD controller.
 * Essentially this operates on the SPI bus and the transactions are complete
 * using it.
 */
class SD_SPI_Digital_Storage  {

	// Public Context
	public:

		/**
		 * The default constructor
		 */
		SD_SPI_Digital_Storage(System_Context* context);

		/**
		 * This initiates the class driver
		 *
		 * @return success						- returns the success
		 */
		bool begin();

		/**
		 * This writes a record
		 *
		 * @param file							- the file
		 * @param record						- the record
		 * @param length						- the length
		 * @param length						- the length to write
		 * @return success						- the write success
		 */
		bool write_record(SD_file_t file, char* record, int length);

		/**
		 * This reads a file
		 *
		 * @param directory						- the directory
		 * @param file							- the file
		 * @return char*						- the file
		 * @param length						- the length to read
		 */
		char* read_record(SD_file_t file, char* file_buffer, int length);

	// Private Context
	private:

		//! The filesystem configs
		struct {

			FATFS _filesystem;

			DIR directory;
			FIL file;

			char _data_filename[18];
			char _log_filename[17];
		}_configs;

		//! System context
		System_Context* _system_context;

		/**
		 * This changes the file pointer
		 *
		 * @param file							- the new file pointer
		 * @param flag							- the read or write flag
		 */
		bool _change_file(SD_file_t file, char flag);
};

#endif /* SDSPIDIGITALSTORAGE_H_ */
