/*
 * NTPClient.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Administrator
 */

#include "NTPClient.h"

// Public Context


/**
 * This is the default constructor for the class
 *
 * @param server_address							- the server dns name
 * @param gmt_diff_hours							- the GMT hours difference
 * @param gmt_diff_minutes							- the GMT minutes difference
 * @param ds1307_context							- the ds1307 context object
 */
NTP_Client::NTP_Client(/*System_Context* context,*/
		unsigned long server_address, unsigned char gmt_diff_hours,
			unsigned char gmt_diff_minutes/*, DS1307_I2C_Digital_Sensor* ds1307_context*/){

	// Set the internals
	this->_configs._gmt_diff_hours = gmt_diff_hours;
	this->_configs._gmt_diff_minutes = gmt_diff_minutes;
	this->_udp_socket._dest_ip = server_address;
}

/**
 * This is the default begin method
 *
 * @param security_params							- the security parameters
 */
bool NTP_Client::begin(SlSecParams_t* security_params){

	// Copy the security params
	memcpy((void*)&this->_configs._security_parameters, security_params, sizeof(SlSecParams_t));

	// Get the AP name in a string
	char data[sizeof(AP_NAME)];
	memcpy(data, AP_NAME, sizeof(data));

	// Create the UDP socket
	int socket_desc = sl_Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check success
	if(socket_desc < 0){
		return false;
	}

	// Set socket id
	this->_udp_socket._socket_id = socket_desc;

	// Get server ip
	if(this->_connect_udp_socket()){
		return true;
	}
	return false;
}

/**
 * This is the default run method for the procedure.
 */
void NTP_Client::run(){

	// Receive data and set internal clock
	if(this->_receive_data()){
		this->_set_internal_clock();
	}
}

// Private Context

/**
 * This method connects the UDP socket to the NTP server
 */
bool NTP_Client::_connect_udp_socket(){

	// Set the buffer to 0
	memset(this->_buffer, 0x00, sizeof(this->_buffer));

	// Create a UDP packet
	this->_buffer[0] = '\x1b';

	// Set the socket parameters
	this->_udp_socket._address.sa_family = AF_INET;

	// Source IP
	this->_udp_socket._address.sa_data[0] = 0x00;
	this->_udp_socket._address.sa_data[1] = 0x7B;
	this->_udp_socket._address.sa_data[2] = (char) ((this->_udp_socket._dest_ip >> 24) & 0xFF);
	this->_udp_socket._address.sa_data[3] = (char) ((this->_udp_socket._dest_ip >> 16) & 0xFF);
	this->_udp_socket._address.sa_data[4] = (char) ((this->_udp_socket._dest_ip >> 8) & 0xFF);
	this->_udp_socket._address.sa_data[5] = (char) ((this->_udp_socket._dest_ip) & 0xFF);

	// Send packet
	int result = sl_SendTo(this->_udp_socket._socket_id,
						   this->_buffer,
						   sizeof(this->_buffer), 0,
						   &this->_udp_socket._address,
						   sizeof(SlSockAddr_t));
	// Do some checking
	if(result != sizeof(this->_buffer)){
		Message("Could not send out the UDP request");
		return false;
	}

	// Wait to receive the NTP time
	this->_udp_socket._local_address.sin_family = SL_AF_INET;
	this->_udp_socket._local_address.sin_port = 0;
	this->_udp_socket._local_address.sin_addr.s_addr = 0;

	// Bind to the socket
	sl_Bind(this->_udp_socket._socket_id,
			(SlSockAddr_t*)&this->_udp_socket._local_address,
			sizeof(SlSockAddrIn_t));
	return true;
}

/**
 * This receives data from the UDP Port.
 */
bool NTP_Client::_receive_data(){

	// Receive time
	int addr = sizeof(SlSockAddrIn_t);
	int result = sl_RecvFrom(this->_udp_socket._socket_id,
						 this->_buffer, 48, 0,
						 (SlSockAddr_t*)&this->_udp_socket._local_address,
						 (SlSocklen_t*)&addr);
	// Do some checking
	if(result <= 0){
		return false;
	}

	// Check the mode of the packet
	if((this->_buffer[0] & 0x07) != 4){
		return false;
	}
	return true;
}

/**
 * This sets the internal clock time with the received data.
 */
void NTP_Client::_set_internal_clock(){

	// We create a long var for time
	for(register char i = 40; i < 44; i++){

		// Shift the time 8
		this->_time <<= 8;

		// Add the values
		this->_time += this->_buffer[i];
	}

	// We have our time packet
	Report("The time is %u", this->_time);

//	// We set the DS1307 context
//	DS1307_Date_Time* date_time = new DS1307_Date_Time(this->_time);
//	this->_configs._rtc->adjust(date_time);
}
