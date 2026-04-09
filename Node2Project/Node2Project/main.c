/*
 * Node2Project.c
 *
 * Created: 18.10.2024 18:23:20
 * Author : denisaip
 */ 


#include "sam.h"
#include "can.h"
#include "uart.h"
#include "time.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ADC.h"
#include "control.h"

#define F_CPU 84000000
#define BAUD 9600
#define SERVO_PIN PB13


int joystickPosition(int val) {
	if (val < 127) {
		return ((val-128)*100/127);
	}
	return ((val-127)*100)/127;
}

void delay(){
	for(int i=0; i<100000000; i++){}
}



int main(void)
{
   SystemInit();
   WDT->WDT_MR = WDT_MR_WDDIS; //keti: Disable Watchdog Timer //important not to shut down mcu

	// Initialize UART
	uart_init(F_CPU, BAUD);

	setup_can();

    CanMsg message;
	CanMsg hits;

	setupPWM();
	setupMOTOR();
	//set_duty(-100);
	
	ADC_init();
	
	encoder_init();
	
	int adc_val;
	
	
	setup_punch_pin();
	while (1) {
		
		//uint32_t position = TC2->TC_CHANNEL[0].TC_CV;
		//printf("Position: %u", position);
		
		can_rx(&message);
			
		set_duty_MOTOR(joystickPosition(message.byte[2])); //other axis for joystick
		set_duty(joystickPosition(message.byte[0]));
			  
		//printf("\nDUTY    %d\n\r", joystickPosition(message.byte[0]));
			 
		if (message.byte[1] == 1 ) {
			printf("LOOP\n\n\r");
					clear_punch_pin();

			delay();
			delay();
			delay();

			printf("\nByte 0th :  %d\n\r", message.byte[1]);
		}
			set_punch_pin();
			
	//	printf("encoder position: %d\n", readEncoderPosition());
printf("\nByte 0th :  %d\n\r", message.byte[2]);
		
		//set_duty_MOTOR(-50);
		//delay();
		//delay();
		////set_duty_MOTOR(50);
		//delay();
		//delay();
		//set_duty_MOTOR(-50);
		//printf("\nByte 0th :  %d\n\r", message.byte[0]);
		//printf("\nPunch PIN val :  %d\n\r", message.byte[1]);

		//printf("ADC Value: %u\n", ADC_read());
		
		
		//keti: sending data to node 1 for the shoot count:/
		printf("\n\rADC for count: %d\n", ADC_read()%255);
		hits = (CanMsg){
			        1, 1, (char)(ADC_read()%255)
			    };
		can_tx(hits);
		
	}
}