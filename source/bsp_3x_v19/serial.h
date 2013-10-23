#ifndef SERIAL
#define SERIAL

///////////////////////////////////////////////////////////
// Local defines
///////////////////////////////////////////////////////////
//#define BAUD0 		31250      				// 57.6K baud for UART0
#define BAUD1 	 	31250      				// 31.25K baud for UART1

#define BRG0 	Z8_FREQ/((unsigned long)BAUD_Z8_EZ*16)
#define BRG1 	Z8_FREQ/((unsigned long)BAUD1*16)

#define TDRE	4U

#define TX_BUFF_LENGTH			128U // V1.5 128U
#define SERIAL_IN_BUFF_LENGHT	256 // V1.5 128U

// TX_STATUS bits definitions
#define tx0_busy_txstat			(1<<6U)
#define tx0_full_txstat			(1<<5U)
#define tx1_busy_txstat			(1<<4U)
#define tx1_full_txstat			(1<<3U) 

#define STD_MODE_MIDI_STR_LENGHT	3U	// Midi str is 3 bytes lenght in standard mode

#define UPLOAD_Z8_FRAME_LENGHT		0x06U 	// FF SS LL AA/NN 00/GG FE
#define LCD_MESS_Z8_FRAME_LENGHT	0x05U 	// FF SS LL AA FE
#define FRAME_HEADER_LENGHT			0x07U	// FF SS LL AA/NN 00/GG ... CS FE

// Programmation Frame mapping (valable only for FRAME_TYPE different than pure midi)
enum { FRAME_SOF_POS, FRAME_TYPE_POS, FRAME_DATA_LENGHT_POS, FRAME_ADDR_INDEX_POS, FRAME_GROUP_POS, FRAME_DATA_START };
// control_status_1 is byte 1 (nibbles 2 or 3), switch_mode is byte 2 then nibble is 2 since MSB nibble first
#define CTR_STATUS_1_SW_MODE_NIBBLE_INDEX	2U
#define CTR_STATUS_1_SW_MODE_MSK	(switch_ctrl_hook_mode >> 4U)

#define INVALID_REQUEST				0xFFU

#define DEFAULT_INTERNAL_TEMPO		100 // 30 will be added : 130

// Prog control bits definitions
// Bit 7 is always 0
//#define prog_with_ack			(1<<4U)	// bit 4 = ack prog request
//#define midi_zlcd_str			(1<<3U)	// bit 3 = select midi/lcd str upload / prog

//enum {MIDI_NOERROR, MIDI_ERROR};       // Used by prog_result
//enum {MIDI_STR_ACK, LCD_STR_ACK};      // Used by programming/upload     
enum {CURVE_LINEAR, CURVE_INV_LIN, CURVE_LOG, CURVE_INV_LOG, CURVE_RANDOM, CURVE_USER_0, CURVE_USER_1};
enum {MIDI_STRUCT_VALID, MIDI_STRUCT_NOT_VALID};       // Used by prog_result

// Prog user curves bits definitions
// Bit 7 is always 0
//#define user_curve_select		(1<<4)	// bit 4 = select user curve 0 or 1
//#define curve_high_low_select	(1<<3)	// bit 3 = select 64 low/high curve bytes to prog 

//#define ACK_SYSX_LENGHT			12 // f0 00 20 4f 00 01 

//////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitSerial(void);
void SendToTx0(uchar value);
void SendToTx1(uchar value);
void CheckSerialInBuffer(void);
void SendControlToMidi(void);
void SendDispatchToMidi(far uchar *buff_ptr, uchar midi_str_size, uchar destination);
void LoadMidiDelay(void);
//void SendProgAckToMidi(uchar ack_type, uchar prog_result);
void SendUploadGenericBufferToMidi(uchar SYSEX_ID0_byte, uchar SYSEX_ID1_byte, uchar SYSEX_ID2_byte, uchar SYSEX_ID3_byte);
void SendCtrlRegToEzBoard(uchar ctrl_reg_to_ez);
uchar Prog_Upload_Common_Param(uchar SYSEX_ID0_byte, uchar serial_in_start_index);
uchar Prog_Upload_Ctrl_Assign(uchar SYSEX_ID0_byte, uchar serial_in_start_index);
void  Truncate_Current_Value(void);
uchar Serial_Nibb_To_Generic_Bytes(uchar start_index, uchar lenght);
void Byte_To_Generic_Nibbles(uchar start_index, uchar lenght, far uchar *buff_src);
uchar Check_Sysex_Validity(uchar SYSEX_ID0, uchar SYSEX_ID2, uchar SYSEX_ID3);
void SendControlInAutotestMode(uchar outputs); // V1.5
uchar CheckMidiStruct(void);
void ReloadSwitchModeFromSysex(uchar sw_index);
void InsertChecksumInMidiString(void); // V1.3
void ParsePureMidiInBuffer(uchar end_index_local, uchar SYSEX_ID0_byte); // V1.5
unsigned char Get_MIDI_Msg_Size(unsigned char status);
void CheckIfEzMustForwardMidiToZ8(void); // V1.5
void FillParamCurrentStruct(void); // V1.61
void FillLcdCurrentStruct(void); // V1.61

#endif