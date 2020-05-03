// IO.h
#include <stdint.h>

void PortF_Init();

static void SysTick_Wait(uint32_t delay);

void SysTick_Wait10ms(uint32_t delay);

void SysTick_Init();

void IO_Init();

void IO_Touch();