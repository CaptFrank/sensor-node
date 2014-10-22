/*
 * Logger.h
 *
 *  Created on: Jun 27, 2014
 *      Author: Administrator
 */

#ifndef LOGGER_H_
#define LOGGER_H_

// -------------------------------------------------------------------------------
// Includes

// The system context
#include "SystemContext.h"

// -------------------------------------------------------------------------------
// Defines

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the interface for all system loggers that are implemented within
 * the system. It provides a standard method layout that needs to be implemented
 * in each logger class.
 */
class Logger {

	// Public Context
	public:

		/**
		 * Default constructor
		 *
		 * @param context				- the system context
		 */
		Logger(System_Context* context){
			this->_context = context;
		}

		/**
		 * This is the default begin method
		 *
		 * @return success				- the setup success
		 */
		virtual bool begin(){}

	// Protected Context
	protected:

		//! The system context
		System_Context* _context;
};

#endif /* LOGGER_H_ */
