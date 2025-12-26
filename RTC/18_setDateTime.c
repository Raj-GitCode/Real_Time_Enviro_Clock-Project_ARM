
//Final Date and Time code...
#include "types.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "kpm.h"
#include "delay.h"
#include <stddef.h>   // for NULL
#include "rtc.h"

#define MAX_FIELDS 7   // 6 old + 1 new (D)

typedef struct {
    u8 start_pos;       
    u8 length;         
    u16 max_value;     
    u8 digits[4];       
    u8 filled[4];       
} Field;

u8 cursor_poss = 0;
u8 total_fieldss = 0;
Field fieldss[MAX_FIELDS];


void Display_Templates(void);
void Handle_D_Fields(u8 key);
void MoveCursorVerticallys(u8 key);
void MoveCursors(u8 key);
void MoveCursorBacks(void);
void EnterDigits(u8 key);
void AddFields(u8 start, u8 len, u16 max);
void SetCursorPoss(u8 pos);
Field* GetCurrentFields(void);
void AutoFillFields(Field *f);
void CheckAndFixFields(Field *f);
u8 IsEditablePoss(u8 pos);



void Display_Templates(void)
{
    CmdLCD(0x01); 
    StrLCD((s8 *)"DATE: DD:MM:YYYY");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((s8 *)"TIME: HH:MM:SS D");
    CmdLCD(0x06);
    CmdLCD(0x0E);
    cursor_poss = 0;
}

void AddFields(u8 start, u8 len, u16 max)
{
    Field *f;
    u8 i;

    f = &fieldss[total_fieldss++];
    f->start_pos = start;
    f->length = len;
    f->max_value = max;

    for (i = 0; i < len; i++) {
        f->digits[i] = '0';
        f->filled[i] = 0;
    }
}

void SetCursorPoss(u8 pos)
{
    if (pos < 16)
        CmdLCD(GOTO_LINE1_POS0 + pos);
    else
        CmdLCD(GOTO_LINE2_POS0 + (pos - 16));

    cursor_poss = pos;
}


Field* GetCurrentFields(void)
{
    u8 i;
    for (i = 0; i < total_fieldss; i++) {
        if (cursor_poss >= fieldss[i].start_pos &&
            cursor_poss < (fieldss[i].start_pos + fieldss[i].length))
            return &fieldss[i];
    }
    return NULL;
}

void MoveCursorBack(void)
{
    if (cursor_poss == 6)
        return;

    if (cursor_poss == (16 + 6))
        cursor_poss = 15;
    else {
        cursor_poss--;
        if (cursor_poss == 8 || cursor_poss == 11 ||
            cursor_poss == 24 || cursor_poss == 27 || cursor_poss == 30)
            cursor_poss--;
    }
}

void MoveCursors(u8 key)
{
    Field *f;

    if (key == '+') {
        if (cursor_poss >= 31)
            cursor_poss = 31;
        else if (cursor_poss == 15) {
            f = GetCurrentFields();
            if (f != NULL)
                AutoFillFields(f);
            CmdLCD(GOTO_LINE2_POS0 + 6);
            cursor_poss = 22;
        } else {
            f = GetCurrentFields();
            if (f != NULL) {
                u8 last_pos = f->start_pos + f->length - 1;
                if (cursor_poss == last_pos)
                    AutoFillFields(f);
            }
            cursor_poss++;
        }
    } else if (key == '=')
        MoveCursorBack();

    if (cursor_poss > 31)
        cursor_poss = 31;

    if (cursor_poss < 16)
        CmdLCD(GOTO_LINE1_POS0 + cursor_poss);
    else
        CmdLCD(GOTO_LINE2_POS0 + (cursor_poss - 16));
}


void AutoFillFields(Field *f)
{
    u8 i;
    for (i = 0; i < f->length; i++) {
        if (!f->filled[i]) {
            f->digits[i] = '0';
            SetCursorPoss(f->start_pos + i);
            CharLCD('0');
            f->filled[i] = 1;
        }
    }
    CheckAndFixFields(f);
    SetCursorPoss(f->start_pos + f->length);
}

void CheckAndFixFields(Field *f)
{
    u8 i;
    u16 value = 0;

    for (i = 0; i < f->length; i++)
        if (!f->filled[i]) return;

    for (i = 0; i < f->length; i++)
        value = (value * 10)+ (f->digits[i] - '0');

    if (value > f->max_value) {
        for (i = 0; i < f->length; i++) {
            f->digits[i] = '0';
            SetCursorPoss(f->start_pos + i);
            CharLCD('0');
        }
        SetCursorPoss(cursor_poss);
    }
}


