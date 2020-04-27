// SlidePot.cpp
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "SlidePot.h"
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x08;
	delay = SYSCTL_RCGCGPIO_R; // allow time for clock to stabilize
	delay = SYSCTL_RCGCGPIO_R; 
	delay = SYSCTL_RCGCGPIO_R; 
	delay = SYSCTL_RCGCGPIO_R; 
	GPIO_PORTD_DIR_R &= ~0x04;
	GPIO_PORTD_AFSEL_R |= 0x04;
	GPIO_PORTD_DEN_R &= ~0x04; // disable digital I/O
	GPIO_PORTD_AMSEL_R |= 0x04; // enable analog I/O
	SYSCTL_RCGCADC_R |= 0x01; // enable ADC clock
	delay = SYSCTL_RCGCADC_R; 
  delay = SYSCTL_RCGCADC_R; 
  delay = SYSCTL_RCGCADC_R; 
  delay = SYSCTL_RCGCADC_R; 	
	
	ADC0_PC_R = 0x01; // configure conversion speed
	ADC0_SSPRI_R = 0x0123; // select sequencer 3 as highest priority
	ADC0_ACTSS_R &= ~0x0008; // disable sequencer 3 during configuration
	ADC0_EMUX_R &= ~0xF000; // sequencer 3 is software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 5; // sets Ain5 as input channel
	ADC0_SSCTL3_R = 0x0006; // activate sample completion flag (END0) and single sample (IE0)
	ADC0_IM_R &= ~0x0008; // disable sequencer 3 interrupts
	ADC0_ACTSS_R |= 0x0008; // enable sequencer 3
	ADC0_SAC_R = 4;   // 16-point averaging, move this line into your ADC_Init()
}

//------------ADCIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	uint32_t data;
	ADC0_PSSI_R = 0x0008; // start sample conversion
	while ((ADC0_RIS_R & 0x08) == 0){}; // busy-wait synchronization (0 is busy)
	data = ADC0_SSFIFO3_R & 0xFFF; // masks 12-bit result
	ADC0_ISC_R = 0x0008; // clears RIS automatically
	return data;
}

// constructor, invoked on creation of class
// m and b are linear calibration coeffients 
SlidePot::SlidePot(uint32_t m, uint32_t b){
	slope = m;
	offset = b;
	data = 0;
	distance = 0;
}

void SlidePot::Save(uint32_t n){
	this->data = n;
	this->distance = Convert(n);
	this->flag = 1;
}

uint32_t SlidePot::Convert(uint32_t n){
	return (this->slope * n) / 10000 + this->offset;
}

void SlidePot::Sync(void){
	while (this->flag == 0) {};
	this->flag = 0;
}

uint32_t SlidePot::ADCsample(void){ // return ADC sample value (0 to 4095)
  return this->data;
}

uint32_t SlidePot::Distance(void){  // return distance value (0 to 2000), 0.001cm
  return this->distance;
}


