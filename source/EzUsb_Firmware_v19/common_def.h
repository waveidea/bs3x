#ifndef COMMON_DEF_H		//Header sentry
#define COMMON_DEF_H

// GLOBAL DEFINES
// #define EZ_COMPIL 1

//#define SIMUL 1

// MIDI MERGERS BUFFERS LENGHT
#define MIDI_IN_FROM_CON_BUFF_LENGHT	256	// V1.5 128
#define MIDI_IN_FROM_USB_BUFF_LENGHT	256 // V1.5 128
#define SERIAL_IN_FROM_Z8_BUFF_LENGHT	256 // V1.5 128 
#define MIDI_IN_REALTIME_BUFF_LENGHT	8

typedef struct {
BYTE start_index;							// start index on input buffer
volatile BYTE end_index;					// end index on input buffer
BYTE merger_state;							// merger state of input buffer
volatile BYTE current_index;				// current index on input buffer
volatile BYTE input_status;					// Status register : see bits definition below
BYTE buffer[MIDI_IN_FROM_CON_BUFF_LENGHT];	// input buffer
} Midi_In_Struct;


// bits definition for input_status in the aove structure
#define input_overflow			(1 << 0)	// bit 0 : input overflow

typedef struct {
BYTE realtime[MIDI_IN_REALTIME_BUFF_LENGHT];// realtime midi input buffer
BYTE realtime_start_index;					// current index on realtime buffer
BYTE realtime_end_index;					// current index on realtime buffer
volatile BYTE input_status;					// Status register : see bits definition below
} Midi_In_Realtime_Struct;

// bits definition for input_status in the above realtime structure
#define realtime_overflow		(1 << 0)	// bit 0 : realtime input overflow

// enums for the following registers updated from Z8 board
// enum {CTRL_REG_0_7BITS_FROM_Z8,CTRL_REG_1_7BITS_FROM_Z8  };

#endif