// put prototypes for public functions, explain what it does
// put your names here, date
#ifndef __DAC_H__ // do not include more than once
#define __DAC_H__
#include <stdint.h>
void TIMER1_handler(void);
void Sound_Init(void);
void Sound_Play(uint32_t period);
void DAC_Out(uint32_t data);
void DAC_Init(void);

#endif
