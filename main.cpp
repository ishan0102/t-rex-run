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


SlidePot sensor(373, 21); // initialize sensor 
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.01 cm

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

// Creating a struct for the Sprite.
typedef enum {dead,alive} status_t;
struct sprite{
  uint32_t x;      // x coordinate
  uint32_t y;      // y coordinate
  const unsigned short *image; // ptr->image
  uint32_t width;
	uint32_t height;
};          
typedef struct sprite sprite_t;

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 80000;  						// maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = 0x07;
}

void PortF_Init(void){
  volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = 0;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
}

void IO_Init(void) {
	volatile int delay = 0;
	SYSCTL_RCGCGPIO_R |= 0x20;	// enable port F clock
	delay = 0;
	GPIO_PORTF_DIR_R &= ~0x10;	// set PF4 to input
	GPIO_PORTF_DEN_R |= 0x10;		// enable PF4
	GPIO_PORTF_PDR_R |= 0x10;
}

sprite_t dinoLeft = {20,120,TRexLeft,20,20};
sprite_t dinoRight = {20,120,TRexRight,20,20};
sprite_t tallCact = {50, 50, tallCactus, 20, 24};

sprite_t* mainSprite = &dinoLeft;

// sprite_t mainScreen = {0, 0, 


int flag;
int jumpFlag;
int jumpSize[] = {4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4};

void movingDino(){
	if(flag){
		mainSprite = &dinoLeft;
		flag = 0;
	} else {
		mainSprite = &dinoRight;
		flag = 1;
	}
}

int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
  Output_Init();
  Timer0_Init(&movingDino,16000000); // 50 Hz
	SysTick_Init();
	PortF_Init();
	IO_Init();
	ADC_Init();
	EnableInterrupts();
	
  ST7735_FillScreen(0x0000);            // set screen to black
	int previousX, previousY, preYCoord;
	int index = 0;
  while(1){
		sensor.Sync(); // wait for semaphore
		previousX = Position;
		previousY = mainSprite->y;
		Data = sensor.ADCsample(); // sample 12-bit channel 5
    Position = sensor.Distance(); Position /= 5; Position *= 5;
		if((GPIO_PORTF_DATA_R & 0x10) == 0x10){
				jumpFlag = 1;
		}
		if(previousX > Position){
			ST7735_DrawBitmap(previousX, mainSprite->y, blank, mainSprite->width, mainSprite->height);
		} else if(previousX < Position) {
			ST7735_DrawBitmap(previousX , mainSprite->y, blank, mainSprite->width, mainSprite->height);
		} else if(previousY != mainSprite->y){
			ST7735_DrawBitmap(mainSprite->x, previousY, blank, mainSprite->width / 2, mainSprite->height);
		}
		
    //ST7735_SetCursor(0,0);
		mainSprite->x = Position;
		if(!jumpFlag){
			ST7735_DrawBitmap(mainSprite->x, mainSprite->y, mainSprite->image, mainSprite->width, mainSprite->height);
			preYCoord = mainSprite->y;
			index = 0;
		} else {
			int yCoord = mainSprite->y - (4 * jumpSize[index]);
			ST7735_DrawBitmap(mainSprite->x, preYCoord, blank, mainSprite->width, mainSprite->height);
			ST7735_DrawBitmap(mainSprite->x, yCoord, mainSprite->image, mainSprite->width, mainSprite->height);
			index++;
			preYCoord = mainSprite->y - (4 * jumpSize[index - 1]);
		}
		
		if(index == 56){
			ST7735_DrawBitmap(mainSprite->x, preYCoord, blank, mainSprite->width, mainSprite->height);
			index = 0;
			jumpFlag = 0;
		}
		ST7735_DrawBitmap(tallCact.x, tallCact.y, tallCact.image, tallCact.width, tallCact.height);

  }

}

void SysTick_Handler(void){ // every sample
	GPIO_PORTF_DATA_R ^= 0x0E;
  sensor.Save(ADC_In());
}




