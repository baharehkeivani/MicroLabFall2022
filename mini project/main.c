/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    uint16_t frequency;
    uint16_t duration;
} Tone;
typedef struct
{
    int *array;        // queue container
    int front;         // index of first element
    int rear;          // index of last element
    int size;          // number of elements currently in queue
    unsigned capacity; // max number of elements we can add to queue
} Queue;
typedef struct{
    const unsigned char *errstr;
    int err;
}errval;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
#define INVALID 1
#define TOOSMALL 2
#define TOOLARGE 3
#define OUT_OF_RANGE 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
errval error_list[5] = {
	{NULL, 0},
	{"invalid", INVALID},
	{"too small", TOOSMALL},
	{"too large", TOOLARGE},
	{"out of range", OUT_OF_RANGE},
};

//strings related to admin commands
const unsigned char loginAdmin[] = "ADMIN#";
const unsigned char setMax[] = "SET MAX LEVEL ";
const unsigned char setLevel[] = "SET LEVEL ";
const unsigned char setWait[] = "SET WAIT ";
const unsigned char setLedON[] = "SET LED ON";
const unsigned char setLedOFF[] = "SET LED OFF";
const unsigned char test[] = "TEST#";
const unsigned char start[] = "START";
const unsigned char adminPass[] = "0324";
//buffering and UART related
unsigned char oneByte[1];
unsigned char dataBuff[20];
int maxDataIndx = 20;
int dataIndx = 0;
//elevator state related
int currentFloor = 0;
int maxFloor = 9;
int chosenFloor = 0;
int nextFloor;
int moving = 0; 
int waiting = 0;
int direction = 1;
int prevDirection = 1;
int waitTime = 60;
int waitCounter = 0;
int moveCounter = 0;
int alarmOn = 0;
int alarmCounter = 0;
int alarmState = 0;
int ledsOn = 1;
int adminMode = 0;
Queue* upQ;
Queue* downQ;
RTC_TimeTypeDef elevatorTime;
RTC_DateTypeDef elevatorDate;
//7segment related
int chooseSegment;
//button related
uint32_t last_btn_interrupt = 0;
//buzzer related
TIM_HandleTypeDef *pwm_timer = &htim4;
uint32_t pwm_channel = TIM_CHANNEL_4;
volatile uint32_t current_tone_end;
volatile uint16_t melody_tone_count;
volatile uint16_t current_tone_number;
volatile uint16_t volume = 50;
const Tone *volatile melody_ptr;
const Tone elevator_song[]= {
{0,1224},
{0,1224},
{262,306},
{330,306},
{392,306},
{330,306},
{262,306},
{330,153},
{392,459},
{330,306},
{220,306},
{262,306},
{330,306},
{262,306},
{220,306},
{262,153},
{330,459},
{262,306},
{196,306},
{247,306},
{294,306},
{247,306},
{196,306},
{247,153},
{294,459},
{247,306},
{196,306},
{196,153},
{196,459},
{196,153},
{196,306},
{196,306},
{196,306},
{196,153},
{196,306},
{262,306},
{330,306},
{392,306},
{330,306},
{262,306},
{330,153},
{392,459},
{330,306},
{220,306},
{262,306},
{330,306},
{262,306},
{220,306},
{262,153},
{330,459},
{262,306},
{196,306},
{247,306},
{294,306},
{247,306},
{196,306},
{247,153},
{294,459},
{247,306},
{196,1836},
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*------------------ Queue related functions ------------------*/
Queue* createQueue(unsigned capacity)
{
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	queue->capacity = capacity;
	queue->front = queue->size = 0;

	queue->rear = capacity - 1;
	queue->array = (int*)malloc(queue->capacity * sizeof(int));
	return queue;
}
int isFull(Queue* queue)
{
	return (queue->size == queue->capacity);
}
int isEmpty(Queue* queue)
{
	return (queue->size == 0);
}
int firstToPush(Queue* queue) //if a new floor is pushed in the queue, will call move
{
	return (queue->size == 1);
}
int rear(Queue* queue)
{
	if (isEmpty(queue))
		return -1;
	return queue->array[queue->rear];
}
int isInQueue(Queue* queue, int item)
{
	if (item == currentFloor){
		unsigned char err[] = "Current floor entered \n";
		HAL_UART_Transmit(&huart1,err,sizeof(err) ,1000);
		return 1;
	}
	if (isEmpty(queue)){
		return 0;
	}
	//check in the queue for duplicate floor
	for (int i = queue->front; ; i = (i + 1)% queue->capacity){
		if (queue->array[i] == item){
			unsigned char err[] = "we are going there don't rush \n";
			HAL_UART_Transmit(&huart1,err,sizeof(err) ,1000);
			return 1;
		}
		if(queue->array[i] == rear(queue))
			break;
	}

	return 0;
}
void enqueue(int item){
	if (item > currentFloor) {
		enqueue_to_queue(upQ, item);
	} else {
		enqueue_to_queue(downQ, item);
	}
}
void sortAssending(Queue* queue){
	for (int i = queue->rear; ; i = (i - 1)% queue->capacity){
		int key = queue->array[i];

		if(i == queue->front)
			break;
		if((i-1)% queue->capacity == queue->front && moving)
			break;
		if (key < queue->array[(i-1)% queue->capacity]){
			queue->array[i] = queue->array[(i-1)% queue->capacity];
			queue->array[(i-1)% queue->capacity] = key;
		}
		else {
			break;
		}
	}
}
void sortDescending(Queue* queue){
	for (int i = queue->rear; ; i = (i - 1)% queue->capacity){
		int key = queue->array[i];

		if(i == queue->front)
						break;
		if(queue->array[(i-1)% queue->capacity] == front(queue) && moving)
			break;
		if (key > queue->array[(i-1)% queue->capacity]){
			queue->array[i] = queue->array[(i-1)% queue->capacity];
			queue->array[(i-1)% queue->capacity] = key;
		}
		else {
			break;
		}
	}
}
void enqueue_to_queue(Queue* queue, int item)
{
	if (isFull(queue) || isInQueue(queue, item))
		return;

	queue->rear = (queue->rear + 1) % queue->capacity;
	queue->array[queue->rear] = item;
	queue->size = queue->size + 1;

	if(item > currentFloor){
		sortAssending(queue);
	}
	if(item < currentFloor){
		sortDescending(queue);
	}

	//transmitting log
	unsigned char data[100]="";
	int n=sprintf((char *)data,"%d is added to queue \nQueue: ", item);
	transmitLog(data);
}
int dequeue(Queue* queue)
{
	if (isEmpty(queue))
		return -1;
	int item = queue->array[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return item;
}
int front(Queue* queue)
{
	if (isEmpty(queue))
		return -1;
	return queue->array[queue->front];
}
void empty(Queue* queue)
{
	while (!isEmpty(queue))
		dequeue(queue);
}
/*------------------ Buzzer related functions ------------------*/
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
/*------------------ 7segment related functions ------------------*/
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
/*------------------ UART related functions ------------------*/
void clearDataBuff(){ //called when a command is processed to clear buffer for new command
	for(int i = 0 ; i < dataIndx; i++){
		dataBuff[i] = 0;
	}
	dataIndx = 0;
}
//this function handles LOGIN and TEST commands
void handleTestNumberStr(const char *command_str, long long minval, long long maxval)
{
    int m = strlen(dataBuff);
    int n = strlen(command_str);
    int error = 0;

    if(command_str == loginAdmin){
    	if (m - n != 4)
		{
			error = INVALID;
			unsigned char data[100]="";
			int n=sprintf((char *)data,"The sequence you entered is invalid, password should be %d digits \n", 4);
			HAL_UART_Transmit(&huart1,data,n,1000);
			return;
		}
    	int passIndex = 0;
		for (int i = n; i < m ; i++)
		{
			if (isdigit(dataBuff[i]))
			{
				if (dataBuff[i] > minval && dataBuff[i] < maxval)
					error = OUT_OF_RANGE;
				if (adminPass[passIndex] == dataBuff[i]){
					passIndex++;
				} else {
					unsigned char data[100]="";
					int n=sprintf((char *)data,"Incorrect Password \n");
					HAL_UART_Transmit(&huart1,data,n,1000);
					return;
				}
			}
			else {
				unsigned char data[100]="";
				int n=sprintf((char *)data,"The sequence you entered is invalid, should be all digits \n");
				HAL_UART_Transmit(&huart1,data,n,1000);
				return;
			}
		}
		if(passIndex == 4){
			unsigned char data[100]="";
			int n=sprintf((char *)data,"Successful logged in \n");
			HAL_UART_Transmit(&huart1,data,n,1000);
			empty(upQ);
			empty(downQ);
			adminMode = 1;
		}
    }
    else {
		if (m - n > 5 || m - n < 1)
		{
			error = INVALID;
			unsigned char data[100]="";
			int n=sprintf((char *)data,"The sequence you entered is invalid, should be between 0 to %d digits \n", 5);
			HAL_UART_Transmit(&huart1,data,n,1000);
			return;
		}
		for (int i = n; i < m ; i++)
		{
			if (isdigit(dataBuff[i]))
			{
				if (dataBuff[i] > minval && dataBuff[i] < maxval)
					error = OUT_OF_RANGE;
				enqueue(dataBuff[i] - '0');
			}
			else {
				unsigned char data[100]="";
				int n=sprintf((char *)data,"The sequence you entered is invalid, should be all digits \n");
				HAL_UART_Transmit(&huart1,data,n,1000);
			}

		}
    }
}
//this function handles OTHER commands
int strToNumber(const char *command_str, long long minval, long long maxval)
{
    int number = 0;
    int error = 0;
    if (minval > maxval){
        error = INVALID;
        return -1;
    }

    else
	{
		int m = strlen(dataBuff);
		int n = strlen(command_str);

		for (int i = m - 1, j = 1; i >= n; i--)
		{
			if (isdigit(dataBuff[i]))
			{
				number += j * (dataBuff[i] - '0');
				j *= 10;
			}
			else if (dataBuff[i] != ' ')
			{
				error = INVALID;
				return -1;
			}
		}

		if (number < minval)
			error = TOOSMALL;
		else if (number > maxval)
			error = TOOLARGE;
	}
    if (error)
    {
    	unsigned char data[100]="";
		int n=sprintf((char *)data,"the value you entered is %s \n",error_list[error].errstr);
		HAL_UART_Transmit(&huart1,data,n,1000);
        return -1;
    }
    return number;
}
void transmitQueueContents(Queue* queue){
	if(!isEmpty(queue)){
		int queueItem = 0;
		for (int i = queue->front; ; i = (i + 1)% queue->capacity){
			queueItem = queue->array[i];
			unsigned char data[1]="";
			int n=sprintf((char *)data,"%d", queueItem);
			HAL_UART_Transmit(&huart1,data,n,1000);
			if(i == queue->rear){
				break;
			}
		}
	}
}
void transmitLog(unsigned char* log_str){
	HAL_RTC_GetTime(&hrtc, &elevatorTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &elevatorDate, RTC_FORMAT_BIN);

	char timeStr[20];
	char dateStr[20];
	sprintf(timeStr, "%2d:%2d:%2d", elevatorTime.Hours, elevatorTime.Minutes, elevatorTime.Seconds);
	sprintf(dateStr, "%d/%d/%d", elevatorDate.Year, elevatorDate.Month, elevatorDate.Date);
	char currentTime[40];
	int sSize=sprintf((char *)currentTime,"%s %s \n", dateStr, timeStr);
	HAL_UART_Transmit(&huart1,currentTime,sSize,1000);


	unsigned char data[150]="";
	int n=sprintf((char *)data,"%s", log_str);
	HAL_UART_Transmit(&huart1,data,n,1000);

	if(direction > 0){
		transmitQueueContents(upQ);
		transmitQueueContents(downQ);
	}
	else{
		transmitQueueContents(downQ);
		transmitQueueContents(upQ);
	}

	unsigned char newline[1]="";
	int size=sprintf((char *)newline," dir: %d\n", direction);
	HAL_UART_Transmit(&huart1,newline,size,1000);
}
/*------------------ Elevator related functions ------------------*/
void move(){
    int tempUp = front(upQ);
    int tempDown = front(downQ);

	if(direction > 0 && tempUp != -1){
		nextFloor = tempUp;
	} else if (direction > 0 && tempUp == -1){ //upQ is empty now
		if(tempDown != -1){
			nextFloor = tempDown;
		}
		else {
			nextFloor = currentFloor;
		}
	}
	if(direction < 0 && tempDown != -1){
		nextFloor = tempDown;
	} else if (direction < 0 && tempDown == -1){ //downQ is empty now
		if(tempUp != -1){
			nextFloor = tempUp;
		}
		else {
			nextFloor = currentFloor;
		}
	} if (direction == 0){
		if (tempDown != -1){
			nextFloor = tempDown;
		}
		else if (tempUp != -1){
			nextFloor = tempUp;
		}
	}


	if(currentFloor - nextFloor > 0){
		direction = -1;
		prevDirection = -1;
	}
	else if (currentFloor - nextFloor < 0){
		direction = 1;
		prevDirection = 1;
	} else {
		direction = 0;
		waiting = 0;
		moving = 0;
	}
	if(direction != 0){
		PWM_Start();
		Change_Melody(elevator_song, ARRAY_LENGTH(elevator_song));
		moving = 1;
	}
}

void handleAdminCommands(){
	int isAdminLogin = memcmp(dataBuff, loginAdmin, sizeof(loginAdmin)/sizeof(unsigned char) - 1 ) == 0;
	int isSetMax = memcmp(dataBuff, setMax, sizeof(setMax)/sizeof(unsigned char) - 1 ) == 0;
	int isSetLevel = memcmp(dataBuff, setLevel, sizeof(setLevel)/sizeof(unsigned char) - 1 ) == 0;
	int isSetWait = memcmp(dataBuff, setWait, sizeof(setWait)/sizeof(unsigned char) - 1 ) == 0;
	int isSetLedON = memcmp(dataBuff, setLedON, sizeof(setLedON)/sizeof(unsigned char) - 1 ) == 0;
	int isSetLedOFF = memcmp(dataBuff, setLedOFF, sizeof(setLedOFF)/sizeof(unsigned char) - 1 ) == 0;
	int isTest = memcmp(dataBuff, test, sizeof(test)/sizeof(unsigned char) - 1 ) == 0;
	int isStart = memcmp(dataBuff, start, sizeof(start)/sizeof(unsigned char) - 1 ) == 0;

	if (isAdminLogin) {
		handleTestNumberStr(loginAdmin, 0, 9);
		return;
	}
	else if (!adminMode){
		return;
	}
	if (isSetMax){
		empty(downQ);
		empty(upQ);
		int tmp = strToNumber(setMax, 0, 9);
		if (tmp != -1)
		{
			maxFloor = tmp;
			if (chosenFloor > maxFloor){
				chosenFloor = maxFloor;
			}
			unsigned char data[100]="";
			int n=sprintf((char *)data,"MAX floor is set to %d \n",tmp);
			HAL_UART_Transmit(&huart1,data,n,1000);
		}
		else {
			unsigned char data[100]="";
			int n=sprintf((char *)data,"The level you entered is invalid, can not be less than %d more then %d \n", 0, 9);
			HAL_UART_Transmit(&huart1,data,n,1000);
		}
		currentFloor = 0;
	}
	else if (isSetLevel){
		empty(downQ);
		empty(upQ);
		int tmp = strToNumber(setLevel, 0, maxFloor);
		if (tmp != -1){
			currentFloor = tmp;
			unsigned char data[100]="";
			int n=sprintf((char *)data,"Current floor is set to %d \n",tmp);
			HAL_UART_Transmit(&huart1,data,n,1000);
		}else{
			unsigned char data[100]="";
			int n=sprintf((char *)data,"The level you entered is invalid, can not be less than %d more then %d \n", 0, maxFloor);
			HAL_UART_Transmit(&huart1,data,n,1000);
		}
	}
	else if (isSetWait){
		int tmp = strToNumber(setWait, 500, 5000);
		if (tmp % 100 == 0) //in this case we will set time
		{
			waitTime = tmp / 50;
			unsigned char data[100]="";
			int n=sprintf((char *)data,"Wait time is set to %d milliseconds\n",tmp);
			HAL_UART_Transmit(&huart1,data,n,1000);
		} else{
			unsigned char data[100]="";
			int n=sprintf((char *)data,"The time you entered is invalid, should be modulus of %d \n", 100);
			HAL_UART_Transmit(&huart1,data,n,1000);
		}
		if (tmp == -1){
			unsigned char data[100]="";
			int n=sprintf((char *)data,"The time you entered is invalid, can not be less than %d more then %d \n", 500, 5000);
			HAL_UART_Transmit(&huart1,data,n,1000);
		}
	}
	else if (isSetLedON){
		ledsOn = 1;
		unsigned char data[100]="";
		int n=sprintf((char *)data,"LEDs are on now \n");
		HAL_UART_Transmit(&huart1,data,n,1000);
	}
	else if (isSetLedOFF){
		ledsOn = 0;
		unsigned char data[100]="";
		int n=sprintf((char *)data,"LEDs are off now \n");
		HAL_UART_Transmit(&huart1,data,n,1000);
	}
	else if (isTest){
		handleTestNumberStr(test, 0, maxFloor);
	}
	else if(isStart){
		adminMode = 0;
		unsigned char data[100]="";
		int n=sprintf((char *)data,"logged out \n");
		HAL_UART_Transmit(&huart1,data,n,1000);

		//transmitting Admin setting state
		unsigned char stateData[150]="";
		sprintf((char *)stateData,"MAX floor is set to %d \nCURRENT floor is set to %d\nWait time is set to %d milliseconds\nAlarm LEDs are %d\nQueue: ", maxFloor, currentFloor, waitTime*50, alarmOn);
		transmitLog(stateData);
		move();
	}
	else{
	  unsigned char UndefinedCommand[] = "Undefined Command";
	  HAL_UART_Transmit(&huart1,UndefinedCommand,sizeof(UndefinedCommand) ,1000);
	}
}
/*------------------ Callback related functions ------------------*/
void HAL_SYSTICK_Callback(){
	  if(moving){
		  Update_Melody();
	  }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart -> Instance == USART1)
    {
    	if(oneByte[0] != 0xa){
    		dataBuff[dataIndx] = oneByte[0];
    		dataBuff[dataIndx+1] = '\0';
    		dataIndx++;
		}else if(oneByte[0] == 0xa){
			handleAdminCommands();
			clearDataBuff();
		}
		HAL_UART_Receive_IT(&huart1, oneByte, sizeof(oneByte));
    }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(adminMode){
		return;
	}
	if(GPIO_Pin == GPIO_PIN_10 && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10)){ //decrease
		  if(chosenFloor > 0){
			  chosenFloor--;
		  }
	}
	if(GPIO_Pin == GPIO_PIN_11 && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11)){ //increase
		if(chosenFloor < maxFloor){
		  chosenFloor++;
		}
	}
	if(GPIO_Pin == GPIO_PIN_12 && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12)){ //submit
		enqueue(chosenFloor);
		if(!moving){
			move();
		}
	}
	if(GPIO_Pin == GPIO_PIN_13 && HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13)){ //alarm
		  if(alarmOn){
			  alarmOn = 0;
			  PWM_Change_Tone(0, volume);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 0);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 0);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, 0);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 0);
			  unsigned char data[100]="";
			  int n=sprintf((char *)data,"Elevator Alarm is OFF\nQueue: ", currentFloor);
			  transmitLog(data);
		  }
		  else {
			  unsigned char data[100]="";
			  int n=sprintf((char *)data,"Elevator Alarm is ON\nQueue: ", currentFloor);
			  transmitLog(data);
			  PWM_Start();
			  alarmOn = 1;
		  }
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if (htim->Instance == TIM2){ //for seven segment
	  if(chooseSegment == 0){
	  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, 0);
	  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, 1);
	  	  numberToBCD(chosenFloor);
	  	  chooseSegment=1;
	    }
	    else if(chooseSegment == 1){
	  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, 0);
	  	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, 1);
	  	  numberToBCD(currentFloor);
	  	  chooseSegment=0;
	    }
  }
  if (htim->Instance == TIM3){ //for other purposes (elevator move and alarm)
	  if(adminMode){
	  		return;
	  }
	  if(moving){
		  moveCounter++;
		  if(moveCounter == 20){
			  currentFloor = currentFloor + direction;
			  if(currentFloor == nextFloor){
				  moving = 0;
				  waiting = 1;
				  PWM_Change_Tone(0, volume);
				  HAL_TIM_PWM_Stop(pwm_timer, pwm_channel);
				  if(direction > 0){
					  dequeue(upQ);
				  }if(direction < 0){
					  dequeue(downQ);
				  }
				  unsigned char data[100]="";
				  int n=sprintf((char *)data,"reached level %d \nQueue: ", currentFloor);
				  transmitLog(data);

			  }
			  moveCounter = 0;
		  }
	  }
	  else if(waiting){
		  waitCounter++;
		  if(waitCounter == waitTime){
			  waiting = 0;
			  waitCounter = 0;
			  move();
		  }
	  }
	  if(alarmOn){
		  alarmCounter++;
		  if(alarmCounter == 5){
			  if(ledsOn){
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_11);
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_14);
				  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);
			  }

			  if(alarmState){
				  PWM_Change_Tone(4000, volume);
				  alarmState = 0;
			  }
			  else{
				  PWM_Change_Tone(0, volume);
				  alarmState = 1;
			  }
			  alarmCounter = 0;
		  }
	  }
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_UART_Receive_IT(&huart1, oneByte, sizeof(oneByte));
  PWM_Start();



  elevatorTime.Hours = 9;
  elevatorTime.Minutes = 29;
  elevatorTime.Seconds = 0;
  HAL_RTC_SetTime(&hrtc, &elevatorTime, RTC_FORMAT_BIN);
  elevatorDate.Date = 15;
  elevatorDate.Month = 12;
  elevatorDate.Year = 22;
  HAL_RTC_SetDate(&hrtc, &elevatorDate, RTC_FORMAT_BIN);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  upQ = createQueue(10);
  downQ = createQueue(10);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 39;
  hrtc.Init.SynchPrediv = 999;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 239;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 299;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1199;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 5999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LD4_Pin|LD3_Pin|LD5_Pin|LD7_Pin
                          |LD9_Pin|LD10_Pin|LD8_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD7_Pin
                           LD9_Pin LD10_Pin LD8_Pin LD6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD7_Pin
                          |LD9_Pin|LD10_Pin|LD8_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PD10 PD11 PD12 PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
