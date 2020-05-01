// main.cpp
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10 in C++

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "SlidePot.h"
#include "Images.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include <string>

SlidePot sensor(373, 21); // initialize sensor 
uint32_t Data;      // 12-bit ADC
uint32_t Position = 10;  // 32-bit fixed-point 0.01 cm

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

// Creating a struct for the Sprite.
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
sprite_t dino = {TRexStationary,20,20};
sprite_t dinoLeft = {TRexLeft,20,20};
sprite_t dinoRight = {TRexRight,20,20};
sprite_t dinoBlank = {blankStand, 20, 20};
sprite_t dinoLeftDuck = {TRexDuckLeft, 26, 13};
sprite_t dinoRightDuck = {TRexDuckRight,26, 13};
sprite_t dinoBlankDuck = {blankDuck, 26, 13};
sprite_t tallCact = {tallCactus, 20, 24};
sprite_t tallCactBlank = {blankTall, 20, 24};

struct Player {
	uint32_t x;      // x coordinate
  uint32_t y;      // y coordinate
	int32_t vx;
	int32_t vy;
	sprite_t* pSprite;
	sprite_t* blankSprite;
public:
	Player(uint32_t ix, uint32_t iy, sprite_t* nSprite, sprite_t* nbSprite, int32_t nvx, int32_t nvy)
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
	
	friend void jumpingDino();
	
};


Player TRex(20, 120, &dino, &dinoBlank, 0, 0);
Player Cactus(128, 120, &tallCact, &tallCactBlank, 4, 0);


class Projectile{
	uint32_t velocity;
	uint32_t damage;
};

void SysTick_Init(void) {
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 80000;  						// maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = 0x07;
}

void PortF_Init(void) {
  volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = 0;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
	GPIO_PORTF_DIR_R &= ~0x01;
	GPIO_PORTF_DEN_R |= 0x01;

}

void IO_Init(void) {
	volatile int delay = 0;
	SYSCTL_RCGCGPIO_R |= 0x20;	// enable port F clock
	delay = 0;
	GPIO_PORTF_DIR_R &= ~0x10;	// set PF4 to input
	GPIO_PORTF_DEN_R |= 0x10;		// enable PF4
	GPIO_PORTF_PDR_R |= 0x10;
}

void TitleScreen() {
	ST7735_SetTextColor(ST7735_CYAN);
	ST7735_OutString("Hello World!");
	while ((GPIO_PORTF_DATA_R & 0x01) != 1) {};
}

int jumpSize[] = {0, 1, 2, 3, 4, 5, 6, 6, 5, 4, 3, 2, 1, 0};  // 14

void movingDino(){
	if(!jumpFlag){
		//TRex.clearPlayer();
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
			//TRex.clearPlayer();
		TRex.y = 120 - jumpSize[index] * 6;
		index++;

		TRex.clearPlayer();
		/*** exit case***/
		if(index == 14){
			jumpComplete = 0;
			index = 0;
			enterJump = 0;
			jumpFlag = 0;
		}
	}		
	
	
	Cactus.clearPlayer();
	if(Cactus.x > 0){
		Cactus.x -= Cactus.vx / 2;
	} else {
		Cactus.x = 128;
	}
	Cactus.clearPlayer();
}

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


int main(void){
	PLL_Init(Bus80MHz); // Bus clock is 80 MHz
	Random_Init(1);
	Output_Init();
	TitleScreen();
	SysTick_Init();
	PortF_Init();
	IO_Init();
	ADC_Init();
	EnableInterrupts();
	Timer0_Init(&movingDino,6000000); // 50 Hz
	// Timer1_Init(&screenRefresh,8000000); // 50 Hz

	ST7735_FillScreen(0x0000); // set screen to black
	int previousY, preYCoord;
	while (1) {
		sensor.Sync();
		Data = sensor.ADCsample();
		Position = sensor.Distance();
		Position /= 5;
		Position *= 5;

		if((GPIO_PORTF_DATA_R & 0x10) == 0x10){
				// jumpFlag = 1;
		}
		checkPosition();
		TRex.drawPlayer();
		Cactus.drawPlayer();
}
	}


void SysTick_Handler(void){ // every sample
  sensor.Save(ADC_In());
}




