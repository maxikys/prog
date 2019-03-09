#include "RingBuffer.h"



 uint8_t IndexIN = 0, IndexOUT = 0;
 uint8_t RingBuff[SizeRingBuff];

/* ������� �������� ���������  ������������ ���������� ������ ��������� true ���� � ������ ���� ������, false - ������ ���� ������*/
  bool RingBuffEmpty(void)
  {
   if(IndexIN != IndexOUT) return false;/*� ������ ���� ������*/
   else return true;       
  }
 /*������ ������ � ��������� �����*/ 
 void WriteToRingBuff( uint8_t Value)
  {
   RingBuff[IndexIN++] = Value; /*�������� � ��������� ����� �������� �������� ������� �������*/
   IndexIN &= MaskRingBuff; /* ���� ������ ������ ����� �� ������� ���������� ������, �� �������� ��� � ������*/
  };
 /*���������� ������ �������������� ������ � ������*/
  uint8_t SizeWriteDataToRingBuff(void)
  {
    return ( IndexIN - IndexOUT) & MaskRingBuff;
  }
 /* ������ �� ���������� ������*/
 uint8_t ReadFromRingBuff( void )
  {
       volatile uint8_t Value;
       Value = RingBuff[IndexOUT++];
       IndexOUT &= MaskRingBuff;
       return Value;
  }
