
#include "config.h"
#include "reg.h"
#include "radio.h"
#include "spi.h"

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"

void radio_init(void)
{
	DDRD &= ~(1 << 2); // 2 (GD2 End of packet)
	PORTD |= (1 << 2);
	DDRB |= (1 << RADIO_CS);

	// Disable radio 
	PORTB |= (1 << RADIO_CS);
	
	radio_reset();
	radio_update_settings();
}

uint8_t radio_reset(void)
{
	uint8_t result;
	HIGH_CS;
	LOW_CS;
	HIGH_CS;
	_delay_us(45);
	LOW_CS;
	while(CHIP_NOT_READY);
	result = radio_write_strobe(TI_CCxxx0_SRES);
	while(CHIP_NOT_READY);
	return result;
}

uint8_t radio_write_strobe(uint8_t _strobe)
{
	uint8_t result;
	LOW_CS;
	while(CHIP_NOT_READY);
	result = spi_transfer(_strobe);
	return result;
}

uint8_t radio_write_single(	uint8_t addr, 
							uint8_t dat){
	uint8_t result;
	LOW_CS;
	while(CHIP_NOT_READY);
	spi_transfer(addr);
	result = spi_transfer(dat);
	HIGH_CS;
	return result;
}

uint8_t radio_write_burst(	uint8_t addr,
							uint8_t * buff,
							uint8_t len){

	uint8_t result, i;
	LOW_CS;
	while(CHIP_NOT_READY);
	spi_transfer(addr | TI_CCxxx0_WRITE_BURST);

	for(i = 0;i < len;i++)
	{
		result = spi_transfer(buff[i]);
		buff[i] = result;
	}

	HIGH_CS;
	return result;
}

uint8_t radio_read_single(uint8_t addr)
{
	uint8_t result = 0;
	LOW_CS;
	while(CHIP_NOT_READY);
	spi_transfer(addr | TI_CCxxx0_READ_SINGLE);
	// dummy write to read
	result = spi_transfer(0);
	HIGH_CS;
	return result;
}

uint8_t radio_read_status(uint8_t addr)
{
	uint8_t result = 0;
	LOW_CS;
	while(CHIP_NOT_READY);

	spi_transfer(addr | TI_CCxxx0_READ_BURST);
	// dummy write to read
	result = spi_transfer(0);
	HIGH_CS;
	return result;
}

uint8_t radio_read_burst(	uint8_t addr,
							uint8_t * buff,
							uint8_t len){

	uint8_t result, i;
	LOW_CS;
	while(CHIP_NOT_READY);
	spi_transfer(addr | TI_CCxxx0_READ_BURST);

	for (i = 0; i < len; i++)
	{
		result = spi_transfer(0);
		buff[i] = result;
	}
	return result;
}


uint8_t radio_transmit(	uint8_t * txbuff,
						uint8_t len){

	uint8_t result, gd0_state;
	radio_write_burst(TI_CCxxx0_TXFIFO, txbuff, len);

	result = radio_write_strobe(TI_CCxxx0_STX);

	// Wait for GD0 to set
	while(!gd0_state)
	{
		gd0_state = PIND & (1 << 2);
	}

	// Wait for GD0 to clear
	while(gd0_state)
	{
		gd0_state = PIND & (1 << 2);
	}
	return result;
}

uint8_t radio_update_settings(void)
{
	 // Write register settings
	radio_write_single(TI_CCxxx0_IOCFG2, VAL_IOCFG2); // GDO2 output pin config.
	radio_write_single(TI_CCxxx0_IOCFG1, VAL_IOCFG1); // GDO1 output pin config. (high impedance)
	radio_write_single(TI_CCxxx0_IOCFG0, VAL_IOCFG0); // GDO0 output pin config.

	radio_write_single(TI_CCxxx0_PKTLEN, VAL_PKTLEN); // Packet length.
	radio_write_single(TI_CCxxx0_PKTCTRL1, VAL_PKTCTRL1); // Packet automation control.
	radio_write_single(TI_CCxxx0_PKTCTRL0, VAL_PKTCTRL0); // Packet automation control.
	radio_write_single(TI_CCxxx0_ADDR, VAL_ADDR); // Device address.
	radio_write_single(TI_CCxxx0_CHANNR, VAL_CHANNR); // Channel number.


	radio_write_single(TI_CCxxx0_FSCTRL1, VAL_FSCTRL1); // Freq synthesizer control.
	radio_write_single(TI_CCxxx0_FSCTRL0, VAL_FSCTRL0); // Freq synthesizer control.
	radio_write_single(TI_CCxxx0_FREQ2, VAL_FREQ2); // Freq control word, high byte
	radio_write_single(TI_CCxxx0_FREQ1, VAL_FREQ1); // Freq control word, mid byte.
	radio_write_single(TI_CCxxx0_FREQ0, VAL_FREQ0); // Freq control word, low byte.
	radio_write_single(TI_CCxxx0_MDMCFG4, VAL_MDMCFG4); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG3, VAL_MDMCFG3); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG2, VAL_MDMCFG2); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG1, VAL_MDMCFG1); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG0, VAL_MDMCFG0); // Modem configuration.
	radio_write_single(TI_CCxxx0_DEVIATN, VAL_DEVIATN); // Modem dev (when FSK mod en)
	radio_write_single(TI_CCxxx0_MCSM1 , VAL_MCSM1); //MainRadio Cntrl State Machine
	radio_write_single(TI_CCxxx0_MCSM0 , VAL_MCSM0); //MainRadio Cntrl State Machine
	radio_write_single(TI_CCxxx0_FOCCFG, VAL_FOCCFG); // Freq Offset Compens. Config
	radio_write_single(TI_CCxxx0_BSCFG, VAL_BSCFG); // Bit synchronization config.
	radio_write_single(TI_CCxxx0_AGCCTRL2, VAL_AGCCTRL2); // AGC control.
	radio_write_single(TI_CCxxx0_AGCCTRL1, VAL_AGCCTRL1); // AGC control.
	radio_write_single(TI_CCxxx0_AGCCTRL0, VAL_AGCCTRL0); // AGC control.
	radio_write_single(TI_CCxxx0_FREND1, VAL_FREND1); // Front end RX configuration.
	radio_write_single(TI_CCxxx0_FREND0, VAL_FREND0); // Front end RX configuration.
	radio_write_single(TI_CCxxx0_FSCAL3, VAL_FSCAL3); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL2, VAL_FSCAL2); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL1, VAL_FSCAL1); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL0, VAL_FSCAL0); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSTEST, VAL_FSTEST); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_TEST2, VAL_TEST2); // Various test settings.
	radio_write_single(TI_CCxxx0_TEST1, VAL_TEST1); // Various test settings.
	radio_write_single(TI_CCxxx0_TEST0, VAL_TEST0); // Various test settings.

	return 0;
}