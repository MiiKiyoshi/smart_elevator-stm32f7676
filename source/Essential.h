/*
 * @Author: 공기정 
 * @Date: 2023-05-15 22:23:46 
 * @Last Modified by: 공기정
 * @Last Modified time: 2023-05-15 22:24:14
 */

/*****************************data type definition****************************/
typedef unsigned char	U08;		 
typedef   signed char	S08;
typedef unsigned short 	U16;
typedef   signed short	S16;
typedef unsigned int 	U32;
typedef   signed int	S32;
/*****************************************************************************/


/***********************function prototype definition*************************/
void Initialize_MCU(void);			// initialize STM32F767VGT6 MCU

void Beep(void);				// beep for 50 ms

void KEY_init(void); // initialize KEY1 ~ KEY4

void LED_on(void);				// turn LED1 on
void LED_off(void);				// turn LED1 off
void LED_toggle(void);				// toggle LED1 to blink

void Delay_ms(U32 time_ms);			// time delay for ms in 216MHz
void Delay_us(U32 time_us);			// time delay for us in 216MHz
/*****************************************************************************/

void SystemInit(void)				/* dummy system function */
{
  asm volatile("NOP");
}

void Initialize_MCU(void)			/* initialize STM32F767VGT6 MCU */
{
// (1) 명령 캐시 및 데이터 캐시 설정
  SCB_EnableICache();				// enable L1 instruction cache
  SCB_EnableDCache();				// enable L1 data cache

// (2) ART 가속기, 프리페치 버퍼, 웨이트 사이클 설정
  FLASH->ACR = 0x00000307;			// 7 waits, enable ART accelerator and prefetch

// (3) HSE 및 PLL 설정(시스템 클록 SYSCLK = 216MHz)
// 시험에 나옴 HSE, HSI

//"p.111 클록 제어 레지스터"
  RCC->CR |= 0x00010001;			// HSE on, HSI on

  while((RCC->CR & 0x00000002) == 0);		// wait until HSIRDY = 1
  RCC->CFGR = 0x00000000;			// SYSCLK = HSI
  while((RCC->CFGR & 0x0000000C) != 0);		// wait until SYSCLK = HSI

  RCC->CR = 0x00010001;				// PLL off, HSE on, HSI on

//"p.113 주파수 체배 회로"
  RCC->PLLCFGR = 0x09403608;			// SYSCLK = HSE*PLLN/PLLM/PLLP = 16MHz*216/8/2 = 216MHz

						// PLL48CK = HSE*PLLN/PLLM/PLLQ = 16MHz*216/8/9 = 48MHz
  RCC->CR = 0x01010001;				// PLL on, HSE on, HSI on
  while((RCC->CR & 0x02000000) == 0);		// wait until PLLRDY = 1

// (4) 오버드라이브 설정
  RCC->APB1ENR |= 0x10000000;			// 전원모듈 클록(PWREN = 1)
  PWR->CR1 |= 0x00010000;			// over-drive enable(ODEN = 1)
  while((PWR->CSR1 & 0x00010000) == 0);		// ODRDY = 1 ?
  PWR->CR1 |= 0x00020000;			// over-drive switching enable(ODSWEN = 1)
  while((PWR->CSR1 & 0x00020000) == 0);		// ODSRDY = 1 ?

// (5) 주변장치 클록 설정(APB1CLK = APB2CLK = 54MHz)
  RCC->CFGR = 0x3040B402;			// SYSCLK = PLL, AHB = 216MHz, APB1 = APB2 = 54MHz
						// MCO1 = HSE, MCO2 = SYSCLK/4 (timer = 108MHz)
  while((RCC->CFGR & 0x0000000C) != 0x00000008);// wait until SYSCLK = PLL
  RCC->CR |= 0x00080000;			// CSS on

// (6) I/O 보상 설정
  RCC->APB2ENR |= 0x00004000;			// 주변장치 클럭(SYSCFG = 1)
  SYSCFG->CMPCR = 0x00000001;			// enable compensation cell

// (7) 키트의 주변장치에 클록을 공급
  RCC->AHB1ENR |= 0x0000001F;			// 포트 A,B,C,D,E에 클럭 공급
  RCC->AHB2ENR |= 0x00000000;			// disable special peripherals
  RCC->AHB3ENR |= 0x00000000;			// disable FSMC
  RCC->APB1ENR |= 0x2024C000;			// 주변장치 클럭(DAC=I2C1=USART3=SPI3=SPI2=1)
  RCC->APB2ENR |= 0x00000100;			// 주변장치 클럭(ADC1 = 1)

  Delay_ms(50);					// wait for system stabilization
}

/*****************************************************************************/

void Beep(void)					/* beep for 50 ms */
{ 
  GPIOC->BSRR = 0x00008000;   			// buzzer on
  Delay_ms(50);					// delay 50 ms
  GPIOC->BSRR = 0x80000000;			// buzzer off
}

/*****************************************************************************/

void KEY_init(void){
  GPIOC->MODER &= ~(0x000000FF);			// PC0 PC1 PC2 PC3 -> input

  SYSCFG->EXTICR[0] = 0x00002222;		// EXTI3,2,1,0 = PC3,2,1,0(KEY4,3,2,1)
  EXTI->IMR = 0x0000000F;			// enable EXTI3,2,1,0 interrupt
  EXTI->RTSR = 0x00000000;			// disable EXTI3,2,1,0 rising edge
  EXTI->FTSR = 0x0000000F;			// enable EXTI3,2,1,0 falling edge
  NVIC->ISER[0] |= 0x000003C0;			// enable (9)EXTI3 ~ (6)EXTI0 interrupt
}

/*****************************************************************************/

void LED_on(void)				/* turn LED1 on */
{
  GPIOC->BSRR = 0x00000010;			// LED1 on
}

void LED_off(void)				/* turn LED1 off */
{
  GPIOC->BSRR = 0x00100000;			// LED1 off
}

void LED_toggle(void)				/* toggle LED1 to blink */
{
  GPIOC->ODR ^= 0x00000010;			// toggle LED1
}

/*****************************************************************************/

void Delay_ms(U32 time_ms)			/* time delay for ms in 216MHz */
{
  register unsigned int i;

  for(i = 0; i < time_ms; i++)
    Delay_us(1000);
}

void Delay_us(U32 time_us)			/* time delay for us in 216MHz */
{
  register unsigned int i;

  for(i = 0; i < time_us*2; i++) // i값 확인하는데 3cycle
    { asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 5
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 10
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 15
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 20
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 25
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 30
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 35
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 40
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 45
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 50
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 55
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 60
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 65 
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 70
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 75
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 80
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 85
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 90
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 95
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 100
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 105
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 110
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 115
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 120
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 125
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 130 
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 135
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 140
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 145
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 150
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 155
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 160
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 165
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 170
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 175
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 180
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 185
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 190
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 195
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 200
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 205
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 210
      asm("NOP");
      asm("NOP");
      asm("NOP");				// 213
    } //213MHz -> 1/213 us * 213 = 1us
}