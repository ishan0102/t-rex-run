// IO.cpp

#include "../inc/tm4c123gh6pm.h"
#include "IO.h"
#include <stdint.h>

void PortF_Init() {
  volatile int delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = 0;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
}

static void SysTick_Wait(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

// Time delay using busy wait.
// This assumes 50 MHz system clock.
void SysTick_Wait10ms(uint32_t delay){
  uint32_t i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms (assumes 80 MHz clock)
  }
}

void SysTick_Init() {
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 80000;  						// maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = 0x07;
}

void IO_Init() {
	volatile int delay = 0;
	SYSCTL_RCGCGPIO_R |= 0x0F;	// enable port D clock
	delay = 0;
	GPIO_PORTD_DIR_R &= ~0x03;	// set PD0 and PD1 to input
	GPIO_PORTD_DEN_R |= 0x03;		// enable PD0 and PD1
}

//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch() {
 // --UUU-- wait for release; delay for 20ms; and then wait for press
	volatile int waitDelay;
	while((GPIO_PORTD_DATA_R & 0x01) != 0x01){
		waitDelay = 0;
	}
	SysTick_Wait10ms(2);
		while((GPIO_PORTD_DATA_R & 0x01) == 0x01){
		waitDelay = 0;
	}
	SysTick_Wait10ms(2);
}  

//------------IO_Touch_Pause------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch_Pause() {
 // --UUU-- wait for release; delay for 20ms; and then wait for press
	volatile int waitDelay;
	while((GPIO_PORTD_DATA_R & 0x02) != 0x02){
		waitDelay = 0;
	}
	SysTick_Wait10ms(2);
		while((GPIO_PORTD_DATA_R & 0x02) == 0x02){
		waitDelay = 0;
	}
	SysTick_Wait10ms(2);
}  
