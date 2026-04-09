#include "sam.h"
#include "ADC.h"
#include <stdio.h>

#define LOWRES 4

void ADC_init () {

	
	// Step #1: enable ADC controller MCK in PMC
	PMC->PMC_PCER1 |= (1 << (ID_ADC - 32));
	


	ADC->ADC_CR |= ADC_CR_SWRST;  
	
	ADC->ADC_MR |= ADC_MR_PRESCAL(50);   // Prescaler 50
	//| ADC_MR_STARTUP_SUT512                                           // Startup time
	//| ADC_MR_SETTLING_AST3
	//| ADC_MR_TRANSFER(2)                                            // Settling time
	//| ADC_MR_TRACKTIM(1);
	
	//ADC->ADC_MR &= ~(1<<LOWRES);
	//ADC->ADC_MR &= ~ADC_MR_LOWRES;  // Set to 12-bit resolution
	ADC->ADC_MR |= ADC_MR_FREERUN_ON;
	ADC->ADC_CHDR = -1;
	ADC->ADC_CHER = (1 << ADC_CHER_CH0);
	ADC->ADC_CR = ADC_CR_START; //set start bit for the conversion
	
}

int ADC_read() {
	
	//while ((ADC->ADC_ISR & ADC_ISR_DRDY) == ADC_ISR_DRDY); // Wait until the conversion is done (DRDY flag is set)
	//return ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk;
	return ADC->ADC_LCDR; // Return the conversion result for channel 0
}