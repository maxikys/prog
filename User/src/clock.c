// ***********************************************************
//	clock.�
// 	���������� � ������������
//
// ***********************************************************

#include "clock.h"
#include "stm32f0xx.h"

TStructClock StructClock;
__IO uint32_t CoreValue = 8000000;

const uint32_t massMultPLL[] = {
                                    RCC_CFGR_PLLMUL2,
                                    RCC_CFGR_PLLMUL3,
                                    RCC_CFGR_PLLMUL4,
                                    RCC_CFGR_PLLMUL5,
                                    RCC_CFGR_PLLMUL6,
                                    RCC_CFGR_PLLMUL7,
                                    RCC_CFGR_PLLMUL8,
                                    RCC_CFGR_PLLMUL9,
                                    RCC_CFGR_PLLMUL10,
                                    RCC_CFGR_PLLMUL11,
                                    RCC_CFGR_PLLMUL12,
                                    RCC_CFGR_PLLMUL13,
                                    RCC_CFGR_PLLMUL14,
                                    RCC_CFGR_PLLMUL15,
                                    RCC_CFGR_PLLMUL16
                               };

/* ����� �������� ������� */
static void ClockReset(void)
{
   /* �������� HSE */
  RCC->CR |= (uint32_t)0x00000001;

#if defined(STM32F051)  
  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits */
  RCC->CFGR &= (uint32_t)0xF8FFB80C;
#else
  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
  RCC->CFGR &= (uint32_t)0x08FFB80C;
#endif /* STM32F051 */
  
  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
  RCC->CFGR &= (uint32_t)0xFFC0FFFF;

  /* Reset PREDIV1[3:0] bits */
  RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

  /* Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFEAC;

  /* Reset HSI14 bit */
  RCC->CR2 &= (uint32_t)0xFFFFFFFE;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;
}

/* ��������� ������������� ���� AHB */
 void AHBPrescaler(TAHBPrescaler AHBPrescaler)
{
    RCC->CFGR |= (uint32_t)AHBPrescaler;
}

/* ��������� ������������� ���� APB */
void APBPrescaler(TAPBPrescaler APBPrescaler)
{
    RCC->CFGR |= (uint32_t)APBPrescaler;
}
 /* ����� ��������� ������������ ��� PLL */
