/*
 * @Author: 공기정 
 * @Date: 2023-05-15 22:22:20 
 * @Last Modified by: 공기정
 * @Last Modified time: 2023-05-15 22:49:29
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "stm32f767xx.h"
#include "Essential.h"
#include "Global_variables.h"
#include "USART3.h"
#include "Step_motor.h"
#include "heap.h"
#include "Ultrasonic.h"

void USART3_IRQHandler(void);         /* USART3 interrupt function */
void EXTI0_IRQHandler(void);			/* EXTI0 interrupt function */
void EXTI1_IRQHandler(void);			/* EXTI1 interrupt function */
void EXTI2_IRQHandler(void);			/* EXTI2 interrupt function */
void EXTI3_IRQHandler(void);			/* EXTI3 interrupt function */
void TIM2_IRQHandler(void);			/* TIM2 interrupt function (5ms) */

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
          el_call(heap, 1);
          break;
        case '2': 
          TX3_string("\r\nthe second floor\r\n\r\n");
          el_call(heap, 2);
          break;
        case '3': 
          TX3_string("\r\nthe third floor\r\n\r\n");
          el_call(heap, 3);
          break;
        case '4':
          TX3_string("\r\nthe fourth floor\r\n\r\n");
          el_call(heap, 4);
          break;
        case 'f':
          Feedback_flag = (Feedback_flag + 1) % 2;
          if(Feedback_flag)
            TX3_string("\r\nfeedback ON\r\n\r\n");
          else
            TX3_string("\r\nfeedback OFF\r\n\r\n");
          break;
        case 'u':
          Ulso_print_flag = (Ulso_print_flag + 1) % 2;
          if(Ulso_print_flag)
            TX3_string("\r\nultrasonic value print ON\r\n\r\n");
          else
            TX3_string("\r\nultrasonic value print OFF\r\n\r\n");
          break;
        case 'a':
          Ulso_accuracy_flag = (Ulso_accuracy_flag + 1) % 2;
          if(Ulso_accuracy_flag)
            TX3_string("\r\nultrasonic accurate mode ON\r\n\r\n");
          else
            TX3_string("\r\nultrasonic accurate mode OFF\r\n\r\n");
          break;
        default:
          TX3_string("\r\n***********************************\r\n");
          TX3_string("Press the following key\r\n");
          TX3_string("key 1: the first floor\r\n");
          TX3_string("key 2: the second floor\r\n");
          TX3_string("key 3: the third floor\r\n");
          TX3_string("key 4: the fourth floor\r\n");
          TX3_string("key f: feedback on/off\r\n");
          TX3_string("key u: ultrasonic value print ON/OFF\r\n");
          TX3_string("key a: ultrasonic accurate mode ON/OFF\r\n");
          TX3_string("***********************************\r\n\r\n");
      }
    }
  }
}
/*******************************초음파 센서값 업데이트********************************/
void TIM3_IRQHandler(void){			/* TIM3 interrupt function (0.1s)*/
  TIM3->SR = 0x0000;
  
  char str[100] = {0};
  ulso_distance = Ulso_distance();

  if(Ulso_accuracy_flag) accrate_Ulso();

  if(Ulso_print > 9){
    if(Ulso_print_flag){
      sprintf(str, "Distance : %lf [cm]\r\n", ulso_distance);
      TX3_string((U08*)str);
      if(Ulso_accuracy_flag){
        sprintf(str, "Accurate distance : %lf [cm]\r\n", ulso_accu_distance);
        TX3_string((U08*)str);
      }
      Ulso_print = 0;
    }
  }
  
  Ulso_print++;
}

/*******************************엘레베이터 버튼**************************************/
// 엘레베이터 정지 버튼
void EXTI0_IRQHandler(void)			/* EXTI0 interrupt function */
{
  stop_elevator = (stop_elevator + 1) % 2;
  GPIOD->ODR &= ~(0x000000F0);
  if(stop_elevator) TX3_string("\r\nElevator stops\r\n\r\n");
  else TX3_string("\r\nElevator starts\r\n\r\n");

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
  Forward_flag = 0, Backward_flag = 0;
  GPIOD->ODR &= ~(0x000000F0);

  if(init_motor_flag)
    TX3_string("\r\nMotor initialization mode\r\n\r\n");
  else
    TX3_string("\r\nElevator operation mode\r\n\r\n");

  while((GPIOC->IDR & 0x00000002) != 0x00000002); // debouncing
  Delay_ms(20);
  EXTI->PR = 0x00000002;			// clear pending bit of EXTI1
  NVIC->ICPR[0] = 0x00000080;			// clear pending bit of (7)EXTI1
}

// 엘레베이터 하강 (모터 초기화 모드)
void EXTI2_IRQHandler(void)			/* EXTI2 interrupt function */
{
  Forward_flag = (Forward_flag + 1) % 2;
  Backward_flag = 0;
  TX3_string("\r\nElevator down\r\n\r\n");
  while((GPIOC->IDR & 0x00000004) != 0x00000004); // debouncing
  Delay_ms(20);
  EXTI->PR = 0x00000004;			// clear pending bit of EXTI2
  NVIC->ICPR[0] = 0x00000100;			// clear pending bit of (8)EXTI2
}

// 엘레베이터 상승 (모터 초기화 모드)
void EXTI3_IRQHandler(void)			/* EXTI3 interrupt function */
{
  Backward_flag = (Backward_flag + 1) % 2;
  Forward_flag = 0;
  TX3_string("\r\nElevator up\r\n\r\n");
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

    if(init_motor_flag == EL_operation_flag){
      init_motor_flag = 1;
      EL_operation_flag = 0;
    }
  }
}

/******************************엘레베이터 호출 시스템**********************************/
int tim_el_call_num;
U08 can_start;

void TIM4_IRQHandler(void){
  TIM4->SR = 0x0000;				// clear pending bit of TIM2 interrupt

    if(steps==0){
      tim_el_call_num++;
    }

    if (tim_el_call_num > 9){
      can_start = 1;
      tim_el_call_num = 0;
    }

    if(can_start){
      can_start = 0;
      move_EL(el_run(heap));
    }

}

/**********************************************************************************/

int main(void){

  Initialize_MCU();

  // 히프 생성
  heap = create(); 
  init(heap);	

  KEY_init();
  USART3_init();
  Ulso_init();
  stepmotor_init();
  
  RCC->APB1ENR |= 0x00000010;			// enable TIM6 clock
  TIM6->PSC = 53999;				// 54Mhz/(53999+1) = 1kHz
  TIM6->CNT = 0;				// clear counter
  TIM6->CR1 = 0x0001;				// enable TIM6
  
  RCC->APB1ENR |= 0x00000004; // TIM4 clock enable
  TIM4->PSC = 10799;				// 108MHz/(10799+1) = 10kHz
  TIM4->ARR = 9999;				// 10kHz/(9999+1) = 1Hz (1s)
  TIM4->CNT = 0;				// clear counter
  TIM4->DIER = 0x0001;				// enable update interrupt
  TIM4->CR1 = 0x0005;				// enable TIM4 and update event
  
  NVIC->ISER[0] |= 0x40000000; // TIM4 interupt enable
  
  while(1);
  
}