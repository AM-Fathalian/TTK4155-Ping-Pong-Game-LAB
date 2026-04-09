#include "sam.h"
#include "pwm_driver.h"
#include <stdio.h>

void setupPWM() {
	// Step 1: Enable PWM peripheral clock
	PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));


	// Step 2: Configure PWM pins
	PIOB->PIO_ABSR |= PIO_ABSR_P13;     // Select Peripheral B
	PIOB->PIO_PDR |= PIO_PDR_P13;       // Disable GPIO mode for PWM
	// prescaler
	// Step 3: Configure PWM clock
	PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);  // 1 MHz

	// Step 4: Configure PWM channel
	PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA;   // Use CLKA
	PWM->PWM_CH_NUM[1].PWM_CPRD = 20000;               // Period
	//PWM->PWM_CH_NUM[1].PWM_CDTY = 500;                // 50% duty cycle

	// Step 5: Enable PWM channel
	PWM->PWM_ENA = PWM_ENA_CHID1;
}


void set_duty(int x){
	unsigned int y;
	y = 6*x;
	PWM->PWM_CH_NUM[1].PWM_CDTY = 20000-(1500+y);
	printf("DUTY CYCLE   %d \n\r", 20000-(1500+y));
}

