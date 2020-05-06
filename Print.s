; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

digit	EQU 0
place	EQU 4

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB



;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; Lab 7 requirement is for at least one local variable on the stack with symbolic binding


LCD_OutDec

	PUSH {LR, R0}

		SUB SP, #8 ; Allocate num

	
		CMP R0, #10
		BLO printSingleDec
		MOV R3, #10
		UDIV R2, R0, R3
		MLS R1, R2, R3, R0 
		MOV R0, R2
		STR R1, [SP, #digit]
		BL LCD_OutDec
		LDR R0, [SP, #digit]
		ADD R0, #0x30
		BL ST7735_OutChar
		B exitDec
printSingleDec
		ADD R0, #0x30
		BL ST7735_OutChar
exitDec
		ADD SP, #8
		POP {LR, R0}
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.003 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
; Lab 7 requirement is for at least one local variable on the stack with symbolic binding
LCD_OutFix
		MOV R1, #0
		
LCD_OutFix_Inductive			; recursive function
		PUSH {LR, R4, R1, R0}
	
		SUB SP, #8  			; Allocate num

		MOV R4, #1000			; check if >= 1000	
		CMP R0, R4
		BHS printStar			; print stars if greater
		CMP R1, #3				; check if at base case
		BEQ exitFix
		MOV R3, #10
		UDIV R2, R0, R3			; R2 = R0 / 10
		MLS R4, R2, R3, R0 		; R4 = R0 - (R2 * 10)
		STR R4, [SP, #digit]
	
		STR R1, [SP, #place]
		ADD R1, #1
	
		MOV R0, R2
		BL LCD_OutFix_Inductive ; recursive call
		
		LDR R0, [SP, #digit]
		ADD R0, #0x30
		BL ST7735_OutChar
		LDR R1, [SP, #place]
		CMP R1, #2
		BNE exitFix
		MOV R0, #0x2E
		BL ST7735_OutChar
		B exitFix

printStar
        MOV R0, #0x2A
        BL ST7735_OutChar
        MOV R0, #0x2E
        BL ST7735_OutChar
        MOV R0, #0x2A
        BL ST7735_OutChar
        MOV R0, #0x2A
        BL ST7735_OutChar
     
exitFix        
		ADD SP,#8
        POP {R0, R1, R4, LR}
     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN          ; make sure the end of this section is aligned
     END            ; end of file
