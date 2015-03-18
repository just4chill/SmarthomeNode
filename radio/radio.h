
#ifndef __radio_h__
#define __radio_h__

#include <stdint.h>

#define CH_NUMBER   1
#define DEV_ADDR	0x05

#define LENGTH_FIELD_INDEX 	0
#define ADDRESS_FIELD_INDEX 1
#define DATA_FIELD_INDEX	2

#define radioSetAddress(addr) radio_write_single(TI_CCxxx0_ADDR, addr)
#define radioSetChannel(chan) radio_write_single(TI_CCxxx0_CHANNR, chan)
#define radioSetPower(powr) radio_write_burst(TI_CCxxx0_PATABLE, &powr,1)

extern void radio_init(void);
extern uint8_t radio_reset(void);
extern uint8_t radio_write_strobe(uint8_t);
extern uint8_t radio_write_single(uint8_t, uint8_t);
extern uint8_t radio_write_burst(uint8_t, uint8_t *, uint8_t);
extern uint8_t radio_read_single(uint8_t);
extern uint8_t radio_read_status(uint8_t);
extern uint8_t radio_read_burst(uint8_t, uint8_t *, uint8_t);
extern uint8_t radio_enable_addressing(void);
extern uint8_t radio_disable_addressing(void);
extern uint8_t radio_sleep(void);
extern uint8_t radio_transmit(uint8_t *, uint8_t);
extern uint8_t radio_transmit_packet(uint8_t *, uint8_t, uint8_t);
extern uint8_t radio_receive_packet(uint8_t *, uint8_t);
extern uint8_t radio_update_settings(void);

#define VAL_IOCFG2 0x29
#define VAL_IOCFG1 0x2E
#define VAL_IOCFG0 0x06
#define VAL_FIFOTHR 0x07
#define VAL_SYNC1 0xD3
#define VAL_SYNC0 0x91
#define VAL_PKTLEN 0xFF
#define VAL_PKTCTRL1 0x0C
#define VAL_PKTCTRL0 0x05
#define VAL_ADDR 0x00
#define VAL_CHANNR 0x00
#define VAL_FSCTRL1 0x08
#define VAL_FSCTRL0 0x00
#define VAL_FREQ2 0x5D
#define VAL_FREQ1 0x93
#define VAL_FREQ0 0xB1
#define VAL_MDMCFG4 0x86
#define VAL_MDMCFG3 0x83
#define VAL_MDMCFG2 0x03
#define VAL_MDMCFG1 0x22
#define VAL_MDMCFG0 0xF8
#define VAL_DEVIATN 0x44
#define VAL_MCSM2 0x07
#define VAL_MCSM1 0x30
#define VAL_MCSM0 0x18
#define VAL_FOCCFG 0x16
#define VAL_BSCFG 0x6C
#define VAL_AGCCTRL2 0x03
#define VAL_AGCCTRL1 0x40
#define VAL_AGCCTRL0 0x91
#define VAL_WOREVT1 0x87
#define VAL_WOREVT0 0x6B
#define VAL_WORCTRL 0xF8
#define VAL_FREND1 0x56
#define VAL_FREND0 0x10
#define VAL_FSCAL3 0xA9
#define VAL_FSCAL2 0x0A
#define VAL_FSCAL1 0x00
#define VAL_FSCAL0 0x11
#define VAL_RCCTRL1 0x41
#define VAL_RCCTRL0 0x00
#define VAL_FSTEST 0x59
#define VAL_PTEST 0x7F
#define VAL_AGCTEST 0x3F
#define VAL_TEST2 0x88
#define VAL_TEST1 0x31
#define VAL_TEST0 0x0B
#define VAL_PARTNUM 0x80
#define VAL_VERSION 0x03

#endif