#define F_CPU	8000000
#include <util/delay.h>
#include "MemMap.h"
#include "Utils.h"
#include "StdTypes.h"
#include "Segment.h"
#include "DIO_Interface.h"
#include "LCD_Interface.h"
#include "KeyPad_Interface.h"
#include "ADC_Interface.h"
#include "Sensors.h"
#include "Sensors_Cfg.h"
#include "EX_Interrupt.h"
#include "Timers.h"
#include "RGB.h"
#include "Timers_Services.h"
#include "Ultrasonic_Cfg.h"
#include "Ultrasonic.h"
#include "Servo.h"
#include "UART.h"
#include "UART_Service.h"

void Led_on(u8 led_num)
{
	SET_BIT(PORTC,led_num);
}


void Led_off(u8 led_num)
{
	CLR_BIT(PORTC,led_num);
}


u8 String_divide(u8 *str_old,u8 *str_new)
{
	u8 i=0,num=0,sum=0;
	str_new[i]=str_old[i];
	for (; str_old[i]!='=';)
	{
		i++;
		str_new[i]=str_old[i];
	}
	str_new[i]=0;
	
	for (i=i+1;str_old[i];i++)
	{
		if ((str_old[i]<='9')&&(str_old[i]>='0'))
		{
			num=(str_old[i])-'0';
			sum=(sum*10)+num;
		}
	}
	return num;
}


typedef struct{
	u8 *str;
	void (*pf)(u8 num);
	}UART_Task_type;
	

u8 String_compare(u8*str_old,u8*str_new)
{
	u8 i,flag=0;
	for (i=0; str_new[i]; i++)
	{
		if(str_old[i]!=str_new[i])
		{
			flag=0;
			return flag;
		}
		else
		{
			flag=1;
		}
	}
	return flag;
}


u8 String_search(UART_Task_type*arr,u8*str)
{
	u8 i=0,rank;
	for(i=0; i<4; i++)
	{
		if (String_compare(arr[i].str,str))
		{
			rank=i;
			return rank;
		}
		else
		{
			rank=200;
		}
	}
	return rank;
}




int main(void)
{
	DIO_Init();
	LCD_Init();
	KEYPAD_Init();
	ADC_Init( VREF_AREF,ADC_SCALER_64);
	UART_Init();
	RGB_Init();
	sei();
	
	u8 str_rec[20], str_new[20], num;
	u8 i=0, j=0,l=0, flag, rank, flag2=0;

UART_Task_type arr[3]=	{	{"LED_ON",Led_on},
							{"LED_OFF",Led_off},
							{"RGB",RGB_Set},
							{"SERVO",SERVO_SetAngle},
						};
	
	//LCD_WriteString("RX:");
	DIO_WritePort(PC,0);
	while(1)
	{
		
		UART_ReceiveString(str_rec);
		
		LCD_SetCursor(0,0);
		LCD_WriteString(str_rec);
		LCD_WriteString("               ");
		
		num=String_divide(str_rec,str_new);

		rank=String_search(arr,str_new);

		if (rank==200)
		{
			LCD_Clear();
			LCD_SetCursor(1,2);
			LCD_WriteString("NOT FOUND");
			_delay_ms(1000);
		} 
		else
		{
			LCD_SetCursor(1,13);
			LCD_WriteNumber(num);
		}
			
		arr[rank].pf(num);

	}
}




