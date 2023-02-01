#include "stm32f3xx_hal.h"

void initForLeds(){
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_InitTypeDef temp;
	temp.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	temp.Mode = GPIO_MODE_OUTPUT_PP;
	temp.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE,&temp);
}
void initForExternalLeds(){
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef temp;
	temp.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	temp.Mode = GPIO_MODE_OUTPUT_PP;
	temp.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD,&temp);
}
void initForBlueButton(){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef temp1;
	temp1.Pin = GPIO_PIN_0;
	temp1.Mode = GPIO_MODE_INPUT;
	temp1.Speed = GPIO_SPEED_FREQ_HIGH;
	temp1.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA,&temp1);
}
void initForSevenSeg(){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef temp;
	temp.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	temp.Mode = GPIO_MODE_OUTPUT_PP;
	temp.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &temp);
}
void increamentSevenSeg(int state){
	if(state == 0){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,0);
	}
	else if(state == 1){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,0);
	}
	else if(state == 2){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,1);
	}
	else if(state == 3){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,0);
	}
	else if(state == 4) {
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,1);
	}
	else if(state == 5){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,1);
	}
	else if(state == 6){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,1);
	}
	else if(state == 7){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,0);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,0);
	}
	else if(state == 8){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,1);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,1);
	}
	else if(state == 9){
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,0);
	}
}
void clockwiseInternalLed(){
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,0);
}

int main(){
	HAL_Init();
	initForLeds();
	initForBlueButton();
	initForSevenSeg();
	initForExternalLeds();

	int state = 0;
	int LEDState = 0;

	increamentSevenSeg(state);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,1);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,0);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,1);


	while(1){
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)){
			if(LEDState == 0){
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,0);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,1);
				LEDState = 1;
			}else {
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,1);
				HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,0);
				LEDState = 0;
			}

			state += 1;
			increamentSevenSeg(state);
			if(state == 10){
				clockwiseInternalLed();
				state = 0;
			}
			increamentSevenSeg(state);
			while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0));
		}
	}

}


















