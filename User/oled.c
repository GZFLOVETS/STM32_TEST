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
//     SCL  ��PB14    // IICʱ���ź�
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
#include "stm32f10x.h"
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "Digital.h"
#include "ADC.h"
myTask3 Task3;

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 	
static void i2c_Delay(void)
{
	uint8_t i;

	/*��
		�����߼������ǲ���I2CͨѶʱ��Ƶ��
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		//�����ԣ�ѭ������Ϊ20~250ʱ����ͨѶ����

	*/
	for (i = 0; i < 17; i++);
}

/**********************************************
//IIC Start
**********************************************/
/**********************************************
//IIC Start
**********************************************/
static void IIC_Start()
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	i2c_Delay();
	OLED_SDIN_Clr();
	i2c_Delay();
	OLED_SCLK_Clr();
	i2c_Delay();
}

/**********************************************
//IIC Stop
**********************************************/
static void IIC_Stop()
{
	OLED_SDIN_Clr();
	OLED_SCLK_Set() ;
	i2c_Delay();
	OLED_SDIN_Set();
}
/*����I2C*/
static void i2c_CfgGpio()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOB
	GPIO_ResetBits(GPIOB,I2C_SDA_PIN|I2C_SCL_PIN);
	IIC_Stop();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
static uint8_t IIC_Wait_Ack()
{
	uint8_t re;
	OLED_SDIN_Set();
	i2c_Delay();
	OLED_SCLK_Set() ;
	i2c_Delay();
	if(GPIO_ReadInputDataBit(I2C_SDA_PORT, I2C_SDA_PIN))
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	OLED_SCLK_Clr();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if OLED_USER
static void i2c_Ack(void)
{
	OLED_SDIN_Clr();
	i2c_Delay();
	OLED_SCLK_Set() ;	/* CPU����1��ʱ�� */
	i2c_Delay();
	OLED_SCLK_Clr();
	i2c_Delay();
	OLED_SDIN_Set();	/* CPU�ͷ�SDA���� */
}
#endif
/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if OLED_USER
static void i2c_NAck(void)
{
	OLED_SDIN_Set();	/* CPU����SDA = 1 */
	i2c_Delay();
	OLED_SCLK_Set() ;	/* CPU����1��ʱ�� */
	i2c_Delay();
	OLED_SCLK_Clr();
	i2c_Delay();	
}
#endif
/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if OLED_USER
static void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			OLED_SDIN_Set();
		}
		else
		{
			OLED_SDIN_Clr();
		}
		i2c_Delay();
		OLED_SCLK_Set() ;
		i2c_Delay();	
		OLED_SCLK_Clr() ;
		if (i == 7)
		{
			OLED_SDIN_Set();// �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
}
#endif
/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
#if OLED_USER
static uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		OLED_SCLK_Set() ;
		i2c_Delay();
		if (GPIO_ReadInputDataBit(I2C_SDA_PORT, I2C_SDA_PIN))
		{
			value++;
		}
		OLED_SCLK_Clr() ;
		i2c_Delay();
	}
	return value;
}
#endif
/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
#if OLED_USER
static uint8_t I2C_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_CfgGpio();		/* ����GPIO */

	
	IIC_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address | OLED_CMD);
	ucAck = IIC_Wait_Ack();	/* ����豸��ACKӦ�� */

	IIC_Stop();			/* ����ֹͣ�ź� */

	return ucAck;
}
#endif
/**********************************************
// IIC Write byte
**********************************************/

static void Write_IIC_Byte(unsigned char _ucByte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			OLED_SDIN_Set();
		}
		else
		{
			OLED_SDIN_Clr();
		}
		i2c_Delay();
		OLED_SCLK_Set() ;
		i2c_Delay();	
		OLED_SCLK_Clr() ;
		if (i == 7)
		{
			OLED_SDIN_Set();// �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}

}
/**********************************************
// IIC Write Command
**********************************************/
static void Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   Write_IIC_Byte(IIC_SLAVE_ADDR);            //Slave address,SA0=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
   IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
static void Write_IIC_Data(unsigned char IIC_Data)
{
   IIC_Start();
   Write_IIC_Byte(IIC_SLAVE_ADDR);			//D/C#=0; R/W#=0
	 IIC_Wait_Ack();	
   Write_IIC_Byte(0x40);			//write data
	 IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Data);
	 IIC_Wait_Ack();	
   IIC_Stop();
}
static void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
  Write_IIC_Data(dat);
  }
	else {
   Write_IIC_Command(dat);
		
	}


}

#if OLED_USER
/********************************************
// fill_Picture
********************************************/
static void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(fill_Data,1);
		}
	}
}
#endif


//��������

static void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	
#if OLED_USER
//����OLED��ʾ    
static void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
static void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
#endif
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(unsigned dat)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(dat,OLED_DATA); 
	} //������ʾ
}
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
static void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n����
static u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
 void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//��ʾһ���ַ��Ŵ�
 void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
#if OLED_USER
//��ʾ����
static void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}
#endif
#if OLED_USER
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
static void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 
#endif
//��ʼ��SSD1306					    
static void OLED_Init(void)
{ 
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}



