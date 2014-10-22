/*
 * MQTTClient.cpp
 *
 *  Created on: Jun 18, 2014
 *      Author: Administrator
 */

#include "MQTTClient.h"

// Public Context

/**
 * This is the default constructor for the class with no parameters.
 * @param timer								- the timer object pointer
 */
MQTT_Client::MQTT_Client(Timer* timer){

	// Set the timer
	this->_timer = timer;
}

/**
 * This is the default deconstructor for the class.
 */
MQTT_Client::~MQTT_Client(){}

/**
 * This is the overriden constructor for the class with the appropriate
 * setup parameters listed below.
 *
 * @param address							- the socket address
 * @param port								- the socket port
 * @param callback							- the callback method
 * @param timer								- the timer object pointer
 */
MQTT_Client::MQTT_Client(unsigned long address, int port,
						void (*callback)(MQTT_Client*, char*, char*), Timer* timer){

	// Set the timer
	this->_timer = timer;

	// Setup
	this->MQTT_setup(address, port, callback);
}

/**
 * This is the setup method for the class.
 *
 * @param address							- the server address
 * @param server_port						- the socket port
 * @param callback							- the client side callback
 */
void MQTT_Client::MQTT_setup(unsigned long address, int port,
				void (*callback)(MQTT_Client*, char*, char*)){

	// Set the internal values
	callback_server = callback;
	this->_remote_configs._address = address;
	this->_remote_configs._port = port;
	this->_remote_configs._port = port + 1; // We increase the port to have a seperate pipe
	this->_configs->username = 0x00;
	this->_configs->password = 0x00;
	this->_connection_state.connected = false;
	this->_connection_state.session_opened = false;

	// Reste the static vars
	MQTT_Client::_ip_obtained = false;
	MQTT_Client::_event = 0x00;

	// We start the timer
	this->_timer->start();

	// We init the sockets
	this->_setup_rx_tcp_socket();
	this->_setup_tx_tcp_socket();
}

/**
 * This is the complete setup method for the class.
 *
 * @param address							- the server address
 * @param port						- the socket port
 * @param username							- the username
 * @param password							- the password
 * @param callback							- the client side callback
 */
void MQTT_Client::MQTT_setup(unsigned long address, int port,
				char *username, char *password, void (*callback)(MQTT_Client*, char*, char*)){

	// We setup the pipes
	this->MQTT_setup(address, port, callback);

	// We set the username and password
	this->_configs->username = username;
	this->_configs->password = password;
}

/**
 * This is the default connect method, which connects to the MQTT server.
 *
 * @param id								- the session id
 * @return int								- the connection status
 */
int MQTT_Client::MQTT_connect(char* id){

	// Connected to the server and hosting a client connection
	// This was done in the setup method

	// We send an open session message to the server
	this->_open_session(id);

	// Wait server notice of open sesion
	int i = 0;
	while (!this->_connection_state.session_opened) {

		Message("Waiting for a connection.\n\r");
		// Delay a bit
		MAP_UtilsDelay(10000);

		// If we got connected
		if (!this->_connection_state.connected || ++ i > CONNECTION_TIMEOUT) {
			Message("Connected.\n\r");
			break;
		}
	}

	// If we started a session but not connected
	if (!this->_connection_state.connected) {
		// Weird error
		Message("Error\n\r");
		return -2;
	}

	this->_connection_state.last_activity = this->_timer->get_ms();
	return 1;
}

/**
 * This is the default disconnection method for the class.
 */
void MQTT_Client::MQTT_disconnect(){

	// Create a packet
    char packet[] = {0xe0, 0x00};

    // send the break
    this->_send_data((char*)packet, sizeof(packet));

    // Change the connection state
    this->_connection_state.connected = false;
}

/**
 * This is the default publish method for the MQTT handler. It publishes
 * a message in a specified topic.
 *
 * @param topic								- the server side topic
 * @param message							- the message to publish
 * @return int								- the status of the publish action
 */
