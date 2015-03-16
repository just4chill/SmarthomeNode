
#include "config.h"
#include "spi.h"
#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

void spi_init(void)
{
	DDRB |= (1 << RADIO_SI) | (1 << RADIO_SCK) | 
			(1 << RADIO_CS);

	// MISO as input
	DDRB &= ~(1 << RADIO_SO);

	// Disable radio 
	PORTB |= (1 << RADIO_CS);

	// Pull low MOSI
	PORTB &= ~(1 << RADIO_SI);

	// Pull high SCK
	PORTB |= (1 << RADIO_SCK);

	// SPI enable and master mode
	SPCR |= (1 << SPE) | (1 << MSTR);

	SPCR &= ~(1 << CPHA);
	SPCR &= ~(1 << CPOL);

	// Double speed
	SPSR |= (1 << SPI2X);
}

uint8_t spi_transfer(uint8_t dat)
{
	// char buff[20];
	uint8_t result;

	// start transmission
    SPDR = dat;

    // wait for end of transmission
    while (!(SPSR & (1 << SPIF))) 
    {
		__asm(
			"NOP\n"
			"NOP\n"
		);
    }
    result = SPDR;

    // return received byte
    return result;
}