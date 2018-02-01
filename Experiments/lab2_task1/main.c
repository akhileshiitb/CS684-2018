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
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include <stdint.h>
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"



//base frequency for servo is 55 Hz
#define PWM_FREQUENCY 55

//to unlock PORTF 0th pin
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F (*((volatile unsigned long *)0x40025524))

//macro for leds for easy reference
#define red_led PWM_OUT_5
#define blue_led PWM_OUT_6
#define green_led PWM_OUT_7

//macro for switches for easy reference
#define switch1 GPIO_PIN_4
#define switch2 GPIO_PIN_0

//macro for switch press
#define switch1_is_press (read_data & 0x00000010)==0
#define switch2_is_press (read_data & 0x00000001)==0

//port values variable  to make LEDs on or off after masking
uint8_t turn_on=0b00001110 ;
uint8_t turn_off=0b00000000 ;

//global variable  to count switch presses
unsigned int sw2Status = 1 ;
unsigned int delay = 4000000 ;
unsigned int read_data = 0 ;
unsigned  int delay_ad = 1 ;
unsigned int counter_sw1 = 0 ;
unsigned int counter_sw2 = 0 ;
unsigned int state_sw1 = 1 ;
unsigned int state_sw2 = 1 ;

unsigned int flag_sw1 = 0 ;
unsigned int flag_sw2 = 0 ;
//main funciton starts here
int main(void)

{

	double delay1 = 0.05 ; //Delay in seconds
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);




	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 400);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 400);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 10);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 10);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 10);

    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);


    // variables used

    unsigned  int level_red = 1;
    unsigned  int level_green = 1;
    unsigned  int level_blue = 1 ;

//timers
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    	TimerLoadSet(TIMER0_BASE, TIMER_A, delay1*SysCtlClockGet());

    	 IntEnable(INT_TIMER0A);
    	 TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    	 IntMasterEnable();

    	 TimerEnable(TIMER0_BASE, TIMER_A);

    	 GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4); //pin4 sw2 pin0 sw1 as input pins (switches)
    	 GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //enable pull down for swichws at pin 0 and 4

    //to unlock pin 4 and 0 to use it as a switch
    	LOCK_F=0x4C4F434BU;
        CR_F=GPIO_PIN_0|GPIO_PIN_4;


    while(1){






 level_red = 255 ;
        for(level_green=1;level_green<=255;level_green+=50){

            PWMPulseWidthSet(PWM1_BASE, red_led, level_red);
            PWMPulseWidthSet(PWM1_BASE, green_led, level_green);
            PWMPulseWidthSet(PWM1_BASE, blue_led, 1);
            SysCtlDelay(delay);
            level_red-= 50 ;

        }


level_green = 255 ;
        for(level_blue=1;level_blue<=255;level_blue+=50){


        	PWMPulseWidthSet(PWM1_BASE, red_led, 1);
            PWMPulseWidthSet(PWM1_BASE, green_led, level_green);
            PWMPulseWidthSet(PWM1_BASE, blue_led, level_blue);
            SysCtlDelay(delay);
            level_green-=50 ;

        }

level_blue = 255 ;


        for(level_red=1;level_red<=255;level_red+=50){
            PWMPulseWidthSet(PWM1_BASE, red_led, level_red);
            PWMPulseWidthSet(PWM1_BASE, green_led, 1);
            PWMPulseWidthSet(PWM1_BASE, blue_led, level_blue);
            SysCtlDelay(delay);
          level_blue-=50 ;
        }



    }

}


void Timer0ISR(void) {



	 TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

      detect_key_press_sw2();
      detect_key_press_sw1();

      if(flag_sw2 == 1){
     	 delay=delay*2 ;
     	 flag_sw2 = 0 ;


      }

      if(flag_sw1 == 1){
    	  delay= delay/2 ;
      	 flag_sw1 = 0 ;

       }
}



void detect_key_press_sw2(){
	// this returns flag
 unsigned int read_data = 0 ; //temporary reading varaible
	switch(state_sw2){
		case 1 :  read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
                  if(switch2_is_press){
                	 state_sw2 = 2 ;
                  }else{
                	  state_sw2 = 1 ;
                  }

	              break;
		case 2 :  read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
		           if(switch2_is_press){
		        	          flag_sw2 = 1 ;
		                	  state_sw2 = 3;

		                  }else{
		                	  state_sw2 = 1 ;
		                  }
	              break;
		case 3 :  read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
              if(switch2_is_press){
      	              flag_sw2 = 0  ;
                               }else{
                            	   state_sw2 = 1 ;
                               }
	              break;


		}



}


void detect_key_press_sw1(){
	// this returns flag
 unsigned int read_data = 0 ; //temporary reading varaible
	switch(state_sw1){
		case 1 :  read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
                  if(switch1_is_press){
                	 state_sw1 = 2 ;
                  }else{
                	  state_sw1 = 1 ;
                  }

	              break;
		case 2 :  read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
		           if(switch1_is_press){
		        	          flag_sw1 = 1 ;
		                	  state_sw1 = 3;

		                  }else{
		                	  state_sw1 = 1 ;
		                  }
	              break;
		case 3 :  read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
              if(switch1_is_press){
      	              flag_sw1 = 0  ;
                               }else{
                            	   state_sw1 = 1 ;
                               }
	              break;


		}

}


