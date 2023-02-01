/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
typedef struct
{
    uint16_t frequency;
    uint16_t duration;
} Tone;
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern TIM_HandleTypeDef htim2;
TIM_HandleTypeDef *pwm_timer = &htim2;	// Point to PWM Timer configured in CubeMX
uint32_t pwm_channel = TIM_CHANNEL_2;   // Select configured PWM channel number

const Tone *volatile melody_ptr;
volatile uint16_t melody_tone_count;
volatile uint16_t current_tone_number;
volatile uint32_t current_tone_end;
volatile uint16_t volume = 50;          // (0 - 1000)
int isPause =0 ;

const Tone a_note[]={
	{466,153},
	{0,0}
};
const Tone s_note[]={
	{294,153},
	{0,0}
};
const Tone d_note[]={
	{392,153},
	{0,0}
};
const Tone f_note[]={
	{466,153},
	{0,0}
};
const Tone g_note[]={
	{392,153},
	{0,0}
};
const Tone h_note[]={
	{784,153},
	{0,0}
};
const Tone j_note[]={
	{587,153},
	{0,0}
};
const Tone k_note[]={
	{392,153},
	{0,0}
};
const Tone l_note[]={
	{415,153},
	{0,0}
};

const Tone super_mario2[]= {
		{659,153},
		{659,153},
		{0,153},
		{659,153},
		{0,153},
		{523,153},
		{659,153},
		{784,306},
		{0,306},
		{392,153},
		{0,306},
		{523,459},
		{392,153},
		{0,306},
		{330,459},
		{440,306},
		{494,306},
		{466,153},
		{440,306},
		{392,229},
		{659,229},
		{784,229},
		{880,306},
		{698,153},
		{784,153},
		{0,153},
		{659,306},
		{523,153},
		{587,153},
		{494,459},
		{523,459},
		{392,153},
		{0,306},
		{330,459},
		{440,306},
		{494,306},
		{466,153},
		{440,306},
		{392,229},
		{659,229},
		{784,229},
		{880,306},
		{698,153},
		{784,153},
		{0,153},
		{659,306},
		{523,153},
		{587,153},
		{494,459},
		{0,306},
		{784,153},
		{740,153},
		{698,153},
		{622,306},
		{659,153},
		{0,153},
		{415,153},
		{440,153},
		{262,153},
		{0,153},
		{440,153},
		{523,153},
		{587,153},
		{0,306},
		{622,306},
		{0,153},
		{587,459},
		{523,612},
		{0,612},
		{0,306},
		{784,153},
		{740,153},
		{698,153},
		{622,306},
		{659,153},
		{0,153},
		{415,153},
		{440,153},
		{262,153},
		{0,153},
		{440,153},
		{523,153},
		{587,153},
		{0,306},
		{622,306},
		{0,153},
		{587,459},
		{523,612},
		{0,612},
		{523,153},
		{523,306},
		{523,153},
		{0,153},
		{523,153},
		{587,306},
		{659,153},
		{523,306},
		{440,153},
		{392,612},
		{523,153},
		{523,306},
		{523,153},
		{0,153},
		{523,153},
		{587,153},
		{659,153},
		{0,1224},
		{523,153},
		{523,306},
		{523,153},
		{0,153},
		{523,153},
		{587,306},
		{659,153},
		{523,306},
		{440,153},
		{392,612},
		{659,153},
		{659,153},
		{0,153},
		{659,153},
		{0,153},
		{523,153},
		{659,306},
		{784,306},
		{0,306},
		{392,306},
		{0,306},
		{523,459},
		{392,153},
		{0,306},
		{330,459},
		{440,306},
		{494,306},
		{466,153},
		{440,306},
		{392,229},
		{659,229},
		{784,229},
		{880,306},
		{698,153},
		{784,153},
		{0,153},
		{659,306},
		{523,153},
		{587,153},
		{494,459},
		{523,459},
		{392,153},
		{0,306},
		{330,459},
		{440,306},
		{494,306},
		{466,153},
		{440,306},
		{392,229},
		{659,229},
		{784,229},
		{880,306},
		{698,153},
		{784,153},
		{0,153},
		{659,306},
		{523,153},
		{587,153},
		{494,459},
		{659,153},
		{523,306},
		{392,153},
		{0,306},
		{415,306},
		{440,153},
		{698,306},
		{698,153},
		{440,612},
		{587,229},
		{880,229},
		{880,229},
		{880,229},
		{784,229},
		{698,229},
		{659,153},
		{523,306},
		{440,153},
		{392,612},
		{659,153},
		{523,306},
		{392,153},
		{0,306},
		{415,306},
		{440,153},
		{698,306},
		{698,153},
		{440,612},
		{494,153},
		{698,306},
		{698,153},
		{698,229},
		{659,229},
		{587,229},
		{523,153},
		{330,306},
		{330,153},
		{262,612},
		{659,153},
		{523,306},
		{392,153},
		{0,306},
		{415,306},
		{440,153},
		{698,306},
		{698,153},
		{440,612},
		{587,229},
		{880,229},
		{880,229},
		{880,229},
		{784,229},
		{698,229},
		{659,153},
		{523,306},
		{440,153},
		{392,612},
		{659,153},
		{523,306},
		{392,153},
		{0,306},
		{415,306},
		{440,153},
		{698,306},
		{698,153},
		{440,612},
		{494,153},
		{698,306},
		{698,153},
		{698,229},
		{659,229},
		{587,229},
		{523,153},
		{330,306},
		{330,153},
		{262,612},
		{523,153},
		{523,306},
		{523,153},
		{0,153},
		{523,153},
		{587,153},
		{659,153},
		{0,1224},
		{523,153},
		{523,306},
		{523,153},
		{0,153},
		{523,153},
		{587,306},
		{659,153},
		{523,306},
		{440,153},
		{392,612},
		{659,153},
		{659,153},
		{0,153},
		{659,153},
		{0,153},
		{523,153},
		{659,306},
		{784,306},
		{0,306},
		{392,306},
		{0,306},
		{659,153},
		{523,306},
		{392,153},
		{0,306},
		{415,306},
		{440,153},
		{698,306},
		{698,153},
		{440,612},
		{587,229},
		{880,229},
		{880,229},
		{880,229},
		{784,229},
		{698,229},
		{659,153},
		{523,306},
		{440,153},
		{392,612},
		{659,153},
		{523,306},
		{392,153},
		{0,306},
		{415,306},
		{440,153},
		{698,306},
		{698,153},
		{440,612},
		{494,153},
		{698,306},
		{698,153},
		{698,229},
		{659,229},
		{587,229},
		{523,153},
		{330,306},
		{330,153},
		{262,612},
		{523,459},
		{392,459},
		{330,306},
		{440,229},
		{494,229},
		{440,229},
		{415,229},
		{466,229},
		{415,229},
		{392,153},
		{294,153},
		{330,918},
		{0,0}
};
const Tone starwars[] = {
		{466,153},
		{466,153},
		{466,153},
		{698,612},
		{1047,612},
		{932,153},
		{880,153},
		{784,153},
		{1397,612},
		{1047,306},
		{932,153},
		{880,153},
		{784,153},
		{1397,612},
		{1047,306},
		{932,153},
		{880,153},
		{932,153},
		{784,612},
		{523,153},
		{523,153},
		{523,153},
		{698,612},
		{1047,612},
		{932,153},
		{880,153},
		{784,153},
		{1397,612},
		{1047,306},
		{932,153},
		{880,153},
		{784,153},
		{1397,612},
		{1047,306},
		{932,153},
		{880,153},
		{932,153},
		{784,612},
		{523,229},
		{523,76},
		{587,459},
		{587,153},
		{932,153},
		{880,153},
		{784,153},
		{698,153},
		{698,153},
		{784,153},
		{880,153},
		{784,306},
		{587,153},
		{659,306},
		{523,229},
		{523,76},
		{587,459},
		{587,153},
		{932,153},
		{880,153},
		{784,153},
		{698,153},
		{1047,229},
		{784,76},
		{784,612},
		{0,153},
		{523,153},
		{587,459},
		{587,153},
		{932,153},
		{880,153},
		{784,153},
		{698,153},
		{698,153},
		{784,153},
		{880,153},
		{784,306},
		{587,153},
		{659,306},
		{1047,229},
		{1047,76},
		{1397,306},
		{1245,153},
		{1109,306},
		{1047,153},
		{932,306},
		{831,153},
		{784,306},
		{698,153},
		{1047,1224},
		{0, 0}
		};