int MQTT_Client::MQTT_publish(char* topic, char* message){

	// Set previous data
    this->_previous.previous_topic = topic;
    this->_previous.previous_data = message;

	// We build a packet to send
	char temp = strlen(topic);

	// VAR HEADER
	// Create the variable header
	char* var_header_pub = (char*) malloc(temp + 3);

	// Copy the topic
	strcpy((char*) &var_header_pub[2], topic);

	// Add a header
	var_header_pub[0] = 0x00;
	var_header_pub[1] = temp;

	// Add a tail
	var_header_pub[sizeof(var_header_pub) - 1] = 0x00;

	// FIXED HEADER
	// Create the fixed header
	temp = strlen(message);
	char fixed_header_pub[] = {MQTT_PUBLISH, sizeof(var_header_pub) + strlen(message)};

	// PACKET
	// Create a packet
    char* packet_pub = (char*) malloc(sizeof(fixed_header_pub) + sizeof(var_header_pub) + temp) ;

    // Reset the packet
    memset(packet_pub, 0, sizeof(packet_pub));

    // Add the fixed header
    memcpy(packet_pub, fixed_header_pub, sizeof(fixed_header_pub));

    // Add the variable header
    memcpy(packet_pub + sizeof(fixed_header_pub), var_header_pub, sizeof(var_header_pub));

    // Add the message
    memcpy(packet_pub + sizeof(fixed_header_pub) + sizeof(var_header_pub), message, temp);

    // Send the data
    if (!this->_send_data((char*)packet_pub, sizeof(packet_pub))) {

    	// We have an error
    	Message("Publish error.\n\r");
    	free(var_header_pub);
        return -1;
    }

    // We are good to go
    Message("Publish success. \n\r");
    free(var_header_pub);
    return 1;
}

/**
 * This is the default subscribe method. It is teh registration of a
 * subscription from within the class to a specified topic.
 *
 * @param topic								- the server side topic
 * @return int								- the status of the subscribe action
 */
int MQTT_Client::MQTT_subscribe(char* topic){

	char temp = strlen(topic);

	// Are we connected?
	if(this->_connection_state.connected){

		// VAR HEADER
		// Create a variable header
		char var_header_topic[] = {0, 10};

		// FIXED HEADER
		// Create a fixed length header
		char fixed_header_topic[] = {MQTT_SUBSCRIBE,
					sizeof(var_header_topic) +
					temp + 3};

		// UTF Topic
		char* utf_topic = (char*) malloc(temp + 3);
		strcpy((char *)&utf_topic[2], topic);

		utf_topic[0] = 0;
		utf_topic[1] = temp;
		utf_topic[sizeof(utf_topic) - 1] = 0;

		// PACKET
		// Create the packet container
		char* packet_topic= (char*) malloc(sizeof(var_header_topic) + sizeof(fixed_header_topic) + temp + 3);

		// Reset the packet mem
		memset(packet_topic, 0, sizeof(packet_topic));

		// Add the fixed header
		memcpy(packet_topic, fixed_header_topic, sizeof(fixed_header_topic));

		// Add the variable header
		memcpy(packet_topic + sizeof(fixed_header_topic), var_header_topic, sizeof(var_header_topic));

		// Add the utf topic
		memcpy(packet_topic + sizeof(fixed_header_topic) + sizeof(var_header_topic), utf_topic, sizeof(utf_topic));

		// Send the data
		if (!this->_send_data(packet_topic, sizeof(packet_topic))) {

			// We got an error
			Message("Error\n\r");
			free(utf_topic);
			return -1;
		}
		// We are good
		Report("Subscribed to: %s", topic);
		free(utf_topic);
		return 1;
	}
	// Not connected

	return -1;
}

/**
 * This is the hearbeat scheduler. It sends out a hearbeat to the
 * server to keep alive.
 */
void MQTT_Client::MQTT_live(){

	// Check if we are connected
    if (this->_connection_state.connected) {

    	// Get the time elapsed
        int time = this->_timer->get_ms();
        if (time > KEEPALIVE) {

            // Send 192 0 to broker
            char packet[] = {0xc0, 0x00};
            this->_send_data((char*)packet, sizeof(packet));
            Message("Sent heartbeat.\n\r");
        }
    }
}

/**
 * This method checks if there has been a response.
 */
