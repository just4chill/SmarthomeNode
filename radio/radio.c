
#include "config.h"
#include "reg.h"
#include "radio.h"
#include "spi.h"

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"
#include <avr/interrupt.h>

static uint8_t tx_buffer[20];

volatile uint8_t isr_flag = 0;

#define enableInterrupt GICR |= (1 << INT0)
#define disableInterrupt GICR &= ~(1 << INT0)

ISR(INT0_vect) 
{
	// if(isr_flag)
	// {
	// 	isr_flag = 0;
	// 	PORTD &= ~(1 << 3);
	// }
	// else
	// {
	// 	isr_flag = 1;
	// 	PORTD |= (1 << 3);
	// }
	isr_flag++;
	if (isr_flag <  100 && isr_flag >  0)
	{
		PORTD &= ~(1 << 3);
	}
	else
		PORTD |= (1 << 3);
}

void radio_init(void)
{
	DDRC &= ~(1 << 0); // A0 (Temp Sensor GD0)
	DDRD &= ~(1 << 2); // 2 (GD2 End of packet)
	PORTC &= ~(1 << 0);
	PORTD |= (1 << 2);

	DDRD |= (1 << 3); // LED
	PORTD |= (1 << 3);

	radio_reset();
	radio_update_settings();

	MCUCR |= (1 << ISC01); // Any logical change
	enableInterrupt;
	sei();
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

	uint8_t result;
	//disableInterrupt;
	radio_write_burst(TI_CCxxx0_TXFIFO, txbuff, len);

	result = radio_write_strobe(TI_CCxxx0_STX);
	// while(!(PIND & (1 << 2)));
	// while((PIND & (1 << 2)));
	//PORTD &= ~(1 << 2);
	//enableInterrupt;
	return result;
}

uint8_t radio_transmit_packet(	uint8_t * txbuff,
								uint8_t len,
								uint8_t destination){

	tx_buffer[LENGTH_FIELD_INDEX]  = len;
	tx_buffer[ADDRESS_FIELD_INDEX] = destination;

	memcpy(&tx_buffer[DATA_FIELD_INDEX], txbuff, len);

	return radio_transmit(tx_buffer, len + 2);
}

uint8_t radio_receive_packet(	uint8_t * rxbuff,
								uint8_t len){
	uint8_t status[2];
	uint8_t packet_len;

	if((radio_read_status(TI_CCxxx0_RXBYTES) & TI_CCxxx0_NUM_RXBYTES))
	{
		packet_len = radio_read_single(TI_CCxxx0_RXFIFO);

		// FIXME packet_len <= *len
		if(packet_len <= len)
		{
			radio_read_burst(TI_CCxxx0_RXFIFO, rxbuff, packet_len);
			len = packet_len;

			radio_read_burst(TI_CCxxx0_RXFIFO, status, 2);

			memcpy(&rxbuff[packet_len], status, 2);

			return (status[TI_CCxxx0_LQI_RX] & TI_CCxxx0_CRC_OK);
		}
		else
		{
			// FIXME
			len = packet_len;
			radio_write_strobe(TI_CCxxx0_SFRX);
			return 0;
		}
	}
	return 0;
}


// Product = CC2500
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state: Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 2433.000000 MHz
// Channel spacing = 199.950000 kHz
// Channel number = 0
// Optimization = Sensitivity
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2) 4 bytes
// Append status = 1
// Address check = Address check and 0 (0x00) broadcast
// CRC autoflush = true
// Device address = 1
// GDO0 signal selection = ( 0x06 ) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = ( 0x0E ) Carrier sense. High if RSSI level is above threshold.

uint8_t radio_update_settings(void)
{
	// Write register settings
	radio_write_single(TI_CCxxx0_IOCFG2, 0x06); // GDO2 output pin config.
	radio_write_single(TI_CCxxx0_IOCFG0, 0x80); // GDO0 output pin config.
	radio_write_single(TI_CCxxx0_PKTLEN, 0x3D); // Packet length.
	radio_write_single(TI_CCxxx0_PKTCTRL1, 0x0E); // Packet automation control.
	radio_write_single(TI_CCxxx0_PKTCTRL0, 0x05); // Packet automation control.
	// radio_write_single(TI_CCxxx0_PKTCTRL1, 0x04); // Packet automation control.
	// radio_write_single(TI_CCxxx0_PKTCTRL0, 0x05); // Packet automation control.
	radio_write_single(TI_CCxxx0_ADDR, MOD_ADDRESS); // Device address.
	radio_write_single(TI_CCxxx0_CHANNR, 0x00); // Channel number.
	radio_write_single(TI_CCxxx0_FSCTRL1, 0x07); // Freq synthesizer control.
	radio_write_single(TI_CCxxx0_FSCTRL0, 0x00); // Freq synthesizer control.
	radio_write_single(TI_CCxxx0_FREQ2, 0x5D); // Freq control word, high byte
	radio_write_single(TI_CCxxx0_FREQ1, 0x93); // Freq control word, mid byte.
	radio_write_single(TI_CCxxx0_FREQ0, 0xB1); // Freq control word, low byte.
	radio_write_single(TI_CCxxx0_MDMCFG4, 0x2D); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG3, 0x3B); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG2, 0x73); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG1, 0x22); // Modem configuration.
	radio_write_single(TI_CCxxx0_MDMCFG0, 0xF8); // Modem configuration.
	radio_write_single(TI_CCxxx0_DEVIATN, 0x00); // Modem dev (when FSK mod en)
	radio_write_single(TI_CCxxx0_MCSM1 , 0x2F); //MainRadio Cntrl State Machine
	radio_write_single(TI_CCxxx0_MCSM0 , 0x18); //MainRadio Cntrl State Machine
	radio_write_single(TI_CCxxx0_FOCCFG, 0x1D); // Freq Offset Compens. Config
	radio_write_single(TI_CCxxx0_BSCFG, 0x1C); // Bit synchronization config.
	radio_write_single(TI_CCxxx0_AGCCTRL2, 0xC7); // AGC control.
	radio_write_single(TI_CCxxx0_AGCCTRL1, 0x00); // AGC control.
	radio_write_single(TI_CCxxx0_AGCCTRL0, 0xB2); // AGC control.
	radio_write_single(TI_CCxxx0_FREND1, 0xB6); // Front end RX configuration.
	radio_write_single(TI_CCxxx0_FREND0, 0x10); // Front end RX configuration.
	radio_write_single(TI_CCxxx0_FSCAL3, 0xEA); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL2, 0x0A); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL1, 0x00); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSCAL0, 0x11); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_FSTEST, 0x59); // Frequency synthesizer cal.
	radio_write_single(TI_CCxxx0_TEST2, 0x88); // Various test settings.
	radio_write_single(TI_CCxxx0_TEST1, 0x31); // Various test settings.
	radio_write_single(TI_CCxxx0_TEST0, 0x0B); // Various test settings.

	return 0;
}