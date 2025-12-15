
# **📟 LPC2148 Real-Time Enviro Clock**

A complete embedded application for **LPC2148 ARM7 microcontroller**, featuring:

✔ Real-Time Clock (RTC)

✔ Alarm with buzzer/LED

✔ LM35 temperature display using ADC

✔ 4×4 keypad interface

✔ 16×2 LCD display

✔ External interrupt–based menu

✔ Fully editable Date, Time and Alarm menus

---

## **📁 Project Overview**

This project implements a real-time **Clock + Calendar + Alarm** system on the **LPC2148** microcontroller.
The system uses:

* **RTC module of LPC2148** for time/date
* **ADC0 channel** for temperature sensor (LM35)
* **16×2 character LCD** for UI
* **4×4 keypad** for user input
* **EINT0** external interrupt to enter Edit Menu
* **LED/buzzer alarm** with auto-stop and manual stop

The user can set:

* **Date (DD/MM/YYYY)**
* **Time (HH:MM:SS)**
* **Day (0–6)**
* **Alarm (HH:MM:SS)**

---
## **📂 Folder Structure**

```
/Project
│── main.c
│── main_helpers.c
│── delay.c
│── lcd.c
│── rtc.c
│── adc.c
│── kpm.c
│── bell.c
│── setAlarm.c
│── setDateTime.c
│
├── headers/
│   ├── types.h
│   ├── delay.h
│   ├── lcd.h
│   ├── lcd_defines.h
│   ├── rtc.h
│   ├── rtc_defines.h
│   ├── adc.h
│   ├── adc_defines.h
│   ├── kpm.h
│   ├── kpm_defines.h
│   ├── bell.h
│   ├── defines.h
│
└── README.md   (this file)
```

## **🔧 Hardware Requirements**

| Component                 | Description                 |
| ------------------------- | --------------------------- |
| LPC2148 / LPC214x MCU     | ARM7-based controller       |
| 16×2 LCD                  | HD44780 compatible          |
| 4×4 Keypad                | Connected to Port-1         |
| LM35 / Analog Temp Sensor | Connected to ADC0.1 (P0.28) |
| LED / Buzzer              | On P0.0                     |
| External Interrupt Button | Connected to P0.1 (EINT0)   |

---

## **📌 Features**

### 🕒 **1. Real-Time Clock**

* Displays **time** and **date**
* Shows **day name** (Sun–Sat)
* Automatically refreshes display every minute

---

### 🔔 **2. Alarm System**

* User can set HH:MM:SS
* LCD shows bell symbol when alarm triggers
* Alarm can be:

  * **Stopped with keypad key**
  * **Auto-stopped after 60 seconds**
* LED/Buzzer on P0.0

---

### 🌡 **3. Temperature Display**

* Reads **ADC0.1**
* Converts analog value to °C
* Displays like: `28.3°C`

---

### 🔢 **4. Menu System (via EINT0 interrupt)**

Press external button → Menu appears:

```
   ****EDIT****
1. SDT  2. SA  3. EXT
```

* **1: SDT → Set Date/Time/Day**
* **2: SA → Set Alarm**
* **3: EXT → Exit menu**

---

### ⌨ 5. **4×4 Keypad Controls**

| Key | Function             |
| --- | -------------------- |
| 0–9 | Enter digits         |
| +   | Move cursor forward  |
| =   | Move cursor backward |
| -   | Move cursor down     |
| x   | Move cursor up       |
| on/c| Save and exit        |

---

---

## **🚀 How the System Works**

### 🔹 **Startup**

* LCD initialized
* RTC initialized
* ADC, keypad initialized
* Default time & date loaded

---

### 🔹 **Normal Mode**

LCD continuously shows:

```
HH:MM:SS  xx.x°C
DD-MM-YYYY  DAY
```

---

### 🔹 **Menu Mode**

Triggered by **EINT0 interrupt**.

```
1. SDT (Set Date & Time)
2. SA  (Set Alarm)
3. EXT (Exit)
```

Navigation using keypad keys.

---

### 🔹 **Alarm Mode**

When triggered:

* Buzzer/LED ON
* LCD shows bell icon + “Press * 2ExtAlrm”
* Auto stop after 60 seconds OR press “on/c”

---

## **🧠 Code Highlights**

* Custom CGRAM bell symbol
* Smooth cursor movement on LCD
* Auto-completion logic for digits
* Bounds checking for time/date fields
* keypad scanning
* Efficient ADC conversion (10-bit)
* RTC prescaler configuration

---

## **🛠 How to Build / Flash**

1. Open project in **Keil uVision** 
2. Select target device **LPC2148**
3. Compile all `.c` files
4. Flash using:
   * FlashMagic
5. Reset board → Project runs automatically

---

## **📸 Demonstration Examples**

* LCD date/time screenshot
* Menu screenshot
* Alarm bell screenshot
* Temperature display photo
<img width="954" height="847" alt="Screenshot (3399)" src="https://github.com/user-attachments/assets/12dbf1e8-a14e-4739-97f8-01b3c0371a82" />

<img width="244" height="96" alt="Screenshot (3400)" src="https://github.com/user-attachments/assets/eeb87ed6-e2d5-47cf-83f5-a0238bf59f96" />
<img width="243" height="96" alt="Screenshot (341)" src="https://github.com/user-attachments/assets/7d5e93b9-66c2-479a-9568-6052e534833b" />




## 👤 Author
M Raju  
Embedded System Developer  
GitHub: Raj-GitCode