const Tone harrypotter[] = {
	{0,612},
	{294,306},
	{392,459},
	{466,153},
	{440,306},
	{392,612},
	{587,306},
	{523,918},
	{440,918},
	{392,459},
	{466,153},
	{440,306},
	{349,612},
	{415,306},
	{294,1836},
	{294,306},
	{392,459},
	{466,153},
	{440,306},
	{392,612},
	{587,306},
	{698,612},
	{659,306},
	{622,612},
	{494,306},
	{622,459},
	{587,153},
	{554,306},
	{277,612},
	{494,306},
	{392,1836},
	{466,306},
	{587,612},
	{466,306},
	{587,612},
	{466,306},
	{622,612},
	{587,306},
	{554,612},
	{440,306},
	{466,459},
	{587,153},
	{554,306},
	{277,612},
	{294,306},
	{587,1836},
	{0,306},
	{466,306},
	{587,612},
	{466,306},
	{587,612},
	{466,306},
	{698,612},
	{659,306},
	{622,612},
	{494,306},
	{622,459},
	{587,153},
	{554,306},
	{277,612},
	{466,306},
	{392,1836},
	{0, 0}
};



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void PWM_Start()
{
    HAL_TIM_PWM_Start(pwm_timer, pwm_channel);
}

void PWM_Change_Tone(uint16_t pwm_freq, uint16_t volume) // pwm_freq (1 - 20000), volume (0 - 1000)
{
    if (pwm_freq == 0 || pwm_freq > 20000)
    {
        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, 0);
    }
    else
    {
        const uint32_t internal_clock_freq = HAL_RCC_GetSysClockFreq();
        const uint16_t prescaler = 1 + internal_clock_freq / pwm_freq / 60000;
        const uint32_t timer_clock = internal_clock_freq / prescaler;
        const uint32_t period_cycles = timer_clock / pwm_freq;
        const uint32_t pulse_width = volume * period_cycles / 1000 / 2;

        pwm_timer->Instance->PSC = prescaler - 1;
        pwm_timer->Instance->ARR = period_cycles - 1;
        pwm_timer->Instance->EGR = TIM_EGR_UG;
        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, pulse_width); // pwm_timer->Instance->CCR2 = pulse_width;
    }
}