u8 IsEditablePoss(u8 pos)
{
    u8 i;
    for (i = 0; i < total_fieldss; i++) {
        if (pos >= fieldss[i].start_pos &&
            pos < (fieldss[i].start_pos + fieldss[i].length))
            return 1;
    }
    return 0;
}


void EnterDigit(u8 key)
{
    Field *f;
    u8 idx;

    if (!IsEditablePoss(cursor_poss))
        return;

    if (cursor_poss == 31) {
        Handle_D_Fields(key);
        return;
    }

    if (key >= '0' && key <= '9') {
        f = GetCurrentFields();
        if (f != NULL) {
            CharLCD(key);
            CmdLCD(0x10);

            idx = cursor_poss - f->start_pos;
            if (idx < f->length) {
                f->digits[idx] = key;
                f->filled[idx] = 1;
                CheckAndFixFields(f);
            }
        }
    }
}

// ---- Handle D Field ----
/*void Handle_D_Fields(u8 key)
{
    static u8 D_value = 0;

    if (key >= '0' && key <= '9') {
        D_value = key - '0';
        if (D_value > 6)
            D_value = 0;
        SetCursorPoss(31);
        CharLCD(D_value + '0');
        CmdLCD(0x10);
    }
}
*/
void Handle_D_Fields(u8 key)
{
    static u8 D_value = 0;
    Field *f = &fieldss[6];   // 7th field = Day field

    if (key >= '0' && key <= '9')
    {
        D_value = key - '0';

        // Limit valid range (0–6)
        if (D_value > 6)
            D_value = 0;


        f->digits[0] = D_value + '0';
        f->filled[0] = 1;

        // Show on LCD
        SetCursorPoss(31);
        CharLCD(f->digits[0]);
        CmdLCD(0x10);
    }
}


void MoveCursorVertically(u8 key)
{
    if (key == '-') {
        if (cursor_poss < 16)
            cursor_poss += 16;
    } else if (key == 'x') {
        if (cursor_poss >= 16)
            cursor_poss -= 16;
    }

    if (cursor_poss < 16)
        CmdLCD(GOTO_LINE1_POS0 + cursor_poss);
    else
        CmdLCD(GOTO_LINE2_POS0 + (cursor_poss - 16));
}
void Set_DateAlarm(void)
{
    u8 key;
    u8 i, j;
		
		u16 dd, mm, yyyy, hh, min, ss, day;

    Init_LCD();
    Init_KPM();

    Display_Templates();


    AddFields(6, 2, 31);     // DD
    AddFields(9, 2, 12);     // MM
    AddFields(12, 4, 4050);  // YYYY
    AddFields(22, 2, 23);    // HH
    AddFields(25, 2, 59);    // MM
    AddFields(28, 2, 59);    // SS
    AddFields(31, 1, 6);     // D field

    SetCursorPoss(6);

    while (1)
    {
        key = KeyScan();

        if (key != '\0') {
            if (key == '+' || key == '=')
                MoveCursors(key);
            else if (key == '-' || key == 'x')
                MoveCursorVertically(key);
            else if (key >= '0' && key <= '9')
                EnterDigit(key);
           else if (key == 'b' || key == 'B') {

								
									dd    = (fieldss[0].digits[0] - '0') * 10 + (fieldss[0].digits[1] - '0');
									mm    = (fieldss[1].digits[0] - '0') * 10 + (fieldss[1].digits[1] - '0');
									yyyy  = (fieldss[2].digits[0] - '0') * 1000 +
													 (fieldss[2].digits[1] - '0') * 100 +
													 (fieldss[2].digits[2] - '0') * 10 +
													 (fieldss[2].digits[3] - '0');
									hh    = (fieldss[3].digits[0] - '0') * 10 + (fieldss[3].digits[1] - '0');
									min   = (fieldss[4].digits[0] - '0') * 10 + (fieldss[4].digits[1] - '0');
									ss    = (fieldss[5].digits[0] - '0') * 10 + (fieldss[5].digits[1] - '0');
									day   = (fieldss[6].digits[0] - '0');

								
									SetRTCTimeInfo(hh, min, ss);
									SetRTCDateInfo(dd, mm, yyyy);
									SetRTCDay(day);

								
									CmdLCD(0x01);
									CmdLCD(CLEAR_LCD);
									StrLCD((s8 *)"Saved DateTime:");
									CmdLCD(GOTO_LINE2_POS0);

									
									for (i = 0; i < total_fieldss; i++) {
											for (j = 0; j < fieldss[i].length; j++)
													CharLCD(fieldss[i].digits[j]);

											if (i == 0 || i == 1) CharLCD(':');
											else if (i == 2) CharLCD(' ');
											else if (i == 3 || i == 4) CharLCD(':');
											else if (i == 5) CharLCD(' ');
									}


									delay_ms(1000);
									return;
							}
            delay_ms(200);
        }
    }
}
