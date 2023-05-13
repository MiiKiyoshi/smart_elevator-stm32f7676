//////////////////////////////////////////////////////////////////////////////////
// Name: 공기정
// Student: 60191792 / 전자공학과
//
// Class: 마이크로 프로세서 응용 
// Professor: 김정국 교수님
//
// Create Date: 5/13
// Project Name: USART3.h
// Description: USART3, 블루투스 통신을 위한 헤더파일
//////////////////////////////////////////////////////////////////////////////////

/***********************function prototype definition*************************/
void USART3_IRQHandler(void);         /* USART3 interrupt function */
void USART3_init(void);
void TX3_char(U08 data);              /* transmit a character by USART3 */
void TX3_string(U08 *string);         /* transmit a string by USART3 */
/*****************************************************************************/


/*****************************************************************************/

volatile unsigned char RXD = 0;

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

/*****************************************************************************/

void TX3_char(U08 data)            /* transmit a character by USART3 */
{
  while(!(USART3->ISR & 0x00000080));      // wait until TXE = 1
  USART3->TDR = data;
}

/*****************************************************************************/

void TX3_string(U08 *string)         /* transmit a string by USART3 */
{
  while(*string != '\0')
  {
    TX3_char(*string);
    string++;
  }
} 

/*****************************************************************************/