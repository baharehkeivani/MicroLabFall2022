/*
 * LCD.h
 *
 *  Created on: Jan 17, 2023
 *      Author: Shiman
 */

#ifndef LCD_H_
#define LCD_H_

typedef unsigned char byte;

void LCDSetUp(void);
void showInitPage(void);
void clearInitPage(void);
void showMenuPage(void);
void clearMenuPage(void);
void showAboutPage(void);
void clearAboutPage(void);
void showInsertNamePage(void);
void clearInsertNamePage(void);
void showChooseDifficultyPage(void);
void clearChooseDifficultyPage(void);
void showGamePage(void);
void createCustoms(void);
void showTime(void);
void chooseDifficulty1(void);
void chooseDifficulty2(void);
void chooseDifficulty3(void);
void showCostumChar(int col, int row, int specChar);
void clearAlien(int col, int row);
void showBullet(int col, int row);
void clearBullet(int col, int row);
void cleargamePage(void);
void showYouWonPage(void);
void showYouLostPage(void);
void clearLCD(void);
void showHeart(void);
void showLives(void);
void showPoints(void);

#endif /* LCD_H_ */
