#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "Digital.h"
#include "stdlib.h"
#include "oled.h"
#include "ADC.h"
#include "PWM.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
myTask1 Task1;                                                    
/*数字量长动开关状态输入*/
 uint8_t Digital_longmove_status(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == DISABLE)
	{
		return success;
	}
	else return fail;
}

/*数字量点动开关状态输入*/
 uint8_t Digital_pointmove_status(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == DISABLE)
	{
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == DISABLE);//必须消除阻塞,采用EXTI外部中断上升沿触发。
		return KEYON;
	}
	else return KEYOFF;
}
 uint8_t Set_Digital_Put(GPIO_TypeDef * GPIOx,uint16_t PIN,uint8_t bool)
{
	if(1==bool)
		{
		GPIO_SetBits(GPIOx,PIN);
		return success;
		}
	else 
		{
		GPIO_ResetBits(GPIOx,PIN);
		return fail;
		}
}	
static u8 Set_LeftTurnLight_control(u8 bool)
{
	u8 ret=0;
	if(bool == 1)
	{	
		Set_TimerX_IT(tim1_it,ENABLE);
		if(1 == timeflag.LeftTurnLightTime_Flag){
		digitalToggle(GPIOA,left_lightput);
		timeflag.LeftTurnLightTime_Flag=0;
		}
		ret =  success;
	}
	else if(0 == bool)
	{
		GPIO_ResetBits(GPIOA,left_lightput);
		Set_TimerX_IT(tim1_it,DISABLE);
		timeflag.LeftTurnLightTime_Flag=0;
		ret =  fail;
	}
	return ret;
}

static u8 Set_RightTurnLight_control(u8 bool)
{
	u8 ret=0;
	if(bool == 1)
	{	
		Set_TimerX_IT(tim2_it,ENABLE);
		if(1 == timeflag.RightTurnLightTime_Flag){
		digitalToggle(GPIOA,right_lightput);
		timeflag.RightTurnLightTime_Flag=0;
		}
		ret =  success;
	}
	else if(0 == bool)
	{
		GPIO_ResetBits(GPIOA,right_lightput);
		Set_TimerX_IT(tim2_it,DISABLE);
		timeflag.RightTurnLightTime_Flag=0;
		ret =  fail;
	}
	return ret;
}

static void GPIO_In_ConfigInit(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DIGITAL_GPIO_CLK,ENABLE);
	/* 开关：远光灯,近光灯,按键1,轮廓灯,尾灯,倒车灯 */
	GPIO_InitStructure.GPIO_Pin = (far_lightkey      |
									near_lightkey    |	
									SW1              |
									profile_lightkey |
									tail_lightkey	 |
									back_lightkey);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_Init(GPIOB,&GPIO_InitStructure);
/* 开关：左转向,右转向,高速,中速,低速,倒车,座位锁,前位锁,按键2 */
GPIO_InitStructure.GPIO_Pin = (left_turnkey			 |
															 right_turnkey     |
															 high_speedkey     |
															 med_speedkey      |
															 low_speedkey      |
															 back_speedkey     |
															 seat_lockkey      |
															 froPos_lockkey    |
															 SW2);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_Init(GPIOC,&GPIO_InitStructure);
/* 开关：喇叭 */
GPIO_InitStructure.GPIO_Pin = (horn_key);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_Init(GPIOD,&GPIO_InitStructure);

}
static void GPIO_Out_ConfigInit(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DIGITAL_GPIO_CLK,ENABLE);
	/* 输出: 左转向灯,右转向灯,轮廓灯,车牌,*/
GPIO_InitStructure.GPIO_Pin = (left_lightput |
						right_lightput 		 |
						profile_lightput 	 |
						carbrand_lightput
						);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_Init(GPIOA,&GPIO_InitStructure);
/* 输出: 雾灯,尾灯,倒车灯,座位锁,前位锁*/
GPIO_InitStructure.GPIO_Pin = (fog_lightput 	|
					tail_lightput 	|
					back_lightput 	|
					seat_lockput	|
					froPos_lockput);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_Init(GPIOB,&GPIO_InitStructure);
