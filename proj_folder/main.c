#include <stdint.h>
#include <stdio.h>

#include "../inc/tm4c123gh6pm.h"
#include "../inc/IO.h"
#include "../inc/SysTick.h"
#include "../inc/ADC.h"
#include "../inc/StringConversion.h"
#include "../inc/ST7735.h"
#include "../inc/PortsFCInit.h"
//not in use
#include "../inc/Timer2A.h"

#define Bus80MHz     4
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PortC     (*((volatile uint32_t *)0x400063FC))


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void Wait1ms(uint32_t count){
  //accuracy unknown
  uint32_t i;
  for(i=0; i <count*3200; i++){}
}


uint32_t Convert(uint32_t x){
    //result is fixed point to three decimals
    return ( (x * 3300) / 4095);
}

void DisplayResults(uint32_t *data, uint32_t *time, uint32_t size){
    uint32_t i = 0;
    ST7735_SetCursor(0,0);
    ST7735_FillScreen(0x0000);
    for (i = 0; i < size; i++){
        //display all data
        ST7735_SetCursor(0,i);
        ST7735_OutFix( Convert(data[i]) );
        ST7735_OutString(" v");
        ST7735_SetCursor(9,i);
        ST7735_OutString("  |  ");
        ST7735_SetCursor(12,i);
        ST7735_OutFix( (time[i] * 0.0125 ));
    }
}

//semaphore setup
uint32_t data;  //global mailbox
uint32_t flag;  //global flag
uint32_t SysTick_beat = 0;
void SysTick_Handler(void){
    if ( SysTick_beat % 100 == 0)
        PF1 ^= 0x02; //Red on-board LED

    SysTick_beat++;
    data = ADC_In();      // Sample ADC, put in mailbox
    flag = 1;             // Synchronize with main thread
}


const uint32_t num_data = 16;
void GPIOPortF_Handler(void){
    uint32_t i;
    uint32_t adc_read;
    uint32_t last_time;
    uint32_t time_buf[num_data];
    uint32_t data_buf[num_data];
    uint32_t initial_voltage;



    //initiate discharge
    //callibrated for 1k resistor and a 1 milli farad capacitor (RC = 1 second)
    //measure in increments of 0.1875 seconds to get a view of the charge on the capacitor ranging from 0-3 time constants with a 16 intervals
    initial_voltage = Convert(ADC_In());
    ST7735_SetCursor(0,0);
    ST7735_OutString("Collecting Data...");
    ST7735_SetCursor(0,2);
    ST7735_OutString("Initial Voltage: ");
    ST7735_OutFix( initial_voltage );
    GPIO_PORTF_DATA_R |= 0x0E;  //white LED while measuring

    last_time = NVIC_ST_CURRENT_R;
    PortC &= ~(0x20);
    for(i = 0; i < num_data; i++){
        //uncertainty here, each loop to record time and do nothing else takes 26 bus cycles
        adc_read = ADC_In();
        time_buf[i] = (last_time-(NVIC_ST_CURRENT_R))&0x00FFFFFF;
        data_buf[i] = adc_read;
        Wait1ms(1);
    }
    Wait1ms(1000);

    PortC |= 0x20;      //recharge cap
    GPIO_PORTF_DATA_R &= ~(0x0E); PF3 |= 0x08;
    
    DisplayResults(data_buf, time_buf, num_data); 
    IO_Touch();
    ST7735_FillScreen(0x0000); PF3 &= ~(0x08);
    GPIO_PORTF_ICR_R = 0x10;    //acknowledge interrupt
}



int main(void)
{
    DisableInterrupts();
    SysTick_Init();
    PLL_Init(Bus80MHz);     //80MHZ bus clock
    PortF_Init();
    PortC_Init();
    ST7735_InitR(INITR_REDTAB);
    ADC_Init();
    EnableInterrupts();

    //PC4 below, but PC5 on PCB
    PortC |= 0x20;      //charge capacitor
    uint32_t cap_voltage = 0;

    //main thread, real-time voltmeter using periodic ADC_In readings from SysTick 
    while(1){
      IO_HeartBeat(); //Blue on-board LED 
      if (flag == 1){
          //if data in SysTick mailbox
          flag = 0;
          cap_voltage = Convert(data);
      }
      ST7735_SetCursor(0,0);
      ST7735_OutFix(cap_voltage);
      ST7735_OutString(" volts");
    }
}
