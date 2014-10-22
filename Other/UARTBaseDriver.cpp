/*
 * UARTBaseDriver.cpp
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#include "UARTBaseDriver.h"

/**
 * This is the default constructor for the class.
 */
UART_Base_Driver::UART_Base_Driver(System_Context* context){

	// Set the system context
	this->_context = context;
}

/**
 * This is the default deconstructor for the class.
 */
UART_Base_Driver::~UART_Base_Driver(){}

// Setup

/**
 * This is the generic setup method for a sensor.
 */
void UART_Base_Driver::setup(){

	// Init the terminal
	InitTerm();
}

/**
 * This clears the terminal window
 */
void UART_Base_Driver::clear_terminal(){

	// Clears the terminal
	ClearTerm();
}

/**
 * This prints a passed string.
 *
 * @param string						- the string to print
 */
void UART_Base_Driver::print_string(char* string){

	// Prints a string
	Message(string);
}

/**
 * This prints a foramtted string
 *
 * @param string						- the string formatter
 */
void UART_Base_Driver::print_formatted(char* string, ...){

	// Prints a formatted string
	Report(string, ...);
}

/**
 * This prints a formatted error string
 *
 * @param string						- the string formatter
 */
void UART_Base_Driver::print_error(char* string, ...){

	// Prints a formatted string
	Error(string, ...);
}

/**
 * This gets a command.
 *
 * @param command						- the command string
 * @param length						- the command string length
 */
void UART_Base_Driver::get_command(char* command, unsigned int length){

	// Gets a command
	GetCmd(command, length);
}
