; BusyWait.asm
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly

; Runs on TM4C123
; Use SPI to send an 8-bit code to the LCD.

; As part of Lab 7, students need to implement these outcommand and outdata
; This driver assumes two low-level LCD functions
; this file is in the inc folder so it automatically will be applied to labs 7 8 9 and 10

       .thumb
       .data
       .align 4
;global variables go here

       .text
       .align  4
      .global   SPIOutCommand
      .global   SPIOutData


; Used in ECE319K Labs 7,8,9,10. You write these two functions

; ***********SPIOutCommand*****************
; This is a helper function that sends an 8-bit command to the LCD.
; Inputs: R0 = 32-bit command (number)
;         R1 = 32-bit SPI status register address
;         R2 = 32-bit SPI data register address
;         R3 = 32-bit GPIO port address for D/C
; Outputs: none
; Assumes: SPI and GPIO have already been initialized and enabled
; Note: must be AAPCS compliant
; Note: access to bit 6 of GPIO must be friendly
SPIOutCommand:  .asmfunc
    PUSH {R4, R5}
busy
; Read the SPI status register (R1 has address) and check if bit 4 is zero
    LDR R4, [R1]    
    ANDS R4, #0x10   
    BNE busy         ;If bit 4 is high, loop back to step 1 (wait for BUSY bit to be low)
;Clear D/C (GPIO bit 6) (R3 has address)
    LDR R4, [R3]
    BIC R4, #0x40
    STR R4, [R3]
;Write the command to the SPI data register (R2 has address)
    STR R0, [R2]
;Command must finish before exiting, wait busy bit to be low
busy_again
    LDR R4, [R1]
    ANDS R4, #0x10      
    BNE busy_again   
       
    POP {R4, R5}
   
    BX  LR             ;   return
    .endasmfunc
; *****************************************


; ***********SPIOutData*****************
; This is a helper function that sends an 8-bit data to the LCD.
; Inputs: R0 = 32-bit data (number)
;         R1 = 32-bit SPI status register address
;         R2 = 32-bit SPI data register address
;         R3 = 32-bit GPIO port address for D/C
; Outputs: none
; Assumes: SPI and GPIO have already been initialized and enabled
; Note: must be AAPCS compliant
; Note: access to bit 6 of GPIO must be friendly
SPIOutData:  .asmfunc
    PUSH {R4, R5}
TNF_busy    
; Read the SPI status register (R1 has address) and check if bit 1 is zero 
    LDR  R4, [R1]
    ANDS R4, #0x01
    BEQ TNF_busy    ; If bit 1 is low, loop back to step 1 (wait for TNF bit to be high)
; Set D/C (GPIO bit 6) to one (R3 has address)
    LDR R4, [R3]
    ORR R4, #0x40
    STR R4, [R3]
; Write the data to the SPI data register (R2 has address)
    STR R0, [R2]
   
    POP {R4, R5}
    BX  LR             ;return
    .endasmfunc
;****************************************************

    .end                             ; end of file
