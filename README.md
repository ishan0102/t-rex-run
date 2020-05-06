# T-Rex Run

## Overview
This game was created by **Michael Chen** and **Ishan Shah** for EE 319H: Introduction to Embedded Systems. The course was taught during Spring 2020 by Dr. Jon Valvano. The purpose of the project is to put multiple modules together to create a working embedded system. The modules were worked on in smaller projects through the semester like the DAC piano, breating LED, and slide potentiometer ruler.

## How to Play
T-Rex Run is based on Google's scroller game that appears when you don't have wifi. In this version, you play as a dinosaur that has to get as far as possible while avoiding obstacles like cacti and pterodactyls. Help the dinosaur move by sliding the slide potentiometer left to jump and right to duck. The game also gets progressively faster the longer you play.

## Features
- **Buttons**: two buttons, one to navigate menus and one to pause
- **LEDs**: onboard LED is used to indicate the status of the player where green is running, blue is jumping, and red is ducking
- **Slide Potentiometer**: used to make the dinosaur run, jump, or duck
- **Sounds**: most game events have corresponding sounds
- **Display**: shows the menus and displays moving sprites and score during gameplay
- **Interrupts**: SysTick, Timer0A, and Timer1A are used for managing various game threads
- **Languages**: player can choose between English and Spanish on the title screen

## Hardware
- Slide pot pin 1 connected to ground
- Slide pot pin 2 connected to PD2
- Slide pot pin 3 connected to 3.3V
- Start button connected to PD0
- Pause button connected to PD1
- $12 \ k\Omega$ DAC bit 0 connected to PB0
- $6 \ k\Omega$ DAC bit 1 connected to PB1
- $3 \ k\Omega$ DAC bit 2 connected to PB2
- $1.5 \ k\Omega$ DAC bit 3 connected to PB3
- LEDs on PF1-PF3

## Demonstration and Visuals
Here is a [link to a video demonstration](https://www.youtube.com/watch?v=mLqsLVLjyEE) of how the game is played and a picture of the completed game.

![Hardware Setup](https://raw.githubusercontent.com/ishan0102/t-rex-run/master/T-Rex%20Run%20Setup.jpg)