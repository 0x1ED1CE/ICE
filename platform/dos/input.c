#include <go32.h>
#include <dpmi.h>
#include <dos.h>

#include "ice.h"

#define KEYBOARD_INT 0x09
#define MOUSE_INT    0x33

#define MAX_KEYS 128

static volatile unsigned char key_states[MAX_KEYS];

static _go32_dpmi_seginfo old_keyboard_isr;
static _go32_dpmi_seginfo new_keyboard_isr;

static unsigned int  mouse_status;
static signed int    mouse_offset_x;
static signed int    mouse_offset_y;
static unsigned char mouse_buttons;

void keyboard_isr() {
	//Get scancode from keyboard
	unsigned char scan_code = inportb(0x60);

	//Ignore extended byte
	if (scan_code!=0xE0) {
		key_states[scan_code&0x7F]=scan_code>>7;
	}

	//Acknowledge interrupt
	outportb(0x20,0x20);
}

ice_uint ice_input_init() {
	//Initialize keyboard
	for (ice_uint i=0; i<MAX_KEYS; i++) {
		key_states[i]=1;
	}

	_go32_dpmi_lock_code(
		keyboard_isr,
		(long)sizeof(keyboard_isr)
	);

	_go32_dpmi_lock_data(
		(void *)&key_states,
		(long)sizeof(key_states)
	);

	_go32_dpmi_get_protected_mode_interrupt_vector(
		KEYBOARD_INT,
		&old_keyboard_isr
	);

	new_keyboard_isr.pm_offset   = (int)keyboard_isr;
	new_keyboard_isr.pm_selector = _go32_my_cs();

	_go32_dpmi_chain_protected_mode_interrupt_vector(
		KEYBOARD_INT,
		&new_keyboard_isr
	);

	//Initialize mouse
	union REGS registers={};

	registers.x.ax = 0;

	int86(
		MOUSE_INT,
		&registers,
		&registers
	);

	mouse_status   = registers.x.ax;
	mouse_offset_x = 0;
	mouse_offset_y = 0;
	mouse_buttons  = 0;

	if (mouse_status) {
		//Set sensitivity
		registers.x.ax = 0x1A;
		registers.x.bx = 1;
		registers.x.cx = 1;
		registers.x.dx = 64;

		int86(
			MOUSE_INT,
			&registers,
			&registers
		);

		//Set position
		registers.x.ax = 0x04;
		registers.x.cx = 16;
		registers.x.dx = 16;

		int86(
			MOUSE_INT,
			&registers,
			&registers
		);
	} else {
		ice_log((ice_char*)"Mouse not detected!");
	}

	return 0;
}

void ice_input_deinit() {
	_go32_dpmi_set_protected_mode_interrupt_vector(
		KEYBOARD_INT,
		&old_keyboard_isr
	);
}

void ice_input_update() {
	if (mouse_status) {
		//Get mouse position and button status
		union REGS registers={};

		registers.x.ax = 0x03;

		int86(
			MOUSE_INT,
			&registers,
			&registers
		);

		mouse_offset_x += registers.x.cx-16;
		mouse_offset_y += registers.x.dx-16;
		mouse_buttons   = registers.x.bx;

		//Set mouse position back to origin
		registers.x.ax = 0x04;
		registers.x.cx = 16;
		registers.x.dx = 16;

		int86(
			MOUSE_INT,
			&registers,
			&registers
		);
	}
}

ice_char ice_input_poll(
	ice_uint *device_id,
	ice_uint *sensor_id,
	ice_real *input_state
) {
	*device_id   = 0;
	*sensor_id   = 0;
	*input_state = 0;

	//TODO

	return 0;
}

ice_real ice_input_get(
	ice_uint device_id,
	ice_uint sensor_id
) {
	ice_real temp;

	switch(device_id) {
		case ICE_INPUT_DEVICE_KEYBOARD:
			if (sensor_id<MAX_KEYS) {
				return key_states[sensor_id]==0?1.0f:0.0f;
			}

			break;
		case ICE_INPUT_DEVICE_MOUSE:
			switch(sensor_id) {
				case ICE_INPUT_SENSOR_MOUSE_OFFSET_X:
					temp = (ice_real)mouse_offset_x;
					mouse_offset_x = 0;

					return temp;
				case ICE_INPUT_SENSOR_MOUSE_OFFSET_Y:
					temp = (ice_real)mouse_offset_y;
					mouse_offset_y = 0;

					return temp;
				case ICE_INPUT_SENSOR_MOUSE_LEFT_BUTTON:
					return (ice_real)(mouse_buttons&1);
				case ICE_INPUT_SENSOR_MOUSE_RIGHT_BUTTON:
					return (ice_real)(mouse_buttons>>1);
			}
	}

	return 0;
}