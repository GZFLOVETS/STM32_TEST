//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103ZET6,晶振72M  单片机工作电压3.3V或5V
//QDtech-OLED液晶驱动 for STM32
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtech.net
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习 
//邮箱:QDtech2008@gmail.com 
//Skype:QDtech2008
//技术交流QQ群:324828016
//创建日期:2018/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2009-2019
//All rights reserved
/****************************************************************************************************
//=========================================电源接线================================================//
//      5V  接DC 5V电源
//     GND  接地
//======================================OLED屏数据线接线==========================================//
//本模块数据总线类型为IIC
//     SCL  接PB14    // IIC时钟信号
//     SDA  接PB13    // IIC数据信号
//======================================OLED屏控制线接线==========================================//
//本模块数据总线类型为IIC，不需要接控制信号线    
//=========================================触摸屏接线=========================================//
//本模块本身不带触摸，不需要接触摸屏线
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

//OLED的显存
//存放格式如下.
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

	/*　
		可用逻辑分析仪测量I2C通讯时的频率
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		//经测试，循环次数为20~250时都能通讯正常

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
/*配置I2C*/
static void i2c_CfgGpio()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB
	GPIO_ResetBits(GPIOB,I2C_SDA_PIN|I2C_SCL_PIN);
	IIC_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
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
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
#if OLED_USER
static void i2c_Ack(void)
{
	OLED_SDIN_Clr();
	i2c_Delay();
	OLED_SCLK_Set() ;	/* CPU产生1个时钟 */
	i2c_Delay();
	OLED_SCLK_Clr();
	i2c_Delay();
	OLED_SDIN_Set();	/* CPU释放SDA总线 */
}
#endif
/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
#if OLED_USER
static void i2c_NAck(void)
{
	OLED_SDIN_Set();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	OLED_SCLK_Set() ;	/* CPU产生1个时钟 */
	i2c_Delay();
	OLED_SCLK_Clr();
	i2c_Delay();	
}
#endif
/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
#if OLED_USER
static void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
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
			OLED_SDIN_Set();// 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}
#endif
/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
#if OLED_USER
static uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
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
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
#if OLED_USER
static uint8_t I2C_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_CfgGpio();		/* 配置GPIO */

	
	IIC_Start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | OLED_CMD);
	ucAck = IIC_Wait_Ack();	/* 检测设备的ACK应答 */

	IIC_Stop();			/* 发送停止信号 */

	return ucAck;
}
#endif
/**********************************************
// IIC Write byte
**********************************************/

static void Write_IIC_Byte(unsigned char _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
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
			OLED_SDIN_Set();// 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
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


//坐标设置

static void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	
#if OLED_USER
//开启OLED显示    
static void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
static void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
#endif
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(unsigned dat)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(dat,OLED_DATA); 
	} //更新显示
}
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
static void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
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
//m^n函数
static u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
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
//显示一个字符号串
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
//显示汉字
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
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
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
//初始化SSD1306					    
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
	OLED_ShowString(0,0,"lek:",16);//左转向开关
		OLED_ShowNum(50,0,Task1.KeyStatus->leftturnkey,4,16);
		
		OLED_ShowString(0,2,"rik",16);//右转向开关
		OLED_ShowNum(50,2,Task1.KeyStatus->rightturnkey,4,16);
		
		OLED_ShowString(0,4,"fak",16);//远光开关
		OLED_ShowNum(50,4,Task1.KeyStatus->farlightkey,4,16);
		
		OLED_ShowString(0,6,"nek",16);//近光开关
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

/*初始化任务变量*/
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

/*功能:任务线程
**参数:myTask:Task任务类型
**	  :Process:unsigned char*类型，线程指针
**返回值:CHAR类型，线程结束，或未结束
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







