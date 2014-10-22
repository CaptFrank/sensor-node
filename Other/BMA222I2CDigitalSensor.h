/*
 * BMA222I2CDigitalSensor.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Administrator
 */

#ifndef BMA222I2CDIGITALSENSOR_H_
#define BMA222I2CDIGITALSENSOR_H_



/**
 * This is the BM222 sensor driver and handler. We use this to access the data
 * registers from the I2C device. We also use it to control the device and its
 * applications. This class extends the @see I2C_Base_Driver
 *
 * **** NOTE: We are not using this class.
 */
class BMA222_I2C_Digital_Sensor: public I2C_Base_Driver {};

#endif /* BMA222I2CDIGITALSENSOR_H_ */
