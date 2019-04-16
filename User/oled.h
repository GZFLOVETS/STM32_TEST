//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103ZET6,����72M  ��Ƭ��������ѹ3.3V��5V
//QDtech-OLEDҺ������ for STM32
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtech.net
//�Ա���վ��http://qdtech.taobao.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ 
//����:QDtech2008@gmail.com 
//Skype:QDtech2008
//��������QQȺ:324828016
//��������:2018/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2009-2019
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
//      5V  ��DC 5V��Դ
//     GND  �ӵ�
//======================================OLED�������߽���==========================================//
//��ģ��������������ΪIIC
//     SCL  ��PB14   // IICʱ���ź�
//     SDA  ��PB13    // IIC�����ź�
//======================================OLED�������߽���==========================================//
//��ģ��������������ΪIIC������Ҫ�ӿ����ź���    
//=========================================����������=========================================//
//��ģ�鱾��������������Ҫ�Ӵ�������
//============================================================================================//
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#ifndef __OLED_H
#define __OLED_H			  
#include "stm32f10x.h"
#include "stdlib.h"	    
#include "Digital.h"

#define OLED_USER 0

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED IIC�˿ڶ���----------------  					   
#define I2C_SDA_PIN 		GPIO_Pin_7
#define I2C_SCL_PIN		GPIO_Pin_6
#define I2C_SDA_PORT		GPIOB
#define I2C_SCL_PORT		GPIOB
#define OLED_SCLK_Clr() GPIO_ResetBits(I2C_SCL_PORT,I2C_SCL_PIN)//SCL IIC�ӿڵ�ʱ���ź�
#define OLED_SCLK_Set() GPIO_SetBits(I2C_SCL_PORT,I2C_SCL_PIN)

#define OLED_SDIN_Clr() GPIO_ResetBits(I2C_SDA_PORT,I2C_SDA_PIN)//SDA IIC�ӿڵ������ź�
#define OLED_SDIN_Set() GPIO_SetBits(I2C_SDA_PORT,I2C_SDA_PIN)

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

#define IIC_SLAVE_ADDR 0x78  //IIC slave device address



typedef struct{
	int keycount;
	u8 isTask_Living;
	u8 Thread_Process;
}myTask3;
extern myTask3 Task3;

//OLED�����ú���
static void OLED_WR_Byte(unsigned dat,unsigned cmd);  
static void OLED_Display_On(void);
static void OLED_Display_Off(void);	   							   		    
static void OLED_Init(void);
void OLED_Clear(unsigned dat);
static void OLED_DrawPoint(u8 x,u8 y,u8 t);
static void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
static void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size);	 
static void OLED_Set_Pos(unsigned char x, unsigned char y);
static void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_On(void);
static void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
static void Delay_50ms(unsigned int Del_50ms);
static void Delay_1ms(unsigned int Del_1ms);
static void fill_picture(unsigned char fill_Data);
static void Picture(void);
static void IIC_Start(void);
static void IIC_Stop(void);
static void Write_IIC_Command(unsigned char IIC_Command);
static void Write_IIC_Data(unsigned char IIC_Data);
static void Write_IIC_Byte(unsigned char IIC_Byte);
static uint8_t I2C_CheckDevice(uint8_t _Address);
static uint8_t IIC_Wait_Ack(void);
void oled_show(int keycount);

void OLED_ConfigInit(void);



void Task3_oled_show(void);

#endif  
	 



