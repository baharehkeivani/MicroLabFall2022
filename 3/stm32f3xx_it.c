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
	  	temp1 = x/10; //1 - 1st digit
	  	temp2 = x%10; //2 - 2nd digit
	  	sum = temp1 + temp2; // sum
	  	temp3 = sum/10; //3 - sum 1st digit
	  	temp4 = sum%10; //4 - sum 2nd digit
}
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
/* USER CODE BEGIN EV */
extern uint8_t x;
extern int temp1;
extern int temp2;
extern int temp3;
extern int temp4;
extern int sum;
int chooseSegment;
uint32_t pressedTime = 1;
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
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
	  x++;
	  if(x>99){
		  x=0;
	  }
	  updateTemps();
	  pressedTime = HAL_GetTick();
  }
  else{
	  uint32_t unpressedTime = HAL_GetTick();
	  if(unpressedTime-pressedTime > 500){
		  x=0;
		  updateTemps();
	  }
  }


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

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
