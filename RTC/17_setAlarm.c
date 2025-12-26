//setAlrm
#include "types.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "kpm.h"
#include "delay.h"
#include <stddef.h>
#define MAX_FIELDS 4   // HH, MM, SS

typedef struct {
    u8 start_pos;       // LCD absolute position (0–31)
    u8 length;          // digits
    u16 max_value;      // maximum allowed
    u8 digits[3];       // digit buffer
    u8 filled[3];       // filled flags
} Field;

u8 cursor_pos = 0;
u8 total_fields = 0;
Field fields[MAX_FIELDS];

extern int alarm_hour, alarm_min, alarm_sec;

void Display_AlarmTemplate(void);
void MoveCurs(u8 key);
void MoveCursBack(void);
void EnterDig(u8 key);
void AddField(u8 start, u8 len, u16 max);
void SetCursorPos(u8 pos);
Field* GetCurrentField(void);
void AutoFillField(Field *f);
void CheckAndFixField(Field *f);
u8 IsEditablePos(u8 pos);


void Display_AlarmTemplate(void)
{
    CmdLCD(0x01);
    StrLCD((s8 *)"***Alarm Time***");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((s8 *)"   HH:MM:SS   ");
    CmdLCD(0x06);
    CmdLCD(0x0E);
   // cursor_pos = 3;      
}


void AddField(u8 start, u8 len, u16 max)
{
    Field *f;
    u8 i;

    f = &fields[total_fields++];
    f->start_pos = start;
    f->length = len;
    f->max_value = max;

    for (i = 0; i < len; i++) {
        f->digits[i] = '0';
        f->filled[i] = 0;
    }
}

void SetCursorPos(u8 pos)
{
    if (pos < 16)
        CmdLCD(GOTO_LINE1_POS0 + pos);
    else
        CmdLCD(GOTO_LINE2_POS0 + (pos - 16));

    cursor_pos = pos;
}

Field* GetCurrentField(void)
{
    u8 i;
    for (i = 0; i < total_fields; i++) {
        if (cursor_pos >= fields[i].start_pos &&
            cursor_pos < (fields[i].start_pos + fields[i].length))
            return &fields[i];
    }
    return NULL;
}


void MoveCursBack(void)
{
    if (cursor_pos == 19) return;  
    cursor_pos--;
    if (cursor_pos == 21 || cursor_pos == 24) cursor_pos--; 
}


void MoveCurs(u8 key)
{
    Field *f;

    if (key == '+') {
        f = GetCurrentField();
        if (f != NULL) {
            u8 last = f->start_pos + f->length - 1;
            if (cursor_pos == last) AutoFillField(f);
        }

        cursor_pos++;
        if (cursor_pos == 21 || cursor_pos == 24) cursor_pos++; // skip :
        if (cursor_pos > 26) cursor_pos = 26;
    }
    else if (key == '=')
        MoveCursBack();

    if (cursor_pos < 16)
        CmdLCD(GOTO_LINE1_POS0 + cursor_pos);
    else
        CmdLCD(GOTO_LINE2_POS0 + (cursor_pos - 16));
}


void AutoFillField(Field *f)
{
    u8 i;
    for (i = 0; i < f->length; i++) {
        if (!f->filled[i]) {
            f->digits[i] = '0';
            SetCursorPos(f->start_pos + i);
            CharLCD('0');
            f->filled[i] = 1;
        }
    }
    CheckAndFixField(f);
    SetCursorPos(f->start_pos + f->length);
}

void CheckAndFixField(Field *f)
{
    u8 i;
    u16 value = 0;

    for (i = 0; i < f->length; i++)
      if (!f->filled[i]) return;

    for (i = 0; i < f->length; i++)
        value = (value * 10) + (f->digits[i] - '0');

    if (value > f->max_value) {
        for (i = 0; i < f->length; i++) {
            f->digits[i] = '0';
            SetCursorPos(f->start_pos + i);
            CharLCD('0');
        }
        SetCursorPos(cursor_pos);
    }
}


u8 IsEditablePos(u8 pos)
{
    u8 i;
    for (i = 0; i < total_fields; i++) {
        if (pos >= fields[i].start_pos &&
            pos < (fields[i].start_pos + fields[i].length))
            return 1;
    }
    return 0;
}


void EnterDig(u8 key)
{
    Field *f;
    u8 idx;

    if (!IsEditablePos(cursor_pos)) return;
    if (key < '0' || key > '9') return;

    f = GetCurrentField();
    if (f != NULL) {
        CharLCD(key);
        CmdLCD(0x10);

        idx = cursor_pos - f->start_pos;
        if (idx < f->length) {
            f->digits[idx] = key;
            f->filled[idx] = 1;
            CheckAndFixField(f);
        }
    }
}
u8 i, j;
void Set_Alarm(void)
{
    u8 key;
    u8 i, j;

  
    total_fields = 0;
    cursor_pos = 0;          

    for (i = 0; i < MAX_FIELDS; i++) {
        fields[i].start_pos = 0;
        fields[i].length = 0;
        fields[i].max_value = 0;
        for (j = 0; j < 3; j++) {
            fields[i].digits[j] = '0';
            fields[i].filled[j] = 0;
        }
    }

    Init_LCD();
    Init_KPM();
    Display_AlarmTemplate();


    AddField(19, 2, 23);   // HH
    AddField(22, 2, 59);   // MM
    AddField(25, 2, 59);   // SS

    SetCursorPos(19);      // start at first H

    while (1) {
        key = KeyScan();
        if (key != '\0') {
            if (key == '+' || key == '=')
                MoveCurs(key);
            else if (key >= '0' && key <= '9')
                EnterDig(key);
            else if (key == 'b' || key == 'B') {
             
                CmdLCD(0x01); 
				CmdLCD(CLEAR_LCD);
                StrLCD((s8 *)"Saved Alarm:");
                CmdLCD(GOTO_LINE2_POS0);
                
         
                for (i = 0; i < total_fields; i++) {
                    for (j = 0; j < fields[i].length; j++)
                        CharLCD(fields[i].digits[j]);
                    if (i < total_fields - 1) CharLCD(':');
                }

															
								alarm_hour = (fields[0].digits[0] - '0') * 10 + (fields[0].digits[1] - '0');
								alarm_min  = (fields[1].digits[0] - '0') * 10 + (fields[1].digits[1] - '0');
								alarm_sec  = (fields[2].digits[0] - '0') * 10 + (fields[2].digits[1] - '0');

                delay_ms(500);  // wait 5 seconds
                return;          // exit Set_Alarm
            }
            delay_ms(20);
        }
    }
}
