#ifndef USB_DEFS_H		//Header sentry
#define USB_DEFS_H

// MIDI-USB DEFINITIONS
#define MIDI_USB_FRAME_LENGHT	        0x04
#define USB_EP_BUFFER_LENGHT			64

#define USB_IN_USES_ENDPOINT			2 // 2

#if USB_IN_USES_ENDPOINT == 1
#define USB_IN_ID				IN1BUF_ID 	// use endpoint 1
#define USB_IN_BUFFER			IN1BUF		// use endpoint 1
#define USB_PAIRED_EP			0x08		// enpoint 2 out paired
#define USB_ITRS_EP_IN			bmEP1		// itr on in ep1 is used
#define USB_EP_IN_DESC			0x81				
#endif

#if USB_IN_USES_ENDPOINT == 2
#define USB_IN_ID				IN2BUF_ID 	// use endpoint 2
#define USB_IN_BUFFER			IN2BUF		// use endpoint 2
#define USB_PAIRED_EP			0x09		// enpoint 2 in& 2 out paired
#define USB_ITRS_EP_IN			bmEP2		// itr on in ep2 is used
#define USB_EP_IN_DESC			0x82
#endif

// MSB=message length + LSB=code identification number
#define USB_MISC_UNUSED  	            0x00
#define USB_UNUSED  	                0x01
#define USB_TWO_BYTE_SYS_COM	        0x22
#define USB_TREE_BYTE_SYS_COM	        0x33
#define USB_SYSEX_START_IN  	        0x34
#define USB_SYSEX_END_SINGLE	        0x15
#define USB_SYSEX_END_TWO    	        0x26
#define USB_SYSEX_END_TREE    	        0x37

#define USB_NOTE_OFF  	                0x38
#define USB_NOTE_ON  	                0x39
#define USB_POLY_KEYPRESS	            0x3A
#define USB_CONT_CHANGE	                0x3B
#define USB_PROG_CHANGE                 0x2C
#define USB_CHAN_PRESSURE  	            0x2D
#define USB_PITCHBEND                   0x3E
#define USB_SINGLE_BYTE                 0x1F
#endif
