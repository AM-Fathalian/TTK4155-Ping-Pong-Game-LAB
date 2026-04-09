#include "fonts.h"


void init_program()
{
	write_c(0xae); // display off
	write_c(0xa1); //segment remap
	write_c(0xda); //common pads hardware: alternative
	write_c(0x12);
	write_c(0xc8); //common output scan direction:com63~com0
	write_c(0xa8); //multiplex ration mode:63
	write_c(0x3f);
	write_c(0xd5); //display divide ratio/osc. freq. mode
	write_c(0x80);
	write_c(0x81); //contrast control
	write_c(0x50);
	write_c(0xd9); //set pre-charge period
	write_c(0x21);
	write_c(0x20); //Set Memory Addressing Mode
	write_c(0x02);
	write_c(0xdb); //VCOM deselect level mode
	write_c(0x30);
	write_c(0xad); //master configuration
	write_c(0x00);
	write_c(0xa4); //out follows RAM content
	write_c(0xa6); //set normal display
	write_c(0xaf); // display on
}

void sendCommand(unsigned char command) {
	// Set D/C# low for command mode
	digitalWrite(DC_PIN, LOW);  // DC_PIN is the Data/Command control pin
	
	// Set CS# low to select the OLED
	digitalWrite(CS_PIN, LOW);  // CS_PIN is the Chip Select pin

	// Put the command on the data bus
	PORTD = command;  // Assuming PORTD is connected to D[7:0]

	// Set WR# low to write the data, then high to latch it
	digitalWrite(WR_PIN, LOW);
	delayMicroseconds(1);  // Small delay to ensure the write signal is registered
	digitalWrite(WR_PIN, HIGH);

	// Set CS# high to deselect the OLED
	digitalWrite(CS_PIN, HIGH);
}

void sendData(unsigned char data) {
	// Set D/C# high for data mode
	digitalWrite(DC_PIN, HIGH);  // DC_PIN is the Data/Command control pin
	
	// Set CS# low to select the OLED
	digitalWrite(CS_PIN, LOW);  // CS_PIN is the Chip Select pin

	// Put the data on the data bus
	PORTD = data;  // Assuming PORTD is connected to D[7:0]

	// Set WR# low to write the data, then high to latch it
	digitalWrite(WR_PIN, LOW);
	delayMicroseconds(1);  // Small delay to ensure the write signal is registered
	digitalWrite(WR_PIN, HIGH);

	// Set CS# high to deselect the OLED
	digitalWrite(CS_PIN, HIGH);
}

void setColumnAddress(unsigned char start, unsigned char end) {
	sendCommand(0x21);  // Set Column Address command
	sendCommand(start); // Start column (0 to 127)
	sendCommand(end);   // End column (0 to 127)
}

void setPageAddress(unsigned char start, unsigned char end) {
	sendCommand(0x22);  // Set Page Address command
	sendCommand(start); // Start page (0 to 7)
	sendCommand(end);   // End page (0 to 7)
}

void displayChar(char c) {
	// Get the 8x8 bitmap for the character
	const unsigned char* bitmap = font8[c - 32];  // Subtract 32 to get index
	
	// Set column and page addresses where you want to display the character
	setColumnAddress(startColumn, endColumn);  // Adjust as per position on display
	setPageAddress(startPage, endPage);        // Adjust page (row) as per position

	// Send each byte of the character to the display
	for (int i = 0; i < 8; i++) {
		sendData(bitmap[i]);  // Function to send data to GDDRAM
	}
}

void displayString(const char* str) {
	while (*str) {
		displayChar(*str);  // Display each character
		str++;
		// Move the cursor for the next character, typically 8 pixels forward
	}
}