#include  "main.h"


/*JTAG做普通引脚时GPIO最后配置*/
static void JTAG_GPIO_Config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);/*使能SWD 禁用JTAG*/
	RCC_APB2PeriphClockCmd(DIGITAL_GPIO_CLK,ENABLE);

	GPIO_InitStructure.GPIO_Pin =Trunk_lockput;//后备箱锁输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = (fog_lightkey|brake_lightkey);//雾灯开关，刹车灯开关
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	
}
//硬件初始化
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
	JTAG_GPIO_Config();//JTAG的GPIO初始化必须放置在GPIO配置的最后面

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
