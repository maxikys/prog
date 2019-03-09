#include "USART_ini.h"

// ����� �� �����
#define RX_BUFFER_SIZE 350 // ������ ������
volatile uint8_t    rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t   rx_wr_index=0, //������ ������ ������(���� ������ ������)
                    rx_rd_index=0, //������ ������ ������ (�� ������ ������)
                    rx_counter=0; //���������� ������ � ������ 
volatile uint8_t    rx_buffer_overflow=0; // ���� ������������ ������

// ����� �� ��������
#define TX_BUFFER_SIZE 350 //������ ������ 
volatile uint8_t   tx_buffer[TX_BUFFER_SIZE];
volatile uint16_t  tx_wr_index=0, //������ ������ ������(���� ������ ������)
                   tx_rd_index=0, //������ ������ ������ (�� ������ ������)
                   tx_counter=0; //���������� ������ � ������

//=====================================================
// ������ ������ ������
//=====================================================

uint8_t get_char(void)
{
uint8_t data;

	data = rx_buffer[rx_counter];
	
return data;
}


//=====================================================
// ������ �������� ������
//=====================================================
void put_char(uint8_t c) //�������� ������
{
while (tx_counter == TX_BUFFER_SIZE); 
USART_ITConfig(USART1, USART_IT_TXE, DISABLE); 
if (tx_counter || (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)) 
  {
  tx_buffer[tx_wr_index++]=c; 
  if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0; 
  ++tx_counter; 
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
  }
else 
  USART_SendData(USART1,c); 
}

//=====================================================
// ������ �������� ������
//=====================================================
void put_string(unsigned char *s)
{
  while (*s != 0)
    put_char(*s++);
}
//=====================================================
// ������ �������� ������ �����
//=====================================================
void put_int(int32_t data)
{
  unsigned char temp[10],count=0;
  if (data<0) 
  {
    data=-data;
    put_char('-');
  }     
  if (data)
  {
    while (data)
    {
      temp[count++]=data%10+'0';
      data/=10;                 
    }                           
    while (count)           
      put_char(temp[--count]);          
  }
  else put_char('0');           
}
//=====================================================
// ���������� ���������� USART1
//=====================================================

void USART1_IRQHandler(void)
{ 
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) //��������� ����������� �� ���������� �� ������
  {
			USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			if(rx_counter < TX_BUFFER_SIZE)
			{
		  rx_buffer[rx_counter] = USART_ReceiveData(USART1);
			//USART_SendData(USART1,rx_buffer[rx_counter]); //���
			rx_counter++;
			}
			else {
				rx_counter=0;
								
			}
	}

  if(USART_GetITStatus(USART1, USART_IT_TXE) == SET) //��������� ���� ����������� ������
  {   
    if (tx_counter) //���� ���� ��� ����������
    {
      --tx_counter; // ��������� ���������� ������������ ������
      USART_SendData(USART1,tx_buffer[tx_rd_index++]); //�������� ������ ������������� ����� ������
      if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0; //���� ����� �� ������ ������
    }
    else //���� ������ ���������� ��������� ���������� �� ��������
    {
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                    
    }
  }
}

void USART_ini(void)
	
{
	//----------------------------------------
	// ��������� ��� USART1
	//----------------------------------------
	GPIO_InitTypeDef GPIO_ini_USART1;
	USART_InitTypeDef USART1_ini;
	//RCC_AHBPeriph_GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_ini_USART1.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_ini_USART1.GPIO_Mode = GPIO_Mode_AF;
	GPIO_ini_USART1.GPIO_OType = GPIO_OType_PP;
	GPIO_ini_USART1.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_ini_USART1.GPIO_Speed = GPIO_Speed_Level_3;
		
	GPIO_Init(GPIOA,&GPIO_ini_USART1);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_7);
	//----------------------------------------
	//���������  USART1 
	//----------------------------------------
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	
	USART1_ini.USART_BaudRate = 9600;
	USART1_ini.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART1_ini.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART1_ini.USART_Parity = USART_Parity_No;
	USART1_ini.USART_StopBits = USART_StopBits_1;
	USART1_ini.USART_WordLength =  USART_WordLength_8b;
	
	USART_Init(USART1,&USART1_ini);
	
	USART_Cmd(USART1,ENABLE);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	
}

