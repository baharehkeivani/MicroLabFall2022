/*
 * game.c
 *
 *  Created on: Jan 19, 2023
 *      Author: Shiman
 */


#include "game.h"
#include "lcd.h"
#include "main.h"
#include "buzzer.h"

Game game;
Player player;

int playerBulletCnt = 0 ;
int enemyBulletCnt = 0 ;
int existingAliensCnt = 0;
char* difficulties[3] = {"EASY", "NORMAL", "HARD"};
int frontAliensIndex[4];
int lifemsgSent = 0;

extern UART_HandleTypeDef huart1;

void gameInit(int level){
	playerBulletCnt = 0 ;
	enemyBulletCnt = 0 ;
	existingAliensCnt = 0;
	for(int i = 0 ; i < 4; i++){
		frontAliensIndex[i] = -1;
	}
	lifemsgSent = 0;

    player.posCol = 0;
    player.posRow = 2;
    game.score = 0;
    if(level == EASY){
        game.alienCnt = 15;
        game.alienSpeed = 5;
        game.enemyBulletSpeed = 8;
        player.lives = 7;
    }
    else if(level == NORMAL){
        game.alienCnt = 25;
        game.alienSpeed = 3;
        game.enemyBulletSpeed = 4;
        player.lives = 5;
    }
    else if(level == HARD){
        game.alienCnt = 35;
        game.alienSpeed = 1;
        game.enemyBulletSpeed = 2;
        player.lives = 3;
    }
}

void deleteFromAliens(int pos){
    if(!(pos < 0 || pos >= existingAliensCnt)){
        game.aliens[pos] = game.aliens[existingAliensCnt-1];
        existingAliensCnt--;
        game.alienCnt--;
        game.score++;
        showPoints();
        if(game.alienCnt == 3){
        	unsigned char msg[40]="";
			int n = sprintf((char *)msg, "GOOD JOB: only 3 enemies left\n");
			HAL_UART_Transmit(&huart1, msg, n, 1000);
        }
        if(game.alienCnt == 0){
        	emptyGameArrays();
        	changeLCDState(0);
    		unsigned char msg[40]="";
    		int n = sprintf((char *)msg, "YOU WON!!\n");
    		HAL_UART_Transmit(&huart1, msg, n, 1000);
        	cleargamePage();
        	showYouWonPage();
        }
    }
}

void deleteFromPlayerBullets(int pos){
    if(!(pos < 0 || pos >= playerBulletCnt)){
        game.playerBullets[pos] = game.playerBullets[playerBulletCnt - 1];
        playerBulletCnt--;
    }
}
void deleteFromEnemyBullets(int pos){
    if(!(pos < 0 || pos >= enemyBulletCnt)){
        game.enemyBullets[pos] = game.enemyBullets[enemyBulletCnt - 1];
        enemyBulletCnt--;
    }
}
void aliensGetClose(int alienChar){
	updateFrontAliens();
	alienCollide();

    for(int i=0;i<4;i++){
		Alien newAlien;
		newAlien.specChar = alienChar;
		newAlien.posRow = i;
    	if(frontAliensIndex[i] != -1){
			newAlien.posCol= game.aliens[frontAliensIndex[i]].posCol - 1;
    	}else{
			newAlien.posCol= 17;
    	}
		game.aliens[existingAliensCnt] = newAlien;
		existingAliensCnt++;
		showCostumChar(newAlien.posCol, newAlien.posRow, newAlien.specChar);
    }
    updateFrontAliens();
    alienCollide();
}

//every y second - we call this in timer
void updateBulletPositions(){
	for(int i = 0 ; i < playerBulletCnt; i++){
		clearBullet(game.playerBullets[i].posCol, game.playerBullets[i].posRow);
		game.playerBullets[i].posCol += game.playerBullets[i].direction;
		if(game.playerBullets[i].posCol > 17 || game.playerBullets[i].posCol < 0){
			deleteFromPlayerBullets(i);
			i--;
		}
		else {
			showCostumChar(game.playerBullets[i].posCol, game.playerBullets[i].posRow, 7);
		}
	}
    for(int i=0;i<enemyBulletCnt;i++){
    	clearBullet(game.enemyBullets[i].posCol, game.enemyBullets[i].posRow);
        game.enemyBullets[i].posCol += game.enemyBullets[i].direction;
        if(game.enemyBullets[i].posCol > 17 || game.enemyBullets[i].posCol < 0){
        	deleteFromEnemyBullets(i);
        	i--;
        }
        else {
        	showCostumChar(game.enemyBullets[i].posCol, game.enemyBullets[i].posRow, 6);
        }
    }
	updateFrontAliens();
	alienCollide();
	spaceshooterCollide();
}

