//main_helper.c
#include <LPC214x.h>
#include "types.h"
#include "lcd.h"
#include "rtc.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "adc.h"
#include "dt.h"
#include "bell.h"


void Set_DateAlarm(void);
void Set_Alarm(void);

extern s32 hour, min, sec, date, month, year, day;
extern s32 alarm_hour, alarm_min, alarm_sec;
extern unsigned char menu_active;
extern char key;
static unsigned char first_run = 1;

extern void EINT0_ISR(void);

void Check_Alarm(void)
{
    static unsigned char alarm_triggered = 0;
    unsigned char key;
    unsigned int seconds_counter = 0;   

    if (alarm_hour == -1)
        return; 


    if (hour == alarm_hour && min == alarm_min && sec == alarm_sec && alarm_triggered == 0)
    {
        alarm_triggered = 1;

        CmdLCD(CLEAR_LCD);
        Show_Bell_On_LCD();

        IOSET0 = (1 << 0);  
        seconds_counter = 0;
        while (1)
        {
            key = KeyScan();

          
            if (key == 'b')
            {
                IOCLR0 = (1 << 0);      
                CmdLCD(CLEAR_LCD);
                StrLCD("Alarm Stopped");
                delay_ms(500);
                CmdLCD(CLEAR_LCD);
                break;
            }

           
            delay_ms(100); 
            seconds_counter++;

            if (seconds_counter >= 50) 
            {
                IOCLR0 = (1 << 0);      
                CmdLCD(CLEAR_LCD);
                StrLCD("Auto Stop Alarm");
                delay_ms(500);
                CmdLCD(CLEAR_LCD);
                break;
            }
        }
    }


    if (sec != alarm_sec)
        alarm_triggered = 0;
}

void EINT0_Init(void)
{
    PINSEL0 &= ~(3 << 2);
    PINSEL0 |=  (3 << 2);   // P0.1 = EINT0

    EXTMODE  = 0x01;        // Edge sensitive
    EXTPOLAR = 0x00;        // Falling edge
    EXTINT   = 0x01;        // Clear flag

    VICIntSelect = 0x00000000;
    VICVectAddr0 = (unsigned long)EINT0_ISR;
    VICVectCntl0 = 0x20 | 14;
    VICIntEnable = (1 << 14);
}
void Show_ADC_Temperature(void)
{
    unsigned int adcValue;
    float voltage, temperature;
    int intPart, decPart;

    adcValue = Read_ADC(1);
    voltage = (adcValue * 3.3f) / 1023.0f;
    temperature = voltage * 100.0f;

    intPart = (int)temperature;
    decPart = (int)((temperature - intPart) * 10 + 0.5f);

    CmdLCD(GOTO_LINE1_POS0 + 11);
    U32LCD(intPart);
    CharLCD('.');
    U32LCD(decPart);
    CharLCD(223);
    StrLCD("C");
}
void Show_RTC_Display(void)
{
    extern unsigned char first_run;
    static int last_min = -1;
    extern s8 week[][4];   

    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);
    GetRTCDay(&day);

    if (first_run || min != last_min)
    {
        CmdLCD(CLEAR_LCD);
        first_run = 0;
        last_min = min;
    }


    CmdLCD(GOTO_LINE1_POS0);
    DisplayRTCTime(hour, min, sec);
    Show_ADC_Temperature();


    CmdLCD(GOTO_LINE2_POS0);


    CharLCD((date / 10) + '0');
    CharLCD((date % 10) + '0');
    CharLCD('-');
    CharLCD((month / 10) + '0');
    CharLCD((month % 10) + '0');
    CharLCD('-');
    U32LCD(year);
    StrLCD("   ");
    StrLCD(week[day]);  

    CmdLCD(0x0C);
    delay_ms(50);
}
void Show_KeyHelp(void)
{
    CmdLCD(CLEAR_LCD);
		StrLCD("  ***PRESS***");
		delay_ms(100);
		CmdLCD(CLEAR_LCD);
    StrLCD((s8 *)" #Bck D:Fw B:Up");
    CmdLCD(GOTO_LINE2_POS0+3);
    StrLCD((s8 *)"C:Dn *:Menu");
    delay_s(1);   
    CmdLCD(CLEAR_LCD);
}
void Show_A_KeyHelp(void)
{
    CmdLCD(CLEAR_LCD);
		StrLCD("  ***PRESS***");
		delay_ms(100);
		CmdLCD(CLEAR_LCD);
    StrLCD((s8 *)"   #Bck D:Fw");
    CmdLCD(GOTO_LINE2_POS0+4);
    StrLCD((s8 *)"*:Menu");
    delay_s(1);   
    CmdLCD(CLEAR_LCD);
}

void Show_Edit_Menu(void)
{
    CmdLCD(CLEAR_LCD);
    StrLCD("  ****EDIT****");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("1.SDT 2.SA 3.EXT");

    while (1)
    {
        key = KeyScan();

        switch (key)
        {
            case '1':
				Show_KeyHelp();
                Set_DateAlarm();
                CmdLCD(CLEAR_LCD);
                StrLCD("  ****EDIT****");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("1.SDT 2.SA 3.EXT");
                break;

            case '2':
								
				Show_A_KeyHelp();
                Set_Alarm();
                CmdLCD(CLEAR_LCD);
                StrLCD("  ****EDIT****");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("1.SDT 2.SA 3.EXT");
                break;

            case '3':
                CmdLCD(CLEAR_LCD);
								first_run = 1;
                menu_active = 0;
                return;

            default:
                break;
        }
    }
}
