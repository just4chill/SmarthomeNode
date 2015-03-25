
#ifndef __spi_h__
#define __spi_h__

#include <avr/io.h>

extern void spi_init();
extern void spi_transfer_sync(uint8_t *, uint8_t *, uint8_t);
extern void spi_transmit_sync(uint8_t *, uint8_t);
extern uint8_t spi_fast_shift(uint8_t);

#endif