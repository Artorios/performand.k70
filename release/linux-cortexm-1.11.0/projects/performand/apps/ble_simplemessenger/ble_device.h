/*
 * ble_device.h
 *
 *  Created on: Sep 20, 2013
 *      Author: rasmus
 */

#ifndef BLE_DEVICE_H_
#define BLE_DEVICE_H_

#include "Queue/Queue.h"

#define STD_WAIT_TIME   1000 // us
#define STD_TIMEOUT     5    // sec
#define STD_TIMEOUT_CNT (STD_TIMEOUT * 1000000)/STD_WAIT_TIME

//#define VERBOSITY 0

#ifndef VERBOSITY
#warning "VERBOSITY not defined: Using VERBOSITY=1"
#define VERBOSITY 1
#endif

#define debug(level, str, ...) if(level > VERBOSITY); else printf(str, ## __VA_ARGS__)

typedef struct {
		char *port;
		int fd;
		char MAC[6];
		char connMAC[6];
		long connHandle;
		int _connected;
		int _run;
		queue_t txQueue;
		queue_t rxQueue;
} BLE_Central_t;

#endif /* BLE_DEVICE_H_ */