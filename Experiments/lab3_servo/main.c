/*
*CS684 Embedded Systems IIT Bombay LAB1 Task3
* Author: Patil Akhilesh Subhash 173079005

* Description:  manual mode of PWM LED controls

* Global Variables: counter_sw1 , counter_sw2, turn_on , turn_off
*
* I have defined some macros at the begining so simplify understanding of code.

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

//to unlock PORTF 0th pin which was initially locled
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

//state variable for colour and delay (model as FSM with 3 state)
unsigned int counter_sw1 = 0 ;
unsigned int counter_sw2 = 0 ;
unsigned int state_sw1 = 1 ;
unsigned int state_sw2 = 1 ;

unsigned int flag_sw1 = 0 ;
unsigned int flag_sw2 = 0 ;
double delay = 0.05 ; //Delay in seconds
unsigned int delay_normal = 400000 ;

unsigned int level = 1 ;
unsigned int location = 34 ;
unsigned int mode = 0 ;
unsigned int global_counter = 0 ;
//port values variable  to make LEDs on or off after masking
uint8_t turn_on=0b00001110 ;
uint8_t turn_off=0b00000000 ;

//Function to Make all LEDs off
void lit_off(){
	GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_off);
	GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_off);
	GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_off);

}


void timer_config(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
		TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

		TimerLoadSet(TIMER0_BASE, TIMER_A, delay*SysCtlClockGet());

		 IntEnable(INT_TIMER0A);
		 TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
		 IntMasterEnable();

		 TimerEnable(TIMER0_BASE, TIMER_A);

}

void PWM_config(){
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

		SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

		GPIOPinConfigure(GPIO_PF1_M1PWM5);
		GPIOPinConfigure(GPIO_PF2_M1PWM6);
		GPIOPinConfigure(GPIO_PF3_M1PWM7);

		GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

		PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
		PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 12500);
	    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 12500);

	    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 10);
	    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 10);
	    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 10);

	    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);

}

void colour_spectrum(){

;
}

void update_counters(){
	 if(flag_sw2 == 1){
	    	 counter_sw2++ ;
	    	 level = level + 34;
	    	 flag_sw2 = 0 ;
	                    }

		 if(flag_sw1 == 1){
		     counter_sw1++;
		     level = level - 34;
		      flag_sw1 = 0 ;
		       }

}



//Main function starts here
int main(void)
{

//setting system clock and enable peripherals
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

//to unlock pin 4 and 0 to use it as a switch
	LOCK_F=0x4C4F434BU;
    CR_F=GPIO_PIN_0|GPIO_PIN_4;

    timer_config();
    PWM_config();
//pins input and output configurations

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4); //pin4 sw2 pin0 sw1 as input pins (switches)
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //enable pull down for swichws at pin 0 and 4

    unsigned int read_data = 0 ;
level = 625 ;
 while(1){
update_counters();


	 PWMPulseWidthSet(PWM1_BASE, blue_led, level);


   }//end of main while loop
}//end of main function



void Timer0ISR(void) {

	 TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

      detect_key_press_sw2();
      detect_key_press_sw1();

      global_counter++ ;
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


