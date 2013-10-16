#
# Makefile for eMPL Linux userland implementation
#

CC = $(CROSS_COMPILE_APPS)gcc
CFLAGS := -Os -mcpu=cortex-m3 -mthumb -L ${INSTALL_ROOT}/A2F/root/usr/lib -pthread
LDFLAGS	:= -mcpu=cortex-m3 -mthumb 

COMPARSER_DIR = COMparser
NETWORKSTAT_DIR = NetworkStat
PARSER_DIR = Parser
QUEUE_DIR = Queue
SERIALLOGIC_DIR = SerialLogic


OBJS = dev_tools.o \
       serialLogic.o \
       Queue.o \
       NetworkStatistics.o \
       COMparser.o \
       Parser.o

all : main

main : $(OBJS) main.o
	$(CC) $(CFLAGS) $(OBJS) main.o -lm -o main

dev_tools.o : dev_tools.c
	$(CC) $(CFLAGS) -I $(COMPARSER_DIR) -I $(NETWORKSTAT_DIR) -I $(PARSER_DIR) -I $(SERIALLOGIC_DIR) -I $(QUEUE_DIR) -c dev_tools.c
	
serialLogic.o : $(SERIALLOGIC_DIR)/serialLogic.c
	$(CC) $(CFLAGS) -I $(COMPARSER_DIR) -I $(NETWORKSTAT_DIR) -I $(PARSER_DIR) -I $(SERIALLOGIC_DIR) -I $(QUEUE_DIR) -c $(SERIALLOGIC_DIR)/serialLogic.c

Queue.o : $(QUEUE_DIR)/Queue.c
	$(CC) $(CFLAGS) -I $(COMPARSER_DIR) -I $(NETWORKSTAT_DIR) -I $(PARSER_DIR) -I $(SERIALLOGIC_DIR) -I $(QUEUE_DIR) -c $(QUEUE_DIR)/Queue.c

NetworkStatistics.o : $(NETWORKSTAT_DIR)/NetworkStatistics.c
	$(CC) $(CFLAGS) -I $(COMPARSER_DIR) -I $(NETWORKSTAT_DIR) -I $(PARSER_DIR) -I $(SERIALLOGIC_DIR) -I $(QUEUE_DIR) -c $(NETWORKSTAT_DIR)/NetworkStatistics.c

COMparser.o : $(COMPARSER_DIR)/COMparser.c
	$(CC) $(CFLAGS) -I $(COMPARSER_DIR) -I $(NETWORKSTAT_DIR) -I $(PARSER_DIR) -I $(SERIALLOGIC_DIR) -I $(QUEUE_DIR) -c $(COMPARSER_DIR)/COMparser.c

Parser.o : $(PARSER_DIR)/Parser.c
	$(CC) $(CFLAGS) -I $(COMPARSER_DIR) -I $(NETWORKSTAT_DIR) -I $(PARSER_DIR) -I $(SERIALLOGIC_DIR) -I $(QUEUE_DIR) -c $(PARSER_DIR)/Parser.c

clean:
	rm -f *.o imu imucal

