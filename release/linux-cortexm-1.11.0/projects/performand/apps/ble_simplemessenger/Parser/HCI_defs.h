/*
 * HCI_defs.h
 *
 *  Created on: Sep 5, 2013
 *      Author: rasmus
 */

#ifndef HCI_DEFS_H_
#define HCI_DEFS_H_

#define STD_DATAGRAM_SIZE 128
#define MAX_DATAGRAM_QUEUE 10

enum Type {
	Command = 0x01,
	Event = 0x04
};

enum Opcode {
	GATT_ReadCharValue         = 0xFD8A,
	GATT_ReadUsingCharUUID     = 0xFD84,
	GATT_DiscCharsByUUID       = 0xFD88,
	GATT_WriteCharValue        = 0xFD92,
	GATT_ReadMultiCharValues   = 0xFD8E,
	GAP_DeviceInit             = 0xFE00,
	GAP_DeviceDiscoveryRequest = 0xFE04,
	GAP_DeviceDiscoveryCancel  = 0xFE05,
	GAP_EstablishLinkRequest   = 0xFE09,
	GAP_TerminateLinkRequest   = 0xFE0A,
	GAP_SetParam               = 0xFE30,
	GAP_GetParam               = 0xFE31
};

enum HCI_EventCode {
	HCI_NumberOfCompletePacketsEvent = 0x0013,
	HCI_LE_ExtEvent = 0x00FF
};

enum HCI_EvtOpCode {
	ATT_ErrorRsp                   = 0x0501,
	ATT_WriteRsp                   = 0x0513,
	ATT_HandleValueNotification    = 0x051B,
	GAP_DeviceInitDone             = 0x0600,
	GAP_DeviceDiscoveryDone        = 0x0601,
	GAP_EstablishLink              = 0x0605,
	GAP_TerminateLink              = 0x0606,
	GAP_HCI_ExtentionCommandStatus = 0x067F
};

enum HCIExt_StatusCodes {
	HCI_SUCCESS = 0x00
};

typedef struct {
	char type;
	long opcode;
	unsigned int data_length;
	char data[ STD_DATAGRAM_SIZE - 4 ];
} datagram_t;

#endif /* HCI_DEFS_H_ */
