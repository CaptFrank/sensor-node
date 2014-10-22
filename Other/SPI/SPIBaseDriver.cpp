/*
 * SPIBaseDriver.cpp
 *
 *  Created on: Jun 19, 2014
 *      Author: Administrator
 */

#include "Sensor Drivers/Base/SPIBaseDriver.h"

// -------------------------------------------------------------------------------
// Class Deconstructor

/**
 * The default deconstructor of the class
 */
SPI_Base_Driver::~SPI_Base_Driver(){

	// Disable the SCLK
	MAP_PRCMPeripheralClkDisable(PRCM_GSPI);

	// Disable the FIFO
	SPIFIFODisable(GSPI_BASE, MCSPI_RX_FIFO | MCSPI_TX_FIFO);

	// Reset the SPI controller
	SPIReset(GSPI_BASE);

	// Disable the SPI bus
	SPIDisable(GSPI_BASE);
}

// -------------------------------------------------------------------------------
// Setup

/**
 * This is the generic setup method for a sensor.
 */
void SPI_Base_Driver::setup(){

	// Setup the SPI CLK
	PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

	// Reset the peripheral
	PRCMPeripheralReset(PRCM_GSPI);

	// Reset the SPI Controller
	SPIReset(GSPI_BASE);

	// Configure the SPI interface
	SPIConfigSetExpClk(GSPI_BASE, GSPI_BASE, MAP_PRCMPeripheralClockGet(PRCM_GSPI),
            			   SPI_IF_BIT_RATE, SPI_MODE_MASTER, SPI_SUB_MODE_0,
            			   (SPI_SW_CTRL_CS |
            					   SPI_4PIN_MODE |
            					   SPI_TURBO_OFF |
            					   SPI_CS_ACTIVEHIGH |
            					   SPI_WL_8));

	// Set the FIFO
	SPIFIFOEnable(GSPI_BASE, MCSPI_RX_FIFO | MCSPI_TX_FIFO);

	// Enable the SPI Comms
	SPIEnable(GSPI_BASE);

	// Set CS -- Disable CS
	SPICSDisable(GSPI_BASE);
}

// -------------------------------------------------------------------------------
// General Data I/O

/**
 * This is the default read method for a sensor which returns a
 * buffer type given a device address or pin.
 *
 * @param request								- the read request
 * @return buffer_t								- the buffer type with the data in it
 */
buffer_t* SPI_Base_Driver::read_byte(request_t* request){

	// Set teh read length to 1
	request->_rx_data->_length = 0x01;

	// We call the read bytes method
	return this->read_bytes(request);
}

/**
 * This is the default write method for any sensor type. We give it a buffer type
 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
 * to send the data.
 *
 * @param request								- the write request
 */
void SPI_Base_Driver::write_byte(request_t* request){

	// Set teh read length to 1
	request->_tx_data->_length = 0x01;

	// We call the read bytes method
	this->write_bytes(request);
}

/**
 * This is the default read method for a sensor which returns a
 * buffer type given a device address or pin.
 *
 * @param request								- the write request
 * @return buffer_t								- the buffer type with the data in it
 */
buffer_t* SPI_Base_Driver::read_bytes(request_t* request){

	// We activate the communication
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_START);

	// Transfer a byte
	if(SPITranfer(GSPI_BASE, 0x00, request->_rx_data->_buffer,
				request->_rx_data->_length, SPI_CS_ENABLE|SPI_CS_DISABLE) != SUCCESS){

		// Data corrupt --- Not good
		request->_rx_data->_valid = false;

	}else{

		// Validate the data
		request->_rx_data->_valid = true;
	}

	// Deactivate the SPI bus
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_STOP);

	// Return the struct
	return request->_rx_data;

}

/**
 * This is the default write method for any sensor type. We give it a buffer type
 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
 * to send the data.
 *
 * @param request								- the write request
 */
void SPI_Base_Driver::write_bytes(request_t* request){

	// We activate the communication
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_START);

	// Transfer a byte
	if(SPITranfer(GSPI_BASE, request->_tx_data->_buffer, 0x00,
				request->_tx_data->_length, SPI_CS_ENABLE|SPI_CS_DISABLE) != SUCCESS){

		// Data corrupt --- Not good
		request->_tx_data->_valid = false;

	}else{

		// Validate the data
		request->_tx_data->_valid = true;
	}

	// Deactivate the SPI bus
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_STOP);
}

// -------------------------------------------------------------------------------
// Register I/O

/**
 * This is the default read register method for a sensor which returns a
 * buffer type given a device address or pin.
 *
 * @param request								- the read request
 * @return buffer_t								- the buffer type with the data in it
 */
buffer_t* SPI_Base_Driver::read_reg(request_t* request){

	// We activate the communication
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_START);

	// Write the command and read
	bool status = SPITransfer(GSPI_BASE, request->_tx_data->_buffer, request->_rx_data->_buffer,
							 request->_tx_data->_length, SPI_CS_ENABLE|SPI_CS_DISABLE));

	if(status != SUCCESS){

		// Data corrupt --- Not good
		request->_rx_data->_valid = false;
	}else{

		// Validate the data
		request->_rx_data->_valid = true;
	}

	// Deactivate the SPI bus
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_STOP);

	// Return the buffer
	return request->_rx_data;
}

/**
 * This is the default write register method for any sensor type. We give it a buffer type
 * with the data that we want to send and the device adress (I2C) or the device pin (SPI)
 * to send the data.
 *
 * @param request								- the write request
 */
void SPI_Base_Driver::write_reg(request_t* request){

	// We activate the communication
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_START);

	// Write the data to write
	bool status = SPITransfer(GSPI_BASE, request->_tx_data->_buffer, request->_rx_data->_buffer,
							  request->_tx_data->_length, SPI_CS_ENABLE|SPI_CS_DISABLE));

	if(status != SUCCESS){

		// Data corrupt --- Not good
		request->_tx_data->_valid = false;
	}else{

		// Validate the data
		request->_tx_data->_valid = true;
	}

	// Deactivate the SPI bus
	GPIOPinWrite(this->_port,
				 this->_pin,
				 SPI_STOP);
	return;

}
