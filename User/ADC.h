#ifndef __ADC_H
#define __ADC_H 

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#define ADC_USER 0

//PC1-ADC12_IN11,PC0-ADC12_IN10,PC4-ADC12_IN14,PC2-ADC12_IN12,PC3-ADC12_IN13
//油门，刹车，红外感应，坐垫压力，光照感应。

//ADC的GPIO宏定义
#define BRAKE_ADC_GPIO_PORT					GPIOC 
#define BRAKE_ADC_GPIO_PIN					GPIO_Pin_0
#define BRAKE_ADC_GPIO_CLK					RCC_APB2Periph_GPIOC
#define BRAKE_ADC										ADC1
#define BRAKE_ADC_CLK								RCC_APB2Periph_ADC1
#define BRAKE_ADC_CHANNEL						ADC_Channel_10
#define BRAKE_ADC_IRQ 							ADC1_2_IRQn
#define BRAKE_ADC_INT_FUNCTION 			ADC1_2_IRQHandler

#define ACCELERATOR_ADC_GPIO_PORT		GPIOC 
#define ACCELERATOR_ADC_GPIO_PIN		GPIO_Pin_1
#define ACCELERATOR_ADC_GPIO_CLK		RCC_APB2Periph_GPIOC
#define ACCELERATOR_ADC							ADC1
#define ACCELERATOR_ADC_CLK					RCC_APB2Periph_ADC1
#define ACCELERATOR_ADC_CHANNEL			ADC_Channel_11
#define ACCELERATOR_ADC_IRQ 							ADC1_2_IRQn
#define ACCELERATOR_ADC_INT_FUNCTION 			ADC1_2_IRQHandler

#define PRESSURE_ADC_GPIO_PORT			GPIOC 
#define PRESSURE_ADC_GPIO_PIN				GPIO_Pin_2
#define PRESSURE_ADC_GPIO_CLK				RCC_APB2Periph_GPIOC
#define PRESSURE_ADC								ADC1
#define PRESSURE_ADC_CLK						RCC_APB2Periph_ADC1
#define PRESSURE_ADC_CHANNEL				ADC_Channel_12
#define PRESSURE_ADC_IRQ 							ADC1_2_IRQn
#define PRESSURE_ADC_INT_FUNCTION 			ADC1_2_IRQHandler

#define LIGHTPHOTO_ADC_GPIO_PORT			GPIOC
#define LIGHTPHOTO_ADC_GPIO_PIN				GPIO_Pin_3
#define LIGHTPHOTO_ADC_GPIO_CLK				RCC_APB2Periph_GPIOC
#define LIGHTPHOTO_ADC								ADC1
#define LIGHTPHOTO_ADC_CLK						RCC_APB2Periph_ADC1
#define LIGHTPHOTO_ADC_CHANNEL				ADC_Channel_13
#define LIGHTPHOTO_ADC_IRQ 						ADC1_2_IRQn
#define LIGHTPHOTO_ADC_INT_FUNCTION 	ADC1_2_IRQHandler

#define INFRARED_ADC_GPIO_PORT			GPIOC 
#define INFRARED_ADC_GPIO_PIN				GPIO_Pin_4
#define INFRARED_ADC_GPIO_CLK				RCC_APB2Periph_GPIOC
#define INFRARED_ADC								ADC1
#define INFRARED_ADC_CLK						RCC_APB2Periph_ADC1
#define INFRARED_ADC_CHANNEL				ADC_Channel_14
#define INFRARED_ADC_IRQ 						ADC1_2_IRQn
#define INFRARED_ADC_INT_FUNCTION 	ADC1_2_IRQHandler

#define ADC_DMA_CLK									RCC_AHBPeriph_DMA1
#define ADC_DMAx_CHANNEL						DMA1_Channel1
#define ADC_STAT_DR_ADDR						(u32)(&ADC1->DR)	
#define ADC_DMA_BufferSize					5
#define ADC_NOFCHANEL								5

typedef struct AD_VALUE{
	u8 brake_Knob;
	u8 accelerator_Knob;
	u8 pressure_Knob;
	u8 illumination_Knob;
	u8 Infrared_Knob;
}AD_VALUE;
typedef struct{
	 struct AD_VALUE *ADvalue;
	u8 isTask_Living;
	u8 Thread_Process;
}myTask2;



extern myTask2 Task2;
extern u16 ADC_ConvertedValue[5];
static void ADC_GPIO_Config(void);
static void ADC_Mode_Config(void);
void ADC_DMA_ConfigInit(void);
u16 read_adc_value(u8 ch);
//void AD_In_value(myTask1 *Task);

void Task2_ADC_In(void);

















#endif
