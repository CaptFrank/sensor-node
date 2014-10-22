/*
 * Thread_System_Boot.h
 *
 *  Created on: Jun 30, 2014
 *      Author: Administrator
 */

#ifndef THREAD_SYSTEM_BOOT_H_
#define THREAD_SYSTEM_BOOT_H_

// -------------------------------------------------------------------------------
// Includes

//extern "C" {
//// Network
//#include "simplelink.h"
//#include "network_if.h"
//
//}

// Include Sensors
#include "Sensors.h"

// Include Loggers
#include "EventLogger.h"

// Include the timer
#include "DS1307I2CDigitalSensor.h"

// Include the network stacks
#include <MQTTClient.h>

// Include the Storages
#include "SDSPIDigitalStorage.h"

// Include System Context
#include "SystemContext.h"

extern bool ip_obtained;
extern SlSockEvent_t* evt;
extern int ticks;

// -------------------------------------------------------------------------------
// Defines

#define I2C_MASTER_ADDRESS			0x00

enum MQTT_callback_commands_t {

	MQTT_CALLBACK_COMMANDS_BOOT			= 0x31,
	MQTT_CALLBACK_COMMANDS_REBOOT		= 0x32,
	MQTT_CALLBACK_COMMANDS_DISCONNECT	= 0x33,
	MQTT_CALLBACK_COMMANDS_STATUS_CHECK = 0x34,
	MQTT_CALLBACK_COMMANDS_GET_DATA		= 0x35,
	MQTT_CALLBACK_COMMANDS_ERROR		= 0x36
};


// -------------------------------------------------------------------------------
// Main code

/**
 * This is the callback function for the MQTT engine.
 *
 * @param topic								- the topic received
 * @param payload							- the payload received
 */
void mqtt_callback(MQTT_Client* client, char* topic, char* payload){

	// We check the payload to see if its a command
	switch(payload[0]){

		// Ww got a boot command
		case MQTT_CALLBACK_COMMANDS_BOOT:
			client->MQTT_live();
			break;

		// We got a reboot
		case MQTT_CALLBACK_COMMANDS_REBOOT:
			_sys->_overheat = true;
			break;

		// We diconnect
		case MQTT_CALLBACK_COMMANDS_DISCONNECT:
			client->MQTT_disconnect();
			break;

		// We send a live
		case MQTT_CALLBACK_COMMANDS_STATUS_CHECK:
			client->MQTT_live();
			break;

		// We send the data already sent
		case MQTT_CALLBACK_COMMANDS_GET_DATA:
			client->MQTT_publish(client->_previous.previous_topic, client->_previous.previous_data);
			break;

		// Nothing to do
		case MQTT_CALLBACK_COMMANDS_ERROR:
		default:
			break;
	}
}

// Include the User defines
#include "SystemDefines.h"

/**
 * This is the main startup routine. It return true if all is
 * loaded correctly.
 *
 * @return success							- the boot success
 */
bool SYSTEM_BOOTUP_THREAD(system_t* _system, System_Context* _sys){

	// The container for the long server address
	//unsigned long _addr;

	// Reset The state of the machine
	Network_IF_ResetMCUStateMachine();

	// Start the driver
	Network_IF_InitDriver(ROLE_STA);

	// Connect to the ap
	SlSecParams_t _params;
	_params.Key = AP_PASSWORD;
	_params.KeyLen = strlen(AP_PASSWORD);
	_params.Type = SL_SEC_TYPE_WPA;
	Network_IF_ConnectAP(AP_NAME, _params);


	// Get the NTP server address....
	// Get the server dns
	char data[sizeof(NTP_SERVER_DNS)];
	memcpy(data, NTP_SERVER_DNS, sizeof(data));

	// Get the IP using the dhcp server
//	_addr = Network_IF_GetHostIP((char*)data);

	// We Initializes the System context
	_sys								= new System_Context(_system);

	// We start by creating new instances of sensors
	_system->_sensors._ads1015_sensor 	= new ADS1015_I2C_Digital_Sensor(_sys, I2C_MASTER_ADDRESS);
	_system->_sensors._bmp180_sensor 	= new BMP180_I2C_Digital_Sensor(_sys, I2C_MASTER_ADDRESS);
	_system->_sensors._ds1307_sensor	= new DS1307_I2C_Digital_Sensor(_sys, I2C_MASTER_ADDRESS);
	_system->_sensors._sht21_sensor		= new SHT21_I2C_Digital_Sensor(_sys, I2C_MASTER_ADDRESS);
	_system->_sensors._tmp006_sensor	= new TMP006_I2C_Digital_Sensor(_sys, I2C_MASTER_ADDRESS);
	_system->_sensors._tmp102_sensor 	= new TMP102_I2C_Digital_Sensor(_sys, I2C_MASTER_ADDRESS);

	// Then we create some storage
	_system->_storage._sd				= new SD_SPI_Digital_Storage(_sys);

	// Then we create loggers
	_system->_loggers._event_logger 	= new Event_Logger(_system->_storage._sd, _sys);

	// Then we create a system timer
	_system->_timer 					= new Timer(&ticks);
//
//	// Then we create the ntp network interface.
//	_system->_network._ntp_client 		= new NTP_Client(_addr,
//												GMT_TIME_HOURS_OFFSET,
//												GMT_TIME_MINUTES_OFFSET);

	// Now that all objects needed are loaded - except for the mqtt client
	// We start them.

	// Sensors
	bool status = true;
	status 						|= _system->_sensors._ads1015_sensor->begin();
	status 						|= _system->_sensors._bmp180_sensor->begin();
	status 						|= _system->_sensors._ds1307_sensor->begin();
	status 						|= _system->_sensors._sht21_sensor->begin();
	status 						|= _system->_sensors._tmp006_sensor->begin();
	status 						|= _system->_sensors._tmp102_sensor->begin();

	// Storage
	status 						|= _system->_storage._sd->begin();

	// Loggers
	status 						|= _system->_loggers._event_logger->begin();

	// Check if all is well
	if(!status){
		return false;
	}

	// Once all is good and initialized, we need to get the time from the network.
	// This allows us to synchronize all nodes together.
	//status 						|= _system->_network._ntp_client->begin(&_params);

	// We run the get
//	_system->_network._ntp_client->run();

	// Set the time from the NTP class.
	DS1307_Date_Time time;
	_system->_sensors._ds1307_sensor->adjust(&time);

	// Once its ran we delete the ntp client memspace
	//delete _system->_network._ntp_client;

	// We then initialize the mqtt client
	_system->_network._mqtt_client		= new MQTT_Client(MQTT_LONG_IP_ADDRESS,
															MQTT_INT_SERVER_PORT,
															mqtt_callback,
															_system->_timer,
															&ip_obtained, evt);

	// We connect
	_system->_network._mqtt_client->MQTT_connect((char*)MQTT_SENSOR_ID);

	// We subscribe to the topics we want
	_system->_network._mqtt_client->MQTT_subscribe((char*)MQTT_SUB_TOPIC_1);
	_system->_network._mqtt_client->MQTT_subscribe((char*)MQTT_SUB_TOPIC_2);

	// We now have our MQTT Engine running...
	// boot up process is done...
	return true;
}



#endif /* THREAD_SYSTEM_BOOT_H_ */