void oled_show(int keycount)
{
	switch(keycount)
	{
	case 0:
	OLED_ShowString(0,0,"lek:",16);//��ת�򿪹�
		OLED_ShowNum(50,0,Task1.KeyStatus->leftturnkey,4,16);
		
		OLED_ShowString(0,2,"rik",16);//��ת�򿪹�
		OLED_ShowNum(50,2,Task1.KeyStatus->rightturnkey,4,16);
		
		OLED_ShowString(0,4,"fak",16);//Զ�⿪��
		OLED_ShowNum(50,4,Task1.KeyStatus->farlightkey,4,16);
		
		OLED_ShowString(0,6,"nek",16);//���⿪��
		OLED_ShowNum(50,6,Task1.KeyStatus->nearlightkey,4,16);
	break;
	case 1:
	OLED_ShowString(0,0,"hik",16);
		OLED_ShowNum(50,0,Task1.KeyStatus->highspeedkey,4,16);
		
		OLED_ShowString(0,2,"mek",16);
		OLED_ShowNum(50,2,Task1.KeyStatus->medspeedkey,4,16);
		
		OLED_ShowString(0,4,"lok:",16);
		OLED_ShowNum(50,4,Task1.KeyStatus->lowspeedkey,4,16);
		
		OLED_ShowString(0,6,"back",16);
		OLED_ShowNum(50,6,Task1.KeyStatus->backspeedkey,4,16);
	break;

	case 2:
	OLED_ShowString(0,0,"setk",16);
		OLED_ShowNum(50,0,Task1.KeyStatus->seatlockkey,4,16);
		
		OLED_ShowString(0,2,"frok",16);
		OLED_ShowNum(50,2,Task1.KeyStatus->froPoslockkey,4,16);
		
		OLED_ShowString(0,4,"sw2",16);
		OLED_ShowNum(50,4,Task1.KeyStatus->sw2,4,16);
		
		OLED_ShowString(0,6,"sw1",16);
		OLED_ShowNum(50,6,Task1.KeyStatus->sw1,4,16);
	break;

	case 3:
	OLED_ShowString(0,0,"hork",16);
		OLED_ShowNum(50,0,Task1.KeyStatus->hornkey,4,16);
		
		OLED_ShowString(0,2,"fogk",16);
		OLED_ShowNum(50,2,Task1.KeyStatus->foglightkey,4,16);
		
		OLED_ShowString(0,4,"brk",16);
		OLED_ShowNum(50,4,Task1.KeyStatus->brakelightkey,4,16);
		
		OLED_ShowString(0,6,"prok",16);
		OLED_ShowNum(50,6,Task1.KeyStatus->profilelightkey,4,16);
	break;

	case 4:
	OLED_ShowString(0,0,"tak",16);
		OLED_ShowNum(50,0,Task1.KeyStatus->taillightkey,4,16);
		
		OLED_ShowString(0,2,"back",16);
		OLED_ShowNum(50,2,Task1.KeyStatus->backspeedkey|Task1.KeyStatus->backlightkey,4,16);
	break;
	case 5:
	OLED_ShowString(0,0,"braKn:",16);
		OLED_ShowNum(50,0,Task2.ADvalue->brake_Knob,4,16);
		
		OLED_ShowString(0,2,"AccKn:",16);
		OLED_ShowNum(50,2,Task2.ADvalue->accelerator_Knob,4,16);

		OLED_ShowString(0,4,"PreKn:",16);
		OLED_ShowNum(50,4,Task2.ADvalue->pressure_Knob,4,16);

		OLED_ShowString(0,6,"IllKn:",16);
		OLED_ShowNum(50,6,Task2.ADvalue->illumination_Knob,4,16);
	break;
	case 6:
		OLED_ShowString(0,0,"InfKn:",16);
		OLED_ShowNum(50,0,Task2.ADvalue->Infrared_Knob,4,16);
	break;	
	default:
	break;
	}
}

/*��ʼ���������*/
static void InitTask3(myTask3 *Task)
{
	Task->keycount=0;
	Task->Thread_Process=0;
	Task->isTask_Living=timeflag.OLED_200ms_FLAG;
}

void OLED_ConfigInit(void)
{
	i2c_CfgGpio();
	OLED_Init();
	OLED_Clear(0);
	InitTask3(&Task3);
}

/*����:�����߳�
**����:myTask:Task��������
**	  :Process:unsigned char*���ͣ��߳�ָ��
**����ֵ:CHAR���ͣ��߳̽�������δ����
*/
static int myThread3(myTask3 *Task,unsigned char *Process)
{
	int ret=0;
	switch(*Process)
	{
		case 0:
		if(GPIO_ReadInputDataBit(GPIOB,SW1) == ENABLE)
		{
			SW1EXIT_Control(ENABLE);
		}
		break;
		case 1:
		if(timeflag.SWEXIT_Fall_FLAG==1)
		{
			SW1EXIT_Control(DISABLE);
			timeflag.SWEXIT_Fall_FLAG=0;
			OLED_Clear(0);
			Task->keycount++;
			if(Task->keycount>=7) Task->keycount=0;
		}
		break;
		case 2:
		oled_show(Task->keycount);
		break;
		default:break;
	}
	(*Process)++;
	if(*Process>2)
	{
		ret = -1;
		*Process = 0;
	}
	return ret;
}

void Task3_oled_show(void)
{
	if(Task3.isTask_Living)
	{
	Task3.isTask_Living = !myThread3(&Task3,&Task3.Thread_Process);
	}
}







