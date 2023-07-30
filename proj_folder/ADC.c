// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 8/24/2022  
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Labs 8, 9, and 10 specify PD2
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function using PD2 
// Input: none
// Output: none
void ADC2_Init(void){
    SYSCTL_RCGCADC_R     |=  0x01;
    SYSCTL_RCGCGPIO_R    |=  0x08;   //PD2 for ADC input, activate port D
    while(SYSCTL_PRGPIO_R&0x08 == 0){};

    GPIO_PORTD_DIR_R     &= ~0x04;   //make pin 2 input
    GPIO_PORTD_AFSEL_R   |=  0x04;
    GPIO_PORTD_DEN_R     &= ~0x04;   //disable digital input
    GPIO_PORTD_AMSEL_R   |=  0x04;

    //ADC0_PC_R &= ~0x0F;              //IDK what this does
    ADC0_PC_R |= 0x01;               // 8) configure for 125K samples/sec

    ADC0_SSPRI_R   = 0x0123;         // 9) Sequencer 3 is highest priority
    ADC0_ACTSS_R  &= ~0x08;          // 10) disable sample sequencer 3
    ADC0_EMUX_R   &= ~0xF000;        // 11) seq3 is software trigger
    ADC0_SSMUX3_R &= ~0x0F;          //set bottom 4 bits to select channel 5
    ADC0_SSMUX3_R += 5;              // 12) set channel
    ADC0_SSCTL3_R  = 0x06;           // 13) no TS0 D0, yes IE0 END0
    ADC0_IM_R &=     ~0x08;          // 14) disable SS3 interrupts
    ADC0_SAC_R = 5;
    ADC0_ACTSS_R  |= 0x08;           // 15) enable sample sequencer 3
}

void ADC_Init(void){
    SYSCTL_RCGCADC_R     |=  0x01;
    SYSCTL_RCGCGPIO_R    |=  0x08;   //PD3 for ADC input, activate port D
    while(SYSCTL_PRGPIO_R&0x08 == 0){};

    GPIO_PORTD_DIR_R     &= ~0x10;   //make pin 3 input
    GPIO_PORTD_AFSEL_R   |=  0x10;
    GPIO_PORTD_DEN_R     &= ~0x10;   //disable digital input
    GPIO_PORTD_AMSEL_R   |=  0x10;

    ADC0_PC_R |= 0x01;               // 8) configure for 125K samples/sec

    ADC0_SSPRI_R   = 0x0123;         // 9) Sequencer 3 is highest priority
    ADC0_ACTSS_R  &= ~0x08;          // 10) disable sample sequencer 3
    ADC0_EMUX_R   &= ~0xF000;        // 11) seq3 is software trigger
    //wrong bits
    ADC0_SSMUX3_R &= ~0x0F;          //set bottom 4 bits to select channel 5
    ADC0_SSMUX3_R += 4;              // 12) set channel
    ADC0_SSCTL3_R  = 0x06;           // 13) no TS0 D0, yes IE0 END0
    ADC0_IM_R &=     ~0x08;          // 14) disable SS3 interrupts
    ADC0_SAC_R = 5;
    ADC0_ACTSS_R  |= 0x08;           // 15) enable sample sequencer 3
}



//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){
  uint32_t result;
  ADC0_PSSI_R = 0x0008;              // 1) initiate SS3
  while((ADC0_RIS_R&0x08) == 0){};   // 2) wait for conversion done
  result = ADC0_SSFIFO3_R&0xFFF;     // 3) read result
  ADC0_ISC_R = 0x0008;               // 4) acknowledge completion
  return result;
}