void MQTT_Client::MQTT_poll(){

	// Get some data from the connection
	this->_read_data();

	// Check the event that occured.
	switch (MQTT_Client::_event->Event) {

		// Accepted conection
		case TCPSOCKET_ACCEPT:
			Message("TCPSOCKET_ACCEPT\r\n");
			break;

		// Connected
		case TCPSOCKET_CONNECTED:
			Message("TCPSOCKET_CONNECTED\r\n");
			this->_connection_state.connected = true;
			break;

		// Writable socket
		case TCPSOCKET_WRITEABLE:
			Message("TCPSOCKET_WRITEABLE\r\n");
			break;

		// Readable socket
		case TCPSOCKET_READABLE:
			Message("TCPSOCKET_READABLE\r\n");

			// Check if the session is opened
			if (!this->_connection_state.session_opened) {

				// Check if the session is opened
				this->_read_open_session();
			}

			// Read some data
			this->_read_data();
			break;

		// Timeout
		case TCPSOCKET_CONTIMEOUT:
			Message("TCPSOCKET_CONTIMEOUT\r\n");
			break;

		// Reset
		case TCPSOCKET_CONRST:
			Message("TCPSOCKET_CONRST\r\n");
			break;

		// Abort
		case TCPSOCKET_CONABRT:
			Message("TCPSOCKET_CONABRT\r\n");
			break;

		// Error
		case TCPSOCKET_ERROR:
			Message("TCPSOCKET_ERROR\r\n");
			break;

		// Disconnected
		case TCPSOCKET_DISCONNECTED:
			Message("TCPSOCKET_DISCONNECTED\r\n");
			sl_Close(this->_remote_configs._id);
			this->_connection_state.connected = false;
			break;

		// Others.
		default:
			break;
	}
}

// Private Context

/**
 * This is the private access to the open session action. This opens a
 * session from within the server.
 *
 * @param id								- the session id
 * @return int								- the status of the session open
 */
int MQTT_Client::_open_session(char* id){

	// VAR HEADER
	// Create a variable header
	char var_header[] = {0x00, 0x06, 0x4d, 0x51, 0x49, 0x73, 0x64, 0x70,
						 0x03, 0x02, 0x00, KEEPALIVE/500 , 0x00, strlen(id)};

	// FIXED HEADER
	// We then create a fixed header : 2 bytes big endian
	char fixed_header[] = {MQTT_CONNECT, 12 + strlen(id) + 2};
	short username_len = strlen( this->_configs->username );
	short password_len = strlen( this->_configs->password );

	var_header[9] |= (username_len > 0 ? 0x80 : 0x00 );
	var_header[9] |= (password_len > 0 ? 0x40 : 0x00 );

	// PACKET
	// Create a container for the packet
	char* packet = (char*) malloc(sizeof(fixed_header) +
	            		   sizeof(var_header) +
	            		   sizeof(id) + 6 +
	            		   username_len +
	            		   (username_len > 0 ? 2 : 0) +
	            		   password_len +
	            		   (password_len > 0 ? 2 : 0));

	// Reset the packet
	memset(packet, 0, sizeof(packet));

	// Add fixed header
	memcpy(packet, fixed_header, sizeof(fixed_header));

	// Add the variable header
	memcpy(packet + sizeof(fixed_header), var_header, sizeof(var_header));

	// Get the size of the fixed header + var header
	int header = sizeof(fixed_header) + sizeof(var_header);

	// Add the id
	memcpy(packet + header, id, strlen(id));

	// Now we get the header + id
	header += strlen(id);

	// Some checking
	if(username_len > 0){

		// We add the username
		packet[header] = 0x00;
		packet[header + 1] = username_len;
		memcpy(packet + header + 2, this->_configs->username, username_len);

		// CRC
		packet[1] += (username_len + 2);
	}

	// With username
	header += username_len;

	// Some checking
	if (password_len > 0) {

		// We add the password
		packet[header + (username_len > 0 ? 2 : 0) ] = 0x00;
		packet[header + (username_len > 0 ? 2 : 0) + 1] = password_len;
		memcpy(packet + header + (username_len > 0 ? 2 : 0) + 2,
					this->_configs->password, password_len);

		// CRC
		packet[1] += (password_len + 2);
	}

	// We send the packet
	if (!this->_send_data(packet, sizeof(packet))) {

		Message("Error\n\r");

		// Did not send properly
		free(packet);
		return -1;
	}

	Report("Message sent.\n\r");

	// Good send
	free(packet);
	return 1;
}

/**
 * This is a check to see if the session that we opened is in fact
 * opened.
 */
