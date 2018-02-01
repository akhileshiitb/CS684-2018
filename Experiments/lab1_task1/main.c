/*

* Author: Patil Akhilesh subhash 

* Editted by: Saurav Shandilya, Vishwanathan Iyer
	      ERTS Lab, CSE Department, IIT Bombay

* Description: This code will test file to check software and hardware setup. This code will blink three colors of RGB LED present of Launchpad board in sequence.

* Filename: lab-0.c

* Functions: setup(), led_pin_config(), main()

* Global Variables: none

*/




#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

//to unlock PORTF 0th pin
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F (*((volatile unsigned long *)0x40025524))

//macro for leds for easy reference
#define red_led GPIO_PIN_1
#define blue_led GPIO_PIN_2
#define green_led GPIO_PIN_3

//macro for switches for easy reference
#define switch1 GPIO_PIN_4
#define switch2 GPIO_PIN_0

//macro for switch press
#define switch1_is_press (read_data & 0x00000010)==0
#define switch2_is_press (read_data & 0x00000001)==0

unsigned int counter = 1 ;

int main(void)
{
//setting system clock and enable peripherals
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

//to unlock pin 4
	LOCK_F=0x4C4F434BU;
    CR_F=GPIO_PIN_0|GPIO_PIN_4;

//pins input and output configurations
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //make portF pins 1 2 3 as output
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4); //pin4 sw2 pin0 sw1 as input pins (switches)
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //enable pull down for swichws at pin 0 and 4

//variables declarations


 uint8_t turn_on=0b00001110 ;
 uint8_t turn_off=0b00000000 ;
 unsigned int read_data = 0;



	while(1)
	{
    read_data=GPIOPinRead(GPIO_PORTF_BASE,switch1|switch2) ; //read switch1 and switch2

    if(switch1_is_press){
    	SysCtlDelay(200000);
    	if(counter==1){
    	    		 GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_on);
    	    	 }else if(counter==2){
    	    		 GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_on);
    	    	 }else if(counter==3){
    	    		 GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_on);

    	    	 }else{
    	    		 GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_off); //green led off
    	    		 GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_off);
    	    		 GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_off);
    	    	 }
     //increment counter and reset for next state

     //wait for release of switch
     while(1){



         read_data=GPIOPinRead(GPIO_PORTF_BASE,switch1|switch2) ; //read switch1 and switch2
         if((read_data & 0x00000010)!=0){
        	 SysCtlDelay(20000);
        	 counter = counter + 1 ;
        	      if(counter==4){
        	      counter=1;
        	        }
        	 break;
         }



     }


    }else{
    	GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_off); //green led off
        GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_off);
        GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_off);
    }





	}
}
