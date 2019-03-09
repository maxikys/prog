
#ifndef I2C_H	// �������� ��������� ��������� ����� ������
#define I2C_H

//#include "main.h"
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include "clock.h"

#define I2C_BUS I2C1


// ��������������� ����������
typedef enum _I2C_Direction {I2C_Transmitter=0, I2C_Receiver=1} I2C_Direction;
/*
������� �������� ��� ��������� ������ ���������
����� �� ������������ � ���� ���������� �����, � ��� ��� ��������
��������:
I2C->CR2 |= 1<<I2C_OFFSET_CR2_NBYTES;	// ����� �������, ��� ��������� 1 ����
I2C->CR2 |= 1<<16;						// ����� ��������� ��������, �� �� 1 ����, �� �� 16
*/
#define I2C_OFFSET_TIMINGR_SCLL		0
#define I2C_OFFSET_TIMINGR_SCLH		8
#define I2C_OFFSET_TIMINGR_SDADEL	16
#define I2C_OFFSET_TIMINGR_SCLDEL	20
#define I2C_OFFSET_TIMINGR_PRESC	28
#define I2C_OFFSET_CR2_NBYTES		16


////////////////////////////////////////////////
//    ������ ���������� ��������� �������     //
////////////////////////////////////////////////
void I2CInit (void);
void I2C_Start (void);
void I2C_Stop (void);
uint8_t I2C_Write_Transaction (uint8_t Adress, uint8_t Register, uint8_t *Data, uint8_t Size);
uint8_t I2C_Read_Transaction (uint8_t Adress, uint8_t Register, uint8_t *Data, uint8_t Size);


#endif



