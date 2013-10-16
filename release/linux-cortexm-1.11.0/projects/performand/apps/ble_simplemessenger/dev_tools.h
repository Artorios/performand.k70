/*
 * dev_tools.h
 *
 *  Created on: Sep 23, 2013
 *      Author: rasmus
 */

#ifndef DEV_TOOLS_H_
#define DEV_TOOLS_H_

#include <stdio.h>
#include <stdlib.h>

char unload_8_bit(char* data, int* i);
long unload_16_bit(char* data, int* i, int swap);
char *unload_mac_addr(char* data, int* i);

#endif /* DEV_TOOLS_H_ */
