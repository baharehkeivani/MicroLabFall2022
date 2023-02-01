/*
 * buzzer.h
 *
 *  Created on: Jan 17, 2023
 *      Author: Shiman
 */

#ifndef BUZZER_H_
#define BUZZER_H_

typedef unsigned char byte;
typedef struct {
    uint16_t frequency;
    uint16_t duration;
} Tone;

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

void PWM_Start(void);
void PWM_Stop(void);
void PWM_Change_Tone(uint16_t pwm_freq, uint16_t volume);
void Change_Melody(const Tone *melody, uint16_t tone_count);
void Update_Melody(void);
void playerShotSound(void);
void alienShotSound(void);

#endif /* BUZZER_H_ */