/* 输出:LED测试灯,刹车灯 */
GPIO_InitStructure.GPIO_Pin = (LED_TEST | brake_lightput);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_Init(GPIOC,&GPIO_InitStructure);

}
void Digital_In(myTask1 * Task)
{
	//printf("数字量输入\r\n");
	Task->KeyStatus->leftturnkey = Digital_longmove_status(GPIOC,left_turnkey);
	//printf("左转向开关:%d 开:1,关:0\r\n",KeyStatus.leftturnkey);

	Task->KeyStatus->rightturnkey= Digital_longmove_status(GPIOC,right_turnkey);
	//printf("右转向灯开关:%d 开:1,关:0\r\n",KeyStatus.rightturnkey);

	Task->KeyStatus->farlightkey = Digital_longmove_status(GPIOB,far_lightkey);
	//printf("远光灯开关:%d 开:1,关:0\r\n",KeyStatus.farlightkey);

	Task->KeyStatus->nearlightkey = Digital_longmove_status(GPIOB,near_lightkey);
	//printf("近光灯开关:%d 开:1,关:0\r\n",KeyStatus.nearlightkey);

	Task->KeyStatus->highspeedkey= Digital_longmove_status(GPIOC,high_speedkey);
	//printf("高速开关:%d 开:1,关:0\r\n",KeyStatus.highspeedkey);

	Task->KeyStatus->medspeedkey= Digital_longmove_status(GPIOC,med_speedkey);
	//printf("中速开关:%d 开:1,关:0\r\n",KeyStatus.medspeedkey);

	Task->KeyStatus->lowspeedkey= Digital_longmove_status(GPIOC,low_speedkey);
	//printf("低速开关:%d 开:1,关:0\r\n",KeyStatus.lowspeedkey);

	Task->KeyStatus->backspeedkey= Digital_longmove_status(GPIOC,back_speedkey);
	//printf("倒车开关:%d 开:1,关:0\r\n",KeyStatus.backspeedkey);

	Task->KeyStatus->seatlockkey= Digital_longmove_status(GPIOC,seat_lockkey);
	//printf("坐垫锁开关:%d 开:1,关:0\r\n",KeyStatus.seatlockkey);

	Task->KeyStatus->froPoslockkey= Digital_longmove_status(GPIOC,froPos_lockkey);
	//printf("前位锁开关:%d 开:1,关:0\r\n",KeyStatus.froPoslockkey);

	Task->KeyStatus->sw2= Digital_longmove_status(GPIOC,SW2);
	//printf("按键2开关:%d 开:1,关:0\r\n",KeyStatus.sw2);

	Task->KeyStatus->sw1= Digital_longmove_status(GPIOB,SW1);
	//printf("按键1开关:%d 开:1,关:0\r\n",KeyStatus.sw1);

	Task->KeyStatus->hornkey= Digital_longmove_status(GPIOD,horn_key);
	//printf("喇叭开关:%d 开:1,关:0\r\n",KeyStatus.hornkey);

	Task->KeyStatus->foglightkey= Digital_longmove_status(GPIOB,fog_lightkey);
	//printf("雾灯开关:%d 开:1,关:0\r\n",KeyStatus.foglightkey);

	Task->KeyStatus->brakelightkey= Digital_longmove_status(GPIOB,brake_lightkey);
	//printf("刹车灯开关:%d 开:1,关:0\r\n",KeyStatus.brakelightkey);

	Task->KeyStatus->profilelightkey= Digital_longmove_status(GPIOB,profile_lightkey);
	//printf("轮廓灯开关:%d 开:1,关:0\r\n",KeyStatus.profilelightkey);

	Task->KeyStatus->taillightkey= Digital_longmove_status(GPIOB,tail_lightkey);
	//printf("尾灯开关:%d 开:1,关:0\r\n",KeyStatus.taillightkey);

	Task->KeyStatus->backlightkey= Digital_longmove_status(GPIOB,back_lightkey);
	//printf("倒车灯开关:%d 开:1,关:0\r\n",KeyStatus.backlightkey);

}
void Digital_Out(myTask1 *Task)
{
	//数字量输出
	Task->Light_LockStatus->brake_light = Set_Digital_Put(GPIOC,brake_lightput,Task->KeyStatus->brakelightkey);
	Task->Light_LockStatus->right_light = Set_RightTurnLight_control(Task->KeyStatus->rightturnkey);
	Task->Light_LockStatus->left_light = Set_LeftTurnLight_control(Task->KeyStatus->leftturnkey);
	Task->Light_LockStatus->profile_light = Set_Digital_Put(GPIOA,profile_lightput,Task->KeyStatus->profilelightkey);
	Task->Light_LockStatus->carbrand_light = Set_Digital_Put(GPIOA,carbrand_lightput,KEYOFF);
	Task->Light_LockStatus->LEDTEST = Set_Digital_Put(GPIOC,LED_TEST,KEYOFF);
	Task->Light_LockStatus->fog_light = Set_Digital_Put(GPIOB,fog_lightput,Task->KeyStatus->foglightkey);
	Task->Light_LockStatus->tail_light = Set_Digital_Put(GPIOB,tail_lightput,Task->KeyStatus->taillightkey);
	Task->Light_LockStatus->back_light = Set_Digital_Put(GPIOB,back_lightput,Task->KeyStatus->backspeedkey|Task->KeyStatus->backlightkey);
	Task->Light_LockStatus->seat_lock = Set_Digital_Put(GPIOB,seat_lockput,Task->KeyStatus->seatlockkey);
	Task->Light_LockStatus->froPos_lock = Set_Digital_Put(GPIOB,froPos_lockput,Task->KeyStatus->froPoslockkey);
	Task->Light_LockStatus->Trunk_lock = Set_Digital_Put(GPIOA,Trunk_lockput,KEYOFF);
	Task->Light_LockStatus->far_light = farlight_Set(Task->KeyStatus->farlightkey);
	Task->Light_LockStatus->near_light = nearlight_Set(Task->KeyStatus->nearlightkey);
}

