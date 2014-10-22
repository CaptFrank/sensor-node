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

extern "C" {
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
//#include "datatypes.h"

#include "simplelink.h"
#include "protocol.h"
#include "uart_if.h"
}

// -------------------------------------------------------------------------------
// Defines

// General constants
#define CONNECTION_TIMEOUT			30
#define UDP_BUFFER_SIZE				48

#define AP_NAME						"Halifax_Home_Monitoring"
#define AP_PASSWORD					"Garf2345"

// NTP
#define NTP_SERVER_DNS				"0.ca.pool.ntp.org"
#define GMT_TIME_HOURS_OFFSET		0 // -4
#define GMT_TIME_MINUTES_OFFSET		0

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
		 * @param server_address							- the server dns name
		 * @param gmt_diff_hours							- the GMT hours difference
		 * @param gmt_diff_minutes							- the GMT minutes difference
		 * @param ds1307_context							- the ds1307 context object
		 */
		NTP_Client(
				unsigned long server_address, unsigned char gmt_diff_hours,
					unsigned char gmt_diff_minutes);

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

		/**
		 * Returns the long time
		 */
		unsigned long get_time(){
			return this->_time;
		}

	// Private Context
	private:

		/**
		 * The procedure context configurations
		 */
		struct {

			// Security parameters
			SlSecParams_t _security_parameters;

			// Time difference
			unsigned char _gmt_diff_hours;
			unsigned char _gmt_diff_minutes;

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
