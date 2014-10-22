/*
 * MQTTClient.h
 *
 *  Created on: Jun 18, 2014
 *      Author: Administrator
 */

#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

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

#include "Timer.h"

// -------------------------------------------------------------------------------
// Defines

// General constants
#define WAIT_LOOP_COUNT				10
#define MAX_PACKET_SIZE 			SL_SOCKET_PAYLOAD_TYPE_TCP_IPV4
#define KEEPALIVE 					15000
#define CONNECTION_TIMEOUT			30
#define SESSION_OPEN_LENGTH			32
#define MQTTPROTOCOLVERSION 		3

/**
 * This is an enum of the posible states of the MQTT client engine
 */
enum MQTT_state_t {

	// Connection States
	MQTT_CONNECT			= 1 << 4,   //!<  MQTT_CONNECT
	MQTT_CONNACK			= 2 << 4,   //!<  MQTT_CONNACK

	// Publish States
	MQTT_PUBLISH			= 3 << 4,   //!<  MQTT_PUBLISH
	MQTT_PUBACK				= 4 << 4,   //!<  MQTT_PUBACK
	MQTT_PUBREC				= 5 << 4,   //!<  MQTT_PUBREC
	MQTT_PUBREL				= 6 << 4,   //!<  MQTT_PUBREL
	MQTT_PUBCOMP			= 7 << 4,   //!<  MQTT_PUBCOMP

	// Subscribe States
	MQTT_SUBSCRIBE			= 8 << 4,  	//!<  MQTT_SUBSCRIBE
	MQTT_SUBACK				= 9 << 4,   //!<  MQTT_SUBACK

	// Unsubscribe States
	MQTT_UNSUBSCRIBE		= 10 << 4,	//!<  MQTT_UNSUBSCRIBE
	MQTT_UNSUBACK			= 11 << 4,  //!<  MQTT_UNSUBACK

	// Ping States
	MQTT_PINGREQ			= 12 << 4,  //!<  MQTT_PINGREQ
	MQTT_PINGRESP			= 13 << 4,  //!<  MQTT_PINGRESP

	// Other States
	MQTT_DISCONNECT			= 14 << 4,	//!<  MQTT_DISCONNECT
	MQTT_RESERVED			= 15 << 4   //!<  MQTT_RESERVED
};

/**
 * This is the QOS definition for the MQTT protocol
 */
enum MQTT_qos_t {

	MQTT_QOS0				= (0 << 1),	//!<  MQTT_QOS0
	MQTT_QOS1				= (1 << 1),	//!<  MQTT_QOS1
	MQTT_QOS2				= (2 << 1) 	//!<  MQTT_QOS2
};

//!TCP Socket error codes
enum TCP_socket_err_t{

	__TCPSOCKET_MIN = -0xFFFF,
	TCPSOCKET_SETUP, 					//!< TCPSocket not properly configured
	TCPSOCKET_TIMEOUT, 					//!< Connection timed out
	TCPSOCKET_IF, 						//!< Interface has problems, does not exist or is not initialized
	TCPSOCKET_MEM, 						//!< Not enough mem
	TCPSOCKET_INUSE, 					//!< Interface / Port is in use
	TCPSOCKET_EMPTY, 					//!< Connections queue is empty
	TCPSOCKET_RST, 						//!< Connection was reset by remote host
	//...
	TCPSOCKET_OK = 0 					//!< Success
};

//!TCP Socket Events
enum TCP_socket_evt_t {

	TCPSOCKET_CONNECTED, 				//!< Connected to host
	TCPSOCKET_ACCEPT,  					//!< Client is connected, must call accept() to get a new Socket
	TCPSOCKET_READABLE, 				//!< Data in buf
	TCPSOCKET_WRITEABLE, 				//!< Can write data to buf
	TCPSOCKET_CONTIMEOUT, 				//!< Connection timed out
	TCPSOCKET_CONRST, 					//!< Connection was reset by remote host
	TCPSOCKET_CONABRT, 					//!< Connection was aborted
	TCPSOCKET_ERROR, 					//!< Unknown error
	TCPSOCKET_DISCONNECTED 				//!< Disconnected
};

/**
 * This is the user configuration structure conatining the
 * necessary credentials to connect to the server.
 */
struct user_configs_t {

	char* client_id;
	char* username;
	char* password;
};

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the MQTT client handler class. It is the main interaction point for the
 * MQTT interface. It provides teh facilities to publish and subscribe content
 * from one node to the other.
 */
class MQTT_Client {

	// Public Context
	public:

		/**
		 * This is the default constructor for the class with no parameters.
		 *
		 * @param timer								- the timer object pointer
		 */
		MQTT_Client(Timer* timer);

