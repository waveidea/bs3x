#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "i2c.h"
#include "motion.h"
#include "eeprom.h"
#include "hardware.h"
#include "digital.h"
#include "expansion.h"

///////////////////////////////////////////////////////////////////////////////////////
// Global vars
///////////////////////////////////////////////////////////////////////////////////////
extern uint  EE_Address;
extern uchar I2cCommand;
//far uchar EE_far_buffer[EEPROM_PAGE_SIZE];

extern uchar control_index;							// current control nb
extern volatile uchar Z8_STATUS_4;
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];				// table of control reg for eerprom saving
extern uchar whole_sw_modes[NB_BYTES_FOR_SW];
extern uchar whole_sw_toggled[NB_BYTES_FOR_SW];
extern uchar current_i2c_exp;

extern rom uchar reorg_other_sw[TRANSPORT_NB]; // V1.8

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	GetCtrlStructEEaddFromIndexGroup()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update EE_Address value according to group, control index, also update eeprom select
//				if current control is transport, do not consider groups
//				From V1.5 this function takes into account the expansions EE addresses located in EEprom 0
///////////////////////////////////////////////////////////////////////////////////////
void GetCtrlStructEEaddFromIndexGroup(void)
{
	// Expansion module ?
	if(control_index > LFO_CTRL_INDEX)
	{
		// First control structure of expansions is located in EEPROM_ZERO
		I2cCommand   = EXP_I2C_SLAVE_ADR_SPI_EE + (current_i2c_exp << 1);
		// Calculate EEAddress where to read/write the structure
		// First offset the base address according to control number
		EE_Address  = (uint)((uint)(control_index-EXP_ANA_CTRL_INDEX) * (uint)(MIDI_STRUCT_SIZE));
		EE_Address += (uint)(EXP_ANA_MIDI_STR_SPI_EE_BASE_ADR);
		// Now need to multiply per the offset for each group
		EE_Address += (uint)((uint)current_group * (uint)EXP_MIDI_STR_SPI_EE_LGT_PER_GRP);	
	}
	else
	{
		// First control structure is located in EEPROM_ONE
		I2cCommand   = BS3X_I2C_SLAVE_ADR_EE + EEPROM_ONE_I2C_ADR;
		// Calculate EEAddress where to read/write the structure
		// First offset the base address according to control number
		EE_Address  = (uint)((uint)(control_index) * (uint)(MIDI_STRUCT_SIZE));
		// if not transport, get group offset 
		if(control_index > FORWARD_CTRL_INDEX)
			// Now need to multiply per the offset for each group
			EE_Address += (uint)((uint)current_group * (uint)MIDI_STR_EE_LENGHT_PER_GROUP);	
		// Caclulate on which EEProm we have to write (1 or 2) depending on address
		if(EE_Address >= (uint)EEPROM_ONE_FULL_SIZE)
		{
			EE_Address -= (uint)EEPROM_ONE_FULL_SIZE;
			I2cCommand   = BS3X_I2C_SLAVE_ADR_EE + EEPROM_TWO_I2C_ADR;
		}
	}

/*
	// First control structure is located in EEPROM_ONE
	I2cCommand   = EEPROM_ONE_I2C_ADR;
	// Calculate EEAddress where to read/write the structure
	// First offset the base address according to control number
	EE_Address  = (uint)((uint)(control_index) * (uint)(MIDI_STRUCT_SIZE));

	// if not transport, get group offset 
	if(control_index > FORWARD_CTRL_INDEX)
		// Now need to multiply per the offset for each group
		EE_Address += (uint)((uint)current_group * (uint)MIDI_STR_EE_LENGHT_PER_GROUP);	

	// Caclulate on which EEProm we have to write (1 or 2) depending on address
	if(EE_Address >= (uint)EEPROM_ONE_FULL_SIZE)
	{
		EE_Address -= (uint)EEPROM_ONE_FULL_SIZE;
		I2cCommand   = EEPROM_TWO_I2C_ADR;
	}
*/
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	GetLcdStrEEaddFromIndexGroup()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update EE_Address value according to group, control index, also update eeprom select
//				if current control is transport, do not consider groups
//				From V1.5 this function takes into account the expansions EE addresses located in EEprom 0
///////////////////////////////////////////////////////////////////////////////////////
void GetLcdStrEEaddFromIndexGroup(void)
{
	// Expansion module ?
	if(control_index > LFO_CTRL_INDEX)
	{
		// First control structure of expansions is located in EEPROM_ZERO
		I2cCommand   = EXP_I2C_SLAVE_ADR_SPI_EE + (current_i2c_exp << 1);
		// Get LCD str EE address (remove transport nb since no lcd str for transport)
		EE_Address  = (uint)(EXP_LCD_STR_SPI_EE_BASE_ADR + (uint)((uint)LCD_STR_SIZE * (uint)((uint)control_index-(uint)EXP_ANA_CTRL_INDEX)));
		// Now need to multiply per the offset for each group
		EE_Address += (uint)((uint)current_group * (uint)EXP_LCD_STR_SPI_EE_LGT_PER_GRP);	
	}
	else
	{
		// First lcd str is located in EEPROM_ONE
		I2cCommand   = BS3X_I2C_SLAVE_ADR_EE + EEPROM_ONE_I2C_ADR;
	
		// Get LCD str EE address (remove transport nb since no lcd str for transport)
		EE_Address  = (uint)(LCD_STR_EE_BASE_ADR + (uint)((uint)LCD_STR_SIZE * (uint)((uint)control_index-(uint)TRANSPORT_NB)));
		// if not transport, get group offset 
		if(control_index > FORWARD_CTRL_INDEX)
			// Now need to multiply per the offset for each group
			EE_Address += (uint)((uint)current_group * (uint)LCD_STR_EE_LENGHT_PER_GROUP);	
	
		// Caclulate on which EEProm we have to write (1 or 2) depending on address
		if(EE_Address >= (uint)EEPROM_ONE_FULL_SIZE)
		{
			EE_Address -= (uint)EEPROM_ONE_FULL_SIZE;
			I2cCommand   = BS3X_I2C_SLAVE_ADR_EE + EEPROM_TWO_I2C_ADR;
		}
	}

/*
	// First lcd str is located in EEPROM_ONE
	I2cCommand   = EEPROM_ONE_I2C_ADR;

	// Get LCD str EE address (remove transport nb since no lcd str for transport)
	EE_Address  = (uint)(LCD_STR_EE_BASE_ADR + (uint)((uint)LCD_STR_SIZE * (uint)((uint)control_index-(uint)TRANSPORT_NB)));
	// if not transport, get group offset 
	if(control_index > FORWARD_CTRL_INDEX)
		// Now need to multiply per the offset for each group
		EE_Address += (uint)((uint)current_group * (uint)LCD_STR_EE_LENGHT_PER_GROUP);	

	// Caclulate on which EEProm we have to write (1 or 2) depending on address
	if(EE_Address >= (uint)EEPROM_ONE_FULL_SIZE)
	{
		EE_Address -= (uint)EEPROM_ONE_FULL_SIZE;
		I2cCommand   = EEPROM_TWO_I2C_ADR;
	}
*/
}
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	LockEEprom10Ms()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	A byte or page has been writen to EEPROM, lock eeprom during write cycle time
//				before doing another operation on this eeprom
///////////////////////////////////////////////////////////////////////////////////////
void LockEEprom10Ms(void)
{
	Z8_STATUS_4 |= eeprom_locked_10ms_z4;
	// Timer 1 (10ms) is used to lock eeprom
	ReinitTimer1();
	// Start timer 1 single shot	
	T1CTL  = 0xB8; 
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	GetCurrentGroupSwitchModesFromEE()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Reload all 16 toggle/push switch modes for a whole group when 
//				a group has changed, also load these regs at init
///////////////////////////////////////////////////////////////////////////////////////
void GetCurrentGroupSwitchModesFromEE(void)
{
	uchar index;
	uchar local_ctrl_status_1;
	uchar mask, reorg_mask; // V1.8
	uchar sw_id = OTHER_SW_ID;
//	uchar reorg_table[TRANSPORT_NB] = {(1<<4), (1<<3), (1<<0), (1<<2), (1<<1)}; // V1.8

	// first switch MIDI struct address
	control_index = BW_ASSIGN_SW_CTRL_INDEX;
	mask = 1;
	for(index=0; index<(OTHER_SW_NB + SW_NB); index++)
	{
//		control_index = BW_ASSIGN_SW_CTRL_INDEX;
//		// V1.8 : bug correction : correct the reorganisation of toggle modes for SHIFT + TRANSPORT
//		if(index < TRANSPORT_NB)
//			control_index += reorg_other_sw[index];
//		else
//			control_index += index;

		// Get EEProm address and I2cCommand
		GetCtrlStructEEaddFromIndexGroup();
		// Add offset to read the midi struct reg that contains toggle/push info (ctrl_status_1)
		EE_Address = (uint)(EE_Address + CTRL_STATUS_1_STRUCT_OFFSET);
		// Read ctrl_status_1 from external eeprom
		local_ctrl_status_1 = ReadByteI2c();
		// V1.8 : bug correction : correct the reorganisation of toggle modes for SHIFT + TRANSPORT
		if(index < TRANSPORT_NB)
		{
			// reorg the mask
			reorg_mask = 1 << reorg_other_sw[index];
		}
		else
			reorg_mask = mask;

		// update whole sw mode table ?
		if(local_ctrl_status_1 & switch_ctrl_hook_mode)
		{
			whole_sw_modes[sw_id] 	|=   reorg_mask;	// this sw is toggle mode
			whole_sw_toggled[sw_id] &= (~reorg_mask); // clear toggled info

		}
		else
			whole_sw_modes[sw_id] &= (~reorg_mask);
		
		control_index++;

		// reinit mask when passing to all_sw_modes
		if(index == (uchar)(OTHER_SW_NB-1))
		{
			mask  = 1;
			sw_id = ALL_SW_ID;
		}
		else
			mask = mask << 1;
	}
}
