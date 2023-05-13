#include "stm32f767xx.h"
#include <stdio.h>

#include "Essential.h"
#include "Step_motor.h"
#include "Ultrasonic.h"
#include "USART3.h"

void USART3_IRQHandler(void);         /* USART3 interrupt function */
void EXTI0_IRQHandler(void);			/* EXTI0 interrupt function */
void EXTI1_IRQHandler(void);			/* EXTI1 interrupt function */
void EXTI2_IRQHandler(void);			/* EXTI2 interrupt function */
void EXTI3_IRQHandler(void);			/* EXTI3 interrupt function */
void TIM2_IRQHandler(void);			/* TIM2 interrupt function (5ms) */


U08 stop_elevator = 1, init_motor_flag = 1; // 엘레베이터 정지, 모터 초기화모드로 시작
U08 EL_operation_flag, Forward_flag, Backward_flag, steps;

char value_str[100] = {0};
uint8_t value_int;

/****************************엘레베이터 호출 시스템*********************************/
void USART3_IRQHandler(void)          /* USART3 interrupt function */
{
  if (USART3->ISR & 0x00000020)      // if RXNE=1, receive a character
  {
    RXD = USART3->RDR;
    if ((RXD >= 0x20) && (RXD <= 0x7F))
    {
      switch(RXD){
        case '1':
          TX3_string("\r\nthe first floor\r\n\r\n");
          break;
        case '2': 
          TX3_string("\r\nthe second floor\r\n\r\n");
          break;
        case '3': 
          TX3_string("\r\nthe third floor\r\n\r\n");
          break;
        default:
          TX3_string("\r\n***********************\r\n");
          TX3_string("Press the following key\r\n");
          TX3_string("key 1: the first floor\r\n");
          TX3_string("key 2: the second floor\r\n");
          TX3_string("key 3: the third floor\r\n");
          TX3_string("***********************\r\n\r\n");
      }
    }
  }
}
/*******************************초음파 센서값 출력**********************************/

void TIM3_IRQHandler(void){			/* TIM3 interrupt function (1s)*/
  TIM3->SR = 0x0000;
  value_int = Ulso_distance();
  sprintf(value_str, "Distance : %d [cm]\r\n", value_int);
  TX3_string((U08*)value_str);
}

/*******************************엘레베이터 버튼**************************************/
// 엘레베이터 정지 버튼
void EXTI0_IRQHandler(void)			/* EXTI0 interrupt function */
{
  stop_elevator = (stop_elevator + 1) % 2;
  GPIOD->ODR &= ~(0x000000F0);

  while((GPIOC->IDR & 0x00000001) != 0x00000001); 
  Delay_ms(20);

  EXTI->PR = 0x00000001;			// clear pending bit of EXTI0
  NVIC->ICPR[0] = 0x00000040;			// clear pending bit of (6)EXTI0
} 

// 엘레베이터 초기화 모드 <-> 작동 모드
void EXTI1_IRQHandler(void)			/* EXTI1 interrupt function */
{
  init_motor_flag = (init_motor_flag + 1) % 2;
  EL_operation_flag = (EL_operation_flag + 1) % 2;
  GPIOD->ODR &= ~(0x000000F0);

  while((GPIOC->IDR & 0x00000002) != 0x00000002); // debouncing
  Delay_ms(20);
  EXTI->PR = 0x00000002;			// clear pending bit of EXTI1
  NVIC->ICPR[0] = 0x00000080;			// clear pending bit of (7)EXTI1
}

// 엘레베이터 하강 (모터 초기화 모드)
void EXTI2_IRQHandler(void)			/* EXTI2 interrupt function */
{
  Backward_flag = (Backward_flag + 1) % 2;
  Forward_flag = 0;
  while((GPIOC->IDR & 0x00000004) != 0x00000004); // debouncing
  Delay_ms(20);
  EXTI->PR = 0x00000004;			// clear pending bit of EXTI2
  NVIC->ICPR[0] = 0x00000100;			// clear pending bit of (8)EXTI2
}

// 엘레베이터 상승 (모터 초기화 모드)
void EXTI3_IRQHandler(void)			/* EXTI3 interrupt function */
{
  Forward_flag = (Forward_flag + 1) % 2;
  Backward_flag = 0;
  while((GPIOC->IDR & 0x00000008) != 0x00000008); // debouncing
  Delay_ms(20);
  EXTI->PR = 0x00000008;			// clear pending bit of EXTI3
  NVIC->ICPR[0] = 0x00000200;			// clear pending bit of (9)EXTI3
}
/**********************************************************************************/


/******************************엘레베이터 작동 시스템**********************************/
void TIM2_IRQHandler(void){			/* TIM2 interrupt function (5ms)*/

  TIM2->SR = 0x0000;				// clear pending bit of TIM2 interrupt

  if(!stop_elevator){
    if(init_motor_flag && !EL_operation_flag){ //엘레베이터 위치 수동 초기화

      if(Forward_flag && !Backward_flag){

        GPIOD->ODR &= ~(0x000000F0);    // [7:4]비트만 clear 시킴
        GPIOD->ODR |= stepForward();		// 시계방향
      }
      if(Backward_flag && !Forward_flag){

        GPIOD->ODR &= ~(0x000000F0);    // [7:4]비트만 clear 시킴
        GPIOD->ODR |= stepBackward();		// 반시계방향
      }
    }

    if(EL_operation_flag && !init_motor_flag){ // 엘레베이터 작동

      if(Forward_flag && (steps != 0) && !Backward_flag){
        GPIOD->ODR &= ~(0x000000F0);    // [7:4]비트만 clear 시킴
        GPIOD->ODR |= stepForward();		// 시계방향
        if(steps > 0) steps--;
      }

      if(Backward_flag && (steps != 0) && !Forward_flag){
        GPIOD->ODR &= ~(0x000000F0);    // [7:4]비트만 clear 시킴
        GPIOD->ODR |= stepBackward();		// 반시계방향
        if(steps > 0) steps--;
      }
    }
  }
}
/**********************************************************************************/

int main(void){

  Initialize_MCU();
  KEY_init();
  USART3_init();
  Ulso_init();
  stepmotor_init();

  while(1){
    
  }
}