		/**
		 * This is the overriden constructor for the class with the appropriate
		 * setup parameters listed below.
		 *
		 * @param address							- the socket address
		 * @param port								- the socket port
		 * @param callback							- the callback method
		 * @param timer								- the timer object pointer
		 */
		MQTT_Client(unsigned long address, int port,
						void (*callback)(MQTT_Client*, char*, char*), Timer* timer);


		/**
		 * This is the default deconstructor for the class.
		 */
		~MQTT_Client();

		/**
		 * This is the setup method for the class.
		 *
		 * @param server							- the server address
		 * @param port								- the socket port
		 * @param callback							- the client side callback
		 */
		void MQTT_setup(unsigned long address, int port,
						void (*callback)(MQTT_Client*, char*, char*));

		/**
		 * This is the setup method for the class.
		 *
		 * @param address							- the server address
		 * @param server_port						- the socket port
		 * @param username							- the session username
		 * @param password							- the session password
		 * @param callback							- the client side callback
		 */
		void MQTT_setup(unsigned long address, int port, char* username,
						char* password, void (*callback)(MQTT_Client*, char*, char*));

		/**
		 * This is the default connect method, which connects to the MQTT server.
		 *
		 * @param id								- the session id
		 * @return int								- the connection status
		 */
		int MQTT_connect(char* id);

		/**
		 * This is the default disconnection method for the class.
		 */
		void MQTT_disconnect();

		/**
		 * This is the default publish method for the MQTT handler. It publishes
		 * a message in a specified topic.
		 *
		 * @param topic								- the server side topic
		 * @param message							- the message to publish
		 * @return int								- the status of the publish action
		 */
		int MQTT_publish(char* topic, char* message);

		/**
		 * This is the default subscribe method. It is teh registration of a
		 * subscription from within the class to a specified topic.
		 *
		 * @param topic								- the server side topic
		 * @return int								- the status of the subscribe action
		 */
		int MQTT_subscribe(char* topic);

		/**
		 * This is the hearbeat scheduler. It sends out a hearbeat to the
		 * server to keep alive.
		 */
		void MQTT_live();

		/**
		 * This method checks if there has been a response.
		 */
		void MQTT_poll();

		/**
		 * This sets the ip obtained flag
		 * @param acquired
		 */
		static void MQTT_set_ip_acquired(bool acquired){
			MQTT_Client::_ip_obtained = acquired;
		}

		/**
		 * This sets the event that occured on the TCP bus
		 * @param event
		 */
		static void MQTT_set_tcp_event(SlSockEvent_t* event){
			MQTT_Client::_event = event;
		}

		/**
		 * Previous data
		 */
		struct {
			char* previous_topic;
			char* previous_data;
		}_previous;

	// Private Context
	private:

		/**
		 * This is the private access to the open session action. This opens a
		 * session from within the server.
		 *
		 * @param id								- the session id
		 * @return int								- the status of the session open
		 */
		int _open_session(char* id);

		/**
		 * This is a check to see if the session that we opened is in fact
		 * opened.
		 */
		void _read_open_session();

		/**
		 * This is the private access to the send message method.
		 *
		 * @param msg								- the message
		 * @param size								- the size of the message
		 * @return int								- the status of the action
		 */
		int _send_data(const char* msg, int size);

		/**
		 * This is the private acces read packet method. It reads received packets and
		 * devides what to do with them accordingly
		 */
		void _read_data();

		/**
		 * This sets up a client side socket to the MQTT server.
		 *
		 * @return int								- the status
		 */
		int _setup_tx_tcp_socket();

		/**
		 * This sets up a server side socket to the MQTT server.
		 *
		 * @return int								- the status
		 */
		int _setup_rx_tcp_socket();

		//! Variables

		//! User credentials
		user_configs_t* _configs;

		//! Server socket -- Client
		struct {

			//! Server address
			SlSockAddrIn_t* _ip;

			//! Application port
			int _port;

			//! Socket handle
			int _id;

			//! Hex IP address
			unsigned long _address;

		} _remote_configs, _local_configs;

		//! State containers
		struct {

			//! Connected to server ?
			bool connected;

			//! Server session opened?
			bool session_opened;

			//! Time of last action
			int last_activity;
		} _connection_state;

		//! Buffer
		UINT8 _buffer[MAX_PACKET_SIZE];

		//! The activity timer
		Timer* _timer;

		static SlSockEvent_t* _event;

		//! IP obtained
		static bool _ip_obtained;

		//! The callback function for server responses
		void (*callback_server)(MQTT_Client*, char*, char*);
};



#endif /* MQTTCLIENT_H_ */
