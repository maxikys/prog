#include "pwm.h"
#include "gpio_driver.h"
#include "clock.h"
#include "stm32f0xx.h"

// TODO: �������� ��������� ������ ��� TIM1/TIM8!

// ����������: 15 ���
#define Count  1000

// ������� ���: 600 ��
#define FREQUENCY   1000               

//const TPin Pb4 = {GPIOB,4};
//const TPin *VPb4[] = {&Pb4};
 static uint16_t CalculationStartValue(void);
//float GammaCorrection(uint32_t TimetMaxCount, uint32_t SetValue);
// �������� ������ ������ �������
typedef struct
{
    TIM_TypeDef *       TIM;                 // ������
    int                 Channel;             // ����� �������
    bool                Advanced;            // � ���������� ���������� ����������? (������� �������������� ������������)
    
    uint32_t            RCCRegMask;          // ����� ��� ���������� �������������
    volatile uint32_t * RCCReg;              // ����� �������� ���������� �������������
    
    ALTFUNC             AlternateFunction;   // ����� �������������� �������
    TPin                Pin;                 // �����
    AktiveState         State;               //  ���������� ����
    
} TChannel;

  typedef struct
  {
    uint16_t            StartValue;
  }TStartValue;
  
 static TStartValue StartValue;
// ������� � ����������� ��� (����� ������� ���� ������������ � �������������� ���������
static const TChannel Channels[] = {
                                    
                                    
                                    {  TIM3,  4, false,  RCC_APB1ENR_TIM3EN,  &RCC->APB1ENR, AF1, {GPIOB,  1},  AktiveLow},  // PB1  - TIM3_CH4
                                    {  TIM3,  3, false,  RCC_APB1ENR_TIM3EN,  &RCC->APB1ENR, AF1, {GPIOB,  0},  AktiveHigh},  // PB2  - TIM3_CH3
                                    
                                    
                                   };

// ���������� �������
static const int ChannelCount = sizeof(Channels) / sizeof(Channels[0]);

// ���������������� ������ ��� ���
static void pwm_InitTimer(const TChannel * Channel)
{
    
    uint32_t Clock = clock_GetAPB(); // ������� ����
    //uint32_t Counts = (1UL << RESOLUTION);
    
    // �������� ������������ ������
    *Channel->RCCReg |= Channel->RCCRegMask;
    
    // �����������: 16 ���
    Channel->TIM->ARR = Count - 1;
    
    // ������������:
    Channel->TIM->PSC = (Clock / FREQUENCY / Count) - 1;
    
    // �������� ����
    Channel->TIM->CR1 |= TIM_CR1_CEN;

    if(Channel->Advanced)
    {
        // ��������� �����
        Channel->TIM->BDTR |= TIM_BDTR_MOE;
    }
    
    StartValue.StartValue =  CalculationStartValue();/*������ ���������� ��������*/
}

// ��������� �������� �������� ���������
static void pwm_SetValue(const TChannel * Channel, uint16_t Value)
{
    switch(Channel->Channel)
    {
    case 1: Channel->TIM->CCR1 = Value; break;
    case 2: Channel->TIM->CCR2 = Value; break;
    case 3: Channel->TIM->CCR3 = Value; break;
    case 4: Channel->TIM->CCR4 = Value; break;
    }
}

