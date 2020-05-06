// put implementations for functions, explain how it works
// put your names here, date
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "DAC.h"
#include "TIMER1.h"
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
