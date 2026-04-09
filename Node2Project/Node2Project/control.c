#include "control.h"
#include "sam.h"
#include <stdlib.h>

void encoder_init() {
	PMC->PMC_PCER0 |= (1 << 29);  // Enable clock for TC2 using PID29
	PIOC->PIO_PDR |= (1 << 25) | (1 << 26);   // Disable PIO control for PC25 and PC26
	PIOC->PIO_ABSR |= (1 << 25) | (1 << 26);  // Select Peripheral B for PC25 and PC26

	//PMC->PMC_PCER0 |= (1 << ID_TC6);  // Enable clock for TC2

	TC2->TC_WPMR &= 0xff; //DISABLE WRITE PROTECTION

	TC2->TC_BMR |= TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA | TC_BMR_SPEEDEN;  // Enable quadrature decoder and position mode
	TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;  // Select XC0 as clock source for TC2 Channel 0
	
	TC2->TC_CHANNEL[0].TC_CMR &= ~TC_CMR_WAVE;
	TC2->TC_CHANNEL[0].TC_CMR |= TC_CMR_ETRGEDG_RISING; //TC_CMR.ETRGEDG = 0x01
	TC2->TC_CHANNEL[0].TC_CMR |= TC_CMR_ABETRG; //TC_CMR.ABETRG = 0x1
	
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;  // Enable and reset the counter
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;  // Reset the counter

	TC2->TC_WPMR &= 511; //ENABLE WRITE PROTECTION
	
	
}

int readEncoderPosition() {
	// Read position from the quadrature decoder
	//TC_CMR0.WAVE = 0;
	return TC2->TC_CHANNEL[0].TC_CV;
}

void resetEncoderPosition() {
	// Reset the encoder position by disabling and re-enabling the counter
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN;
}

float calculateSpeed(int positionDifference, float timeIntervalSec) {
	// Convert position difference to speed (counts per second)
	return positionDifference / timeIntervalSec;
}


void setupMOTOR() { //reusing
	// Enable clocks for Port C and Port B
	PMC->PMC_PCER0 |= (1 << ID_PIOC) | (1 << ID_PIOB);

	// Configure PC23 (Phase) as output
	PIOC->PIO_PER = PIO_PC23;   // Enable control of PC23 via PIO
	PIOC->PIO_OER = PIO_PC23;   // Enable output
	
	PIOB->PIO_SODR = PIO_PB12;

	// Step 1: Enable PWM peripheral clock
	PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));


	// Step 2: Configure PWM pins
	PIOB->PIO_ABSR |= PIO_ABSR_P12;     // Select Peripheral B
	PIOB->PIO_PDR |= PIO_PDR_P12;       // Disable GPIO mode for PWM
	// prescaler
	// Step 3: Configure PWM clock
	PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);  // 1 MHz

	// Step 4: Configure PWM channel
	PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA;   // Use CLKA
	PWM->PWM_CH_NUM[0].PWM_CPRD = 50;               // Period
	//PWM->PWM_CH_NUM[1].PWM_CDTY = 500;                // 50% duty cycle

	// Step 5: Enable PWM channel
	PWM->PWM_ENA = PWM_ENA_CHID0;
	
	//set_duty_MOTOR(100);
	
	
	
		// Step 1: Enable PWM peripheral clock
		//PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));


		//// Step 2: Configure PWM pins
		//PIOB->PIO_ABSR |= PIO_ABSR_P12;     // Select Peripheral B
		//PIOB->PIO_PDR |= PIO_PDR_P12;       // Disable GPIO mode for PWM
		//// prescaler
		//// Step 3: Configure PWM clock
		//PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);  // 1 MHz
//
		//// Step 4: Configure PWM channel
		//PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA;   // Use CLKA
		//PWM->PWM_CH_NUM[0].PWM_CPRD = 20000;               // Period
		////PWM->PWM_CH_NUM[1].PWM_CDTY = 500;                // 50% duty cycle
//
		//// Step 5: Enable PWM channel
		//PWM->PWM_ENA = PWM_ENA_CHID0;
}

void set_duty_MOTOR(int x){
	//// Determine direction
	if (x > 35) {
		PIOC->PIO_SODR = PIO_PC23;  // Set PC23 HIGH for forward direction
		PWM->PWM_CH_NUM[0].PWM_CDTY = x/2-1;
		} 
	else if (x < 20) {
		PIOC->PIO_CODR = PIO_PC23;  // Set PC23 LOW for reverse direction
		PWM->PWM_CH_NUM[0].PWM_CDTY = -(x/2-1);
		
	}
	else
	{
		PWM->PWM_CH_NUM[0].PWM_CDTY = 0;
	}
	
	//printf("DUTY CYCLE   %d \n\r", 20000-(1500+y));
	
		//unsigned int y;
		//y = 6*x;
		//PWM->PWM_CH_NUM[0].PWM_CDTY = 20000-(1500+y);
		//printf("DUTY CYCLE   %d \n\r", 20000-(1500+y));
}







// SOLENOID STUFF:
void setup_punch_pin() {
	// Step 1: Enable PIOA clock
	PMC->PMC_PCER0 |= (1 << ID_PIOA);  // Enable PIOA clock

	// Step 2: Configure PA10 as an output
	PIOA->PIO_OER = (1 << 10);         // Enable output on PA10
	PIOA->PIO_PER = (1 << 10);         // Enable control of PA10 by PIOA

	// Step 3: Set PA10 high
	
}

void clear_punch_pin() {
	PIOA->PIO_CODR = PIO_PA10;
}

void set_punch_pin() {
	PIOA->PIO_SODR = PIO_PA10;
}