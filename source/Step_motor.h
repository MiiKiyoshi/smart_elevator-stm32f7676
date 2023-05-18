/*
 * @Author: 공기정 
 * @Date: 2023-05-15 22:24:51 
 * @Last Modified by: 공기정
 * @Last Modified time: 2023-05-15 22:38:34
 */

#define PI 3.141592
#define RADIUS 1.2
#define UNIT_ANGLE 720
#define FLOOR2FLOOR 15
#define GROUND_OFFSET 3 // 초음파센서를 위한거
#define CEILING_OFFSET 5 // 모터가 위아래로 이동시키는 관점에서의 엘레베이터의 높이를 구하기 위한 변수

void stepmotor_init(void){
  RCC->AHB1ENR |= 0x00000008;			// port D clock enable
  GPIOD->MODER &= ~(0x0000FF00); // PD4~PD7 clear
  GPIOD->MODER |= 0x00005500;     // PD4~PD7의 mode를 출력모드(01)로 설정

  RCC->APB1ENR |= 0x00000001; // TIM2 clock enable
  TIM2->PSC = 1079;				// 54MHz/(1079+1) = 50kHz
  TIM2->ARR = 249;				// 50kHz/(249+1) = 200Hz (5ms)
  TIM2->CNT = 0;				// clear counter
  TIM2->DIER = 0x0001;				// enable update interrupt
  TIM2->CR1 = 0x0005;				// enable TIM2 and update event

  NVIC->ISER[0] |= 0x10000000; // TIM2 interupt enable
}

unsigned char stepForward(void){			// 시계방향 회전
	step_index++;
	if(step_index >= 4) step_index = 0;

	return step_data[step_index];
}

unsigned char stepBackward(void){		// 반시계방향 회전
	step_index--;
	if(step_index < 0) step_index = 3;
	
	return step_data[step_index];
}

void Forward(double angle){
  steps = (int) round((2100/360) * angle);
  //operating_flag = 1;

  init_motor_flag = 0;
  EL_operation_flag = 1;

  Backward_flag = 0;
  Forward_flag = 1;
}

void Backward(double angle){
  steps = (int) round((2100/360) * angle);
  //operating_flag = 1;

  init_motor_flag = 0;
  EL_operation_flag = 1;

  Forward_flag = 0;
  Backward_flag = 1;
}

void move_EL(int dest_floor){
  char str[100] = {0};
  int dest_angle = (4 - dest_floor) * UNIT_ANGLE;
  int curr_angle = (4 - curr_floor) * UNIT_ANGLE;

  if(curr_angle > dest_angle){
    Backward((double)(curr_angle - dest_angle));
    sprintf(str, "going to %d\r\n", dest_floor);
    TX3_string((U08*)str);
  }
  else if(dest_angle > curr_angle){
    Forward((double)(dest_angle - curr_angle));
    sprintf(str, "going to %d\r\n", dest_floor);
    TX3_string((U08*)str);
  }
  else return;

  if(Feedback_flag){
    feedback_EL();
    feedback_EL();
    feedback_EL();
  }

  curr_floor = dest_floor;
}

void feedback_EL(void){
  double real_height;
  double angle;

  if(Ulso_accuracy_flag)
    real_height = ulso_accu_distance;
  else
    real_height = ulso_distance;

  int ideal_height = GROUND_OFFSET + FLOOR2FLOOR * (curr_floor - 1);

  if(real_height > ideal_height){
    angle = 360 * (real_height - ideal_height) / (2 * PI * RADIUS);
    Forward(angle);
  }
  else if(ideal_height > real_height){
    angle = 360 * (ideal_height - real_height) / (2 * PI * RADIUS);
    Backward(angle);
  }

  while(steps != 0);
}