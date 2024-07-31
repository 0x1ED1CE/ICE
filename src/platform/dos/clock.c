//This uses RTC since PIT has been reprogrammed for audio

#include <dos.h>
#include <go32.h>
#include <dpmi.h>

#include "ice.h"

#define RTC_INT 0x70
#define RTC_DIV 6    //32768 >> (6-1) = 1024Hz

#define RTC_INDEX_PORT 0x70 //Register select
#define RTC_VALUE_PORT 0x71 //Register value

#define RTC_REG_A 0x8A
#define RTC_REG_B 0x8B
#define RTC_REG_C 0x0C

static _go32_dpmi_seginfo old_isr;
static _go32_dpmi_seginfo new_isr;

static volatile unsigned long clock_tick;

void rtc_isr() {
	clock_tick++;

	//Acknowledge interrupt
	outportb(
		RTC_INDEX_PORT,
		RTC_REG_C
	);
	inportb(RTC_VALUE_PORT);
}

ice_uint ice_clock_init() {
	clock_tick=0;
	/*
	//Install interrupt handler
	_go32_dpmi_lock_code(
		rtc_isr,
		(long)sizeof(rtc_isr)
	);

	_go32_dpmi_lock_data(
		(void *)&clock_tick,
		(long)sizeof(clock_tick)
	);

	_go32_dpmi_get_protected_mode_interrupt_vector(
		RTC_INT,
		&old_isr
	);

	new_isr.pm_offset   = (int)rtc_isr;
	new_isr.pm_selector = _go32_my_cs();

	_go32_dpmi_chain_protected_mode_interrupt_vector( //Don't chain
		RTC_INT,
		&new_isr
	);

	char pre_value;

	//Disable interrupts
	disable();

	//Enable RTC IRQ
	outportb(RTC_INDEX_PORT,RTC_REG_B);
	pre_value = inportb(RTC_VALUE_PORT);
	outportb(RTC_INDEX_PORT,RTC_REG_B);
	outportb(RTC_VALUE_PORT,pre_value|0x40);

	//Change RTC interrupt rate
	//outportb(RTC_INDEX_PORT,RTC_REG_A);
	//pre_value = inportb(RTC_VALUE_PORT);
	//outportb(RTC_INDEX_PORT,RTC_REG_A);
	//outportb(RTC_VALUE_PORT,(pre_value&0xF0)|RTC_DIV);

	//Enable interrupts
	enable();
	*/
	return 0;
}

void ice_clock_deinit() {
	/*
	_go32_dpmi_set_protected_mode_interrupt_vector(
		RTC_INT,
		&old_isr
	);

	disable();

	//Disable IRQ
	outportb(RTC_INDEX_PORT,RTC_REG_B);
	char pre_value = inportb(RTC_VALUE_PORT);
	outportb(RTC_INDEX_PORT,RTC_REG_B);
	outportb(RTC_VALUE_PORT,pre_value&~0x40);

	enable();
	*/
}

ice_real ice_clock_get() {
	return (ice_real)clock_tick/1024;
}
