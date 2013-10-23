#ifndef EXPANSION
#define EXPANSION

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////

#include "eeprom.h"

#define EXPANSION_MAX_NB			8U			// Cannot be higher than 8
#define EXP_ANA_MAX_PER_MODULE		16U			
#define EXP_DIG_MAX_PER_MODULE		16U
#define EXP_MAX_TOTAL_PER_MODULE	EXP_ANA_MAX_PER_MODULE + EXP_DIG_MAX_PER_MODULE

#if EXP_ANA_MAX_PER_MODULE > EXP_DIG_MAX_PER_MODULE
#define CURRENT_EXP_MAX_VALUES	EXP_ANA_MAX_PER_MODULE
#else
#define CURRENT_EXP_MAX_VALUES	EXP_DIG_MAX_PER_MODULE
#endif


typedef struct {
	uchar exp_id_found;
	uchar nb_an_ctrl;
	uchar nb_dig_ctrl;
} expansion_struct;

//////////////////// I2C ADDRESSES ////////////////////
///////////////////////////////////////////////////////

// MSB 				LSB
// 0-1-1-0-A2-A1-A0-R/W : To Read/Write from/to RAM
// 0-1-1-1-A2-A1-A0-R/W : To Read/Write from/to FLASH
// 0-1-0-1-A2-A1-A0-R/W : To Read/Write from/to SPI
// 1-0-1-0-A2-A1-A0-R/W : Reserved to bs3x eeproms 24c256

#define EXP_I2C_SLAVE_ADR_RAM		0x60U	// + A2 A1 A0 + RW
#define EXP_I2C_SLAVE_ADR_FLASH		0x70U	// + A2 A1 A0 + RW
#define EXP_I2C_SLAVE_ADR_SPI_EE	0x50U	// + A2 A1 A0 + RW

// Expansion registers addresses
#define EXP_CONFIG_REG_NB			16U 
#define EXP_STATUS_REG_NB			16U 

#define EXP_ID_REG_ADR				0x00U
#define EXP_GLOB_STATUS_REG_ADR		0x01U
#define EXP_CONFIG_REG_ADR			0x02U 
#define EXP_STATUS_REG_ADR			(EXP_CONFIG_REG_ADR   + EXP_CONFIG_REG_NB)
#define EXP_CTRL_ANA_REG_ADR		(EXP_STATUS_REG_ADR   + EXP_STATUS_REG_NB)
#define EXP_CTRL_DIG_REG_ADR		(EXP_CTRL_ANA_REG_ADR + EXP_ANA_MAX_PER_MODULE)

// Expansion registers Bits definition

// EXP_ID_REG_ADR
// Bits 7-4 : Expansion ID (0 to 15)
// Bits 3-0 : Expansion Firmware version (0-15)

// EXP_GLOB_STATUS_REG_ADR
#define exp_gbl_status_an_changed 	(1<<7)	 // Bit 7 : An analog control has changed : 1
#define exp_gbl_status_dig_changed 	(1<<6)	 // Bit 6 : A digital control has changed : 1


// EXP_CONFIG_REG_ADR
// CONFIG[0] : address 0x02 in expansion module
#define hook_status_to_exp		(1 << 7)	// Bit 7	: Hook mode (analog) enabled = 1 / disabled = 0
#define factory_default_to_exp	(1 << 6)	// Bit 6	: Must reset to factory default = 1 
#define GROUP_TO_EXP_MAK 		0x1F		// Bits 5-0 : Current bs3x group (0-20)

#define CONFIG_REGS_REAL_NB		1U			// Only one config reg implemented for now

// TOTAL SIZE OF THIS TABLE IS : 
#define ALL_EXP_VALUES_SIZE	(EXPANSION_MAX_NB * (EXP_ANA_MAX_PER_MODULE + (EXP_DIG_MAX_PER_MODULE/8)))

// EXP_STATUS_REG_ADR
// to define

///////////////////////////////////////////////////////////
// How expansions values are mapped into the registers
///////////////////////////////////////////////////////////
// Exp0 analog 	0	// Contains a single analog value
// Exp0 analog 	1	// Contains a single analog value
// Exp0 analog 	2	// Contains a single analog value
// ...
// Exp0 dig 	0	// Contains 8 digital values
// Exp0 dig 	1	// Contains 8 digital values
// Exp0 dig 	2	// Contains 8 digital values