static void SelectSourceForPLL(Tsource source)
{
    if(source == HSE)
    {
        RCC->CFGR |=   1 << 16;
        RCC->CFGR &= ~(1 << 15);
    }
    if(source == HSI)
    {
     RCC->CFGR |=   1 << 15;
     RCC->CFGR &= ~(1 << 16);
    }
}
  /* ������������ ����� PLL */
 static void SelectPredivForPLL(uint8_t prediv)
 {
    RCC->CFGR2 = (prediv & 0x1F) - 1;
 }

 /* ����� ��������� ��� ��������� ������� */
 static void SelectBaseSystemSource(Tsource source)
 {
 
    if(source == HSE)
    {
     RCC->CFGR &= ~ (1 << 1);
     RCC->CFGR |=    1 << 0;
     while ((RCC->CFGR & 0x0C) != (uint32_t)0x04){};
    }
    
    if(source == HSI)
    {
    RCC->CFGR &= ~( (1 << 0)|(1 << 1) );
    while ((RCC->CFGR & 0x0C) != (uint32_t)0x00){};
    }
    
    if (source == PLL)
    {
     RCC->CFGR &= ~ (1 << 0);
     RCC->CFGR |=    1 << 1;
     while ((RCC->CFGR & 0x0C) != (uint32_t)0x08){};
    }
 
 }
 
 /* ����� �����. ��������� */
 static void SelectMultPLL( uint8_t Mult)
 {
     if(Mult > 1 && Mult <= 16 ) RCC->CFGR |= massMultPLL[(Mult) - 2];
 }
 
 /* ��������� HSI, ��������� ������� */
 static void InitHSI(void)
 {
     AHBPrescaler(AHBDIV1);/* ������������ ���� AHB = 1, ����� ������ (1,2,4...512) */
     APBPrescaler(APBDIV1);/* ������������ ���� APB = 1  ����� ������ (1,2,4,8,16)  */
        
      /*  ������� ��������� �������� PLL, ���:
          - ������� �������� ��� PLL - HSI/2;
          - ������� ������������ ����� PLL;
          - ������� ���������� PLL.
      */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        SelectPredivForPLL(1);/* ������������� ������������ ����� PLL (1...16), ����� �������� */
        if( StructClock.Mult > 1 )  
      {
          SelectSourceForPLL(HSI);/* ������� �������� ��� PLL */
          SelectMultPLL (StructClock.Mult);/* ������������� ����. ��������� */
          RCC->CR |= RCC_CR_PLLON; /* ������� PLL */
          while((RCC->CR & RCC_CR_PLLRDY) == 0){};/* �������� �������  PLL */
          SelectBaseSystemSource(PLL);
      }
      else 
      {
          /* ����� ������� � �������� ��������� ������� HSE */
          SelectBaseSystemSource(HSI);
      }
 }
 
 /* ��������� HSE, ��������� ������� */
 static void InitHSE(void)
 {
     volatile uint32_t counter = 0;
        RCC->CR |= (uint32_t) RCC_CR_HSEON; /* �������� HSE */
       /*�������� ������������ ����� ���������� HSE*/
       while( ( !(RCC->CR & RCC_CR_HSERDY ) ) && (counter != HSE_STARTUP_TIMEOUT))
        {
        counter++; 
        };
  /* ���� ����� HSE */
  if( (RCC->CR & RCC_CR_HSERDY)  != NoReady )
  {
        AHBPrescaler(AHBDIV1);/* ������������ ���� AHB = 1, ����� ������ (1,2,4...512) */
        APBPrescaler(APBDIV1);/* ������������ ���� APB = 1  ����� ������ (1,2,4,8,16)  */
      /*  ������� ��������� �������� PLL, ���:
          - ������� �������� ��� PLL - HSI/2;
          - ������� ������������ ����� PLL;
          - ������� ���������� PLL.
      */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)); 
        
        SelectPredivForPLL(1);/* ������������� ������������ ����� PLL (1...16), ����� �������� */
      
      if( StructClock.Mult > 1 )  
      {
          SelectSourceForPLL(HSE);/* ������� �������� ��� PLL */
          SelectMultPLL (StructClock.Mult);/* ������������� ����. ��������� */
          RCC->CR |= RCC_CR_PLLON; /* ������� PLL */
          while((RCC->CR & RCC_CR_PLLRDY) == 0){};/* �������� �������  PLL */
          SelectBaseSystemSource(PLL);
      }
      else 
      {
          /* ����� ������� � �������� ��������� ������� HSE */
          SelectBaseSystemSource(HSE);
      }
      
  }
   else
    {
        /* ��������� � ������� HSI */
        InitHSI();
    }
    
 }    
 
 
 /*������� ������������� ��������� �������� ������������ ��������� �������� ������� */
 static void InitClockStruct(uint32_t* ClockValue, uint32_t* CoreClock, uint8_t* Mult)
 {
  StructClock.ValueClock =  *ClockValue;
  StructClock.ValueCore  =  *CoreClock;
  StructClock.Mult       =  *Mult;
 }
 
// �������������
void ClockInit(Tsource source, uint32_t ClockValue, uint32_t CoreClock )
{   
    
    uint8_t temp = 0;
    temp = CoreClock/ClockValue;/* ����. ��������� */
    InitClockStruct(&ClockValue,&CoreClock,&temp);
    ClockReset();
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
    if(source == HSE)
    {
      InitHSE();
    }
    if(source == HSI) /* ���� � �������� �������� ������ HSI */
    {
     InitHSI();
    }
   CoreValue = (uint32_t) CoreClock;  /*���������������� ��� ���������� ������*/
}

// ������� ������ ����
uint32_t clock_GetSYS(void)
{
    return CoreValue;
}

// ������� ���� APB
uint32_t clock_GetAPB(void)
{
    // RCC->CFGR 10:8
    int Prescaler = 1;
    uint8_t PrescalerSelect = ((RCC->CFGR >> 8) & 0x07);
    
    if(PrescalerSelect & 0x04) Prescaler = (2 << (PrescalerSelect & 0x03));
    
    return (CoreValue / Prescaler);
}


