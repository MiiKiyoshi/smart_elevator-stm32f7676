/*
 * @Author: ������ 
 * @Date: 2023-05-15 22:25:25 
 * @Last Modified by: ������
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
	// Ʈ���� ������ �޽� ���
	GPIOD->BSRR = 0x00010000;	// LOW �� ���
	Delay_us(1); 
	GPIOD->BSRR = 0x00000001;			// HIGH �� ���
	Delay_us(20);				// 20 ����ũ���� ���
	GPIOD->BSRR = 0x00010000;			// LOW �� ���
        
	// ���� ���� HIGH�� �� ������ ���
	TIM7->CNT = 0;
	while(!(GPIOD->IDR & 0x00000004))
	  if(TIM7->CNT > 65000) return 0;		// ��ֹ��� ���� ���

	// ���� ���� LOW�� �� �������� �ð� ����
	TIM7->CNT = 0;				// ī���͸� 0���� �ʱ�ȭ
	while(GPIOD->IDR & 0x00000004){
		if (TIM7->CNT > 650000){			// ���� �Ұ���
			TIM7->CNT = 0;
			break;
		}
	}

	// ���� ���� �޽� �� (���İ� �պ��� �ð�) ���
	double time = TIM7->CNT * 0.000064;
	return time * 340.0/2.0*100.0;		// ��Ƽ���� ���� �Ÿ� ��ȯ
}

/*****************************************************************************/

//�̵� ��� ����
void accrate_Ulso(void)
{
	total = total - readings[readindex]; //���� ������ data�� �հ迡�� ���� ���ش�.
	readings[readindex] = ulso_distance; //�����Է°��� �迭�� ����
	total = total + readings[readindex]; //���� ���� �հ迡 ���Ѵ�.
	readindex++;						 //��ȣ���� ���� �ε����� 1 ���� ��Ų��.

	if(readindex >= numReadings) readindex = 0; 
	//���� ��ȣ�� �д� �ε����� ���� ��հ������� Ŀ���� 0���� ����� ó������ �ٽ� �����Ѵ�.
	
	ulso_accu_distance = total / numReadings; //��հ��� ���Ѵ�.
}