// main.cpp
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10 in C++

// Last Modified: 1/17/2020 
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
#include <string>
#include <string.h>
#include <stdlib.h>
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
#include "IO.h"
#include "Player.h"
#include "Menu.h"
#include "Print.h"
#include "Sound.h"

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

void currentScore() {
	char* s1 = (char*) "Score: ";
	char* s2 = (char*) "Resultado: ";
	if (!LanguageFlag) {
		ST7735_SetCursor(8, 2);
		ST7735_OutString(s1);
		ST7735_SetCursor(15, 2);
		LCD_OutDec(Score);
	} else {
		ST7735_SetCursor(5, 2);
		ST7735_OutString(s2);
		ST7735_SetCursor(16, 2);
		LCD_OutDec(Score);
	}
}	

int prevScore = 0;
void increaseDifficult(){
	int temp = Score / 300;
	
	for(int i = 0;i < temp; i++){
		currentDifficulty++;
	}
	
	if(prevScore < temp){
		Bird.vx *= 1.1;
		Cactus.vx *= 1.1;
		prevScore = temp;
	}
	
}
	
int main(void){
	// all initializations
	PLL_Init(Bus80MHz);
	Random_Init(1);
	Output_Init();
	SysTick_Init();
	PortF_Init();
	IO_Init();
	ADC_Init();
	Timer0_Init(&movingDino, 600000); // 50 Hz
	EnableInterrupts();
	
	// Timer1_Init(&screenRefresh,8000000); // 50 Hz
	ST7735_FillScreen(0x0000); // set screen to black
	
	while (1) {
		// only do first time
		if (!PreGameFlag) {
			PreGame();
			PreGameFlag = 1;
			RestartFlag = 0;
		}
		
		if ((GPIO_PORTD_DATA_R & 0x02) == 2) {
			PauseFlag = 1;
			pauseDino = 1;
		}
		
		if (PauseFlag) {
			if (!LanguageFlag) {
				PauseScreenEng();
			} else {
				PauseScreenSpan();
			}
			PauseFlag = 0;
			pauseDino = 0;
			ST7735_FillScreen(0x0000);
		}
		
		// gather data
		sensor.Sync();
		Data = sensor.ADCsample();
		Position = sensor.Distance(); Position /= 5; Position *= 5;
		
		if ((GPIO_PORTF_DATA_R & 0x10) == 0x10){
				// jumpFlag = 1;
		}
		
		if(RestartFlag) {
			Cactus.x = 1880;
			Bird.x = 3680;
			currentDifficulty = 0;
			RestartFlag = 0;
		}
		
		// draw sprites
		checkPosition();
		Bird.drawPlayer();
		TRex.drawPlayer();
		Cactus.drawPlayer();
		
		// score checker
		currentScore();
		increaseDifficult();
		
		// check if collision occurred
		if (collision()) {
			ST7735_FillScreen(0000);
			if (!LanguageFlag) {
				DeathScreenEng(Score);
			} else {
				DeathScreenSpan(Score);
			}
			Score = 0;
			ST7735_FillScreen(0000);
			RestartFlag = 1;
		}
	}
}

void SysTick_Handler(void){ // every sample
  sensor.Save(ADC_In());
}
