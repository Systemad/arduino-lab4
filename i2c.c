#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"

#define F_CPU 16000000UL
#define frequency 100000 // 100 kHz as prescaler

// This is the address EEPROM, first 4 bits are control bits, 3 after are irrelevant and last one is READ(0) or WRITE(1) 
#define eepromAdr 0b10100000

void i2c_init(void) //Set the prescaler to 1 and writes our TWBR-value so we get our desired SCL_FREQUENCY.
{
	TWCR &= ~(1 << TWPS1) | ~(1 << TWPS0);
	TWBR = ((F_CPU / frequency - 16) / 2);
}

void i2c_meaningful_status(uint8_t status) {
	switch (status) {
		case 0x08: // START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("START\n"));
			break;
		case 0x10: // repeated START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("RESTART\n"));
			break;
		case 0x38: // NAK or DATA ARBITRATION LOST
			printf_P(PSTR("NOARB/NAK\n"));
			break;
		// MASTER TRANSMIT
		case 0x18: // SLA+W transmitted, ACK received
			printf_P(PSTR("MT SLA+W, ACK\n"));
			break;
		case 0x20: // SLA+W transmitted, NAK received
			printf_P(PSTR("MT SLA+W, NAK\n"));
				break;
		case 0x28: // DATA transmitted, ACK received
			printf_P(PSTR("MT DATA+W, ACK\n"));
			break;
		case 0x30: // DATA transmitted, NAK received
			printf_P(PSTR("MT DATA+W, NAK\n"));
			break;
		// MASTER RECEIVE
		case 0x40: // SLA+R transmitted, ACK received
			printf_P(PSTR("MR SLA+R, ACK\n"));
			break;
		case 0x48: // SLA+R transmitted, NAK received
			printf_P(PSTR("MR SLA+R, NAK\n"));
			break;
		case 0x50: // DATA received, ACK sent
			printf_P(PSTR("MR DATA+R, ACK\n"));
			break;
		case 0x58: // DATA received, NAK sent
			printf_P(PSTR("MR DATA+R, NAK\n"));
			break;
		default:
			printf_P(PSTR("N/A %02X\n"), status);
			break;
	}
}

inline void i2c_start() {

	// Send start condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

inline void i2c_stop() {
	// Send stop condition
	TWCR = (1 << TWINT)  | (1 << TWSTO) | (1 << TWEN);
	
	while ((TWCR & (1 << TWSTO)));
}

inline uint8_t i2c_get_status(void) {

	// Send TWSR to i2c_meaningful_status to print readable status
	i2c_meaningful_status(TWSR);
}

inline void i2c_xmit_addr(uint8_t address, uint8_t rw) {

	// Load address to register
	TWDR = address | rw;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // == 0 not working
}

inline void i2c_xmit_byte(uint8_t data) {
	
	// Load data to register
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

inline uint8_t i2c_read_ACK() {
	TWCR = (1 << TWINT) | (1 << TWEA) | (1<<TWEN); // Send acknowledge bit
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

inline uint8_t i2c_read_NAK() {
	TWCR = (1 << TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

inline void eeprom_wait_until_write_complete() {
	// Acknowledge polling, loop to check if if write is complete
	for(;;){
		i2c_start();
		i2c_xmit_addr(eepromAdr, I2C_W);
		if(TWSR == 0x18) break; // break loop if transmitted and acknowledged
	}
}


uint8_t eeprom_read_byte(uint8_t addr) {
	// Inital reading byte by starting and sending eeprom address and where in eeprom where we want read from
	i2c_start();
	i2c_xmit_addr(eepromAdr, I2C_W); // send write bit
	i2c_xmit_byte(addr);
	
	i2c_start();
	i2c_xmit_addr(eepromAdr, I2C_R); // Send read bit
	
	// Store to read variable and return it
	uint8_t read = i2c_read_NAK();
	i2c_stop(); // stop i2C
	return read;
}

void eeprom_write_byte(uint8_t addr, uint8_t data) {
	
	// Inital reading byte by starting i2c and sending eeprom address and where in eeprom where we want to store
	i2c_start();
	i2c_xmit_addr(eepromAdr, I2C_W);
	i2c_xmit_byte(addr);
	i2c_xmit_byte(data);
	i2c_stop();
}



void eeprom_write_page(uint8_t addr, uint8_t *data) {
	// ... (VG)
}

void eeprom_sequential_read(uint8_t *buf, uint8_t start_addr, uint8_t len) {
	// ... (VG)
}