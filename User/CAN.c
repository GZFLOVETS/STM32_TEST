#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "CAN.h"
#include "stm32f10x_it.h"
myTask5 Task5;
CanTxMsg TxMessage;
CanRxMsg RxMessage;
static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(CAN_TX_GPIO_CLK|CAN_RX_GPIO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);
	
}

static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef CAN_InitStructure;
	
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);
	
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = ENABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	
	CAN_InitStructure.CAN_Prescaler = 36;
	CAN_Init(CANx,&CAN_InitStructure);
	/*计算CAN总线的波特率：
		1Tq = 1/(36M/CAN_Prescaler) 
		Tbit = （CAN_BS2_Xtq+CAN_BS1_Xtq+1）*1Tq = 8us
		波特率 = 1/Tbit = 125Khz
	*/
}

static void CAN_Filter_Config(u32 FilterId)
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	/*使能筛选器，按照标志符的内容进行对比筛选*/
	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((u32)FilterId<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF0000)>>16;
	CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)FilterId<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((u32)FilterId>>16)&0XFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((u32)FilterId)&0XFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CANx,CAN_IT_FMP0,ENABLE);
}
static void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = CAN_RX_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/*CAN报文内容设置*/
void CAN_SetMsg(CanTxMsg *TxMessage)
{
	u8 ubCounter = 0;
	if(Task5.Can_Reve_Flag!=1)
	{
	TxMessage->ExtId = 0X1314;
	TxMessage->IDE = CAN_ID_EXT;
	TxMessage->RTR = CAN_RTR_DATA;
	TxMessage->DLC = 8;
	for(ubCounter=0;ubCounter < 8;ubCounter++)
	{
		TxMessage->Data[ubCounter] = ubCounter;
	}
	}else {
	TxMessage->ExtId = 0X13461234;
	TxMessage->IDE = CAN_ID_EXT;
	TxMessage->RTR = CAN_RTR_DATA;
	TxMessage->DLC = 8;
	for(ubCounter=0;ubCounter < 8;ubCounter++)
	{
		TxMessage->Data[ubCounter] = ubCounter;
	}
	}
	
}
static void CAN_Message_Analysis(void)
{
	return ;
}
static void CAN_Message_Send(void)
{
	return ;
}


/*初始化任务变量*/
static void InitTask5(myTask5 *Task)
{
	Task->Can_Reve_Flag = 0;
	Task->isTask_Living=timeflag.USB_CAN_50ms_FLAG;
	Task->Thread_Process = 0;
}
void CAN_Transmit_data(void)
{
	CAN_SetMsg(&TxMessage);
}
void CANPro_Init(void)
{
	CAN_NVIC_Config();
	CAN_GPIO_Config();
	CAN_Mode_Config();
	CAN_Filter_Config(0x1314);
	InitTask5(&Task5);
}

void CAN_Receive_data(void)
{
	CAN_Receive(CANx, CAN_FIFO0, &RxMessage);
	if((RxMessage.ExtId == 0X13141314)&&(RxMessage.IDE == CAN_ID_EXT)&&(RxMessage.DLC == 8))
	{
		Task5.Can_Reve_Flag = 1;
	}else {
		Task5.Can_Reve_Flag = 0;
	}
}

/*功能:任务线程
**参数:myTask:Task任务类型
**	  :Process:unsigned char*类型，线程指针
**返回值:CHAR类型，线程结束，或未结束
*/
static int myThread5(myTask5 *Task,unsigned char *Process)
{
	int ret=0;
	switch(*Process)
	{
		case 0:
			CAN_Message_Analysis();
		break;
		case 1:
			CAN_Message_Send();
		break;
		case 2:
			CAN_SetMsg(&TxMessage);
		break;
		case 3:
			CAN_Transmit(CANx,&TxMessage);
		break;
		default:break;
	}
	(*Process)++;
	if(*Process>3)
	{
		ret = -1;
		*Process = 0;
	}
	return ret;
}


void Task5_CAN_SEND_REV(void)
{
	if(Task5.isTask_Living)
	{
	Task5.isTask_Living = !myThread5(&Task5,&Task5.Thread_Process);
	}
}





























