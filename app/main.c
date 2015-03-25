
#include "config.h"
#include "spi.h"
#include "radio.h"
#include "reg.h"
#include "uart.h"
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mirf.h"
#include <avr/interrupt.h>

// SCK PB5 (SCK)
// MISO PB4 (MISO)
// MOSI PB3 (MOSI)
// CSN PB1
// CE PB0
// IRQ PD2 (INT0)

uint8_t txBuff[100];
uint8_t rxBuff[100];

int main(void)
{
	uart_init();
	spi_init();
	// radio_init();
	uart_cls();
	uart_puts("System started\r\n");

	spi_init();
		// Initialize AVR for use with mirf
	mirf_init();
	// Wait for mirf to come up
	_delay_ms(50);
	// Activate interrupts
	sei();
	// Configure mirf
	mirf_config();
	// Test communication

	uint8_t i = 0;
	for(uint8_t j = 0x30;i < 0x7E;i++)
	{
		txBuff[i++] = j;
	}
	mirf_send(txBuff,strlen(txBuff));
	if(mirf_data_ready())
	{
		mirf_get_data(rxBuff);	
	}
	
	// while(1)
	// {
	// 	// Tx
	// 	uint8_t i = 0;
	// 	for(uint8_t j = 0x30;i < 0x7E;i++)
	// 	{
	// 		txBuff[i++] = j;
	// 	}
	// 	radio_write_single(TI_CCxxx0_TXFIFO, strlen(txBuff));
	// 	radio_write_single(TI_CCxxx0_TXFIFO, 0x00);
	// 	radio_transmit(txBuff, strlen(txBuff));

	// 	// Rx

	// }
		
	return 0;
}