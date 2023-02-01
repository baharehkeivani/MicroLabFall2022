/*
 * LCD.c
 *
 *  Created on: Jan 17, 2023
 *      Author: Shiman
 */

#include "LCD.h"
#include "LiquidCrystal.h"
#include "game.h"
#include "main.h"

extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef currTime;
extern RTC_DateTypeDef currDate;
extern Game game;
extern Player player;
extern char timeStr[20];
extern char dateStr[20];
extern char currentTime[40];
extern char username[21];

byte alien1[8] = {
  0x0F,
  0x17,
  0x0D,
  0x15,
  0x0F,
  0x15,
  0x0D,
  0x17
};
byte alien2[8] = {
  0x00,
  0x17,
  0x0D,
  0x0D,
  0x0F,
  0x0D,
  0x0D,
  0x17
};
byte spaceShooter[8] = {
  0x10,
  0x18,
  0x1A,
  0x1F,
  0x1F,
  0x1A,
  0x18,
  0x10
};
byte oneInvert[8] = {
	0x1B,
	0x13,
	0x1B,
	0x1B,
	0x1B,
	0x1B,
	0x1B,
	0x11
};
byte twoInvert[8] = {
	0x11,
	0x0E,
	0x0E,
	0x1D,
	0x1B,
	0x17,
	0x00,
	0x1F
};
byte threeInvert[8] = {
  0x1F,
  0x00,
  0x1D,
  0x1B,
  0x1D,
  0x1E,
  0x0E,
  0x11
};
byte enemyBullet[8] = {
  0x00,
  0x04,
  0x08,
  0x1F,
  0x08,
  0x04,
  0x00,
  0x00
};

byte playerBullet[8] = {
  0x00,
  0x04,
  0x02,
  0x1F,
  0x02,
  0x04,
  0x00,
  0x00
};
byte heart[8] = {
  0x00,
  0x0A,
  0x1F,
  0x1F,
  0x0E,
  0x04,
  0x00,
  0x00
};

GPIO_TypeDef* const LCD_port = GPIOD;
const uint16_t LCD_pins[] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14};

