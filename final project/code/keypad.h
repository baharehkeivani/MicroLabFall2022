/*
 * keypad.h
 *
 *  Created on: Jan 17, 2023
 *      Author: Shiman
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>

void KeypadSetUp(void);
uint8_t pressedButton(uint16_t GPIO_Pin);

#endif /* KEYPAD_H_ */
