
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

volatile uint8_t rxbuff[25] = {0};
volatile uint8_t status[2];
//#define TX_EN

int main(void)
{
	uart_init();
	spi_init();
	radio_init();
	//adc_init();
	uart_cls();
	uart_puts("System started\r\n");	
	while(1)
	{
		char buff[25];	
		uint16_t temp = 0;
		uint8_t _marcstate, _txresult, _rxbytes, _txfifo, _rxresult, _idleresult, _len = 0;
		_txresult = radio_transmit_packet(msg, sizeof(msg), DEST_ADDRESS);
		_marcstate = radio_read_status(TI_CCxxx0_MARCSTATE);
		if(_marcstate == 0x16)
		{
			_txresult = radio_write_strobe(TI_CCxxx0_SFTX);
			_marcstate = radio_read_status(TI_CCxxx0_MARCSTATE);
		}
		_txfifo = radio_read_status(TI_CCxxx0_TXBYTES);
		_rxresult = radio_write_strobe(TI_CCxxx0_SRX);
		_rxbytes = radio_read_status(TI_CCxxx0_RXBYTES);
		if(_rxbytes > 0)
		{
			_len = radio_read_single(TI_CCxxx0_RXFIFO);
		}
		sprintf(buff, "%#02x,%#02x,%#02x,%#02x,%#02x,%#02x,%#02x,%#02x,%#02x\r\n", 
																		((_txresult & 0b01110000) >> 4),
																		(_txresult & 0b00001111),
																		((_txfifo & 0x80) >> 7), 
																		(_txfifo & 0x7F),
																		((_rxresult & 0b01110000) >> 4),
																		(_rxresult & 0b00001111),
																		 _marcstate, 
																		 _rxbytes,
																		 _len);
		uart_puts(buff);
		_delay_ms(1000);
	}
	
	return 0;
}