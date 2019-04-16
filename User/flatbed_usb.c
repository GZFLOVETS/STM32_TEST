#include "stm32f10x.h"
#include "flatbed_usb.h"
#include "USART.h"
#include "string.h"
#include "stdio.h"
#include "Digital.h"
#include "ADC.h"
/*发送数据缓存数组*/
myTask4 Task4;

static void Usart1_Send(USART_TypeDef * pUSARTx,u8 *str,u8 length)
{
	unsigned int k = 0;
	do{
		Usart_SendByte(pUSARTx,(u8)*(str+k));
		k++;
	}while(--length);
	while(USART_GetFlagStatus(USB_USARTx,USART_FLAG_TC) == RESET);
}
static void DT_Send_Data(u8* dataToSend,u8 length)
{
	/*串口发送要发送的数据组数据长度*/
		Usart1_Send(USB_USARTx,dataToSend,length);
}

static void command10_sendFun(void)
{
	u8 i=0, crc =0;
	buff_array.data_to_send[0] = command10_send_addr;
	buff_array.data_to_send[1] = fralength;
	buff_array.data_to_send[2] = command10Num;
	buff_array.data_to_send[3] = handshakeAddr;
	buff_array.data_to_send[4] = lordversionNum;
	buff_array.data_to_send[5] = boutversionNum;
	buff_array.data_to_send[6] = Timeday;
	buff_array.data_to_send[7] = Timemonth;
	buff_array.data_to_send[8] = Timeyear;
	
	for(i=0;i<15;++i)
		crc ^= buff_array.data_to_send[i];
	buff_array.data_to_send[15] = crc;
	/*发送数据组*/
	DT_Send_Data(buff_array.data_to_send,16);
}

static void Command21_sendFun(myTask4 *f)
{
	u8 i=0, crc =0;

	buff_array.data_to_send[0] = command21_send_addr;
	buff_array.data_to_send[1] = fralength;
	buff_array.data_to_send[2] = command21Num;
	buff_array.data_to_send[3] = f->USBSend->CBstatus;
	buff_array.data_to_send[4] = f->USBSend->CBerror;
	buff_array.data_to_send[5] = f->USBSend->Lightstatus; 
	buff_array.data_to_send[6] = 0X00;//BYTE1(f->speedpulse);
	buff_array.data_to_send[7] = BYTE0(f->USBSend->speedpulse);
	buff_array.data_to_send[8] = f->USBSend->loadstatus;

	for(i=0;i<15;++i)
		crc ^= buff_array.data_to_send[i];
	buff_array.data_to_send[15] = crc;
	/*发送数据组*/
	DT_Send_Data(buff_array.data_to_send,16);
}

void DT_Data_Receive_Prepare(void)
{
	/*局部静态变量:接收缓存*/
	static u8 _data_len = 0,_data_cnt = 0;
	static u8 state =0;
	u8 crc=0,i=0;
	if(0==state&&(buff_array.RxBuffer[0]==command1_rev_addr||buff_array.RxBuffer[0]==0x01))
	{
		state =1;
		buff_array.Memory_Pool[0]=buff_array.RxBuffer[0];
	}else if(1==state&&buff_array.RxBuffer[1]==fralength)
	{
		state =2;
		buff_array.Memory_Pool[1]=buff_array.RxBuffer[1];
		_data_len = buff_array.RxBuffer[1];
		_data_cnt = 0;
	}else if(2==state&&_data_len>0)
	{
		_data_len--;
		buff_array.Memory_Pool[2+_data_cnt++]=buff_array.RxBuffer[2];
		if(_data_len==0)
			state = 3;
	}else if(state==3)
	{
		state = 0;
		for(i=0;i<fralength-1;++i)
			crc ^= buff_array.Memory_Pool[i];
		if(crc==buff_array.RxBuffer[15])
		buff_array.Memory_Pool[2+_data_cnt]= buff_array.RxBuffer[15];
	}else{
	state = 0;
	}
}

void APPdata_deal(myTask4 *Task)
{
#if DMA_Define
	static u8 flag=0,flag2=0;
	u8 i,askmode,comaddr,*p;
	p=buff_array.RxBuffer;
	for(i=0;i<USB_memorypool_size;++i) buff_array.Memory_Pool[i] =*p++;
	comaddr = buff_array.Memory_Pool[0];
	askmode = buff_array.Memory_Pool[5];
	Task->USBSend->Lightstatus = Task1.KeyStatus->farlightkey|
						(Task1.KeyStatus->nearlightkey<<1)|
						(Task1.KeyStatus->leftturnkey<<2)|
						(Task1.KeyStatus->rightturnkey<<3)|
						(Task1.Light_LockStatus->tail_light<<4)|
						(Task1.Light_LockStatus->brake_light<<5)|
						(Task1.Light_LockStatus->froPos_lock<<6)|
						(Task1.Light_LockStatus->carbrand_light<<7);
	
	Task->USBSend->loadstatus = 0x00;
	Task->USBSend->CBstatus = 0x00;
	Task->USBSend->speedpulse = Task2.ADvalue->accelerator_Knob;
	if(0x01 == askmode)
	{
		flag=1;
		if(flag==1){
		Command21_sendFun(Task);
		flag=0;
		memset(buff_array.Memory_Pool,0,USB_memorypool_size);
		}
	}
	if((askmode==0x00)&&(comaddr==0x01))
	{
		flag2=1;
		if(flag2==1){
		command10_sendFun();
		flag2=0;
		memset(buff_array.Memory_Pool,0,USB_buffer_size);
		}
	}
#else 
	u8 t,len,i,j,k,*p;
	if(buff_array.USART_RX_STA&0x8000)
	{
		len = buff_array.USART_RX_STA&0X3FFF;
		p=buff_array.RxBuffer;
		for(i=0;i<16;++i) buff_array.Memory_Pool[i] = *p++;
		t = buff_array.Memory_Pool[5];
		k = buff_array.Memory_Pool[0];
		j = 16;
		switch(t)
		{
			case 0:
			{
				if(k==1) command10_sendFun();
			}
			break;
			case 1:
			{
				Command21_sendFun(&Senddetail);
			}
			break;
			default: break;
		}
		buff_array.USART_RX_STA=0;
		memset(buff_array.Memory_Pool,0,USB_memorypool_size);		
	}
#endif
}

/*初始化任务变量*/
void InitTask4(myTask4 *Task)
{
	Task->USBSend=(struct SendDetail*)malloc(sizeof(SendDetail));
	memset(Task->USBSend,0,sizeof(SendDetail));
	Task->isTask_Living=timeflag.USB_CAN_50ms_FLAG;
	Task->Thread_Process=0;
}

/*功能:任务线程
**参数:myTask:Task任务类型
**	  :Process:unsigned char*类型，线程指针
**返回值:CHAR类型，线程结束，或未结束
*/
static int myThread4(myTask4 *Task,unsigned char *Process)
{
	int ret=0;
	switch(*Process)
	{
		case 0:
			APPdata_deal(Task);
		break;
		default:break;
	}
	(*Process)++;
	if(*Process>0)
	{
		ret = -1;
		*Process = 0;
	}
	return ret;
}

void Task4_flatbed_Communication(void)
{
	if(Task4.isTask_Living)
	{
	Task4.isTask_Living = !myThread4(&Task4,&Task4.Thread_Process);
	}
}



















