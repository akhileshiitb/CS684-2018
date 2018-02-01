/*CS 684 Embedded system IIT Bombay
 * LAB1 Task2

* Author: Patil Akhilesh Subhash 173079005

* Description: This code will increment value of sw2Status variable every time switch2 is pressed.

* Global Variables: sw2Status

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

//global variable  to count switch presses
unsigned int sw2Status = 0 ;

//main funciton starts here
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

//internal variables declarations

 unsigned int read_data = 0;

	while(1)
	{
    read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2) ; //read switch2

    //This if construct is used here to pool switch2 and key debouncing purpose
    if(switch2_is_press){
    	SysCtlDelay(200000); //for key debouncing purpose
    	sw2Status++ ; //increment by 1
    	//To ensure there is no mumtiple increments on one press of the switch
    	while(1){
    		read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2) ;
    		if((read_data & 0x00000001)!=0){

    			break ;
    		}

    	}

    }





	}
}
