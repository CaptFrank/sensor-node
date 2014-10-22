/*
 *  ======== main.c ========
 */

// -------------------------------------------------------------------------------
// Includes

#include <SystemContext.h>

// -------------------------------------------------------------------------------
// Defines

// Task priorities
#define SPAWN_TASK_PRIORITY 9

// Stack size for processes
#define OSI_STACK_SIZE		300

// The system context class
System_Context* _sys;

// System structure
system_t _system;

extern "C" {

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatypes.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_apps_rcm.h"
#include "rom.h"
#include "rom_map.h"
#include "pin.h"
#include "utils.h"
#include "prcm.h"
#include "interrupt.h"

#include "socket.h"
#include "SimpleLink.h"

#include "gpio.h"
#include "pinmux.h"
#include "timer.h"

// Interfaces
#include "osi.h"
#include "timer_if.h"
#include "wdt_if.h"
#include "uart_if.h"
#include "gpio_if.h"
#include "network_if.h"
#include "udma_if.h"

}

// Threads needed to boot and work
#include "Thread_System_Boot.h"
#include "Thread_Data_Getter.h"
#include "Thread_Data_Logger.h"
#include "Thread_System_Monitor.h"
#include "Thread_MQTT_Engine.h"

// Globals
bool ip_obtained;

// Socket event
SlSockEvent_t* evt;

extern "C" {

// -------------------------------------------------------------------------------
// Callbacks
/**
 * This is the NETAPP event handler.
 *
 * @param pNetAppEvent								- the NETAPP event
 */
void sl_NetAppEvtHdlr(SlNetAppEvent_t *pNetAppEvent){

	// We check the event
	switch(pNetAppEvent->Event){

		// We get an IP?
		case SL_NETAPP_IPV4_ACQUIRED:
		case SL_NETAPP_IPV6_ACQUIRED:

			// Get the IP assigned by the DHCP server
			// g_ulIpAddr = pNetAppEvent->EventData.ipAcquiredV4.ip;

			/*************************************************************
			 * Put a break point here to check IP address device aquired
			 *************************************************************/

			// Set the ip obtained flag withiin the MQTT class
			//MQTT_Client::MQTT_set_ip_acquired(true);
			ip_obtained = true;
			break;

		// Nothing happened
		default:
			break;
	}
}

/**
 * This is the handler function for socket events.
 *
 * @param pSocketEvent								- the socket event
 */
void sl_SockEvtHdlr(SlSockEvent_t* pSocketEvent){

	// Set the event internally inside the MQTT client class.
	//MQTT_Client::MQTT_set_tcp_event(pSocketEvent);
	evt = pSocketEvent;

	// TODO
}

/**
 * This is the HTTP server callback.
 * Nothing is done here.
 *
 * @param pSlHttpServerEvent						- the http event
 * @param pSlHttpServerResponse						- the server response
 */
void sl_HttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
		SlHttpServerResponse_t *pSlHttpServerResponse){}

/**
 * This is the WLAN event handler
 * @param pSlWlanEvent
 */
void sl_WlanEvtHdlr(SlWlanEvent_t *pSlWlanEvent){
	switch(((SlWlanEvent_t*)pSlWlanEvent)->Event){
		case SL_WLAN_CONNECT_EVENT:
			Network_IF_SetMCUMachineState(MCU_AP_ASSOC);
			break;
		case SL_WLAN_DISCONNECT_EVENT:
			Network_IF_UnsetMCUMachineState(MCU_AP_ASSOC);
			break;
	}
}

// -------------------------------------------------------------------------------
// Subroutine code

/**
 * This is the board initialization and configuration, based
 * on what OS we are using. In this case we are using the TIRTOS.
 */
static void InitBoard(void) {

/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS

	// We set teh vector table
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
	MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif

	// We enalbe the processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);
    PRCMCC3200MCUInit();
}

/**
 * This is the adc interrupt routine.. Nothing is done here...
 */
static void handle_adc_interrupt(){

	// Return
}

/**
 * This is the temperature sensor interrupt service routine
 * we assert the system overheat flag to reset.
 */
static void handle_temp_interrupt(){

	// Assert the overheat flag
	_sys->_overheat = true;
}

/**
 * This is the applicaiton startup message on the UART bus. We display this
 * on the user console for interaction....
 */
static void DisplayBanner(){

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t	CC3200 HomeMonitor Application       \n\r");
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}

// -------------------------------------------------------------------------------
// Main code

/**
 * This is the main routine of the program. This is the main entry point
 * of the system.
 */
void main(){

	// Config the board
	InitBoard();

	// Config the pin muxing
	PinMuxConfig();

	// Set the SD card clock as an output pin
    MAP_PinDirModeSet(PIN_07, PIN_DIR_MODE_OUT);

    // Change Pin 58 Configuration from Default to Pull Down
    PinConfigSet(PIN_58, PIN_STRENGTH_2MA|PIN_STRENGTH_4MA, PIN_TYPE_STD_PD);

    // Setup interrupts
    GPIO_IF_ConfigureNIntEnable(GPIOA2_BASE, PIN_16, GPIO_RISING_EDGE, handle_temp_interrupt);
    GPIO_IF_ConfigureNIntEnable(GPIOA3_BASE, PIN_53, GPIO_RISING_EDGE, handle_adc_interrupt);

    // Init the leds
    GPIO_IF_LedConfigure(LED1|LED2|LED3);

    // Turn Off the LEDs
    GPIO_IF_LedOff(MCU_ALL_LED_IND);

	// Init the terminal
	InitTerm();
	ClearTerm();
	DisplayBanner();

	// We boot the system
	if(!SYSTEM_BOOTUP_THREAD(&_system, _sys)){

		// Not a good boot
		return;
	}


	// Start simplelink thread
	VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);

	// Create tasks
	osi_TaskCreate(P_OSI_TASK_ENTRY(SYSTEM_DATA_GETTER_THREAD), 	(const signed char*)"DATA", 	OSI_STACK_SIZE, NULL, 1, NULL);
	osi_TaskCreate(P_OSI_TASK_ENTRY(SYSTEM_DATA_LOGGER_THREAD), 	(const signed char*)"LOGGER", 	OSI_STACK_SIZE, NULL, 1, NULL);
	osi_TaskCreate(P_OSI_TASK_ENTRY(SYSTEM_MONITOR_THREAD), 		(const signed char*)"MONITOR", 	OSI_STACK_SIZE, NULL, 1, NULL);
	osi_TaskCreate(P_OSI_TASK_ENTRY(SYSTEM_MQTT_ENGINE_THREAD), 	(const signed char*)"DATA", 	OSI_STACK_SIZE, NULL, 1, NULL);

	// Start the os
	osi_start();
}
} // c
