/*
 * @Author: 공기정 
 * @Date: 2023-05-15 22:25:25 
 * @Last Modified by: 공기정
 * @Last Modified time: 2023-05-15 22:53:26
 */

/***********************function prototype definition*************************/
void Ulso_init(void);
double Ulso_distance(void);
void accrate_Ulso(void);
/*****************************************************************************/

/*****************************************************************************/
void Ulso_init(void){
  RCC->AHB1ENR |= 0x00000008;			  // port D clock enable (GPIODEN = 1)
  GPIOD->MODER &= ~(0x00000033);    // PD2 PD0 -> clear
  GPIOD->MODER |= 0x00000001;       // PD2(echo) -> input,  PD0(trig) -> output
  GPIOD->ODR &= ~(0x00000001);			// PD0 -> clear

  RCC->APB1ENR |= 0x00000020;			// enable TIM7 clock
  TIM7->PSC = 3456;				// 54Mhz/(3455+1) = 15.625kHz
  TIM7->CNT = 0;				// clear counter
  TIM7->CR1 = 0x0001;				// enable TIM7

  RCC->APB1ENR |= 0x00000002; // TIM3 clock enable
  TIM3->PSC = 10799;				// 108MHz/(10799+1) = 10kHz
  TIM3->ARR = 999;				// 10kHz/(999+1) = 10Hz (0.1s)
  TIM3->CNT = 0;				// clear counter
  TIM3->DIER = 0x0001;				// enable update interrupt
  TIM3->CR1 = 0x0005;				// enable TIM3 and update event

  NVIC->ISER[0] |= 0x20000000; // TIM3 interupt enable
}

/*****************************************************************************/

double Ulso_distance(void)
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

	// 에코 핀의 펄스 폭 (음파가 왕복한 시간) 계산
	double time = TIM7->CNT * 0.000064;
	return time * 340.0/2.0*100.0;		// 센티미터 단위 거리 반환
}

/*****************************************************************************/

//이동 평균 필터
void accrate_Ulso(void)
{
	total = total - readings[readindex]; //가장 오래된 data를 합계에서 빼서 없앤다.
	readings[readindex] = ulso_distance; //센서입력값을 배열에 저장
	total = total + readings[readindex]; //읽은 값을 합계에 더한다.
	readindex++;						 //신호값을 읽은 인덱스를 1 증가 시킨다.

	if(readindex >= numReadings) readindex = 0; 
	//만약 신호를 읽는 인덱스의 값이 평균갯수보다 커지면 0으로 만들어 처음부터 다시 시작한다.
	
	ulso_accu_distance = total / numReadings; //평균값을 구한다.
}