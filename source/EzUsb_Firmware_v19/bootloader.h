#ifndef BOOTLOADER_H		//Header sentry
#define BOOTLOADER_H

enum { BOOTLOADER_OFF, SEND_IDENTITY_SYSEX, WAIT_EZ_FW_DATA, WAIT_Z8_FW_DATA, TERMINATE_FW_UPGRADE/*, VERIFY_Z8_CRC */};

// Z8 ENCORE ON CHIP DEBUGGER DEFINITION
#define OCD_FIRST_BYTE				0x80	// used to autodetect ocd bit rate
//ocd commands
#define OCD_WRITE_PRG_MEM_CMD		0x0A	// 
#define OCD_READ_PRG_MEM_CMD		0x0B	// V1.4
#define OCD_WRITE_CTR_REG_CMD		0x04	// 
#define OCD_WRITE_REG_CMD			0x08
#define OCD_READ_FLASH_CRC			0x0E
// ocd datas
#define OCD_FLASH_FIRST_UNLOCK		0x73
#define OCD_FLASH_SECOND_UNLOCK		0x8C
#define OCD_FLASH_MASS_ERASE		0x63
#define OCD_FLASH_CTR_REG_RESET		0x00
#define OCD_CTR_REG_DBGMODE			0x81
// registers addresses
#define OCD_OPTION_REG_ADD			0x0000
#define OCD_FLASH_CTR_REG_ADD		0x0FF8
#define OCD_FLASH_FREQ_REG_HIGH_ADD	0x0FFA
#define OCD_FLASH_FREQ_REG_LOW_ADD	0x0FFB
#define OCD_FLASH_SELECT_PAGE		0x0FF9
		
#define OCD_Z8_FLASH_FREQ_REG	(Z8_FREQ / 1000)

#define I2C_EE_HH_ADDR			0x01
#define SERIAL_ADDR				0x50
#define	I2C_Addr				(SERIAL_ADDR | I2C_EE_HH_ADDR)  
#define	FW_SYSX_DATA_START		11		// f0 00 20 4f 00 01 ss ll aa aa aa  

//#define fw_checksum			sync24_clear_clock_cpt
//#define fw_bl_src				sync24_clear_cont_cpt

enum { FW_BL_SRC_UNKNOWN, FW_BL_SRC_MIDI_IN, FW_BL_SRC_USB_IN };

// BOOTLOADER SYSEX DEFINITION


// FUNCTIONS PROTOTYPES
//void SoftReset(void);
void Check_Bootloader(void);
void SendIdentityBootLoader(void);
void SendFwAcknoledge(unsigned char result);
void AssignWidSysexHeader(void);
void SendDispatchFwSysex(void);
void WaitNextBootLoaderState(unsigned char awaited_state);
void DecodeFirmwareSysex(Midi_In_Struct *midi_struct);
//void EEPROMWriteByte(WORD addr, BYTE value);
BYTE EEPROMWriteVerify(WORD addr, BYTE length, BYTE xdata *buf); //, BYTE buffer_max_length); //TPM EEPROM Write
void WriteSingleRegOCD(WORD address, BYTE value);
void InitFlashFrqAndUnlock(void);

#endif