void MQTT_Client::_read_open_session(){

	// Containers
	int len = 0;
	int status;

	// Reset the internal buffer
	memset((void*)this->_buffer, 0x00, sizeof(this->_buffer));

	// Read until there is nothing left
	do {

		// Receive data
		status = sl_Recv(this->_local_configs._id, this->_buffer, SESSION_OPEN_LENGTH, 0);

		// Check the status
		if(status < 0){

			// We got an error
			sl_Close(this->_local_configs._id);
			Message("Error\n\r");
			return;
		}

		// Add 1 to the length
		len ++;

	// Do until we have a 0 status
	}while(status != 0);

	// After the check the session is open
	this->_connection_state.session_opened = true;
	Message("Session opened.\n\r");
}

/**
 * This is the private access to the send message method.
 *
 * @param msg								- the message
 * @param size								- the size of the message
 * @return int								- the status of the action
 */
int MQTT_Client::_send_data(const char* msg, int size){

	// We send the message
	int status = sl_Send(this->_remote_configs._id, msg, size, 0);

	// Check the size that was sent
	if(status <= 0 ){

		// Error
		sl_Close(this->_remote_configs._id);
		return -1;

	// The message was not all sent
	}else if(status != size){

		for (int i = 0; i < size; i++) {
			Report("%x, ", msg[i]);
		}
		Message("Error\n\r");

		// Error
		return -1;

	// All ok
	}else{
		Message("Sent msg success.\n\r");
		return 1;
	}
}

/**
 * This is the private acces read packet method. It reads received packets and
 * devides what to do with them accordingly
 */
void MQTT_Client::_read_data(){

	// Containers
	int len = 0, read_len;
	char type;
	char packet[2];

	// Get data
	while ((read_len = sl_Recv(this->_local_configs._id, this->_buffer, sizeof(this->_buffer), 0)) != 0) {
		len += read_len;
	}

	// Set the end of the string
	this->_buffer[len] = '\0';

	// Report the read values
	Report("Read length: %d, %d\r\n", len, read_len);
	for (int i = 0; i < len; i++) {
		Report("%2X ", this->_buffer[i]);
	}
	Message("\r\n");

	// We check the type that is received
	type = this->_buffer[0] >> 4;

	// Switch on the type of packet received
	switch(type){

		// Connection acknowledged
		case MQTT_CONNACK:
			Message("MQTT_CONNACK\n\r");
			break;

		case MQTT_PUBLISH:

			// If we support the callback functions
			if(this->callback_server){\

				// Containers
				short index = 0;
				short multiplier = 1;
				short value = 0;

				char digit;

				// Convert the values within the buffer
				do {
					digit = this->_buffer[index++];
					value += (digit & 127) * multiplier;
					multiplier *= 128;
				}while((digit & 128) != 0);
				Report("Variable legnth header: %d, index: %d\n\r", value, index);

				// Get the topic length
				char topic_length = (this->_buffer[index] << 3) + this->_buffer[index + 1];
				Report("Topic length: %d\n\r", topic_length);

				// Get the topic
				char* topic = (char*) malloc(topic_length + 1);
				for (register char i = 0; i < topic_length; i++) {
					topic[i] = this->_buffer[index + 2 + i];
				}
				topic[topic_length] = 0;

				// ignore msgID - only support QoS 0 subs
				// Get the payload
				char* payload = (char*)this->_buffer + index + 2 + topic_length;

				// Call the callback with the topic and the payload.
				callback_server(this, topic, (char*)payload);
				free(topic);
			}
			break;

		// Publish Acknowledged
		case MQTT_PUBACK:
			Message("MQTT_PUBACK\n\r");
			break;

		// Subscribe Acknowledge
		case MQTT_SUBACK:
			Message("MQTT_SUBACK\n\r");
			break;

		// Ping Request
		case MQTT_PINGREQ:

			packet[0] = 0xd0;
			packet[1] = 0x00;
			this->_send_data((char*)packet, sizeof(packet));
			this->_connection_state.last_activity = this->_timer->get_ms();
			break;

		// Other callbacks not supported
		default:
			break;
	}
}

/**
 * This sets up a client side socket to the MQTT server.
 * It connects the socket to the server address.
 *
 * @return int								- the status
 */
