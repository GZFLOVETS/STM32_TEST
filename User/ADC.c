#include "stm32f10x.h"
#include "ADC.h"
#include "main.h"
#include "stdio.h"
#include "string.h"

u16 ADC_ConvertedValue[5];
myTask2 Task2;
static void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd((BRAKE_ADC_GPIO_CLK|
					 ACCELERATOR_ADC_GPIO_CLK|
						PRESSURE_ADC_GPIO_CLK|
					  LIGHTPHOTO_ADC_GPIO_CLK|
						INFRARED_ADC_GPIO_CLK),ENABLE);
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
GPIO_InitStructure.GPIO_Pin = (BRAKE_ADC_GPIO_PIN					|
									 ACCELERATOR_ADC_GPIO_PIN		|	
									 PRESSURE_ADC_GPIO_PIN			|
										LIGHTPHOTO_ADC_GPIO_PIN		|
										INFRARED_ADC_GPIO_PIN);
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init((BRAKE_ADC_GPIO_PORT),&GPIO_InitStructure);
}

static void ADC_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);
	RCC_APB2PeriphClockCmd((BRAKE_ADC_CLK|ACCELERATOR_ADC_CLK|PRESSURE_ADC_CLK|LIGHTPHOTO_ADC_CLK|INFRARED_ADC_CLK),ENABLE);
	/*--DMA模式配置--*/
	DMA_DeInit(ADC_DMAx_CHANNEL);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_STAT_DR_ADDR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(&ADC_ConvertedValue);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//从外设到内存
	DMA_InitStructure.DMA_BufferSize =ADC_DMA_BufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//两个字节
	DMA_InitStructure.DMA_MemoryDataSize =  DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环传输模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMAx_CHANNEL,&DMA_InitStructure);
	DMA_Cmd(ADC_DMAx_CHANNEL,ENABLE);
	
	/*--ADC模式配置--*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//adc独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//自动连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐格式
	ADC_InitStructure.ADC_NbrOfChannel = ADC_NOFCHANEL;
	ADC_Init(ADC1,&ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//配置ADC时钟为PCLK2的8分频,即9MHz
	ADC_RegularChannelConfig(BRAKE_ADC, BRAKE_ADC_CHANNEL, 1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ACCELERATOR_ADC, ACCELERATOR_ADC_CHANNEL, 2,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(PRESSURE_ADC, PRESSURE_ADC_CHANNEL, 3,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(LIGHTPHOTO_ADC, LIGHTPHOTO_ADC_CHANNEL, 4,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(INFRARED_ADC, INFRARED_ADC_CHANNEL, 5,ADC_SampleTime_239Cycles5);

	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	/*初始化ADC校准寄存器*/
	ADC_ResetCalibration(ADC1);
	//等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	//ADC开始校准
	ADC_StartCalibration(ADC1);
	//等待校准完成
	while(ADC_GetCalibrationStatus(ADC1));
	//由于没有采用外部触发,所以使用软件触发ADC转换
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}
#if ADC_USER
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(BRAKE_ADC_CLK,ENABLE);
		/*--ADC模式配置--*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//adc独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//自动连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐格式
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1,&ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//配置ADC时钟为PCLK2的8分频,即9MHz
	ADC_Cmd(ADC1,ENABLE);
	/*初始化ADC校准寄存器*/
	ADC_ResetCalibration(ADC1);
	//等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	//ADC开始校准
	ADC_StartCalibration(ADC1);
	//等待校准完成
	while(ADC_GetCalibrationStatus(ADC1));

}
#endif
void AD_In_value(myTask2 *Task)
{
	Task->ADvalue->brake_Knob = ((float)ADC_ConvertedValue[0]/2500)*255;
	Task->ADvalue->accelerator_Knob = ((float)ADC_ConvertedValue[1]/2500)*255;
	Task->ADvalue->pressure_Knob = ((float)ADC_ConvertedValue[2]/2500)*255;
	Task->ADvalue->illumination_Knob = ((float)ADC_ConvertedValue[3]/2500)*255;
	Task->ADvalue->Infrared_Knob = ((float)ADC_ConvertedValue[4]/2500)*255;
	//printf("刹车旋钮AD值:%f \r\n",ADC_ConvertedValueLocal[0]);
	//printf("油门旋钮AD值:%f \r\n",ADC_ConvertedValueLocal[1]);
	//printf("坐垫压力AD值:%f \r\n",ADC_ConvertedValueLocal[2]);
	//printf("光照感应AD值:%f \r\n",ADC_ConvertedValueLocal[3]);
	//printf("红外感应AD值:%f \r\n",ADC_ConvertedValueLocal[4]);
}

u16 read_adc_value(u8 ch)
{
	u16 adc_value =0;
	
	ADC_RegularChannelConfig(BRAKE_ADC, ch, 1,ADC_SampleTime_55Cycles5);
	//由于没有采用外部触发,所以使用软件触发ADC转换
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	adc_value = ADC_GetConversionValue(ADC1);
	return adc_value;
}

/*初始化任务变量*/
static void InitTask2(myTask2 *Task)
{
	Task->ADvalue=(struct AD_VALUE*)malloc(sizeof(AD_VALUE));
	memset(Task->ADvalue,0,sizeof(AD_VALUE));
	Task->isTask_Living=timeflag.DIN_PWM_ADC_20ms_FLAG;
}

static void ADC_NVIC_Init(void)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = BRAKE_ADC_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
}

void ADC_DMA_ConfigInit(void)
{
	 ADC_NVIC_Init();
	 ADC_GPIO_Config();
	 ADC_Mode_Config();
	 //ADCx_Mode_Config();
	 InitTask2(&Task2);
}


/*功能:任务线程
**参数:myTask:Task任务类型
**	  :Process:unsigned char*类型，线程指针
**返回值:CHAR类型，线程结束，或未结束
*/
static int myThread2(myTask2 *Task,unsigned char *Process)
{
	int ret=0;
	switch(*Process)
	{
		case 0:
			AD_In_value(Task);
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

void Task2_ADC_In(void)
{
	if(Task2.isTask_Living)
	{
	Task2.isTask_Living = !myThread2(&Task2,&Task2.Thread_Process);
	}
}



