#ifndef __SIM_MEGA32U4_H__
#define __SIM_MEGA32U4_H__

#ifdef __cplusplus
extern "C" {
#endif



#define SIM_VECTOR_SIZE	4
#define SIM_MMCU	"atmega32u4"
#define SIM_CORENAME	mcu_mega32u4
#define USBRF 5 // missing in avr/iom32u4.h

#define PD7	7
#define PD6	6
#define PD5	5
#define PD4	4
#define PD3	3
#define PD2	2
#define PD1	1
#define PD0	0

#define PE6	6

#include "sim_avr.h"
#include "avr_eeprom.h"
#include "avr_flash.h"
#include "avr_watchdog.h"
#include "avr_extint.h"
#include "avr_ioport.h"
#include "avr_uart.h"
#include "avr_adc.h"
#include "avr_timer.h"
#include "avr_spi.h"
#include "avr_twi.h"
#include "avr_acomp.h"
#include "avr_usb.h"

void m32u4_init(struct avr_t * avr);
void m32u4_reset(struct avr_t * avr);

#define _AVR_IO_H_
#define __ASSEMBLER__
#ifndef __AVR_ATmega32U4__
#define __AVR_ATmega32U4__
#endif
#include "avr/iom32u4.h"

#include "sim_core_declare.h"


extern avr_kind_t mega32u4;



#ifdef __cplusplus
};
#endif

#endif // __SIM_MEGA32U4_H__