int MQTT_Client::_setup_tx_tcp_socket(){

	int status;
	int address_size;
	long non_blocking = 1;

	// ----------------------------------------------------------------------------
	// Local Connection (RX)

	// Fill the socket address - local
	this->_remote_configs._ip->sin_family = SL_AF_INET;
	this->_remote_configs._ip->sin_port = sl_Htons(this->_remote_configs._port);
	this->_remote_configs._ip->sin_addr.s_addr = sl_Htonl(this->_remote_configs._address);

	// Get teh size of the address type
	address_size = sizeof(SlSockAddrIn_t);

	// Create the socket object
	this->_remote_configs._id = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);

	// Check what happened
	if(this->_remote_configs._id < 0){

		// Not successful in creating the socket
		Message("Error\n\r");
		return -1;
	}
	Message("Socket creation success.\n\r");

	// Set the socket as non blocking
	status = sl_SetSockOpt(this->_remote_configs._id,
							SL_SOL_SOCKET,
							SL_SO_NONBLOCKING,
							&non_blocking,
							sizeof(non_blocking));

	// Check what happened
	if(status < 0){

		// We did not succeed in connecting to the server
		sl_Close(this->_remote_configs._id);
		Message("SError\n\r");
		return -1;
	}
	Message("Socket options set success.\n\r");

	// connecting to TCP server
	status = sl_Connect(this->_remote_configs._id,
			(SlSockAddr_t*)&this->_remote_configs._ip, address_size);

	// Check connection status
	if( status < 0 ) {

		// error
		sl_Close(this->_remote_configs._id);
		Message("Error\n\r");
		return -1;
	}
	Message("Socket connected.\n\r");
	return 0;
}

/**
 * This sets up a server side socket to the MQTT server.
 * This is setup to listen for sockets.
 *
 * @return int								- the status
 */
int MQTT_Client::_setup_rx_tcp_socket(){

	int status;
	int address_size;
	long non_blocking = 1;

	// ----------------------------------------------------------------------------
	// Remote Server Connection (TX)

	// Fill the socket address - Remote
	this->_local_configs._ip->sin_family = SL_AF_INET;
	this->_local_configs._ip->sin_port = sl_Htons(this->_local_configs._port);
	this->_local_configs._ip->sin_addr.s_addr = 0;

	// Get teh size of the address type
	address_size = sizeof(SlSockAddrIn_t);

	// Create the socket object
	this->_local_configs._id = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);

	// Check what happened
	if(this->_local_configs._id < 0){

		// Not successful in creating the socket
		Message("Error\n\r");
		return -1;
	}
	Message("Socket created.\n\r");

	// We bind the socket to the address specified
	status = sl_Bind(this->_local_configs._id,
					(SlSockAddr_t*) &this->_local_configs._ip,
					address_size);

	// Check is bounded
	if(status < 0){

		// Not successful in binding to the address
		sl_Close(this->_local_configs._id);
		Message("Error\n\r");
		return -1;
	}
	Message("Socket bounded.\n\r");

	// We listen to the incoming connections
	status = sl_Listen(this->_local_configs._id, 0);

	// Error in listening
	if(status < 0){

		// We got an error
		sl_Close(this->_local_configs._id);
		Message("Error\n\r");
		return -1;
	}
	Message("Socket listen active.\n\r");

	// Set the socket as non blocking
	status = sl_SetSockOpt(this->_local_configs._id,
							SL_SOL_SOCKET,
							SL_SO_NONBLOCKING,
							&non_blocking,
							sizeof(non_blocking));
	// Check what happened
	if(status < 0){

		// We did not succeed in connecting to the server
		sl_Close(this->_local_configs._id);
		Message("Error\n\r");
		return -1;
	}
	Message("Socket options set.\n\r");

	// The socket id
	int connection_socket;

	// We check to see if there are any connections that are pending
	// Otherwise we return SL_EAGAIN
	for(register char i = 0; i < WAIT_LOOP_COUNT; i ++){

		// Try to accept
		connection_socket = sl_Accept(this->_local_configs._id,
									 (SlSockAddr_t*)&this->_local_configs._ip,
									 (SlSocklen_t*) &address_size);

		// If we get an again signal
		if(connection_socket == SL_EAGAIN){

			Report("Socket connect try #%d.\n\r", i + 1);

			// Delay a bit
			MAP_UtilsDelay(10000);

		// We got an error
		} else if (connection_socket < 0){

			// Error
			sl_Close(this->_local_configs._id);
			Message("Error\n\r");
			return -1;

		// We are good, we have a connection established
		} else {
			Message("Socket connected..\n\r");
			break;
		}
	}
	return 0;
}
