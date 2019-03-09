#include "usart.h"

/*******************************************************************/

GPIO_InitTypeDef gpio;
USART_InitTypeDef usart;

uint8_t sendData[8];
uint8_t bytesToSend = 8;

uint8_t sendDataCounter = 0;



// ����� �� �����
#define RX_BUFFER_SIZE 256 // ������ ������
volatile uint8_t    rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t   rx_wr_index=0, //������ ������ ������(���� ������ ������)
                    rx_rd_index=0, //������ ������ ������ (������ ��������� ������)
                    rx_counter=0; //���������� ������ � ������ 
volatile uint8_t    rx_buffer_overflow=0; //���� � ������������ ������

// ����� �� �����
#define TX_BUFFER_SIZE 256 //������ ������
volatile uint8_t   tx_buffer[TX_BUFFER_SIZE];
volatile uint16_t  tx_wr_index=0, //������ ������ ������(���� ������ ������)
                   tx_rd_index=0, //������ ������ ������ (������ ��������� ������)
                   tx_counter=0; //���������� ������ � ������ 



 
 NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure the Priority Group to 2 bits */

 const TPin TX ={GPIOA,2};
 const TPin RX ={GPIOA,3};
 const TPin* Usart1Pin[] = {&TX,&RX};
 


uint8_t get_char(void) //����� ������
{
uint8_t data; //��������� ����������
while (rx_counter==0);  //���� ������ ���, ����
data=rx_buffer[rx_rd_index++]; //����� ������ �� ������
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0; //���� �� �����
USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); //��������� ����������
--rx_counter; //����� ��� �� �������� �������� ����������
USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������� ����������
return data;
}

void put_char(uint8_t c) //����� ������
{
while (tx_counter == TX_BUFFER_SIZE); //���� ������ ���, ����
USART_ITConfig(USART1, USART_IT_TXE, DISABLE); //��������� ����������, ����� ��� �� ������ ������ ����������
if (tx_counter || (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)) //���� � ������ ��� ���-�� ���� ��� ���-�� ����������
  {
  tx_buffer[tx_wr_index++]=c; //�� ������ ������ � �����
  if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0; //?���� �� �����
  ++tx_counter; //����������� ������� ������ � ������
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //��������� ����������
  }
else //���� UART ��������
  USART_SendData(USART1,c); //�������� ������ ��� ����������
}

 /*���������� ����������*/
 void USART1_IRQHandler(void)
{ 
   /*------------------�����---------------------------------------*/
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) //���������� �� ������ ������
  {
    if ((USART1->ISR & (USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_ORE)) == 0) //��������� ����� ������
    {                   
      rx_buffer[rx_wr_index++]= (uint8_t) (USART_ReceiveData(USART1)& 0xFF); //������� ������ � �����, ������������� �����
      if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0; //�������� �� ������������ ������ ������(���� �� �����)
      if (++rx_counter == RX_BUFFER_SIZE) //���� ������������ ������
      {
        rx_counter=0; //�������� �������(�������� �������)
        rx_buffer_overflow=1;  //���� �����������
      }
    }
    else USART_ReceiveData(USART1); //��������� ������ (������ ���������� ��������� ����)
  }
   
  if(USART_GetITStatus(USART1, USART_IT_ORE) == SET) //���������� �� ������������ ������
  {
     USART_ReceiveData(USART1); //��������� ������ (������ ���������� ���� ������� �� ������)
  }
   /*------------------��������---------------------------------------*/
  if(USART_GetITStatus(USART1, USART_IT_TXE) == SET) //��������� ����
  {   
    if (tx_counter) //���� ���� ������
    {
      --tx_counter; // ��������� �������
      USART_SendData(USART1,tx_buffer[tx_rd_index++]); //�������� ������ ������������� �����
      if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0; //���� �� �����
    }
    else //���� ������ ���������� ��������� ���������� �� ��������
    {
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                    
    }
  }
}
 
 
 
/*******************************************************************/
void UsartInit(void)
{
    
     ConfigVirualPort(Usart1Pin,2,AF,_50MHz,PULLUP,PP); 
     PinAltFunc(&TX,AF1);
     PinAltFunc(&RX,AF1);
 
    // �������� ������������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  usart.USART_BaudRate = 19200;// ��������
  usart.USART_WordLength = USART_WordLength_8b; //8 ��� ������
  usart.USART_StopBits = USART_StopBits_1; //���� ���� ���
  usart.USART_Parity = USART_Parity_No; //�������� - ���
  usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ���������� ������� - ���
  usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       // ���������� ������ � ��������
  USART_Init(USART1, &usart);
  
   //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //������������ ����� � �������� ����������
  
  /* Enable the USARTx Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //���������� �� uart2 
   NVIC_InitStructure.NVIC_IRQChannelPriority = 0; //��������� � ������ 
   //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�������� � ���������
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //��������� ����������
   NVIC_Init(&NVIC_InitStructure); //������������� ���������
   
   USART_Cmd(USART1, ENABLE);/* �������� ������ usart1 */
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
