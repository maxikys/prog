
#include "can.h"

const TPin CAN_RX = {GPIOB,8};
const TPin CAN_TX = {GPIOB,9};
const TPin* VCAN[] = {&CAN_RX,&CAN_TX};

CAN_InitTypeDef CAN_InitStructure;
CAN_FilterInitTypeDef CAN_FilterInitStructure;
/**
  * @brief  ��������� ������ �����-������, CAN
  * @note   ��������� CAN
  * @param  Baundrate - ��������� ������� 
  * @retval None
  */
void CanInit(uint32_t Baundrate)
{
   uint32_t temp = 0;
    
RCC->APB1ENR |= RCC_APB1ENR_CANEN; // �������� ������������ CAN

ConfigVirualPort(VCAN,2,AF,_50MHz,PULLUP,PP);
PinAltFunc(&CAN_RX,AF4);
PinAltFunc(&CAN_TX,AF4);

temp = (uint32_t) clock_GetAPB()/(Baundrate * 16);
    
	// ������������� ����
	

	CAN_DeInit( CAN);
	CAN_StructInit(&CAN_InitStructure);

	// CAN cell init
	CAN_InitStructure.CAN_TTCM = DISABLE; /* ������ ����������� ������� ��� �������� �� �������� ������� ��������� */
	CAN_InitStructure.CAN_ABOM = DISABLE; /* �������������� ���������� �� ���� */
	CAN_InitStructure.CAN_AWUM = DISABLE; /* ��� ���������� �� ���� �������� �� ��� */
	CAN_InitStructure.CAN_NART = ENABLE;  /* ���������� ��������� ���� �� ������� ��������� ����� ������ ����������� ���������, ���� ���� ���� �� ����������-��������� ������ */
	CAN_InitStructure.CAN_RFLM = DISABLE; /* ���������� ���������� FIFO, ���������� �� ������������ ��������� ��������� */
	CAN_InitStructure.CAN_TXFP = DISABLE; /* ��������� �������� ��������� � ����, anable - �� ����������, disable - �� �������������� */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Silent_LoopBack;	// ��� ������������ ��� ������������ ��������� ����
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = temp;	// �������� ������ �������� 250���� = Fapb / ((Prescaler)* (1 + BS1 + BS2))
	CAN_Init(CAN, &CAN_InitStructure);

	// CAN filter init

	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	// CAN FIFO0 message pending interrupt enable
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);

    NVIC_SetPriority(CEC_CAN_IRQn, 0); 
    NVIC_EnableIRQ(CEC_CAN_IRQn);

}

/**
  * @brief  ���������� ���������� CAN ����
  * @note   None
  * @param  None
  * @retval None
  */
void CEC_CAN_IRQHandler(void)
{
	CanRxMsg RxMessage;

	// ������� ��������� ��� �������� ������
	RxMessage.DLC = 	0x00;
	RxMessage.ExtId = 	0x00;
	RxMessage.FMI = 	0x00;
	RxMessage.IDE = 	0x00;
	RxMessage.RTR = 	0x00;
	RxMessage.StdId = 	0x00;
	RxMessage.Data [0] = 0x00;
	RxMessage.Data [1] = 0x00;
	RxMessage.Data [2] = 0x00;
	RxMessage.Data [3] = 0x00;
	RxMessage.Data [4] = 0x00;
	RxMessage.Data [5] = 0x00;
	RxMessage.Data [6] = 0x00;
	RxMessage.Data [7] = 0x00;

	if (CAN_GetITStatus(CAN, CAN_IT_FMP0) != RESET)		// �������� �������� ����
	{
		CAN_Receive(CAN, CAN_FIFO0, &RxMessage); /*��������� ���������, � ���������� ��� � RxMessage */

		// �������� ��� ��������� ���������
		if (RxMessage.IDE == CAN_Id_Standard)
		{
			if (RxMessage.StdId == CAN_CMD_Test_Send)		// ���� �������� �������� ���������, �� �������� � ����� ������������� � ���������
			{
				CAN_Send_Ok();
			}
			if (RxMessage.StdId == CAN_CMD_Test_Ok)			// ���� �������� ������������� ���������
			{
				// ��������� ����� ���� ���, �������� ������� ������
			}
		}
	}
}

/**
  * @brief  �������� �������� ������� � ����
  * @note   ���������� ��� �������� �����
  * @param  None
  * @retval None
  */
void CAN_Send_Test(void)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = CAN_CMD_Test_Send;			// ������� ����

	TxMessage.ExtId = 0x00;							// ����������� ������� ��������� ��� ������

	TxMessage.IDE = CAN_Id_Standard;				// ������ �����
	TxMessage.RTR = CAN_RTR_DATA;					// ��� ���������
	TxMessage.DLC = 8;								// ����� ����� ������ 8 - ��������� ��� �����

	TxMessage.Data[0] = 0x00;						// ���� ������ �1
	TxMessage.Data[1] = 0x01;						// ���� ������ �2
	TxMessage.Data[2] = 0x02;						// ���� ������ �3
    TxMessage.Data[3] = 0x03;						// ���� ������ �4
    TxMessage.Data[4] = 0x04;						// ���� ������ �5
    TxMessage.Data[5] = 0x05;						// ���� ������ �6
    TxMessage.Data[6] = 0x06;						// ���� ������ �7
    TxMessage.Data[7] = 0x07;						// ���� ������ �8
	CAN_Transmit(CAN, &TxMessage);
}


/**
  * @brief  �������� ������������� ��������� �������� ������� � ����
  * @note   ���������� ������ ������� ��� ������
  * @param  None
  * @retval None
  */
void CAN_Send_Ok(void)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = CAN_CMD_Test_Ok;				// ������� ����

	TxMessage.ExtId = 0x00;							// ����������� ������� ��������� ��� ������

	TxMessage.IDE = CAN_Id_Standard;				// ������ �����
	TxMessage.RTR = CAN_RTR_DATA;					// ��� ���������
	TxMessage.DLC = 0;								// ����� ����� ������ 0 - ������ �� ��������

	CAN_Transmit(CAN, &TxMessage);
}

