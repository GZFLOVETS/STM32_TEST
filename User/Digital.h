#ifndef __DIGITAL_H
#define __DIGITAL_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stdint.h"
#include "Timer.h"
#include "main.h"
#define left_turnkey 			GPIO_Pin_14
#define right_turnkey 			GPIO_Pin_15
#define far_lightkey			GPIO_Pin_0
#define near_lightkey			GPIO_Pin_1
#define high_speedkey			GPIO_Pin_6
#define med_speedkey			GPIO_Pin_7
#define low_speedkey			GPIO_Pin_8
#define back_speedkey			GPIO_Pin_9
#define seat_lockkey			GPIO_Pin_10
#define froPos_lockkey			GPIO_Pin_11
#define SW2					GPIO_Pin_12
#define SW1					GPIO_Pin_11
#define horn_key				GPIO_Pin_2
#define fog_lightkey			GPIO_Pin_3
#define brake_lightkey			GPIO_Pin_4
#define profile_lightkey 		GPIO_Pin_5
#define tail_lightkey			GPIO_Pin_8
#define back_lightkey			GPIO_Pin_9


#define LED_TEST	 			GPIO_Pin_5
#define brake_lightput			GPIO_Pin_13
#define right_lightput			GPIO_Pin_0
#define left_lightput			GPIO_Pin_3
#define profile_lightput		GPIO_Pin_4
#define carbrand_lightput		GPIO_Pin_5
#define fog_lightput			GPIO_Pin_10
#define tail_lightput			GPIO_Pin_12
#define back_lightput			GPIO_Pin_13
#define seat_lockput			GPIO_Pin_14
#define froPos_lockput			GPIO_Pin_15
#define Trunk_lockput			GPIO_Pin_15

#define DIGITAL_GPIO_CLK		(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)

#define success			1
#define fail				0

#define KEYON				1
#define KEYOFF			0

#define digitalToggle(p,i)	 {p->ODR ^=i;}		//Êä³ö·´×ª×´Ì¬




typedef struct keystatus{
	uint8_t leftturnkey   :1;
	uint8_t rightturnkey  :1;
	uint8_t farlightkey   :1;
	uint8_t nearlightkey:1	;
	uint8_t backlightkey:1	;
	uint8_t highspeedkey:1	;
	uint8_t medspeedkey:1	;
	uint8_t lowspeedkey:1	;
	uint8_t backspeedkey:1	;
	uint8_t seatlockkey:1	;
	uint8_t froPoslockkey:1	;
	uint8_t sw2		:1	;
	uint8_t sw1		:1	;
	uint8_t hornkey:1		;
	uint8_t foglightkey:1	;
	uint8_t brakelightkey:1	;
	uint8_t profilelightkey:1 ;
	uint8_t taillightkey:1	;
	uint8_t redlightkey:1	;
	uint8_t greenlightkey:1	;
	uint8_t bluelightkey:1	;
	uint8_t carbrandlightkey:1 ;
}keystatus;

typedef struct light_lockstatus{
	uint8_t brake_light:1;
	uint8_t right_light:1;
	uint8_t LEDTEST:1;
	uint8_t left_light:1;
	uint8_t profile_light:1;
	uint8_t carbrand_light:1;
	uint8_t fog_light:1;
	uint8_t tail_light:1;
	uint8_t back_light:1;
	uint8_t seat_lock:1;
	uint8_t froPos_lock:1;
	uint8_t Trunk_lock:1;
	uint8_t near_light:1;
	uint8_t far_light:1;
	uint8_t red_light:1;
	uint8_t green_light:1;
	uint8_t blue_light:1;
}light_lockstatus;
typedef struct{
	 struct keystatus *KeyStatus;
	 struct light_lockstatus *Light_LockStatus;
	u8 isTask_Living;
	u8 Thread_Process;
}myTask1;
extern myTask1 Task1;

void Digital_In(myTask1 *Task);
void Digital_Out(myTask1 *Task);
void Digital_GPIO_Config(void);
uint8_t Digital_longmove_status(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
uint8_t Digital_pointmove_status(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
static void GPIO_In_ConfigInit(void);
 uint8_t Set_Digital_Put(GPIO_TypeDef * GPIOx,uint16_t PIN,uint8_t bool);
static void GPIO_Out_ConfigInit(void);
void Task1_Digital_In_Out(void);
void SW1EXIT_Control(FunctionalState newStart);


#endif

