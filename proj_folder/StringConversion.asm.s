; StringConversion.asm
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 number to string conversions
;
; Used in ECE319K Labs 7,8,9,10. You write these two functions

 
    .global   Dec2String
    .global   Fix2String
       .thumb
       .data
       .align 4
;global variables go here
Ran    .space  4
       .text
       .align  4

count      .long 0

num             .long 0
count_written   .long 4



  
;-----------------------Dec2String-----------------------
; Convert a 32-bit number into unsigned decimal format
; String the string into the empty array add null-termination
; Input: R0 (call by value) 32-bit unsigned number
;        R1 pointer to empty array
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
Dec2String:  .asmfunc
; binding
   	;count = 0 (no EQU pseudo op, see above global variables)
; allocation
    PUSH {R11, LR}
    SUB R13, #4    ;allocate one 32-bit wide space on stack
    MOV R11, R13
; initialization
    MOV R2, #0      ; initialize local variable count to zero
    LDR R3, count
    STR R2, [R11, R3]
    PUSH {R4, R5}
; main operation
loop_push_digit
    ;d = n % 10
    MOV R2, #10 
    UDIV  R3, R0, R2      ; div to get quotient, n//10
    MUL   R4, R3, R2      ; need for computing remainder, (n//10) * 10
    SUB   R3, R0, R4      ; the mod (remainder), d = n % 10
    PUSH {R3}             ; push digit onto stack
    ;count += 1
    LDR R3, count
    LDR R2, [R11, R3]
    ADD R2, #1
    STR R2, [R11, R3]
    ;n=n//10
    MOV R2, #10
    UDIV R0, R0, R2
    CMP R0, #0
    BEQ loop_pop_ascii
    ;if not zero, loop_push

    B loop_push_digit

loop_pop_ascii
    ;break if count reaches zero
    LDR R3, count
    LDR R2, [R11, R3]
    CMP R2, #0
    BEQ end_dec_string

    ; buf[i] = (d+'0'); i+=1
    POP {R2}
    ADD R2, #0x30
    STRB R2, [R1], #1

    ;count -= 1
    LDR R3, count
    LDR R2, [R11, R3]
    SUB R2, #1
    STR R2, [R11, R3]
    B loop_pop_ascii

end_dec_string
;write null character to end
    MOV  R2, #0
    STRB R2, [R1]

;deallocation
    POP {R4, R5}
    ADD R13, #4
    POP {R11, PC}
    .endasmfunc
;* * * * * * * * End of Dec2String * * * * * * * *


; -----------------------Fix2String----------------------
; Create characters for LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
;          R1 pointer to empty array
; Outputs: none
; E.g., R0=0,    then create "0.000 "
;       R0=3,    then create "0.003"
;       R0=89,   then create "0.089"
;       R0=123,  then create "0.123"
;       R0=9999, then create "9.999"
;       R0>9999, then create "*.***"
; Invariables: This function must not permanently modify registers R4 to R11
Fix2String:  .asmfunc
; binding
	;num = 0
	;count_written  = 4
; allocation
    PUSH {R11, LR}
    SUB R13, #8    ;allocate two 32-bit wide space on stack for local variables
    MOV R11, R13
; initialization
    LDR R2, num
    STR R0, [R11, R2]

    MOV R2, #0      ; initialize local variable count to zero
    LDR R3, count_written
    STR R2, [R11, R3]

    PUSH {R4, R5}

; main operation
; increment pointer by fixed amount to start at end of string (includes index for radix point)
; 0.001 resolution
    ADD R1, #5
    MOV R2, #0x00
    STRB R2, [R1], #-1

loop_fix
    ;get local variable number
    LDR R3, num
    LDR R3, [R11, R3]     ;R3 = num

    ;d = n % 10
    MOV R2, #10 
    UDIV  R4, R3, R2      ; div to get quotient, n//10
    MUL   R5, R4, R2      ; need for computing remainder, (n//10) * 10
    SUB   R4, R3, R5      ; the mod (remainder), d = n % 10

    ;write digit in R4 to buf, R4 may be 0, that is fine
    ADD R4, #0x30
    STRB R4, [R1], #-1

    ;n=n//10
    LDR R4, num
    UDIV R3, R3, R2
    STR R3, [R11, R4]

	;count += 1
    LDR R2, count_written
    LDR R3, [R11, R2]
    ADD R3, #1
    STR R3, [R11, R2]

    ;if count equals 3, append radix point to buf, and if count exceeds 3 end.
    MOV R2, #3
    CMP R3, R2
    BEQ append_radix
    BHI end_loop
    B loop_fix

end_loop
    ;No more digits should be left to be written, if this is not the case, there is a size error.
    LDR R4, num
    LDR R4, [R11, R4]
    CMP R4, #0
    BNE size_error

deallocate
    POP {R4, R5}
    ADD R13, #8
    POP {R11, PC}

size_error
    ;write *.*** string to buf then exit with deallocation
    MOV  R2, #42         ;ascii *  = 42
    MOV  R3, #46         ;ascii .  = 46  
    MOV  R4, #0
    ADD R1, #1
    STRB R2, [R1], #1   ;store then increment, all ascii characters are one byte
    STRB R3, [R1], #1    
    STRB R2, [R1], #1
    STRB R2, [R1], #1
    STRB R2, [R1], #1
    STRB R4, [R1]       ;null character to end string
    B deallocate

append_radix
    MOV R2, #46         ;ascii .  = 46
    STRB R2, [R1], #-1
    B loop_fix

    .endasmfunc

    .end                             ; end of file
