//////////////////////////////////////////////////////////////////////////////////
// Name: ������
// Student: 60191792 / ���ڰ��а�
//
// Class: ����ũ�� ���μ��� ���� 
// Professor: ������ ������
//
// Create Date: 5/13
// Project Name: step_motor.h
// Description: ���ܸ��� ���� 
//////////////////////////////////////////////////////////////////////////////////

void stepmotor_init(void);
unsigned char stepForward(void);			// �ð���� ȸ��
unsigned char stepBackward(void);		// �ݽð���� ȸ��

// 1�� ���� ����� ���� ������
unsigned char step_data[4] = {0x00000010, 0x00000020, 0x00000040, 0x00000080};
// ���� �����͸� ���� �ε��� 
int step_index = -1;

void stepmotor_init(void){
  RCC->AHB1ENR |= 0x00000008;			// port D clock enable
  GPIOD->MODER &= ~(0x0000FF00); // PD4~PD7 clear
  GPIOD->MODER |= 0x00005500;     // PD4~PD7�� mode�� ��¸��(01)�� ����

  RCC->APB1ENR |= 0x00000001; // TIM2 clock enable
  TIM2->PSC = 1079;				// 54MHz/(1079+1) = 50kHz
  TIM2->ARR = 249;				// 50kHz/(249+1) = 200Hz (5ms)
  TIM2->CNT = 0;				// clear counter
  TIM2->DIER = 0x0001;				// enable update interrupt
  TIM2->CR1 = 0x0005;				// enable TIM2 and update event

  NVIC->ISER[0] |= 0x10000000; // TIM2 interupt enable
}

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