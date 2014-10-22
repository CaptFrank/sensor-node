/*
 * NTPClient.h
 *
 *  Created on: Jun 26, 2014
 *      Author: Administrator
 */

#ifndef NTPCLIENT_H_
#define NTPCLIENT_H_

// -------------------------------------------------------------------------------
// Includes

// Drivers
extern "C" {

#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "datatypes.h"
#include "simplelink.h"
#include "protocol.h"
#include "network_if.h"
#include "uart_if.h"
}

// RTC Access
#include "DS1307I2CDigitalSensor.h"
#include "SystemContext.h"

// -------------------------------------------------------------------------------
// Defines

// General constants
#define CONNECTION_TIMEOUT			30
#define UDP_BUFFER_SIZE				48

// Macro
//#define SIZE_OF_SERVER_DNS(const char* dns_name) (sizeof(dns_name))

// Define a new type
typedef char UDP_buffer_t;

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the NTP client handler class. It is the main interaction point for the
 * NTP interface. It provides teh facilities to get the network time and synchronizes
 * the internal clock with it.
 */
class NTP_Client {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class
		 *
		 * @param context									- the server system context
		 * @param server_address							- the server dns name
		 * @param gmt_diff_hours							- the GMT hours difference
		 * @param gmt_diff_minutes							- the GMT minutes difference
		 * @param ds1307_context							- the ds1307 context object
		 */
		NTP_Client(System_Context* context,
				char* server_address, unsigned char gmt_diff_hours,
					unsigned char gmt_diff_minutes, DS1307_I2C_Digital_Sensor* ds1307_context);

		/**
		 * This is the default begin method
		 *
		 * @param security_params							- the security parameters
		 */
		bool begin(SlSecParams_t* security_params);

		/**
		 * This is the default run method for the procedure.
		 */
		void run();

	// Private Context
	private:

		//! System Context
		System_Context* _context;

		/**
		 * The procedure context configurations
		 */
		struct {

			// Security parameters
			SlSecParams_t _security_parameters;
			char* server_address;

			// Time difference
			unsigned char _gmt_diff_hours;
			unsigned char _gmt_diff_minutes;

			// Internal time
			DS1307_I2C_Digital_Sensor* _rtc;
		}_configs;

		/**
		 * The UDP socket parameters
		 */
		struct {

			unsigned long _dest_ip;
			int _socket_id;

			SlSockAddr_t _address;
			SlSockAddrIn_t _local_address;

		} _udp_socket;

		//! Buffer
		UDP_buffer_t _buffer[48];

		//! Time
		unsigned long _time;

		/**
		 * This gets the destination ip based on the set dns name
		 */
		bool _get_server_ip();

		/**
		 * This method connects the UDP socket to the NTP server
		 */
		bool _connect_udp_socket();

		/**
		 * This receives data from the UDP Port.
		 */
		bool _receive_data();

		/**
		 * This sets the internal clock time with the received data.
		 */
		void _set_internal_clock();

};

#endif /* NTPCLIENT_H_ */