void LCDSetUp(){
	LiquidCrystal(LCD_port, LCD_pins[0], LCD_pins[1], LCD_pins[2], LCD_pins[3], LCD_pins[4], LCD_pins[5], LCD_pins[6]);
	begin(20, 4);
	createCustoms();
}
void createCustoms(){
	createChar(0, alien1);
	createChar(1, alien2);
	createChar(2, spaceShooter);
	createChar(3, oneInvert);
	createChar(4, twoInvert);
	createChar(5, threeInvert);
	createChar(6, enemyBullet);
	createChar(7, playerBullet);
}
void showInitPage(){
	setCursor(0, 2);
	write(2);

	setCursor(2, 0);
	print("Space Invaders");

	setCursor(9, 2);
	print("-");

	for(int i = 0 ; i < 4; i++){
		setCursor(19, i);
		write(1);
	}
	for(int i = 0 ; i < 4; i++){
		setCursor(18, i);
		write(0);
	}
	for(int i = 0 ; i < 4; i++){
		setCursor(17, i);
		write(1);
	}
	setCursor(16, 2);
	write(0);
	setCursor(15, 2);
	write(1);
}
void clearInitPage(){
	setCursor(0, 2);
	print(" ");

	setCursor(2, 0);
	print("              ");

	setCursor(9, 2);
	print(" ");

	for(int i = 0 ; i < 4; i++){
		setCursor(19, i);
		print(" ");
	}
	for(int i = 0 ; i < 4; i++){
		setCursor(18, i);
		print(" ");
	}
	for(int i = 0 ; i < 4; i++){
		setCursor(17, i);
		print(" ");
	}
	setCursor(16, 2);
	print(" ");
	setCursor(15, 2);
	print(" ");
}
void showMenuPage(){
	setCursor(0, 0);
	print("menu");
	setCursor(0, 1);
	print("--------------------");
	setCursor(0, 2);
	print("1. start game");

	setCursor(0, 3);
	print("2. about");
}
void clearMenuPage(){
	setCursor(0, 0);
	print("    ");
	setCursor(0, 1);
	print("                    ");
	setCursor(0, 2);
	print("             ");

	setCursor(0, 3);
	print("        ");
}
void showAboutPage(){
	showTime();

	setCursor(0, 1);
	print("--------------------");

	setCursor(0, 2);
	print("Bahareh Keivani");

	setCursor(0, 3);
	print("Zeinab Mahdizadeh");
}
void clearAboutPage(){
	setCursor(0, 0);
	print("                 ");
	setCursor(0, 1);
	print("                    ");
	setCursor(0, 2);
	print("                ");
	setCursor(0, 3);
	print("                 ");
}
void showInsertNamePage(){
	setCursor(0, 0);
	print("please insert your ");
	setCursor(0, 1);
	print("name to continue...");
	setCursor(0, 3);
	print("press 1 to submit");
}
void clearInsertNamePage(){
	setCursor(0, 0);
	print("                    ");
	setCursor(0, 1);
	print("                    ");
	setCursor(0, 2);
	print("                    ");
	setCursor(0, 3);
	print("                    ");
}
void showChooseDifficultyPage(){
	setCursor(0, 0);
	print("choose difficulty");
	setCursor(0, 1);
	print("1. Easy");
	setCursor(0, 2);
	print("2. Normal");
	setCursor(0, 3);
	print("3. Hard");
}
void clearChooseDifficultyPage(){
	setCursor(0, 0);
	print("                 ");
	setCursor(0, 1);
	print("        ");
	setCursor(0, 2);
	print("           ");
	setCursor(0, 3);
	print("       ");
}
void showGamePage(){
	setCursor(0, player.posRow);
	write(2);
	aliensGetClose(0);
	aliensGetClose(1);
	showHeart();
	showLives();
	showPoints();
}
void showTime(){
	HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);

	sprintf(timeStr, "%2d:%2d:%2d", currTime.Hours, currTime.Minutes, currTime.Seconds);
	sprintf(dateStr, "%d/%d/%d", currDate.Year, currDate.Month, currDate.Date);
	sprintf(currentTime,"%s %s", dateStr, timeStr);
	setCursor(0, 0);
	print(currentTime);
}
void chooseDifficulty1(){
	setCursor(0, 1);
	  write(3);
	  setCursor(0, 2);
	  print("2");
	  setCursor(0, 3);
	  print("3");
}
void chooseDifficulty2(){
	setCursor(0, 1);
	print("1");
	setCursor(0, 2);
	write(4);
	setCursor(0, 3);
	print("3");
}
void chooseDifficulty3(){
	  setCursor(0, 1);
	  print("1");
	  setCursor(0, 2);
	  print("2");
	  setCursor(0, 3);
	  write(5);
}
void showCostumChar(int col, int row, int specChar){
	setCursor(col, row);
	write(specChar);
}
void clearAlien(int col, int row){
	setCursor(col, row);
	print(" ");
}
void showBullet(int col, int row){
	setCursor(col, row);
	print("-");
}
void clearBullet(int col, int row){
	setCursor(col, row);
	print(" ");
}
void cleargamePage(){
	setCursor(0, 0);
	print("                     ");
	setCursor(0, 1);
	print("                     ");
	setCursor(0, 2);
	print("                      ");
	setCursor(0, 3);
	print("                      ");
}
void showYouWonPage(void){
	char winString[40];
	sprintf(winString,"You Won %s!", username);
	setCursor(0, 0);
	print(winString);
	sprintf(winString,"Points: %d", game.score);
	setCursor(0, 1);
	print(winString);
}
void showYouLostPage(void){
	char lostString[40];
	sprintf(lostString,"Game Over %s!", username);
	setCursor(0, 0);
	print(lostString);
	sprintf(lostString,"Points: %d", game.score);
	setCursor(0, 1);
	print(lostString);
}
void clearLCD(void){
	setCursor(0, 0);
	print("                    ");
	setCursor(0, 1);
	print("                    ");
	createChar(3, oneInvert);
}
void showHeart(){
	createChar(3, heart);
	setCursor(18, 3);
	write(3);
}
void showLives(void){
	setCursor(19, 3);
	char number[3];
	sprintf(number,"%d", player.lives);
	print(number);
}
void showPoints(void){
	setCursor(18, 0);
	char number[3];
	sprintf(number,"%d", game.score);
	print(number);
}
