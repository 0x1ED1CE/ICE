#include <dpmi.h>
#include <pc.h>

#define KEYBOARD_INTERRUPT_VECTOR 0x09

#define MAX_KEYS 128

#define BUTTON_UP   0
#define BUTTON_DOWN 1

#define DEVICE_KEYBOARD 0
#define DEVICE_MOUSE    1
#define DEVICE_JOYSTICK 2

volatile unsigned char key_states[MAX_KEYS];
volatile unsigned int  key_read;
volatile unsigned int  key_write;

_go32_dpmi_seginfo old_handler;
_go32_dpmi_seginfo new_handler;

void keyboard_handler() {
	static unsigned char scan_code;
	
	scan_code = inportb(0x60); //Get scancode from keyboard
	
	if (
		scan_code!=0xE0 //Ignore extended byte
		//&& key_write!=key_read
	) {
		key_states[scan_code&0x7F]=scan_code>>7;
		
		//key_write=(key_write+1)%KEY_BUFFER_SIZE;
	}
	
	outportb(0x20,0x20); //Acknowledge interrupt
}

ice_uint ice_input_init() {
	for (ice_uint i=0; i<MAX_KEYS; i++) {
		key_states[i]=1;
	}
	
	new_handler.pm_offset = (int)keyboard_handler;
	
	//Save old keyboard handler to restore later
	_go32_dpmi_get_protected_mode_interrupt_vector(
		KEYBOARD_INTERRUPT_VECTOR,
		&old_handler
	);
	
	//Chain new keyboard handler
	_go32_dpmi_chain_protected_mode_interrupt_vector(
		KEYBOARD_INTERRUPT_VECTOR,
		&new_handler
	);
	
	return 0;
}

void ice_input_deinit() {
	//Restore old keyboard handler
	_go32_dpmi_set_protected_mode_interrupt_vector(
		KEYBOARD_INTERRUPT_VECTOR,
		&old_handler
	);
}

ice_char ice_input_poll(
	ice_uint *device_id,
	ice_uint *input_id,
	ice_real *input_state
) {
	*device_id   = 0;
	*input_id    = 0;
	*input_state = BUTTON_UP;
	
	return 0;
}

ice_real ice_input_get(
	ice_uint device_id,
	ice_uint input_id
) {
	if (device_id==DEVICE_KEYBOARD) {
		if (input_id<MAX_KEYS) {
			return key_states[input_id]==0?1.0f:0.0f;
		}
	}
	
	return 0;
}