// Exp1 analog 	0	// Contains a single analog value
// Exp1 analog 	1	// Contains a single analog value
// Exp1 analog 	2	// Contains a single analog value
// ...
// Exp1 dig 	0	// Contains 8 digital values
// Exp1 dig 	1	// Contains 8 digital values
// Exp1 dig 	2	// Contains 8 digital values

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// EXPANSION BOARD MIDI STRINGS MAPPING ////////////////////////////////////////////
// Expansion board controls midi and lcd strings are located at the end of eeprom 0
// These Midi strings must be aligned with a page size or half page size since MIDI struct size is 32

#define EXP_MIDI_STR_SPI_EE_BASE_ADR 			0

// expansion analog midi strings
#define EXP_ANA_MIDI_STR_SPI_EE_BASE_ADR		EXP_MIDI_STR_SPI_EE_BASE_ADR		
// expansion digital midi strings
#define EXP_DIG_MIDI_STR_SPI_EE_BASE_ADR		(EXP_ANA_MIDI_STR_SPI_EE_BASE_ADR  		+ (EXP_ANA_MAX_PER_MODULE		* MIDI_STRUCT_SIZE))
// end of midi strings for group 0

// total size for group 0
#define EXP_MIDI_STR_SPI_GRP_0_EE_END_ADR		(EXP_DIG_MIDI_STR_SPI_EE_BASE_ADR  		+ (EXP_DIG_MAX_PER_MODULE		* MIDI_STRUCT_SIZE))

#define EXP_MIDI_STR_SPI_EE_LGT_PER_GRP			(EXP_MIDI_STR_SPI_GRP_0_EE_END_ADR - EXP_MIDI_STR_SPI_EE_BASE_ADR)
#define EXP_MIDI_STR_SPI_EE_END_ADR				(EXP_MIDI_STR_SPI_EE_BASE_ADR + (GROUP_NB * EXP_MIDI_STR_SPI_EE_LGT_PER_GRP))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// EXPANSION BOARD LCD STRINGS MAPPING ////////////////////////////////////////////

#define EXP_LCD_STR_SPI_EE_BASE_ADR			 	(EXP_MIDI_STR_SPI_EE_END_ADR)

// check if coherent :
#if (EXP_LCD_STR_SPI_EE_BASE_ADR % MIDI_STRUCT_SIZE)
	#error _EXP_LCD_STR_SPI_EE_BASE_ADR_ is not aligned on a eeprom page size
#endif

// expansion analog lcd strings
#define EXP_ANA_LCD_STR_SPI_EE_BASE_ADR			(EXP_LCD_STR_SPI_EE_BASE_ADR)

// expansion digital lcd strings
#define EXP_DIG_LCD_STR_SPI_EE_BASE_ADR			(EXP_ANA_LCD_STR_SPI_EE_BASE_ADR  		+ (EXP_ANA_MAX_PER_MODULE		* LCD_STR_SIZE))

// total size of expansion lcd strings for group 0
#define EXP_LCD_STR_GRP_0_SPI_EE_END_ADR		(EXP_DIG_LCD_STR_SPI_EE_BASE_ADR		+ (EXP_DIG_MAX_PER_MODULE 		* LCD_STR_SIZE))

#define EXP_LCD_STR_SPI_EE_LGT_PER_GRP			(EXP_LCD_STR_GRP_0_SPI_EE_END_ADR - EXP_LCD_STR_SPI_EE_BASE_ADR)
#define EXP_LCD_STR_SPI_EE_END_ADR				(EXP_LCD_STR_SPI_EE_BASE_ADR  + (GROUP_NB * EXP_LCD_STR_SPI_EE_LGT_PER_GRP))

// check if coherent :
#if (EXP_LCD_STR_EE_END_ADR % LCD_STR_SIZE)
	#error _EXP_LCD_STR_EE_END_ADR_ is not aligned on a eeprom page size
#endif

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckExpBoardPresence(void);
void CheckExpansionChanges(void);
void ConfigureAllExpansions(void);
//uchar ReadBytesFromExpansion(uchar i2c_command, uchar address, uchar lenght, far uchar *data_ptr);
//uchar WriteBytesToExpansion(uchar i2c_command, uchar address, uchar lenght, far uchar *data_ptr);

// Some checks
#if (EXPANSION_MAX_NB > 8)
	#error EXPANSION_MAX_NB must be 8 Max
#endif

#endif
