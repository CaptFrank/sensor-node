/*
 * SystemDefines.h
 *
 *  Created on: Jun 27, 2014
 *      Author: Administrator
 */

#ifndef SYSTEMDEFINES_H_
#define SYSTEMDEFINES_H_

// -------------------------------------------------------------------------------
// Defines

#define AP_NAME						"Halifax_Home_Monitoring"
#define AP_PASSWORD					"Garf2345"

// NTP
#define NTP_SERVER_DNS				"0.ca.pool.ntp.org"
#define GMT_TIME_HOURS_OFFSET		0 // -4
#define GMT_TIME_MINUTES_OFFSET		0

// MQTT
#define MQTT_LONG_IP_ADDRESS		(unsigned long)167772426
#define MQTT_INT_SERVER_PORT		(int)1883
#define MQTT_SENSOR_ID				"sensor1@home_monitoring"

#define MQTT_SERVER_USERNAME		""
#define MQTT_SERVER_PASSWORD		""

#define MQTT_SERVER_ADDRESS			{(MQTT_LONG_IP_ADDRESS >> 24) & 0xFF, 	\
									(MQTT_LONG_IP_ADDRESS >> 16) & 0xFF, 	\
									(MQTT_LONG_IP_ADDRESS >> 8) & 0xFF,		\
									MQTT_LONG_IP_ADDRESS & 0xFF} //long == 10.0.1.10

#define MQTT_SERVER_PORT			{(MQTT_INT_SERVER_PORT >> 8) & 0xFF,	\
									MQTT_INT_SERVER_PORT & 0xFF}

// DB Constants
#define DB_RECORD_SIZE				sizeof(system_state_t)
#define DB_TABLE_SIZE				DB_RECORD_SIZE * 20
#define DB_RECORD					1

// Sensor ID
#define SENSOR_ID					"sensor1"

// SUB Topics
#define MQTT_SUB_TOPIC_1			"/glb_cmd"
#define MQTT_SUB_TOPIC_2			"control/sensor1_cmd"

// PUB Topics
#define MQTT_PUB_TOPIC_1			"home/sensor1_d"
#define MQTT_PUB_TOPIC_2			"home/sensor1_l"

#endif /* SYSTEMDEFINES_H_ */
