#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "SlidePot.h"
#include "IO.h"
#include "Print.h"
#include "Sound.h"

SlidePot sensor(373, 21); // initialize sensor
uint32_t Data;      // 12-bit ADC
uint32_t PosMenu;
uint8_t LanguageFlag;
uint8_t PreGameFlag;
uint8_t RestartFlag = 0;
uint8_t PauseFlag = 0;
char scoreArray[5] = {'0', '0', '0', '0', '\0'};

void SelectLanguage() {
	char* s4 = (char*) "-->";
	char* s5 = (char*) "   ";
	do {
		sensor.Sync();
		Data = sensor.ADCsample();
		PosMenu = sensor.Distance();
		
		ST7735_SetCursor(8, 3);
		char* s0 = (char*) "T-Rex Run";
		ST7735_OutString(s0);
		
		ST7735_SetCursor(5, 7);
		char* s1 = (char*) "Select Language";
		ST7735_OutString(s1);
		
		ST7735_SetCursor(10, 9);
		char* s2 = (char*) "English";
		ST7735_OutString(s2);
		
		ST7735_SetCursor(10, 10);
		char* s3 = (char*) "Spanish";
		ST7735_OutString(s3);
		
		if (PosMenu < 80) {
			LanguageFlag = 0;
			ST7735_SetCursor(7, 10); ST7735_OutString(s5);
			ST7735_SetCursor(7, 9); ST7735_OutString(s4);
		} else {
			LanguageFlag = 1;
			ST7735_SetCursor(7, 9); ST7735_OutString(s5);
			ST7735_SetCursor(7, 10); ST7735_OutString(s4);
		}
	}
	while ((GPIO_PORTD_DATA_R & 0x01) != 1);
}

void TitleScreenEng() {
	ST7735_SetCursor(7, 2);
	char* s1 = (char*) "Welcome to\n        T-Rex Run";
	ST7735_OutString(s1);
	
	ST7735_SetCursor(4, 5);
	char* s2 = (char*) "By Michael Chen\n    and Ishan Shah";
	ST7735_OutString(s2);
	
	ST7735_SetCursor(6, 9);
	char* s3 = (char*) "Press Button\n        to Start";
	ST7735_OutString(s3);

	IO_Touch();
	Sound_Fastinvader2();
}

void TitleScreenSpan() {
	ST7735_SetCursor(6, 2);
	char* s1 = (char*) "Bienvenido a\n        T-Rex Run";
	ST7735_OutString(s1);
	
	ST7735_SetCursor(4, 5);
	char* s2 = (char*) "Por Michael Chen\n      y Ishan Shah";
	ST7735_OutString(s2);
	
	ST7735_SetCursor(3, 9);
	char* s3 = (char*) "Presione el Boton\n      para Comenzar";
	ST7735_OutString(s3);
	
	IO_Touch();
	Sound_Fastinvader2();
}

void DeathScreenEng(uint32_t final) {
	ST7735_SetCursor(8, 2);
	char* s1 = (char*) "You Died";
	ST7735_OutString(s1);
	
	ST7735_SetCursor(3, 5);
	char* s2 = (char*) "Final Score: ";
	ST7735_OutString(s2);
	ST7735_SetCursor(16, 5);
	LCD_OutDec(final);

	ST7735_SetCursor(8, 8);
	char* s3 = (char*) "Press to\n       Play Again";
	ST7735_OutString(s3);
	
	IO_Touch();
	Sound_Fastinvader4();
}

void DeathScreenSpan(uint32_t final) {
	ST7735_SetCursor(8, 2);
	char* s1 = (char*) "Tu Moriste";
	ST7735_OutString(s1);
	
	ST7735_SetCursor(5, 5);
	char* s2 = (char*) "Resultado: ";
	ST7735_OutString(s2);
	ST7735_SetCursor(16, 5);
	LCD_OutDec(final);

	ST7735_SetCursor(6, 8);
	char* s3 = (char*) "Presione para\n      Jugar de Nuevo";
	ST7735_OutString(s3);
	
	IO_Touch();
	Sound_Fastinvader4();
}

void PreGame() {
	ST7735_SetRotation(1);
	SelectLanguage();
	ST7735_FillScreen(0x0000);
	if (LanguageFlag == 0) {
		TitleScreenEng();
	} else {
		TitleScreenSpan();
	}
	ST7735_FillScreen(0000);
}

void PauseScreenEng() {
	ST7735_FillScreen(0x0000);
	ST7735_SetCursor(10, 5);
	ST7735_OutString("Paused");
	IO_Touch_Pause();
	Sound_Highpitch();
}

void PauseScreenSpan() {
	ST7735_FillScreen(0x0000);
	ST7735_SetCursor(9, 5);
	ST7735_OutString("Pausado");
	IO_Touch_Pause();
	Sound_Highpitch();
}
