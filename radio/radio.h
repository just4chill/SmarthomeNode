
#ifndef __radio_h__
#define __radio_h__

#include <stdint.h>

#define MOD_ADDRESS  0x60
#define DEST_ADDRESS 0x61

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

#endif