// Creating a struct for the Sprite.
#include <stdint.h>
#include "Images.h"
#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"

void movingDino();
void jumpingDino();

int flag;
int duckFlag;
int jumpFlag;

struct sprite{
	const unsigned short *image; // ptr->image
  uint32_t width;
	uint32_t height;
};          
typedef struct sprite sprite_t;
sprite_t dino = {TRexStationary, 30, 24};
sprite_t dinoLeft = {TRexLeft, 30, 24};
sprite_t dinoRight = {TRexRight, 30, 24};
sprite_t dinoBlank = {blankStand, 30, 24};
sprite_t dinoLeftDuck = {TRexDuckLeft, 30, 24};
sprite_t dinoRightDuck = {TRexDuckRight, 30, 24};
sprite_t dinoBlankDuck = {blankDuck, 30, 17};
sprite_t tallCact = {tallCactus, 26, 28};
sprite_t tallCactBlank = {blankTall, 26, 28};

struct Player {
	int32_t x;      // x coordinate
  int32_t y;      // y coordinate
	int32_t vx;
	int32_t vy;
	sprite_t* pSprite;
	sprite_t* blankSprite;
public:
	Player(int32_t ix, int32_t iy, sprite_t* nSprite, sprite_t* nbSprite, int32_t nvx, int32_t nvy)
	: x(ix), y(iy), vx(nvx), vy(nvy),  pSprite(nSprite) ,  blankSprite(nbSprite) {}

	void move(int dx, int dy){
		this->x += dx;
		this->y += dy;
	}

	void drawPlayer(){
		ST7735_DrawBitmap(x, y, pSprite->image, pSprite->width, pSprite->height);
	}

	void clearPlayer(){
		ST7735_DrawBitmap(x, y, blankSprite->image, blankSprite->width, blankSprite->height);
	}


	void movePlayer(int32_t nvx, int32_t nvy){
		vx = nvx;
		vy = nvy;
	}



	friend void movingDino();
};

Player TRex(20, 120, &dino, &dinoBlank, 0, 0);
Player Cactus(128, 120, &tallCact, &tallCactBlank, 4, 0);

class Projectile{
	uint32_t velocity;
	uint32_t damage;
};

int jumpSize[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1, 0};  // 14

void movingDino(){
	if(!jumpFlag){
		// TRex.clearPlayer();
		static int counter = 0;
		if(counter == 0){
			if(duckFlag){
				GPIO_PORTF_DATA_R &= 0x00;
				GPIO_PORTF_DATA_R ^= 0x04;
				if(flag){
					TRex.pSprite = &dinoLeftDuck;
					flag = 0;
				} else {
					TRex.pSprite = &dinoRightDuck;
					flag = 1;
				}
			} else {
				GPIO_PORTF_DATA_R &= 0x00;
				GPIO_PORTF_DATA_R ^= 0x08;
				if(flag){
					TRex.pSprite = &dinoLeft;
					flag = 0;
				} else {
					TRex.pSprite = &dinoRight;
					flag = 1;
				}
			}
		} else if(counter >= 10000){
			counter = 0;
		}	else {
			counter++;
		}
		//TRex.clearPlayer();

	} else {
		static int jumpComplete = 0;
		static int enterJump = 0;
		static int index = 0;

		if(!enterJump){
			TRex.pSprite = &dino;
			jumpComplete = 1;
			enterJump = 1;
		}
		GPIO_PORTF_DATA_R &= 0x00;
		GPIO_PORTF_DATA_R ^= 0x02;
		TRex.clearPlayer();
		TRex.y = 120 - jumpSize[index] * 6;
		index++;

		TRex.clearPlayer();
		/*** exit case***/
		if(index == 18){
			jumpComplete = 0;
			index = 0;
			enterJump = 0;
			jumpFlag = 0;
		}
	}		



	if(Cactus.x > -22){
		Cactus.x -= Cactus.vx;
	} else {
		Cactus.x = 128;
	}
}

uint32_t Position = 10;  // 32-bit fixed-point 0.01 cm

void checkPosition(){
	if(Position > 120){
		duckFlag = 1;
	} else if(Position < 40){
		jumpFlag = 1;
		duckFlag = 0;
	} else {
		duckFlag = 0;
	}
}