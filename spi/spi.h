
#ifndef __spi_h__
#define __spi_h__

#include <stdint.h>

#define HIGH_CS PORTB |= (1 << RADIO_CS)
#define LOW_CS  PORTB &= ~(1 << RADIO_CS)

#define CHIP_NOT_READY (PINB & (1 << RADIO_SO))

// SPI Pins
#define RADIO_CS	2  // PB1 , Arduino - pin 10
#define RADIO_SI	3  // PB3 , Arduino - pin 11
#define RADIO_SO	4  // PB4 , Arduino - pin 12
#define RADIO_SCK	5  // PB5 , Arduino - pin 13

// Prototypes
extern void 	spi_init(void);
extern uint8_t  spi_transfer(uint8_t);


#endif