void Change_Melody(const Tone *melody, uint16_t tone_count)
{
    melody_ptr = melody;
    melody_tone_count = tone_count;
    current_tone_number = 0;
}

void Update_Melody()
{
    if ((HAL_GetTick() > current_tone_end) && (current_tone_number < melody_tone_count))
    {
        const Tone active_tone = *(melody_ptr + current_tone_number);
        PWM_Change_Tone(active_tone.frequency, volume);
        current_tone_end = HAL_GetTick() + active_tone.duration;
        current_tone_number++;
    }
}
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  if(!isPause){
	  Update_Melody();
  }


  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
  extern unsigned char data[1];
  switch(data[0]){
  case '1' :
	  PWM_Start();
	  Change_Melody(starwars, ARRAY_LENGTH(starwars));
	  break;
  case '2' :
  	  PWM_Start();
  	  Change_Melody(super_mario2, ARRAY_LENGTH(super_mario2));
  	  break;
  case '3' :
  	  PWM_Start();
  	  Change_Melody(harrypotter, ARRAY_LENGTH(harrypotter));
  	  break;
  case '-' :
	  volume = 5;
	  break;
  case '+' :
  	  volume = 50;
  	  break;
  case '0' :
  	  isPause = 1;
	  HAL_TIM_PWM_Stop(&htim2, pwm_channel);
	  break;
  case 'p' :
	  isPause = 0;
	  HAL_TIM_PWM_Start(&htim2, pwm_channel);
  	  break;
  case 'a' :
  	  PWM_Start();
      Change_Melody(a_note, ARRAY_LENGTH(a_note));
	  break;
  case 's' :
	  PWM_Start();
	  Change_Melody(s_note, ARRAY_LENGTH(s_note));
	  break;
  case 'd' :
	  PWM_Start();
	  Change_Melody(d_note, ARRAY_LENGTH(d_note));
	  break;
  case 'f' :
	  PWM_Start();
	  Change_Melody(f_note, ARRAY_LENGTH(f_note));
  	  break;
  case 'g' :
	  PWM_Start();
	  Change_Melody(g_note, ARRAY_LENGTH(g_note));
	  break;
  case 'h' :
	  PWM_Start();
	  Change_Melody(h_note, ARRAY_LENGTH(h_note));
	  break;
  case 'j' :
	  PWM_Start();
	  Change_Melody(j_note, ARRAY_LENGTH(j_note));
	  break;
  case 'k' :
	  PWM_Start();
	  Change_Melody(k_note, ARRAY_LENGTH(k_note));
	  break;
  case 'l' :
	  PWM_Start();
	  Change_Melody(l_note, ARRAY_LENGTH(l_note));
	  break;

  }
  HAL_UART_Receive_IT(&huart1, data, sizeof(data));
  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
