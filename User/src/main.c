/**
  ******************************************************************************
  * @file    main.c
  * @author  Ткаченко Максим
  * @version V1.0.0
  * @date    21.04.16
  * @brief   
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 

#include  "main.h"

const TPin A0 = {GPIOA,0};
const TPin A1 = {GPIOA,1};
const TPin A4 = {GPIOA,4};
const TPin B0 = {GPIOB,0};

const TPin A13 = {GPIOA,13};
const TPin A14 = {GPIOA,14};
const TPin C0  = {GPIOC, 0};
/*-------------------------------------------------------------*/

const TPin* SimpleAnalogInput[]  = {&A0,&A1,&A4,&B0};
const TPin* ExtAnalogInput[]     = {&A13,&A14};
const TPin* TriggerInput[]       = {&C0};

uint16_t Temper;
uint16_t Volt0;
uint16_t Volt1;
uint16_t Volt2;
uint16_t Volt3;
uint16_t Volt4;
int i;
uint16_t Vref;
uint32_t y;
extern uint16_t ADC_array[];
bool f1 = false;

uint16_t HightLevel;
uint16_t LowLevel;

int main(void)
{
    ClockInit(HSI,8000000,48000000);
    SystickInit(1000); // инициилизация Systick
    ConfigVirualPort(SimpleAnalogInput,4,AN,_2MHz,NOPULL,PP);     /* Настраиваем нажки на считывание инфы с аналоговых входов */
    ConfigVirualPort(TriggerInput,1,IN,_2MHz,NOPULL,PP);          /* Настраиваем ножку триггера */
    adc_setup();

   
   pwm_Init(0);
   pwm_Set(0, 100 - 1);
  while (1)
  {
   SystickScanEvent(); //функция сканирования событий Systick

     Volt0 = ADC_array[0]*3300/(( 1 << 12 ) - 1);
   }
} 

