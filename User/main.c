#include  "main.h"


/*JTAG����ͨ����ʱGPIO�������*/
static void JTAG_GPIO_Config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);/*ʹ��SWD ����JTAG*/
	RCC_APB2PeriphClockCmd(DIGITAL_GPIO_CLK,ENABLE);

	GPIO_InitStructure.GPIO_Pin =Trunk_lockput;//���������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = (fog_lightkey|brake_lightkey);//��ƿ��أ�ɲ���ƿ���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	
}
//Ӳ����ʼ��
static void HardWareInit(void)
{

	SysTick_Init();
	ADC_DMA_ConfigInit();
	OLED_ConfigInit();
	USB_USART_Config();
	Timer_out_ConfigInit(); 
	PWM_Init_enable();
	Digital_GPIO_Config();
	TimerNVIC_ConfigInit();
	CANPro_Init();
	JTAG_GPIO_Config();//JTAG��GPIO��ʼ�����������GPIO���õ������

}

int main(void)
{	
	HardWareInit();
	while(1)   
	{
		if(timeflag.DIN_PWM_ADC_20ms_FLAG)
		{
			timeflag.DIN_PWM_ADC_20ms_FLAG =0;
			Task1_Digital_In_Out();
			Task2_ADC_In();
		}
		if(timeflag.USB_CAN_50ms_FLAG)
		{
			timeflag.USB_CAN_50ms_FLAG =0;
			Task4_flatbed_Communication();	
			Task5_CAN_SEND_REV();
		}
		if(timeflag.OLED_200ms_FLAG)
		{
			timeflag.OLED_200ms_FLAG =0;
			Task3_oled_show();
		}
	}
}
