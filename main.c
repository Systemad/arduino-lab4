#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "serial.h"
#include "timer.h"


void main (void) {

	i2c_init();
	uart_init();	

	sei();

	while (1) {

		eeprom_write_byte(0x10, 'd');
		eeprom_wait_until_write_complete();

		eeprom_write_byte(0x11, 'a');
		eeprom_wait_until_write_complete();

		eeprom_write_byte(0x12, 'n');
		eeprom_wait_until_write_complete();

		char ByteOfData = eeprom_read_byte(0x10);
		printf("%c", ByteOfData);

		char ByteOfData2 = eeprom_read_byte(0x11);
		printf("%c", ByteOfData2);

		char ByteOfData3 = eeprom_read_byte(0x12);
		printf("%c", ByteOfData3);
		printf("\n");
		
		_delay_ms(1000);

	}
}
