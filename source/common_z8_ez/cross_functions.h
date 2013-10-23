#ifndef CROSS_FUNCTIONS
#define CROSS_FUNCTIONS


//---------------------------------------------------------------------------
// COMMON BITSTREAM 3X CONFIGURATION SOFTWARE CROSS-PLATFORM FUNCTIONS
//---------------------------------------------------------------------------
// jerome dumas copyright Wave Idea 2004
// jdumas@waveidea.com
//---------------------------------------------------------------------------

#define SYSEX_BUFFER_MAX	128

#define IDENTITY_LENGHT				11
#define IDENTITY_READONLY_LENGHT	(IDENTITY_LENGHT - 2)
#define EZ_IDENTITY_ADDRESS			0x80
#define Z8_IDENTITY_ADDRESS			0x0E00
#define EZ_IDENTITY_READONLY_ADR	(EZ_IDENTITY_ADDRESS+2) // first 2 bytes are writable (fw version)
#define Z8_IDENTITY_READONLY_ADR	(Z8_IDENTITY_ADDRESS+2) // first 2 bytes are writable (fw version)
#define I2C_EE_HEADER				7 // b2 + 6 bytes of vid/pid

#define CTRL_UNDEF 0xFF

#define Ctrl_Midi_Struct All_Params

enum {	GROUP_0,GROUP_1,GROUP_2,GROUP_3,GROUP_4, GROUP_5,
		GROUP_6,GROUP_7,GROUP_8,GROUP_9,GROUP_10, GROUP_11,
		GROUP_12,GROUP_13,GROUP_14,GROUP_15,GROUP_16, GROUP_17,
		GROUP_18,GROUP_19,GROUP_20};

enum {COMMON_PARAMETER_TYPE, LCD_ASSIGN_TYPE, MIDI_ASSIGN_TYPE, ANY_SYSEX_TYPE};
enum {DOWNLOAD_WITH_ACK_REQUEST, DOWNLOAD, UPLOAD };
enum {START_FW_UPGRADE, DOWNLOAD_EZ, DOWNLOAD_Z8, PREPARE_Z8_UPGRADE, FINISH_FW_UPGRADE };
enum {CHECK_FW_ACKNOLEDGE_RESULT, CHECK_MIDI_ACKNOLEDGE_RESULT, CHECK_LCD_ACKNOLEDGE_RESULT, FILL_STRUCTURE_FROM_UPLOAD, FILL_LCD_STR_FROM_UPLOAD, FILL_COMMON_PARAM_FROM_UPLOAD};
enum {ACK_ANSWER_NO_ERROR }; //, ACK_ANSWER_ERROR};
enum {DIGITAL_CTRL, ANALOG_CTRL};
enum {INIT_EMPTY, INIT_DEFAULT};
enum {// these are not error
		MIDI_STRUCT_NO_ERROR,
		// These are warning only
		LCD_STRING_NO_ERROR,MIDI_STRUCT_UNDEF_WARNING, MIDI_STR_DELAY_REQUIRED_WARNING, 
		MIDI_CHECKS_END_WARNINGS,
		// these are errors
		MIN_SUP_127_ERROR,MAX_SUP_127_ERROR,MIN_SUP_MAX_ERROR,MIDI_STR_LENGHT_ERROR,MIDI_STR_CHAN_POS_ERROR,
		MIDI_STR_VAL0_POS_ERROR,MIDI_STR_VAL1_POS_ERROR,MIDI_STR_CHKSUM_POS_ERROR, MIDI_STR_STATUS_MISSING_ERROR,
		MIDI_STR_INVALID_BYTE_ERROR, NO_MIDI_OUT_ERROR, CTRL_CHAIN_LOOP_ERROR, CTRL_CHAIN_RANGE_ERROR, DUPLICATE_POS_ERROR, 
		MIDI_STR_UNCOMPL_ERROR, CTRL_NIBBLE_ERROR, LCD_STR_CAR_ERROR, IMPORT_BSPRO_LIB_ERROR, LCD_STR_LENGHT_ERROR,
		// V1.5 some checks on global params
		TEMPO_MIN_ERROR, TEMPO_MAX_ERROR, MACKIE_FAD_FBK_GLOBAL_WARNING, MACKIE_FAD_FBK_INDIVIDUAL_WARNING };

#define WAIT_FIRMWARE_ACK               1

