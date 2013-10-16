/*
 * serialLogic.c
 *
 *  Created on: Sep 19, 2013
 *      Author: rasmus
 */
#include "serialLogic.h"
#include <termios.h>

void print_byte_array(char *buff, int length, int offset)
{
#if (defined VERBOSITY) && (VERBOSITY >= 3)
	int i;
	for(i=offset; i<length; i++) {
		printf("%02X ", buff[i] & 0xff);
	}
	printf("\n");
#endif
}

int open_serial(char *port, int oflags)
{
	struct termios tio;
	fd_set rdset;
	unsigned char c=0;

	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag  = 0; //RAw output &= ~OPOST; //0;
	tio.c_cflag &= CRTSCTS | CS8 | CLOCAL | CREAD; //| CRTSCTS | CS8 | CLOCAL | CREAD;
	tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // (ICANON | ECHO | ECHOE);
	tio.c_cc[VMIN]=0;
	tio.c_cc[VTIME]=1;

	int fd;
	fd = open(port, oflags);
	if( fd < 0 ){
		printf("Error opening port %s\n",port);
		return fd;
	}

	cfsetospeed(&tio,B115200);
	cfsetispeed(&tio,B115200);
	tcsetattr(fd,TCSANOW,&tio);

	initNetworkStat();
	return fd;
}

void close_serial(int fd)
{
	close(fd);
}

int read_bytes(int fd, char* rxBuf, int numToRead, int block) {
    if (block) {
          // Blocking mode, so don't return until we read all the bytes requested
          int bytesRead;
          // Keep getting data if we have a number of bytes to fetch 
          while (numToRead) {
               bytesRead = read(fd,rxBuf,numToRead);//ReadRxBuffer(rxBuf, numToRead);
               if (bytesRead) {
                    rxBuf += bytesRead;
                    numToRead -= bytesRead;
               }
          }
	  //printf("block ...");
	
          return bytesRead;
     } else {
          // Non-blocking mode, just read what is available in buffer 
          return read(fd,rxBuf,numToRead);
     }
	return 0;
}

void *read_serial(void *_bleCentral)
{
	int n, next;
	char buff[128]; memset(buff, 0, sizeof(buff));
	BLE_Central_t *bleCentral = (BLE_Central_t *)_bleCentral;
	datagram_t datagram;
	debug(1, "Read function started\n");

	while(bleCentral->_run) {
		n = read_bytes(bleCentral->fd, buff, 3, 1);
		//n = read(bleCentral->fd, buff, 3);

		if(n < 0) {
			printf("ERROR reading message header: Read return %d\n", n);
			break;
		}

		updateRxStat(1, n);
		parserState = parse_data(&datagram, buff, n, parserState);

		debug(2, "\nMessage header received (%d bytes) ", n);
		print_byte_array(buff, n, 0);


		next = (int)buff[2];
		if(next > 0) {
			n = read_bytes(bleCentral->fd, buff, next, 1);
			//n = read(bleCentral->fd, buff, next);

			if(n < 0) {
				printf("ERROR reading message body: Read return %d\n", n);
				break;
			}

			updateRxStat(0, n);
			parserState = parse_data(&datagram, buff, n, parserState);

			debug(2, "Message body received (%d bytes) ", n);
			print_byte_array(buff, n, 0);

		}
		//printf("REC\n");
		enqueue(&bleCentral->rxQueue, &datagram);
		pretty_print_datagram(&datagram);
	}

	debug(1, "Read thread exiting\n");
	return NULL;
}

void *write_serial(void *_bleCentral)
{
	int n, l;
	char msg[128]; memset(msg, 0, sizeof(msg));
	BLE_Central_t *bleCentral = (BLE_Central_t *)_bleCentral;
	datagram_t datagram;
	debug(1, "Write thread started\n");

	while(bleCentral->_run){
		if(queueCount(&bleCentral->txQueue) == 0) {
			usleep(500);
			continue;
		}

		dequeue(&bleCentral->txQueue, &datagram);
		compose_datagram(&datagram, msg, &l);

		n = write(bleCentral->fd, msg, l);
		updateTxStat(1, n);
		debug(2, "\nMessage sent (%d bytes) ", n);
		print_byte_array(msg, l, 0);
		pretty_print_datagram(&datagram);
	}
	debug(1, "Write thread exiting\n");
	return NULL;
}
