#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
//#include "RingBuffer.h"
#include "define.h"
//#include  "usbd_custom_hid_core.h"
//#include  "usbd_usr.h"
#include "systick.h"
     
/*�������� �������� ��������� ������*/  
typedef struct 
     {    
          uint8_t *cntKeyPress; /* ��������� �� ������ � ������� �������� �������� ������������ ��� ����������� ����� ������� ������*/
         // uint8_t cntColumn;
          //uint8_t cntRow;
          bool *FlagDetectPress; /* ��������� �� ������, �������� ����� ������� ����������*/
          bool *FlagDetectDePress;
          bool flag1;
          uint8_t CountInOneMoment;
          //uint8_t cntTimeScan;
     }TKeyStatus;

    
/*���������*/
void TimerInit(Tperiod TimeScan);
void KeyboardInit(Tperiod TimeScan, Tperiod TimeDrebezg, uint8_t SRow,uint8_t SColumn);
void KeyboardScan(void);
void KeyboardTheardInit(void);
uint8_t DecodingScanCode(void);  /* ������� ������������� � �������� ������ ������� - 3 */
void resetdata(void);
     
#endif