#define FIRMWARE_DATA_MAX_Z8            48 //12 //DBG 48
#define FIRMWARE_DATA_MAX_EZ            48 //must be <= page of 24c64=32 bytes
#define UPLOAD_SYSEX_LENGHT             11                      // F0 00 20 4F 00 01 UU LL NN GG F7
#define DOWNLOAD_LCD_STR_SYSEX_LENGHT   44                      // F0 00 20 4F 00 01 UU LL NN GG + 32 nibbles + CS + F7
#define SYSEX_DATA_START                10                      // F0 00 20 4F 00 01 UU LL NN GG
#define SYSEX_HEADER_SIZE               7                       // F0 00 20 4F 00 01 UU
//#define LCD_STRING_LENGHT               16                      // lcd str is always 16 chars
#define DOWNLOAD_LCD_STR_NIBBLES_LENGHT (/*LCD_STRING_LENGHT*/LCD_STR_MAX_LENGHT<<1)  // lcd str is always 16 bytes = 32 nibbles
#define INVALID_SYSEX                   0xFF
#define INVALID_REQUEST                 0xFE
#define INVALID_BFW                     0xFD
#define ACK_ANSWER_ERROR				0xFC
#define MISSING_ANSWER_ERROR			0xFB
#define FIRMWARE_ERROR_BASE_NB			0xF0
#define Z8_FW_SIZE_MAX                  65536
#define EZ_FW_SIZE_MAX                  8192

//#define FORWARD_CTRL_INDEX		0x04U	// as defined in firmware
#define MIDI_STR_LENGHT_REQUIRING_DELAY	10	// From midi lenght > 10 it would be preferable to insert MIDI delay
/*
// first index of NVM registers dedicated to EZ
#define CTRL_REG_TO_EZ_START_INDEX	 CTRL_REG_0_7BITS_TO_EZ_INDEX	
// last index of NVM register that can be programmed by external sysex
// #define LAST_EXTERNAL_PARAM_INDEX    IDENTITY_REQUEST_INDEX
#define LAST_EXTERNAL_PARAM_INDEX    GROUP_NAME_INDEX
// last index of NVM register that can be programmed by external sysex or EZ board
#define LAST_COMMON_PARAM_INDEX      LCD_MESSAGE_TO_Z8_INDEX
// Nb of registers updated from the Z8 board
#define CTRL_REG_NB_FROM_Z8_TO_EZ	 (LAST_CTRL_REG_7BITS_TO_EZ_INDEX-CTRL_REG_0_7BITS_TO_EZ_INDEX+1)		
*/
// also used for LCD messages to display from Z8

//SYSEX ID3 (BYTE 8)
//  bit 7  	: always 0
//  bits 6:0 	: group or address	: control assign group GG


// FUNCTIONS PROTOTYPES

// SUB FUNCTIONS
uchar Check_invalid_request(uchar SYSEX_ID0, uchar SYSEX_ID2, uchar SYSEX_ID3);
void  Fill_Sysex_Header(uchar *sysex, uchar SYSEX_ID0, uchar SYSEX_ID1,uchar SYSEX_ID2, uchar SYSEX_ID3);
uchar Single_Byte_To_Nibbles(uchar data, uchar *destination);
uchar Nibbles_To_Single_Byte(uchar *data, uchar *nibbles_src);
uchar Check_Valid_Sysex_Header(uchar *sysex, uchar index, uchar group);
uchar Get_Common_Param_Data_Lenght(uchar address);
uchar GetControlType(uchar index);

// USEFULL FUNCTIONS ARE THERE
uchar Bs3x_Lcd_Assign_To_Sysex(uchar action, uchar *lcd_string, uchar *sysex, uchar index, uchar group);
uchar Bs3x_Midi_Assign_To_Sysex(uchar action, Ctrl_Midi_Struct *MidiStruct, uchar *sysex, uchar index, uchar group);
uchar Bs3x_Sysex_To_Lcd_Assign(uchar action, uchar *lcd_string, uchar *sysex, uchar index, uchar group);
uchar Bs3x_Sysex_To_Midi_Assign(uchar action, Ctrl_Midi_Struct *MidiStruct, uchar *sysex, uchar index, uchar group);
uchar Bs3x_Common_Param_To_Sysex(uchar action, uchar address, uchar *values, uchar *sysex, uchar group);
uchar Bs3x_Sysex_To_Common_Param(uchar address, uchar *values, uchar *sysex, uchar group);

uchar Bs3x_Firmware_To_Sysex(uchar action, uint address, uchar lenght, uint *values, uchar *sysex);
uchar Bs3x_BfwFile_To_Firmwares(char *filename, uchar *identity, uint *ez_fw_size, uint *z8_fw_size, uint *ez_firmware, uint *z8_firmware);
uchar Bs3x_Sysex_To_Firmware(uchar action, uchar *sysex);

uchar CheckMidiStruct(Ctrl_Midi_Struct *MidiStruct, uchar index);
uchar CheckLcdString(uchar *lcd_str);
void  InitMidiStruct(Ctrl_Midi_Struct *MidiStruct, uchar group, uchar index, uchar init_type);
void  InitLcdString(uchar *lcd_str, uchar group, uchar index, uchar init_type);
void  InitCommonParams(uchar *common_params, uchar init_type, float fw_version_float); // V1.5
void  InitUserCurves(uchar *user_curve, uchar init_type); // V1.2

#endif

