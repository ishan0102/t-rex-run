// put implementations for functions, explain how it works
// put your names here, date
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "DAC.h"
  
// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on TM4C123
// Program written by: Michael Chen and Ishan Shah
// Date Created: 3/6/17 
// Last Modified: 4/4/2020

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "TIMER1.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

uint8_t soundIndex;

const unsigned short wave[32] = {
  8,9,11,12,13,14,14,15,15,15,14,
  14,13,12,11,9,8,7,5,4,3,2,
  2,1,1,1,2,2,3,4,5,7};					 
	
void TIMER1_handler(void){
	GPIO_PORTF_DATA_R ^= 0x08;		// heartbeat at PF3
	soundIndex = (soundIndex + 1) & 0x1F; // if SoundIndex > 31 then revert to 0;
	DAC_Out(wave[soundIndex]);
}

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();
	Timer1_Init(&TIMER1_handler, 0);
	soundIndex = 0;
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){		// period is in terms of bus cycles
	TIMER1_TAILR_R = period; 
}

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once
// sets PB0-3 to a 4-bit output
// Input: none
// Output: none
void DAC_Init(void){
	volatile int delay = 0;
  SYSCTL_RCGCGPIO_R |= 0x02;		// enable port B clock
	delay = 0;
	GPIO_PORTB_DIR_R &= ~0x0F;		// set PB0-3 to output (4-bit output)
	GPIO_PORTB_DIR_R |= 0x0F;
	GPIO_PORTB_AFSEL_R &= ~0x07;  // disable alt funct on PB2-0
	GPIO_PORTB_DEN_R &= ~0x0F;		// enable GPIO for PB0-3
	GPIO_PORTB_DEN_R |= 0x0F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = data;
}
