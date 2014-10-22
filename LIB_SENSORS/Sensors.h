/*
 * Sensors.h
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#ifndef SENSORS_H_
#define SENSORS_H_

/**
 * This is the include file that includes all sensor types.
 * This file is needed to overcome the fact that all files are linked via
 * a tree directory structure.
 * 
 * The most needed file or interface class is located within the base directory
 * of the sensor drivers.
 */


// Include the files...
// Base
#include "SensorDriver.h"

// I2C
#include "SHT21I2CDigitalSensor.h"
#include "BMP180I2CDigitalSensor.h"
#include "TMP006I2CDigitalSensor.h"
#include "TMP102I2CDigitalSensor.h"
#include "ADS1015I2CDigitalSensor.h"

// Base
#include "I2CBaseDriver.h"

#endif /* SENSORS_H_ */
