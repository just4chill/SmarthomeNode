
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
	uint8_t _version, _marcstate, _rxbytes, _txstate, _strobe, packet_len;
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
		_version 	= radio_read_status(TI_CCxxx0_VERSION);
		#ifdef TX_EN
		_txstate 	= radio_transmit_packet((uint8_t *)msg, sizeof(msg), 0x00);
		#endif
		_marcstate  = radio_read_status(TI_CCxxx0_MARCSTATE);

		#ifdef TX_EN
		if(_marcstate == 0x16)	// Tx Buff UNDERFLOW
		{
			_strobe = radio_write_strobe(TI_CCxxx0_SFTX); // Flush TXFIFO
			_marcstate = radio_read_status(TI_CCxxx0_MARCSTATE);
		}
		#else
		_strobe = radio_write_strobe(TI_CCxxx0_SRX);
		_rxbytes = radio_read_status(TI_CCxxx0_RXBYTES) & TI_CCxxx0_NUM_RXBYTES;

		if(_rxbytes > 0)
		{
			packet_len = radio_read_single(TI_CCxxx0_RXFIFO);
			if(packet_len > 0)
			{
				radio_read_burst(TI_CCxxx0_RXFIFO, rxbuff, packet_len);
				radio_read_burst(TI_CCxxx0_RXFIFO, status, 2);
			}
		}
		// else if (_rxbytes >= 24)
		// {
		// 	_strobe = radio_write_strobe(TI_CCxxx0_SFRX);
		// }
		else
		{
			packet_len = 0;
		}

		// if((_strobe & 0b01110000) == 0b01100000)
		// {
		// 	// RX Buffer Overflow
		// 	_strobe = radio_write_strobe(TI_CCxxx0_SFRX);
		// 	_marcstate = radio_read_status(TI_CCxxx0_MARCSTATE);
		// }

		#endif

		#ifdef TX_EN
		sprintf(buff, "%#02x,%#02x,%#02x,%#02x",_version, _marcstate,_txstate, _strobe);
		uart_puts(buff);
		uart_appendcrlf();
		_delay_ms(1000);
		#else
		//sprintf(buff, "%#02x,%#02x,%#02x,%#02x,%#02x",_version, _marcstate, _rxbytes, _strobe, packet_len);
		if(packet_len > 0)
		{
			// for(uint8_t i = 0;i < packet_len;i++)
			// {
			// 	sprintf(buff, "%c",rxbuff[i]);
			// 	uart_puts(buff);
			// }
			
			//_strobe = radio_write_strobe(TI_CCxxx0_SFRX);
			// uart_puts(" Status: ");
			// for(uint8_t i = 0;i < 2;i++)
			// {
			// 	sprintf(buff, "%#02x, ",status[i]);
			// 	uart_puts(buff);				
			// }
			sprintf(buff, "%#02x,%#02x,%#02x,%#02x",status[0],status[1],packet_len,rxbuff[0]);
				uart_puts(buff);	
			uart_appendcrlf();
			_delay_ms(500);
			// sprintf(buff, "%#02x\r\n",status[1] & 0x80);
			// uart_puts(buff);
			//uart_puts(buff);	
		}
		#endif

		// uart_puts(buff);
		// uart_appendcrlf();
		//_delay_ms(100);
	}
	return 0;
}