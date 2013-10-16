/*
 * main.c
 *
 *  Created on: Sep 12, 2013
 *      Author: rasmus
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "Parser/HCI_defs.h"
#include "ble_device.h"
#include "Queue/Queue.h"
#include "NetworkStat/NetworkStatistics.h"
#include "SerialLogic/serialLogic.h"
#include "Parser/Parser.h"
#include "COMparser/COMparser.h"

int main (void)
{
	int rt, wt, rct;
	pthread_t read_thread, write_thread, readCOM_thread;
	pthread_attr_t thread_attr;
	BLE_Central_t bleCentral;
	memset(&bleCentral, 0, sizeof(bleCentral));

	bleCentral.port = "/dev/ttyS4";
	bleCentral._run = 1;
	bleCentral.rxQueue = queueCreate();
	bleCentral.txQueue = queueCreate();
	bleCentral.fd = open_serial(bleCentral.port, O_RDWR);
	if(bleCentral.fd < 0){
		printf("ERROR Opening port\n");
		return -1;
	}

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	rt = pthread_create( &read_thread, &thread_attr, read_serial, (void *)&bleCentral);
	if( rt ) {
	  printf("ERROR read thread: Return code from pthread_create() is %d\n", rt);
	  return -1;
	}

	wt = pthread_create( &write_thread, &thread_attr, write_serial, (void *)&bleCentral);
	if( wt ) {
	  printf("ERROR write thread: Return code from pthread_create() is %d\n", wt);
	  return -1;
	}

	rct = pthread_create( &readCOM_thread, &thread_attr, RxComParser, (void *)&bleCentral);
	if( rct ) {
	  printf("ERROR readCOM thread: Return code from pthread_create() is %d\n", rct);
	  return -1;
	}

	sleep(1);
	datagram_t datagram;
// Init Device
	memset(&datagram, 0, sizeof(datagram));
	get_GAP_DeviceInit(&datagram);
	enqueue(&bleCentral.txQueue, &datagram);
	sleep(2);
// Establish Link
	memset(&datagram, 0, sizeof(datagram));
	get_GAP_EstablishLinkRequest(&datagram);
	enqueue(&bleCentral.txQueue, &datagram);
	sleep(2);
// Sign up for freq service
	if(bleCentral._connected) {
		memset(&datagram, 0, sizeof(datagram));
		char d[] = {0x00, 0x01};
		get_GATT_WriteCharValue(&datagram, bleCentral.connHandle, 0x4D00, d, 2);
		enqueue(&bleCentral.txQueue, &datagram);
		sleep(30);
	}
// Sign off freq service
	if(bleCentral._connected) {
		memset(&datagram, 0, sizeof(datagram));
		char d[] = {0x00, 0x00};
		get_GATT_WriteCharValue(&datagram, bleCentral.connHandle, 0x4D00, d, sizeof(d));
		enqueue(&bleCentral.txQueue, &datagram);
		sleep(2);
	}
// Terminate Link
	if(bleCentral._connected) {
		memset(&datagram, 0, sizeof(datagram));
		get_GAP_TerminateLinkRequest(&datagram, bleCentral.connHandle);
		enqueue(&bleCentral.txQueue, &datagram);
	}

	char letter;
	scanf("%c", &letter);

	bleCentral._run = 0;
	sleep(2);
	pthread_join(rct, NULL);
	pthread_join(rt, NULL);
	pthread_join(wt, NULL);

	queueDestroy(&bleCentral.txQueue);
	queueDestroy(&bleCentral.rxQueue);
	close_serial(bleCentral.fd);

	printNetworkStat();

	printf("Exiting main thread\n\n");
	pthread_exit(NULL);
}