/*初始化任务变量*/
static void InitTask1(myTask1 *Task)
{
	Task->KeyStatus=(struct keystatus*)malloc(sizeof(keystatus));
	memset(Task->KeyStatus,0,sizeof(keystatus));
	Task->Light_LockStatus=(struct light_lockstatus*)malloc(sizeof(light_lockstatus));
	memset(Task->Light_LockStatus,0,sizeof(light_lockstatus));
	Task->Thread_Process= 0;
	Task->isTask_Living=timeflag.DIN_PWM_ADC_20ms_FLAG;
}
void SW1EXIT_Control(FunctionalState newStart)
{
	/*配置EXIT外部中断源*/
	EXTI_InitTypeDef EXTI_InitStructure;
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = newStart;
	EXTI_Init(&EXTI_InitStructure);
}
static void Digital_NVIC_Init(void)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = SW1EXIT_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
}

void Digital_GPIO_Config(void)
{
	Digital_NVIC_Init();
	GPIO_In_ConfigInit();
	GPIO_Out_ConfigInit();
	InitTask1(&Task1);
}

/*功能:任务线程
**参数:myTask:Task任务类型
**	  :Process:unsigned char*类型，线程指针
**返回值:CHAR类型，线程结束，或未结束
*/
static int myThread1(myTask1 *Task,unsigned char *Process)
{
	int ret=0;
	switch(*Process)
	{
		case 0:
			Digital_In(Task);
		break;
		case 1:
			Digital_Out(Task);
		break;
		default:break;
	}
	(*Process)++;
	if(*Process>1)
	{
		ret = -1;
		*Process = 0;
	}
	return ret;
}


void Task1_Digital_In_Out(void)
{
	if(Task1.isTask_Living)
	{
	Task1.isTask_Living = !myThread1(&Task1,&Task1.Thread_Process);
	}
}

