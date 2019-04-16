#ifndef __FLATBED_USB_H
#define __FLATBED_USB_H

#include "stm32f10x.h"
#include "stdlib.h"
#define AreaSize	128
/*数据拆分宏定义*/
#define BYTE0(dwTemp) (*(char*)(&dwTemp)+0)
#define BYTE1(dwTemp) (*(char*)(&dwTemp)+1)
#define BYTE2(dwTemp) (*(char*)(&dwTemp)+2)
#define BYTE3(dwTemp) (*(char*)(&dwTemp)+3)
/*定义帧头*/
#define command1_rev_addr	0x21
#define command10_send_addr	0x02
#define command21_send_addr	0x22
#define fralength			0x10
/*接收命令1的数据内容*/
#define command1Num			0x01
#define CBsendIntervalmax	0x3C
#define askmodemax			0x02

/*命令10的数据内容*/
#define command10Num		0x10
#define handshakeAddr		0x21
#define lordversionNum		0x00
#define boutversionNum		0x01
#define Timeday				0x06
#define Timemonth			0x03
#define Timeyear			0x11

/*命令21的数据内容*/
#define command21Num		0x21
/*发送数据结构体*/
typedef struct SendDetail{
	u8 CBstatus;
	u8 CBerror;
	u8 Lightstatus;
	u16 speedpulse;
	u8 loadstatus;
}SendDetail;
typedef struct{
	struct SendDetail *USBSend;
	u8 isTask_Living;
	u8 Thread_Process;
}myTask4;

extern myTask4 Task4;
void APPdata_deal(myTask4 *Task);
void InitTask4(myTask4 *Task);
void Task4_flatbed_Communication(void);


#endif
















