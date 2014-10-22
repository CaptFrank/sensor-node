/*
 * UARTBaseDriver.h
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#ifndef UARTBASEDRIVER_H_
#define UARTBASEDRIVER_H_

// -------------------------------------------------------------------------------
// Includes

// UART Interface
extern "C" {

#include "uart_if.h"
}

// The base sensor class
#include "SystemContext.h"

// -------------------------------------------------------------------------------
// Defines


// -------------------------------------------------------------------------------
// Main code

/**
 * This is the UART system driver class. It is used to handle all
 * UART communications and to post data through the UART interface.
 */
class UART_Base_Driver {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class.
		 */
		UART_Base_Driver(System_Context* context);

		/**
		 * This is the default deconstructor for the class.
		 */
		~UART_Base_Driver();

		// Setup

		/**
		 * This is the generic setup method for a sensor.
		 */
		void setup();

		/**
		 * This clears the terminal window
		 */
		void clear_terminal();

		/**
		 * This prints a passed string.
		 *
		 * @param string						- the string to print
		 */
		void print_string(char* string);

		/**
		 * This prints a foramtted string
		 *
		 * @param string						- the string formatter
		 */
		void print_formatted(char* string, ...);

		/**
		 * This prints a formatted error string
		 *
		 * @param string						- the string formatter
		 */
		void print_error(char* string, ...);

		/**
		 * This gets a command.
		 *
		 * @param command						- the command string
		 * @param length						- the command string length
		 */
		void get_command(char* command, unsigned int length);

	// Private Context
	private:

		//! The system context
		System_Context* _context;

};

#endif /* UARTBASEDRIVER_H_ */
