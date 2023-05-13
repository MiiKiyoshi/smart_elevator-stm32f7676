#include "stm32f767xx.h"
#include "OK-STM767.h"
#include "stdio.h"

void USART3_IRQHandler(void);         /* USART3 interrupt function */
void USART3_init(void);
void TX3_char(U08 data);              /* transmit a character by USART3 */
void TX3_string(U08 *string);         /* transmit a string by USART3 */

void Timer7_init(void);
void Ulso_init(void);
uint8_t Ulso_distance(void);

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */
volatile unsigned char RXD = 0;

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
/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  char value_str[100] = {0};
  uint8_t value_int;

  Initialize_MCU();               // initialize MCU and kit
  Delay_ms(50);                   // wait for system stabilization

  USART3_init();
  Timer7_init();
  Ulso_init();

  while (1)
  {
    value_int = Ulso_distance();
    sprintf(value_str, "Distance : %d [cm]\r\n", value_int);
    TX3_string((U08*)value_str);
    Delay_ms(1000);
  }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

void USART3_init(void){
  GPIOB->MODER &= 0xFF0FFFFF;			// PB10 = USART3_TX, PB11 = USART3_RX
  GPIOB->MODER |= 0x00A00000;
  GPIOB->AFR[1] &= 0xFFFF00FF;
  GPIOB->AFR[1] |= 0x00007700;
  RCC->APB1ENR |= 0x00040000;			// enable USART3 clock

  USART3->CR1 = 0x0000002D;			// RXNEIE=TE=RE=UE = 1, 8 data bit, oversampling by 16
  USART3->CR2 = 0x00000000;			// asynchronous mode, 1 stop bit
  USART3->CR3 = 0x00000000;			// 3 sampling bit
  USART3->BRR = 2813;				// 19200 bps = 54MHz/2812.5
  Delay_ms(1);
  RXD = USART3->RDR;				// dummy read

  NVIC->ISER[1] = 0x00000080;			// enable (39)USART3 interrupt
}

void TX3_char(U08 data)            /* transmit a character by USART3 */
{
  while(!(USART3->ISR & 0x00000080));      // wait until TXE = 1
  USART3->TDR = data;
}

void TX3_string(U08 *string)         /* transmit a string by USART3 */
{
  while(*string != '\0')
  {
    TX3_char(*string);
    string++;
  }
} 

void Timer7_init(void){
  RCC->APB1ENR |= 0x00000020;			// enable TIM7 clock
  TIM7->PSC = 3456;				// 54Mhz/(3455+1) = 15.625kHz
  TIM7->CNT = 0;				// clear counter
  TIM7->CR1 = 0x0001;				// enable TIM7
}

void Ulso_init(void){
  RCC->AHB1ENR |= 0x00000008;			  // port D clock enable (GPIODEN = 1)
  GPIOD->MODER &= ~(0xFFFFFFFF);    // PD2 PD0 -> clear
  GPIOD->MODER |= 0x00000001;       // PD2(echo) -> input,  PD0(trig) -> output
  GPIOD->ODR &= ~(0x00000001);			// PD0 -> clear
}

uint8_t Ulso_distance(void)
{
	// 트리거 핀으로 펄스 출력
	GPIOD->BSRR = 0x00010000;	// LOW 값 출력
	Delay_us(1);
	GPIOD->BSRR = 0x00000001;			// HIGH 값 출력
	Delay_us(20);				// 20 마이크로초 대기
	GPIOD->BSRR = 0x00010000;			// LOW 값 출력
        
        
	// 에코 핀이 HIGH가 될 때까지 대기
	TIM7->CNT = 0;
	while(!(GPIOD->IDR & 0x00000004))
	  if(TIM7->CNT > 65000) return 0;		// 장애물이 없는 경우

	// 에코 핀이 LOW가 될 때까지의 시간 측정
	TIM7->CNT = 0;				// 카운터를 0으로 초기화
	while(GPIOD->IDR & 0x00000004){
		if (TIM7->CNT > 650000){			// 측정 불가능
			TIM7->CNT = 0;
			break;
		}
	}

	// 에코 핀의 펄스 폭을 마이크로초 단위로 계산
	double time = TIM7->CNT * 0.000064;
	return (int)(time * 340.0/2.0*100.0);		// 센티미터 단위 거리 반환
}