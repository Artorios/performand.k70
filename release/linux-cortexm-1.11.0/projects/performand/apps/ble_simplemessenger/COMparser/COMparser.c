/*
 * COMparser.c
 *
 *  Created on: Sep 20, 2013
 *      Author: rasmus
 */
#include "COMparser.h"

char* getSuccessString(char status){
	static char sStr[26];
	//sStr = (char*)malloc(26);

	switch(status) {
	case HCI_SUCCESS:
		sprintf(&sStr, "success");
		break;
	default:
		sprintf(&sStr, "Unknown HCI Ext Status %02X", (unsigned int)status & 0xFF);
		break;
	}
	return &sStr;
}


void *RxComParser(void *_bleCentral)
{
	BLE_Central_t *bleCentral = (BLE_Central_t *)_bleCentral;
	datagram_t datagram;
	int i, j;
	long connHandle, opCode;
	debug(1, "RX COM Parser started\n");

	while(bleCentral->_run) {
		if(queueCount(&bleCentral->rxQueue) == 0) {
			usleep(STD_WAIT_TIME*10);
			continue;
		}

		i = 0;
		connHandle = 0;
		opCode = 0;
		dequeue(&bleCentral->rxQueue, &datagram);

		if(datagram.type == Event) {

			if((datagram.opcode & 0xFF) == HCI_LE_ExtEvent) {
				long evtCode = unload_16_bit(datagram.data, &i, 0);
				char success = unload_8_bit(datagram.data, &i);
				debug(3, "Event:\t(%02X %02X)\n", (unsigned int)evtCode >> 8 & 0xFF, (unsigned int)evtCode & 0xFF);
				debug(3, "Status:\t(%02X) %s\n",(unsigned int)success & 0xFF, getSuccessString(success));

				switch(evtCode) {
				case ATT_ErrorRsp:
					debug(1, "ATT_ErrorResponce return %s\n", getSuccessString(success));
					break;
				case ATT_WriteRsp:
					debug(1, "ATT_WriteResponce return %s\n", getSuccessString(success));
					break;
				case ATT_HandleValueNotification:
					debug(1, "ATT_HandleValueNotifiction return %s ", getSuccessString(success));
					connHandle = unload_16_bit(datagram.data, &i, 1);

					char pduLength = unload_8_bit(datagram.data, &i);
					if(pduLength < 2)
						break;

					long handle = unload_16_bit(datagram.data, &i, 1);

					debug(1, "from connHandle %04X for handle %04X with data: ", (unsigned int)connHandle & 0xFFFF, (unsigned int)handle & 0xFFFF);
					for(j=0; j<pduLength-2; j++) {
						//debug(1, "%02X", (unsigned int)datagram.data[i++] & 0xFF);
						/* HERE WE WRITE INTO THE SHARED MEM */						
						printf("%2X", (unsigned int)datagram.data[i++] & 0xFF);
					}
					debug(1, "\n");
					printf("\n");

					break;
				case GAP_DeviceInitDone:
					debug(1, "GAP_DeviceInitDone return %s with MAC addr: ", getSuccessString(success));

					for(j=5; j>=0; j--) {
						bleCentral->MAC[j] = datagram.data[i++];
					}
					for(j=0; j<6; j++) {
						debug(1, "%02X%c", (unsigned int)bleCentral->MAC[j] & 0xFF, (j<5)?':':'\n');
					}
					break;
				case GAP_EstablishLink:
					debug(1, "GAP_EstablishLink ");
					debug(1, "%s to device MAC ", (success==0)?"successfully connected":"failed to connect");
					i++; // Device address type

					for(j=5; j>=0; j--) {
						bleCentral->connMAC[j] = datagram.data[i++];
					}
					for(j=0; j<6; j++) {
						debug(1, "%02X%c", (unsigned int)bleCentral->connMAC[j] & 0xFF, (j<5)?':':' ');
					}

					if(success == 0) {
						bleCentral->_connected = 1;
						bleCentral->connHandle = datagram.data[i++];
					}
					if(success == 0) {
						debug(1, "with connHandle %04X\n", bleCentral->connHandle);
					}
					break;
				case GAP_TerminateLink:
					debug(1, "GAP_TerminateLink ");
					connHandle = unload_16_bit(datagram.data, &i, 1);
					char reason = unload_8_bit(datagram.data, &i);

					debug(1, "connHandle 0x%04X with reason 0x%02X\n", (unsigned int)connHandle & 0xFFFF, (unsigned int)reason & 0xFF);
					bleCentral->_connected = 0;
					break;
				case GAP_HCI_ExtentionCommandStatus:
					debug(1, "GAP_HCI_ExtentionCommandStatus ");
					opCode = 0;
					opCode = unload_16_bit(datagram.data, &i, 0);

					switch(opCode) {
					case GATT_WriteCharValue:
						debug(1, "return %s for WriteCharValue\n", getSuccessString(success));
						break;
					case GAP_DeviceInit:
						debug(1, "return %s for DeviceInit\n", getSuccessString(success));
						break;
					case GAP_EstablishLinkRequest:
						debug(1, "return %s for EstablishLinkRequest\n", getSuccessString(success));
						break;
					case GAP_TerminateLinkRequest:
						debug(1, "return %s for TerminateLinkRequest\n", getSuccessString(success));
						break;
					default:
						debug(1, "return %s for unknown opcode %04X\n", getSuccessString(success), (unsigned int)opCode);
					}
					break;
				default:
					debug(1, "HCI_LE_ExtEvent OpCode %04X not supported\n", (unsigned int)opCode);
				}
			}
			else {
				debug(1, "Datagram eventCode not supported: %04X\n", (unsigned int)datagram.opcode);
			}
		}
		else {
			debug(1, "Datagram type not supported: %04X\n", datagram.type);
		}
	}

	debug(1, "RX COM Parser exiting\n");
	return NULL;
}
