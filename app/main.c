
#include "config.h"
#include "spi.h"
#include "radio.h"
#include "reg.h"
#include "uart.h"
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "adc.h"

const char msg[] = "Hello World!";


int main(void)
{
	uint8_t result1, result2;
	uint16_t temp;
	char buff[20];
	uart_init();
	spi_init();
	radio_init();
	adc_init();
	uart_cls();
	uart_puts("System started\r\n");
	
	while(1)
	{
		result1 = radio_read_status(TI_CCxxx0_VERSION);
		radio_transmit_packet((uint8_t *)msg, sizeof(msg), 0x00);
		result2 = radio_read_status(TI_CCxxx0_MARCSTATE);

		if(result2 == 0x16)
		{
			uart_puts("TX UNDERFLOW\r\n");
			radio_write_strobe(TI_CCxxx0_SFTX); // Flush TXFIFO
			result2 = radio_read_status(TI_CCxxx0_MARCSTATE);
		}

		//temp = analogRead(0);
		sprintf(buff, "%#02x,%#02x",result1, result2);
		
		uart_puts(buff);
		uart_appendcrlf();
		_delay_ms(1000);
	}
	return 0;
}