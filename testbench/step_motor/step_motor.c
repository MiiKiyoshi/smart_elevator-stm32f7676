/* ============================================================================ */
/*	                         ���ܸ��� ���� ���α׷�                                */
/* ============================================================================ */
/*		        		  	Programmed by Ki-Jung Kong in 2023.                       */

#include "stm32f767xx.h"
#include "OK-STM767.h"

unsigned char stepForward(void);			// �ð���� ȸ��
unsigned char stepBackward(void);		// �ݽð���� ȸ��

// 1�� ���� ����� ���� ������
unsigned char step_data[4] = {0x00000010, 0x00000020, 0x00000040, 0x00000080};
// ���� �����͸� ���� �ε��� 
int step_index = -1;

int main(void)
{
  unsigned int i; 

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization

  RCC->AHB1ENR |= 0x00000008;			// port D clock enable
  GPIOD->MODER |= 0x00005500;     // PD4~PD7�� mode�� ��¸��(01)�� ����

  while(1)
  {
    for(i = 0; i < 2000; i++){		  
      GPIOD->ODR &= ~(0x000000F0);    // [7:4]��Ʈ�� clear ��Ŵ
      GPIOD->ODR |= stepForward();		// �ð����
      Delay_ms(5);			
    }

    Delay_ms(100);			

    for(i = 0; i < 2000; i++){		  
      GPIOD->ODR &= ~(0x000000F0);    // [7:4]��Ʈ�� clear ��Ŵ
      GPIOD->ODR |= stepBackward();		// �ݽð����
      Delay_ms(5);			
    }

    Delay_ms(100);			
  }

}

/* ----- ����� �Լ� ---------------------------------------------------------- */

unsigned char stepForward(void)			// �ð���� ȸ��
{
	step_index++;
	if(step_index >= 4) step_index = 0;

	return step_data[step_index];
}

unsigned char stepBackward(void)		// �ݽð���� ȸ��
{
	step_index--;
	if(step_index < 0) step_index = 3;
	
	return step_data[step_index];
}