void spaceshooterCollide(){
    for(int i=0;i<enemyBulletCnt;i++){
        if(game.enemyBullets[i].posCol == player.posCol && game.enemyBullets[i].posRow == player.posRow){
            player.lives--;
            showLives();
            showCostumChar(player.posCol, player.posRow, 2); //actually it shows the space shooter
            deleteFromEnemyBullets(i);
            break;
        }
    }
    if(game.alienCnt == 3 && !lifemsgSent){
    	lifemsgSent = 1;
		unsigned char msg[40]="";
		int n = sprintf((char *)msg, "WARNING: only 1 life left\n");
		HAL_UART_Transmit(&huart1, msg, n, 1000);
	}
    if(player.lives == 0){
    	emptyGameArrays();
		changeLCDState(1);
		unsigned char msg[40]="";
		int n = sprintf((char *)msg, "GAME OVER: you died\n");
		HAL_UART_Transmit(&huart1, msg, n, 1000);
		cleargamePage();
		showYouLostPage();
		return;
	}
}

void alienCollide(){
    for(int i=0;i<playerBulletCnt;i++){ //iterate on bullets
        for(int j=0;j<4;j++){ //iterate on front aliens
            int alienPos = frontAliensIndex[j];
            if(alienPos == -1){
            	continue;
            }
            if(game.playerBullets[i].posCol == game.aliens[alienPos].posCol && game.playerBullets[i].posRow == game.aliens[alienPos].posRow){
                clearAlien(game.aliens[alienPos].posCol, game.aliens[alienPos].posRow);
                deleteFromPlayerBullets(i);
                deleteFromAliens(alienPos);
                alienShotSound();
                i--;
                break;
            }
        }
    }
    updateFrontAliens();
}


//if player select the shot button we call this method
void newPlayerBullet(){
	if(game.playerBullets[playerBulletCnt - 1].posCol == player.posCol + 1 && game.playerBullets[playerBulletCnt - 1].posRow == player.posRow){
		return;
	}
    Bullet newBullet;
    newBullet.direction = PLAYER_BULLET;
    newBullet.posCol = player.posCol + 1;
    newBullet.posRow = player.posRow;
    game.playerBullets[playerBulletCnt] = newBullet;
    playerBulletCnt++;
    playerShotSound();
    showCostumChar(newBullet.posCol, newBullet.posRow, 7);
}

void updateFrontAliens(){
    int row_0 = -1;
    int row_1 = -1;
    int row_2 = -1;
    int row_3 = -1;
    for(int i=0;i<existingAliensCnt;i++){
        Alien alien = game.aliens[i];
        if(alien.posCol == 0){
        	emptyGameArrays();
    		changeLCDState(1);
    		unsigned char msg[40]="";
    		int n = sprintf((char *)msg, "GAME OVER: enemies took over\n");
    		HAL_UART_Transmit(&huart1, msg, n, 1000);
    		cleargamePage();
    		showYouLostPage();
    		return;
    	}
        if(alien.posRow == 0){
            if(row_0 == -1){
                row_0 = i;
            }
            else if(alien.posCol < game.aliens[row_0].posCol){
                row_0 = i;
            }
        }
        if(alien.posRow == 1){
            if(row_1 == -1){
                row_1 = i;
            }
            else if(alien.posCol < game.aliens[row_1].posCol){
                row_1 = i;
            }
        }
        if(alien.posRow == 2){
            if(row_2 == -1){
                row_2 = i;
            }
            else if(alien.posCol < game.aliens[row_2].posCol){
                row_2 = i;
            }
        }
        if(alien.posRow == 3){
            if(row_3 == -1){
                row_3 = i;
            }
            else if(alien.posCol < game.aliens[row_3].posCol){
                row_3 = i;
            }
        }
    }
    if(row_0 != -1){
        frontAliensIndex[0] = row_0;
    }
    else{
    	frontAliensIndex[0] = -1;
    }
    if(row_1 != -1){
        frontAliensIndex[1] = row_1;
    }else{
    	frontAliensIndex[1] = -1;
    }
    if(row_2 != -1){
        frontAliensIndex[2] = row_2;
    }else {
    	frontAliensIndex[2] = -1;
    }
    if(row_3 != -1){
        frontAliensIndex[3] = row_3;
    }else{
    	frontAliensIndex[3] = -1;
    }
}

//random bullets shot by aliens
void newRandomAlienBullet(){
    if(existingAliensCnt > 0){
        updateFrontAliens();
        Alien randomAlian = game.aliens[frontAliensIndex[rand() % (4)]];
        Bullet newBullet;
        newBullet.direction = ALIEN_BULLET;
        newBullet.posCol = randomAlian.posCol - 1;
        newBullet.posRow = randomAlian.posRow;
        game.enemyBullets[enemyBulletCnt] = newBullet;
        enemyBulletCnt++;
        showCostumChar(newBullet.posCol, newBullet.posRow, 6);
    }
}

void moveLeft(){
	if(player.posRow - 1 < 0){
	  player.posRow = 3;
	}else {
	  player.posRow--;
	}
}
void moveRight(){
	player.posRow = (player.posRow + 1) % 4;
}
void emptyGameArrays(){
	for(int i=0;i<enemyBulletCnt;i++){
		deleteFromEnemyBullets(i);
		i--;
	}
}

