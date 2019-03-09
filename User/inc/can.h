#ifndef __CAN
#define __CAN

#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_can.h"
#include "gpio_driver.h"
#include "clock.h"


/******************************************************************************
 * ����������� ������ ���������
 ******************************************************************************/
#define CAN_CMD_Test_Send			0x0001		// ������� �������� ��������� ���������
#define CAN_CMD_Test_Ok				0x0002		// ������� ������������� ��������� ���������




/******************************************************************************
 * ����������� ���������� �������
 ******************************************************************************/
void CanInit(uint32_t Baundrate);
void USB_LP_CAN1_RX0_IRQHandler(void);

void CAN_Send_Test(void);
void CAN_Send_Ok(void);

#endif //__CAN
