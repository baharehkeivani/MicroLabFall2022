/*
 * game.h
 *
 *  Created on: Jan 19, 2023
 *      Author: Shiman
 */

#ifndef GAME_H_
#define GAME_H_

#define EASY 1
#define NORMAL 2
#define HARD 3

// #define BOSS 1
// #define SOLDIER 2

#define ALIEN_BULLET -1
#define PLAYER_BULLET 1

typedef struct {
    int lives;
    int posRow;
    int posCol; //always 0
}Player;

typedef struct {
    int direction; // 1=player(left to right) -1=aliens(right to left)
    int posRow; //position
    int posCol;
}Bullet;

typedef struct {
    //int type; //extra point (boss or soldier)
	int specChar;
    int posRow;
    int posCol;
}Alien;

typedef struct
{
    int alienSpeed;
    int enemyBulletSpeed;
    int alienCnt;
    Alien aliens[80];
    Bullet playerBullets[20];
    Bullet enemyBullets[20];
    int score;
}Game;

void gameInit(int level);
void deleteFromAliens(int pos);
void deleteFromPlayerBullets(int pos);
void deleteFromEnemyBullets(int pos);
void aliensGetClose(int alienChar);
void updateBulletPositions();
void spaceshooterCollide();
void alienCollide();
void newPlayerBullet();
void updateFrontAliens();
void newRandomAlienBullet();
void moveLeft();
void moveRight();
#endif /* GAME_H_ */
