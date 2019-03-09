#ifndef _GPIO_DRIVER
#define _GPIO_DRIVER

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

//    typedef enum { PA, PB, PC, PD, PE, PF, PG, PH, PI } TPort;

    typedef enum {_2MHz = 0x02,_10MHz = 0x01,_50MHz = 0x03} SPEED;// ��������
    typedef enum {IN = 0,OUT = 1,AF = 2,AN = 3} MODE;             // ��� �����: ����,�����,�������������� �������, ���������� ����/�����
    typedef enum {NOPULL = 0,PULLUP = 1,  PULLDOWN = 2}PULLUPDOWN; // ��������
    typedef enum {PP,OD}PP_OD;// ������, ��������� ��������
    typedef enum {AF0,AF1,AF2,AF3,AF4,AF5,AF6,AF7,AF8,AF9,AF10,AF11,AF12,AF13,AF14,AF15}ALTFUNC;// �������������� �������
  
	
	
	
	
    typedef struct
    {
        GPIO_TypeDef *   Port;
        uint8_t Pin;
    } TPin;
    
    /* ��������� �������� �� �����, � ����������� �� ����� */
    void SetPinValue(const TPin * PtrStructPin, uint16_t Value);
    
    // �������������
    void TalkPort (const TPin *, FunctionalState);
    
    // ��������� ������ OUT, IN, AF, AN
    void PinMode(const TPin * PtrStructPin,MODE mode);

    // �������� ����
    void gpio_DigitalInput(const TPin * Pin);

    // ���������� ������� ������� 
    void output_hight(const TPin * Pin);

    // ���������� ������ �������
    void output_low(const TPin * Pin);
    
    // �������� �������� ��������� ������
    void TogglePin(const TPin * PtrStructPin);

    // ������������ �������� �� �����
    bool gpio_Value(const TPin * Pin);

    // ����� � �������� ����������� Open Drain ()
    void PinOpenDrain(const TPin * PtrStructPopt);
	
	// ����� ������
	void PinPushPull(const TPin * PtrStructPopt);
    
    //�������� �����
    void PinPullUp(const TPin * PtrStructPin);
    
     //�������� ����
    void PinPullDown(const TPin * PtrStructPin);
    
     //��� �������� 
    void PinNoPUPD(const TPin * PtrStructPin);
    
    // ��������� �������� �����
    void SetSpeedPin(const TPin *,SPEED);
       
    // ��������� �������������� ������� (0-15)
     void PinAltFunc(const TPin * PtrStructPin, ALTFUNC altfunc);
     
     // ������� ������ �������� � ����������� ����
     
    void WriteToVirtualPort(const TPin* VPort[], uint8_t size,uint16_t Value);
     
     //����� ���� ������ ���� �������� ���������
     void PinPPorOD(const TPin * PtrStructPopt, PP_OD pp_od);
     
     // ����� ���� ��������� �����, ���� ����, ���� �����������
     void PinPUPD(const TPin * PtrStructPin, PULLUPDOWN pulluppd);
     
     // ���������������� ����������� ����� ����� ������
     // � ������ ������ ������������ ���������� �� ��� ������,�� ������� ������� ����������� �������
    void ConfigVirualPort(const TPin* VPort[], uint8_t size, MODE mode,SPEED speed,PULLUPDOWN PUPD,PP_OD pp_od);
     
     // ������ ��������� �� �����
     bool Pin_Value(const TPin * PtrStructPin);

     // ��������� ����� ������������ �����
     void PinModeVirtualPort(const TPin* VPort[], uint8_t size, MODE mode);
    
    /* ���������������� ������ ������� �� ��������� ������������ � ����������� �������������*/
    void ConfigVirualPortDinamic(TPin* VPort[], uint8_t size, MODE mode,SPEED speed,PULLUPDOWN PUPD,PP_OD pp_od);

#endif


