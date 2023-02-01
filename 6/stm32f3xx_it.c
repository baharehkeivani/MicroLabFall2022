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

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t light = 0 ;
uint8_t temprature = 0;
int temp1;
int temp2;
int temp3;
int temp4;
int sum;
int chooseSegment;
extern TIM_HandleTypeDef htim4;
TIM_HandleTypeDef *pwm_timer = &htim4;	// Point to PWM Timer configured in CubeMX
uint32_t pwm_channel = TIM_CHANNEL_2;   // Select configured PWM channel number
int alarmOff = 0;
uint32_t lastButtonPressed = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SevenSegment_Update(uint8_t number){
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, ((number>>0)&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, ((number>>1)&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, ((number>>2)&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, ((number>>3)&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, ((number>>4)&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, ((number>>5)&0x01));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, ((number>>6)&0x01));
}
void numberToBCD(int i){
	int x1 = i&1;
	int x2=i&2;
	int x3=i&4;
	int x4=i&8;
	if(x1>0) x1=1;
	if(x2>0) x2=1;
	if(x3>0) x3=1;
	if(x4>0) x4=1;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, x4);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, x3);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, x2);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, x1);

}
void updateTemps(){
	temp3=light/10;
	temp4=light%10;

	temp1=temprature/10;
	temp2=temprature%10;

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
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
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

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles ADC1 and ADC2 interrupts.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  HAL_ADC_IRQHandler(&hadc2);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

  /* USER CODE END ADC1_2_IRQn 1 */
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
  updateTemps();
  if(chooseSegment == 0){
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10| GPIO_PIN_11, 0);
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_10, 1);
  	  numberToBCD(temp4);
  	  chooseSegment=1;
    }
    else if(chooseSegment == 1){
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, 0);
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_11, 1);
  	  numberToBCD(temp3);
  	  chooseSegment=2;
    }
    else if(chooseSegment == 2){
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, 0);
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8 |GPIO_PIN_10 | GPIO_PIN_11, 1);
  	  numberToBCD(temp2);
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
  	  chooseSegment=3;
    }
    else if(chooseSegment == 3){
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, 0);
  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9 |GPIO_PIN_10 | GPIO_PIN_11, 1);
  	  numberToBCD(temp1);
  	  chooseSegment=0;
    }
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */
    //light A0
	int x=HAL_ADC_GetValue(&hadc1);
	float fx=((float)x*100/1500);
	light = (uint8_t)fx;
	HAL_ADC_Start_IT(&hadc1);
    //temp A4
	int y=HAL_ADC_GetValue(&hadc2);
	float fy=(y * 3300 / 4095)/10;
	temprature = (uint8_t)fy;
	HAL_ADC_Start_IT(&hadc2);

	if(temprature >= 25 && !alarmOff){
		   PWM_Change_Tone(5000, 50);
	}
	else if (temprature < 25){
		alarmOff = 0;
		PWM_Change_Tone(0, 50);
	}
  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
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

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  if(HAL_GetTick() > lastButtonPressed + 200){
	  lastButtonPressed = HAL_GetTick();
	  if(!alarmOff){
	  	  alarmOff = 1;
	  	  PWM_Change_Tone(0, 50);
	    }
	    else {
	  	  alarmOff = 0;
	    }
  }
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
