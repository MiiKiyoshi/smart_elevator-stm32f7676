/* ============================================================================ */
/*	                         스텝모터 제어 프로그램                                */
/* ============================================================================ */
/*		        		  	Programmed by Ki-Jung Kong in 2023.                       */

#include "stm32f767xx.h"
#include "OK-STM767.h"

unsigned char stepForward(void);			// 시계방향 회전
unsigned char stepBackward(void);		// 반시계방향 회전

// 1상 여자 방식의 구동 데이터
unsigned char step_data[4] = {0x00000010, 0x00000020, 0x00000040, 0x00000080};
// 구동 데이터를 위한 인덱스 
int step_index = -1;

int main(void)
{
  unsigned int i; 

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization

  RCC->AHB1ENR |= 0x00000008;			// port D clock enable
  GPIOD->MODER |= 0x00005500;     // PD4~PD7의 mode를 출력모드(01)로 설정

  while(1)
  {
    for(i = 0; i < 2000; i++){		  
      GPIOD->ODR &= ~(0x000000F0);    // [7:4]비트만 clear 시킴
      GPIOD->ODR |= stepForward();		// 시계방향
      Delay_ms(5);			
    }

    Delay_ms(100);			

    for(i = 0; i < 2000; i++){		  
      GPIOD->ODR &= ~(0x000000F0);    // [7:4]비트만 clear 시킴
      GPIOD->ODR |= stepBackward();		// 반시계방향
      Delay_ms(5);			
    }

    Delay_ms(100);			
  }

}

/* ----- 사용자 함수 ---------------------------------------------------------- */

unsigned char stepForward(void)			// 시계방향 회전
{
	step_index++;
	if(step_index >= 4) step_index = 0;

	return step_data[step_index];
}

unsigned char stepBackward(void)		// 반시계방향 회전
{
	step_index--;
	if(step_index < 0) step_index = 3;
	
	return step_data[step_index];
}