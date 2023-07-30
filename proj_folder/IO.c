// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  August 24, 2022
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>


void Wait10milli(uint32_t);

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // PF2 is heartbeat
 GPIO_PORTF_DATA_R ^= 0x04;
}
//------------------------------------


//------------IO_Touch------------
// wait for release and press of the switch
// Input: none
// Output: none
void IO_Touch(void) {
  uint32_t wait_flag = 1;
  uint32_t last_state = 0;
  uint32_t button_state;

  while( wait_flag ){
     button_state = (GPIO_PORTF_DATA_R & 0x01);
     if ( !button_state != last_state && !button_state == 0) {
        wait_flag = 0;
     }
     Wait10milli(2);
     // save the current state as the last state, for next time through the loop
     last_state = !button_state;
  }

}
//----------------------------------
  

//Must really wait for 10ms x count
void Wait10milli(uint32_t count){
  uint32_t i;
  for(i=0; i <count*32000; i++){}
}
