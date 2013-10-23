#ifndef SERIAL_H		//Header sentry
#define SERIAL_H

#define Z8_MEM_SIZE					65536
#define EZUSB_MEM_SIZE				8192

#define XTAL			  			24000000L	// in hertz
#define MIDI_OUT_1_BAUD_RATE		31250L		// in bauds

#define MIDI_OUT_1_BAUD_RELOAD			(65536-(XTAL/(32L*MIDI_OUT_1_BAUD_RATE)))	// timer 2
#define SERIAL_TO_Z8_BAUD_RELOAD		((2*XTAL)/(32*12*BAUD_Z8_EZ))	// timer 1
#define SERIAL_TO_OCD_Z8_BAUD_RELOAD	((2*XTAL)/(32*12*BAUD_OCD_Z8_EZ))	// timer 1 (ON CHIP DEBUGGER Z8)


#define TIMER0_PERIOD				2 // in milliseconds
#define TIMER0_RELOAD				(65536-(((XTAL*TIMER0_PERIOD)/(12*1000L))))

#define SERIAL_TX_BUFF_LENGTH		64 // used as a buffer for MIDI outputs	 // V1.8 16

enum {WAIT_SOF, FRAME_STATUS_BYTE, WAIT_EOF }; // Frame state from Z8 serial
enum {WAIT_FOR_CORRECT_DATA, IN_SYSEX, SYSEX_RECEIVED, NORMAL, FIFO_FULL }; // MIDI Merger State Byte
enum {CN0, CN1 };	 // USB MIDI CABLE NUMBER
enum {SERIAL, USB }; // Bootloader source port

#define CTRL_REG_Z8_FRAME_LENGHT		0x04 // FF SS VV FE FF SS VV FE

// WID BS3X RECOGNIZED SYSEXS (RECEIVED BY BS3X)

// WID BS3X RECOGNIZED SYSEXS (SENT BY BS3X)


// FUNCTION PROTOTYPES
void Init_Serial_Ports(void);
void Send_To_Midi_Out_Con(BYTE data_to_send);
void Check_MIDI_Buff_Send_To_Z8_Board(Midi_In_Struct *midi_struct, BYTE end_index_local);
void Send_To_Serial_Z8(BYTE data_to_send);
void Send_Single_Framed_byte_To_Z8(/*v1.1 BYTE SYSEX_ID0,*/ BYTE SYSEX_ID2, BYTE byte_to_send);

#endif // SERIAL_H
