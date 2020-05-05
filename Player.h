// Creating a struct for the Sprite.
#include <stdint.h>
#include "Images.h"
#include "ST7735.h"
#include "Random.h"
#include "../inc/tm4c123gh6pm.h"

void movingDino();
void jumpingDino();

int flag;
int duckFlag;
int jumpFlag;

uint32_t Score;
int currentDifficulty = 0;

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
sprite_t birdWingsUp = {birdUp, 24, 24};
sprite_t birdWingsDown = {birdDown, 24, 24};
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
		ST7735_DrawBitmap(x / 10, y / 10, pSprite->image, pSprite->width, pSprite->height);
	}

	void clearPlayer(){
		ST7735_DrawBitmap(x / 10, y / 10, blankSprite->image, blankSprite->width, blankSprite->height);
	}


	void movePlayer(int32_t nvx, int32_t nvy){
		vx = nvx;
		vy = nvy;
	}

	int collision();

	void movingDino();
};

Player TRex(200, 1200, &dino, &dinoBlank, 0, 0);
Player Cactus(1880, 1200, &tallCact, &tallCactBlank, 40, 0);
Player Bird(3680, 1000, &birdWingsDown, &dinoBlank, 40, 0);

class Projectile{
	uint32_t velocity;
	uint32_t damage;
};

#define calibration 170
int collision() {
	int BcenterX = Bird.x + (Bird.pSprite->width / 2);
	int BcenterY = Bird.y + (Bird.pSprite->height / 2) - 20;
	int TcenterX = TRex.x + (TRex.pSprite->width / 2);
	int TcenterY = TRex.y + (TRex.pSprite->height / 2);
	int CcenterX = Cactus.x + (Cactus.pSprite->width / 2);
	int CcenterY = Cactus.y + (Cactus.pSprite->height / 2);

	if((TcenterX - CcenterX < calibration && TcenterX - CcenterX > -calibration) && (TcenterY - CcenterY < calibration && TcenterY - CcenterY > -calibration) ){
			return 1; // has collided
	}
	if((TcenterX - BcenterX < calibration + 20 && TcenterX - BcenterX > -(calibration + 20) && (!duckFlag)) && (TcenterY - BcenterY < (calibration + 100) && TcenterY - BcenterY > -(calibration + 100)) && (TcenterY - BcenterY < (calibration) && TcenterY - BcenterY > -(calibration))){
			return 1; // has collided
	}
	return 0;// no collision
}

int jumpSize[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};  // 14

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
			counter++;

		} else if(counter >= 15){
			counter = 0;
		}	else {
			counter++;
		}
		//TRex.clearPlayer();

	} else {
		static int jumpComplete = 0;
		static int enterJump = 0;
		static int index = 0;
		static int indexFlag = 0;

		if(!enterJump){
			TRex.pSprite = &dino;
			jumpComplete = 1;
			enterJump = 1;
		}
		GPIO_PORTF_DATA_R &= 0x00;
		GPIO_PORTF_DATA_R ^= 0x02;
		// TRex.clearPlayer();
		TRex.y = 1200 - index * 5;
		if(index == 120){
			indexFlag = 1;
		}

		if(!indexFlag){
			index++;
		} else {
			index--;
		}

		// TRex.clearPlayer();
		/*** exit case***/
		if(index == -1){
			jumpComplete = 0;
			index = 0;
			enterJump = 0;
			jumpFlag = 0;
			indexFlag = 0;
		}
	}		

	static int counter2 = 0;
	static int birdFlag = 0;
	if(counter2 == 0){
		if(birdFlag){
			Bird.pSprite = &birdWingsDown;
			birdFlag = 0;
		} else {
			Bird.pSprite = &birdWingsUp;
			birdFlag = 1;
		}

		counter2++;
	} else if(counter2 >= 30){
		counter2 = 0;
	}	else {
		counter2++;
	}
	
	Score++;

	if(Cactus.x > -220){
		Cactus.x -= Cactus.vx / 10;
	} else {
		Cactus.x = Random32() % 1000 + 1880;
	}

	if(Bird.x > -220){
		Bird.x -= Bird.vx / 10;
	} else {
		Bird.x = Random32() % 1000 + 1880;
	}
}

uint32_t Position;  // 32-bit fixed-point 0.01 cm

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
