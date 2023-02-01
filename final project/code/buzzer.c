/*
 * buzzer.c
 *
 *  Created on: Jan 17, 2023
 *      Author: Shiman
 */

#include "stm32f3xx_hal.h"
#include "buzzer.h"
#include "main.h"

extern TIM_HandleTypeDef htim4;

TIM_HandleTypeDef *pwm_timer = &htim4;
uint32_t pwm_channel = TIM_CHANNEL_4;
volatile uint32_t current_tone_end;
volatile uint16_t melody_tone_count;
volatile uint16_t current_tone_number;
volatile uint16_t volume = 50;
const Tone *volatile melody_ptr;
int musicOff = 0;
const Tone song[100]={
{659,306.0},
{494,153.0},
{523,153.0},
{587,306.0},
{523,153.0},
{494,153.0},
{440,306.0},
{440,153.0},
{523,153.0},
{659,306.0},
{587,153.0},
{523,153.0},
{494,459.0},
{523,153.0},
{587,306.0},
{659,306.0},
{523,306.0},
{440,306.0},
{440,153.0},
{440,306.0},
{494,153.0},
{523,153.0},
{587,459.0},
{698,153.0},
{880,306.0},
{784,153.0},
{698,153.0},
{659,459.0},
{523,153.0},
{659,306.0},
{587,153.0},
{523,153.0},
{494,306.0},
{494,153.0},
{523,153.0},
{587,306.0},
{659,306.0},
{523,306.0},
{440,306.0},
{440,306.0},
{0,306.0},
{659,306.0},
{494,153.0},
{523,153.0},
{587,306.0},
{523,153.0},
{494,153.0},
{440,306.0},
{440,153.0},
{523,153.0},
{659,306.0},
{587,153.0},
{523,153.0},
{494,459.0},
{523,153.0},
{587,306.0},
{659,306.0},
{523,306.0},
{440,306.0},
{440,153.0},
{440,306.0},
{494,153.0},
{523,153.0},
{587,459.0},
{698,153.0},
{880,306.0},
{784,153.0},
{698,153.0},
{659,459.0},
{523,153.0},
{659,306.0},
{587,153.0},
{523,153.0},
{494,306.0},
{494,153.0},
{523,153.0},
{587,306.0},
{659,306.0},
{523,306.0},
{440,306.0},
{440,306.0},
{0,306.0},
{659,612.0},
{523,612.0},
{587,612.0},
{494,612.0},
{523,612.0},
{440,612.0},
{415,612.0},
{494,306.0},
{0,153.0},
{659,612.0},
{523,612.0},
{587,612.0},
{494,612.0},
{523,306.0},
{659,306.0},
{880,612.0},
{831,612.0},
};
const Tone alien_Shot[3]={
{494,153.0},
{587,306.0},
{0,153.0},
};
const Tone shooter_Shot[3]={
{587,153.0},
{494,306.0},
{0,153.0},
};

void PWM_Start()
{
    HAL_TIM_PWM_Start(pwm_timer, pwm_channel);
}
void PWM_Stop(){
	HAL_TIM_PWM_Stop(pwm_timer, pwm_channel);
	musicOff = 1;
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
    }else if (current_tone_number >= melody_tone_count) {
    	musicOff = 1;
    }
}
void playerShotSound(){
	PWM_Start();
	  Change_Melody(shooter_Shot, ARRAY_LENGTH(shooter_Shot));
}
void alienShotSound(){
	PWM_Start();
	  Change_Melody(alien_Shot, ARRAY_LENGTH(alien_Shot));
}