// ������������� ������ ���
static void pwm_InitChannel(const TChannel * Channel)
{
   /*��������� ����� ��*/
       TalkPort(&Channel->Pin,ENABLE);
       PinMode(&Channel->Pin,AF);   
       SetSpeedPin(&Channel->Pin,_50MHz);   
       PinPUPD(&Channel->Pin,NOPULL);
       PinPPorOD(&Channel->Pin,PP);
       PinAltFunc(&Channel->Pin,Channel->AlternateFunction);
    // ��������� ���� �� ����� ���������
    Channel->TIM->CR1 &= ~TIM_CR1_CEN;
    
    switch(Channel->Channel)
    {
    case 1:
        // �����: ���1 0b110
        Channel->TIM->CCMR1 &= ~TIM_CCMR1_OC1M;  // ������� ���� ������
        Channel->TIM->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1); // ������� 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC1P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC1P;
        //Channel->TIM->CCER 
        
        // ����� 1 ��������
        Channel->TIM->CCER |= TIM_CCER_CC1E;
        break;
    case 2:
        // �����: ���1 0b110
        Channel->TIM->CCMR1 &= ~TIM_CCMR1_OC2M;  // ������� ���� ������
        Channel->TIM->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1); // ������� 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC2P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC2P;
        // ����� 2 ��������
        Channel->TIM->CCER |= TIM_CCER_CC2E;
        break;
    case 3:
        // �����: ���1 0b110
        Channel->TIM->CCMR2 &= ~TIM_CCMR2_OC3M;  // ������� ���� ������
        Channel->TIM->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // ������� 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC3P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC3P;
        // ����� 3 ��������
        Channel->TIM->CCER |= TIM_CCER_CC3E;
        break;
    case 4:
        // �����: ���1 0b110
        Channel->TIM->CCMR2 &= ~TIM_CCMR2_OC4M;  // ������� ���� ������
        Channel->TIM->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // ������� 0b110
        if(Channel->State == AktiveHigh)Channel->TIM->CCER |= TIM_CCER_CC4P;
        else Channel->TIM->CCER &= ~TIM_CCER_CC4P;
        // ����� 4 ��������
        Channel->TIM->CCER |= TIM_CCER_CC4E;
        break;
    }
    
    // ������� �����
    pwm_SetValue(Channel, 0);
    
    // ��������� ������ � ������ ��������, ���� ��� ���������� �������� ����� ����������.
    Channel->TIM->EGR = TIM_EGR_UG;
    
    // �������� ����
    Channel->TIM->CR1 |= TIM_CR1_CEN;
}

// ������ � ������
static const TChannel * pwm_GetChannel(int Channel)
{
    if (Channel >= ChannelCount) return 0;
    
    return &Channels[Channel];
}

// ��������� �����
void pwm_Init(int Channel)
{
    const TChannel * C = pwm_GetChannel(Channel);
    if(!C) return;
    
    // ��������
    pwm_InitTimer(C);
    pwm_InitChannel(C);
}

/*����� ������������� 3-��� �������*/
float GammaCorrection(uint32_t TimetMaxCount, uint32_t SetValue)
{
      double f = ((double)SetValue/(float)TimetMaxCount);
      return f*f*f; // gamma = 3

}
/* ������� ���������� �������� ��� ������������� ����� ��������� ������ �������� ���� "1" */
static uint16_t CalculationStartValue(void)
{
   uint16_t i = 0, ScaledValue = 0,PwmValue = 0;
   float level = 0;
   
   
   for (i = 0; i < Count; i++)
   {
    ScaledValue = Count/100 * i;
    level = GammaCorrection(Count, ScaledValue);
    PwmValue = (uint16_t)(level*ScaledValue);  
    if(PwmValue>=1)return i;
   }
  return 1;
}


// ������ ����������  (0-100%)
void PwmSetPersent(int Channel, uint16_t Value)
{
    // ������� � ������� ����������
    uint16_t ScaledValue = 0, PwmValue = 0;
    float level = 0;
   
    const TChannel * C = pwm_GetChannel(Channel);
    if(!C) return;
    
    if(Value > 100) return;
    
    ScaledValue = Count/100 * (Value + StartValue.StartValue - 1);
    if(Value + StartValue.StartValue > 100) 
    {
       pwm_SetValue(C,Count - 1);
    }
    level = GammaCorrection(Count, ScaledValue);
    PwmValue = (uint16_t)(level*ScaledValue);
    // ��������� ��������
    pwm_SetValue(C,PwmValue);
}



// ������ ���������� (0-0xFFFF)
void pwm_Set(int Channel, uint16_t Value)
{
    // ������� � ������� ����������
    uint16_t ScaledValue = Value;
    
    const TChannel * C = pwm_GetChannel(Channel);
    if(!C) return;
    
    // ��������� ��������
    pwm_SetValue(C, ScaledValue);
}
