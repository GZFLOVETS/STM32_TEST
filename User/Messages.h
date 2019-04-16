#ifndef __MESSAGES_H
#define __MESSAGES_H
#include "stm32f10x.h"

#define QUEUELEN	64
typedef struct QueueType{
unsigned char Messages[QUEUELEN];
unsigned char MessageHead ;
unsigned char MessageTail ;
}QueueType;

typedef enum messages
{
	NO_MESSAGES = 0,
	flatbedRecv,
	canRecv,
	time20msFlag,
	time50msFlag,
	time200msFlag,
	
}messages;







































#endif
