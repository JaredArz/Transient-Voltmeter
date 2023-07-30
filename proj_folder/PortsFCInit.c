//Enable interrupts on PF4
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

void PortF_Init(void){
  volatile int delay;
  SYSCTL_RCGCGPIO_R |= 0x20;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTF_DIR_R |= 0x0E;
  GPIO_PORTF_DEN_R |= 0x0E;

  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x11;         // allow changes to PF4
  GPIO_PORTF_DIR_R &= ~0x11;      // make PF4,0 in (built-in button)
  GPIO_PORTF_DEN_R |= 0x11;       // enable digital I/O on PF4,0
  GPIO_PORTF_PUR_R |= 0x11;       // enable pull-up on PF4,0
  GPIO_PORTF_IS_R &= ~0x10;       // PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;      // PF4 is not both edges
  GPIO_PORTF_IEV_R |= 0x10;       // PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;        // clear flags 4
  GPIO_PORTF_IM_R |= 0x10;        // arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;        // enable interrupt 30 in NVIC
}

void PortC_Init(void){
//digital enable PC4
    volatile int delay;
    SYSCTL_RCGCGPIO_R |= 0x04; 
    delay = SYSCTL_RCGCGPIO_R;
    GPIO_PORTC_DIR_R  |= 0x20;  
    GPIO_PORTC_DEN_R  |= 0x20;
}

