

//main.c
#include <LPC214x.h>
#include "types.h"
#include "lcd.h"
#include "rtc.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "adc.h"
#include "dt.h"

// Global variables
s32 hour, min, sec, date, month, year, day;
unsigned char eint_flag = 0;
unsigned char menu_active = 0;
char key;

// Alarm globals
s32 alarm_hour = -1, alarm_min = -1, alarm_sec = -1;


void EINT0_Init(void);
void Check_Alarm(void);
void Show_RTC_Display(void);
void Show_ADC_Temperature(void);
void Show_Edit_Menu(void);

// External interrupt ISR
void EINT0_ISR(void) __irq
{
    eint_flag = 1;          
    EXTINT = 0x01;          
    VICVectAddr = 0x00;      
}

#include "bell.h"

int main(void)
{
    Init_LCD();
    RTC_Init();
    Init_KPM();
    Init_ADC();
		

    IODIR0 |= (1 << 0);  
    IOCLR0 = (1 << 0);   
    Show_ADC_Temperature();

    SetRTCTimeInfo(19, 30, 0);
    SetRTCDateInfo(18, 10, 2025);
    SetRTCDay(6); // Saturday


    EINT0_Init();
    CmdLCD(0x0C);  

    while (1)
    {
        if (eint_flag)
        {
            menu_active = 1;
            eint_flag = 0;
            Show_Edit_Menu();
        }

        if (!menu_active)
        {
            Show_RTC_Display();     
            Show_ADC_Temperature();
            Check_Alarm();          
        }
    }
}


