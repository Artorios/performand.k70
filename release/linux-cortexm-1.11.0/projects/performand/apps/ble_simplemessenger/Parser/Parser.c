/*
 * Parser.c
 *
 *  Created on: Sep 5, 2013
 *      Author: rasmus
 */

#include "Parser.h"

/*************************************************************************
 *  Input parameters:
 *  datagram_t *datagram	- Pointer to an 0'ed datagram
 *  char *data				- Char data array
 *  int *bytes				- Number of bytes in char data array
 *  int parserState			- current parser state of type parserState_t
 *
 *  Output parameters:'
 *  ssize_t 				- New parser state
 *************************************************************************/
ssize_t parse_data(datagram_t *datagram, char *data, int bytes, int _parserState)
{
	int _continue = 1;
	int i = 0;

	while(_continue) {
		switch(_parserState){
		case package_type_token:
			if(data[i] == Command){
//				printf("Type:\t%02X Command\n", data[i++] & 0xff);
				datagram->type = Command;
				_parserState = command_code_token;
			}
			else if(data[i] == Event) {
//				printf("Type:\t%02X Event\n", data[i++] & 0xff);
				datagram->type = Event;
				_parserState = event_code_token;
			}
			else {
//				printf("Unknown type token: %02X\n", data[i] & 0xff);
				_continue = 0;
			}
			i++;
			break;

		case command_code_token:
//			printf("Command code token not implemented\n");
			_parserState = package_type_token;
			_continue = 0;
			break;

		case event_code_token:
//			printf("Opcode:\t%02X\n", data[i] & 0xff);
			datagram->opcode = (long)data[i++];
			_parserState = length_token;
			break;

		case length_token:
//			printf("Length:\t%02X (%d)\n", data[i] & 0xff, (int)data[i]);
			datagram->data_length = (unsigned int)data[i++];
			_parserState = data_token;
			break;

		case data_token:
//			printf("Data:\t");

			if(sizeof(data) > sizeof(datagram->data)) {
				printf("Too much data for storage buffer. Trimming data to fit\n\t");
			}

			int j = 0;
			while(j < datagram->data_length) {
//				printf("%02X ", data[i] & 0xff);
				datagram->data[j++] = data[i++];
			}
//			printf("\n");
			_parserState = package_type_token;
			break;

		default:
			/* Should never go here */
			_parserState = package_type_token;
			_continue = 0;
			break;
		}

		if(i >= bytes) {
			_continue = 0;
		}
	}
	return _parserState;
}

/******************************************************************
 *  Input parameters:
 *  datagram_t *datagram	- Filled datagram to convert to char array
 *  char data[]				- Char array big enough to hold datagram
 *  int *length				- Number of bytes filled in char array
 *
 *  Output parameters:
 *  int						- Success (0)
 ******************************************************************/
int compose_datagram(datagram_t *datagram, char data[], int *length)
{
	int i=0, j;

	*length = datagram->data_length + 3;
	if(datagram->type == Command) {
		(*length)++;
	}

	if(*length > STD_DATAGRAM_SIZE)
		return -1;

	data[i++] = datagram->type;
	data[i++] = (char)datagram->opcode;

	if(datagram->type == Command) {
		data[i++] = (char)(datagram->opcode >> 8);
	}

	data[i++] = (char)datagram->data_length;

	for(j=0; j<datagram->data_length; j++) {
		data[i++] = datagram->data[j];
	}

	return 0;
}

void pretty_print_datagram(datagram_t *datagram)
{
#if (defined VERBOSITY) && (VERBOSITY >= 3)
	int i;

	printf("Type:\t(%02X) %s\n", datagram->type & 0xff, datagram->type==Command?"Command":"Event");

	printf("Opcode:\t(");
	if(datagram->type == Command)
		printf("%02X ", (char)(datagram->opcode >> 8) & 0xff);
	printf("%02X)\n", (char)datagram->opcode & 0xff);

	printf("Length:\t(%02X) %d bytes\n", datagram->data_length & 0xff, datagram->data_length);

	printf("Data:\t(");
	for(i=0; i<datagram->data_length; i++)
		printf("%02X ", datagram->data[i] & 0xff);
	printf(")\n");
#endif
}

int get_GAP_DeviceInit(datagram_t *datagram)
{
	datagram->type = Command;
	datagram->opcode = GAP_DeviceInit;
	datagram->data_length = 38;

	datagram->data[0] = 0x08;
	datagram->data[1] = 0x05;
	datagram->data[34] = 0x01;

	return 0;
}

int get_GAP_EstablishLinkRequest(datagram_t *datagram)
{
	datagram->type = Command;
	datagram->opcode = GAP_EstablishLinkRequest;
	datagram->data_length = 9;

	// Peer address 78:c5:e5:a0:14:12
	datagram->data[3] = 0x12;
	datagram->data[4] = 0x14;
	datagram->data[5] = 0xA0;
	datagram->data[6] = 0xE5;
	datagram->data[7] = 0xC5;
	datagram->data[8] = 0x78;
	return 0;
}

int get_GAP_TerminateLinkRequest(datagram_t *datagram, long connHandle)
{
	datagram->type = Command;
	datagram->opcode = GAP_TerminateLinkRequest;
	datagram->data_length = 2;

	datagram->data[0] = (char)connHandle;
	datagram->data[1] = (char)(connHandle >> 8);
	return 0;
}

int get_GATT_WriteCharValue(datagram_t *datagram, long connHandle, long handle, char *data, int length)
{
	datagram->type = Command;
	datagram->opcode = GATT_WriteCharValue;
	datagram->data_length = 4 + length;

	int i = 0;
	datagram->data[i++] = (char)(connHandle >> 8);
	datagram->data[i++] = (char) connHandle;
	datagram->data[i++] = (char)(handle >> 8);
	datagram->data[i++] = (char) handle;

	int j;
	for(j=length-1; j>=0; j--) {
		datagram->data[i++] = data[j];
	}
	return 0;
}
