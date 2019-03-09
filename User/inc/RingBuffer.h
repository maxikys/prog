#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>
 
 #define SizeRingBuff 8
 #define MaskRingBuff SizeRingBuff -1
 
 


/* ������ ���������� ����������*/

/*��������� ��������� ������ true - �����, false - �� �����*/
bool RingBuffEmpty(void);

 /* �������� ������ � �����*/
void WriteToRingBuff(uint8_t);

/* ������ �� ���������� ������ */
 uint8_t ReadFromRingBuff( void );
  
  
#endif
