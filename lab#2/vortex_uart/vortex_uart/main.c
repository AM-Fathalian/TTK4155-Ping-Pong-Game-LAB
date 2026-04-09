#define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD1 9600
#define MYUBRR ((float)(F_CPU * 64 / (16 * (float)BAUD1)) + 0.5)
volatile char received_char;



void port_init(){
	//PORTB.DIRSET |= PIN3_bm; //set pb3 as output for LED
	PORTB.DIR |= PIN0_bm; //set pb0 as output Txd
	PORTB.DIR |= PIN1_bm; //set pb1 as input rxd
}

void uart_init() {
			USART3.CTRLA |= USART_RXCIE_bm; //The Receive Complete Interrupt is enabled 
		    USART3.BAUD = (uint16_t)MYUBRR;  //baud rate setting
			//USART3.CTRLC |= USART_CHSIZE_8BIT_gc; //8 data bits
		    //USART3.CTRLC &= ~USART_SBMODE_bm;  //1 bit stop
		    USART3.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
}
	
void uart_send(char chr){ 
	while(!(USART3.STATUS & USART_DREIF_bm)){;}
	USART3.TXDATAL = chr;
}


	
/*
uint8_t uart_receive(){
	while(!(USART3.STATUS & USART_RXCIF_bm));
	return USART3.RXDATAL;
}
*/

ISR(USART3_RXC_vect) {
	received_char = USART3.RXDATAL; //assigning value of received data
	//_delay_ms(500);
	uart_send((received_char)); //echoing it back
}



int main(void)
{
    port_init();
	uart_init();
	
	sei();
	
    while (1) 
    {
		_delay_ms((500));
    }
	
	return 0;
}

