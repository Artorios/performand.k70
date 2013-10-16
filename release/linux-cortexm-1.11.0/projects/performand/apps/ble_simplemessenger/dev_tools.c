/*
 * dev_tools.c
 *
 *  Created on: Sep 23, 2013
 *      Author: rasmus
 */
#include "dev_tools.h"

char unload_8_bit(char* data, int *i)
{
	char b = data[*i] & 0xFF;
	*i += 1;
	return b;
}

long unload_16_bit(char* data, int *i, int swap)
{
	long h, rval;
	long l;
	if(swap) {
		h = *(data + *i) << 8;
		l = *(data + *i + 1) & 0x00FF;
	}
	else {
		l = *(data + *i) & 0x00FF;
		h = *(data + *i + 1) << 8;
	}
	*i += 2;
	rval = (h + l) & 0xFFFF;
	return rval;
}

char *unload_mac_addr(char* data, int *i)
{
	int j;
	char *mac;
	mac = (char*)malloc(6);

	for(j=5; j>=0; j--) {
		mac[j] = data[*i++];
	}

	return mac;
}
