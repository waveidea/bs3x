#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "i2c_low_level.h"
#include "i2c.h"
#include "expansion.h"
#include "hardware.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "serial.h"
#include "assign.h"
#include "eeprom.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

far uchar exp_id_found[EXPANSION_MAX_NB];			// This table contains the IDs of the 8 possibles expansions
far expansion_struct expansion[EXPANSION_MAX_NB];	// Expansion Structure
far uchar exp_cfg_reg[CONFIG_REGS_REAL_NB];			// All config regs to expansions

far uchar STATUS_Z8_EXP;
uchar current_i2c_exp;

far uchar all_exp_values[ALL_EXP_VALUES_SIZE];		// memory where are stored analog and digital values of all expansions
far uchar current_exp_values[CURRENT_EXP_MAX_VALUES];
//far uchar Physical_exp[GROUP_NB];		// All physical values and difference sign stored here

extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_7;
extern rom uchar expansion_txt[];
// extern rom uchar info_txt[];
extern rom uchar connected_txt[];
extern rom uchar remove_txt[];
extern uchar lcd_current_incrust;
extern uchar lcd_lock_incrust_cpt;
// V1.66 extern rom uchar font_5x8[];
extern rom uchar exp_small_txt_bmp[]; // V1.6
extern uchar control_index;							// current control nb
extern uchar last_control_index;					// last control nb
extern uchar current_value;							// current control value
extern uchar control_assign_index;
extern uchar assign_state;
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 
extern uint  EE_Address;
extern uchar I2cCommand;
extern uchar Z8_STATUS_10;							// V1.6
extern far uchar current_exp_assign;				// V1.6

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckExpBoardPresence()
// Inputs	: None
// Outputs	: None
// Process  : global var byte STATUS_Z8_EXP is updated to true/false (1 bit per expansion found)
//			  This function is performed only during the boot
///////////////////////////////////////////////////////////////////////////////////////
void CheckExpBoardPresence(void)
{
	//uchar i;
	uchar Mask = 1;
	uchar data_read_from_i2c[3];
	uchar exp_found_nb = 0;
	uchar total_ana_ctrl = 0;
	uchar total_dig_ctrl = 0;

	// Clear expansion presence register before searching
	STATUS_Z8_EXP = 0;

	// Search up to 8 expansions on the I2C bus (i represents A2,A1,A0 similar to dipswitches)
	for(current_i2c_exp=0; current_i2c_exp < EXPANSION_MAX_NB; current_i2c_exp++)
	{
		// set I2C command to execute
		I2cCommand = EXP_I2C_SLAVE_ADR_RAM + (current_i2c_exp<<1);
		EE_Address = EXP_ID_REG_ADR;

		// in the table of I2C IDs found, 0xFF means no I2C module found
		expansion[current_i2c_exp].exp_id_found = 0xFF;
		data_read_from_i2c[0] = ReadByteI2c();
		if(!(Z8_STATUS_10 & i2c_timout_z10))
		//if(ReadBytesFromExpansion(EXP_I2C_SLAVE_ADR_RAM + (i<<1), EXP_ID_REG_ADR, 1, (far uchar *) &data_read_from_i2c[0]) == I2C_NO_TIMEOUT)
		{
			// Acknoledge received - the expansion at address i is connected
			STATUS_Z8_EXP |= Mask;
			// Store the I2C address found in a table
			expansion[current_i2c_exp].exp_id_found = data_read_from_i2c[0];
			// update total expansion found
			exp_found_nb++;
			// read the nb of analog and digital controls available in the expansion connected
			// ReadBytesFromExpansion(EXP_I2C_SLAVE_ADR_RAM + (i<<1), EXP_STATUS_REG_ADR, 2, (far uchar *) &data_read_from_i2c[0]);	
			EE_Address = EXP_STATUS_REG_ADR;
			ReadMultiByteI2c((far uchar *) &data_read_from_i2c[0], 2);
			// Update nb of analog control presents
			total_ana_ctrl += data_read_from_i2c[0];
			// Update nb of digital control presents
			total_dig_ctrl += data_read_from_i2c[1];
			// Fill the expansion structures
			expansion[current_i2c_exp].nb_an_ctrl  = data_read_from_i2c[0];
			expansion[current_i2c_exp].nb_dig_ctrl = data_read_from_i2c[1];
	
		}
		Mask = Mask << 1;
	} // end for all expansions

	// if at least one expansion was found, configure it
	ConfigureAllExpansions();
	
	// reset I2C controller
	I2CCTL &= ~I2C_ENABLE;
	InitI2C();

	// Display expansions found
	// V1.6 : nb of expansion found are displayed directly on the home screen
	// Display small expansion text + clear zone where was written fw version
	DisplayBmp(LCD_ZONE_FW_VERSION_HOME_X-17, LCD_ZONE_FW_VERSION_HOME_Y, LCD_INVERT, exp_small_txt_bmp);
	FillLcdZone(LCD_ZONE_FW_VERSION_HOME_X-3, LCD_ZONE_FW_VERSION_HOME_Y, 21, 1, FILL_BLANK);
	DisplayLcdInt(LCD_ZONE_FW_VERSION_HOME_X+3, LCD_ZONE_FW_VERSION_HOME_Y, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, exp_found_nb ); 

	// CHECK THAT ALL CONNECTED MODULES ARE FITTING THE BS3X MEMORY
	if( (total_dig_ctrl >= (EXP_DIG_MAX_PER_MODULE * EXPANSION_MAX_NB)) || 
	 	(total_ana_ctrl >= (EXP_ANA_MAX_PER_MODULE * EXPANSION_MAX_NB)))
	{
		rom_to_lcd_text_copy(expansion_txt);
		DisplayEmptyIncrust();
		rom_to_lcd_text_copy(remove_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+1, LCD_ZONE_INCRUST_Y+3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	
		rom_to_lcd_text_copy(expansion_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+39, LCD_ZONE_INCRUST_Y+3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	
		// Z8_STATUS_7 &= ~must_reinit_all_lcd_z7;
		//asm ("STOP");
		while(1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckExpansionChanges()
// Inputs	: None
// Outputs	: 
// Process  : One expansion has informed the bs3x that one control has changed
///////////////////////////////////////////////////////////////////////////////////////
void CheckExpansionChanges(void)
{
	uchar j,k;
	uchar Mask = 1;
	uchar Mask2;	
	uchar dig_changed;
	uchar expansion_glob_status;
	uchar read_lenght;
	uchar all_values_offset = 0;

	// Here the expansion pin is asserted : need to get data from expansions and send them to midi
	for(current_i2c_exp=0; current_i2c_exp<EXPANSION_MAX_NB; current_i2c_exp++)
	{
		// Is the expansion connected ?
		if(STATUS_Z8_EXP & Mask)
		{
			// read the global status register to see if this module has changed in value
			// ReadBytesFromExpansion(EXP_I2C_SLAVE_ADR_RAM + (i<<1), EXP_GLOB_STATUS_REG_ADR, 1, (far uchar *) &expansion_glob_status);
			// set I2C command to execeute
			I2cCommand = EXP_I2C_SLAVE_ADR_RAM + (current_i2c_exp << 1);
			EE_Address = EXP_GLOB_STATUS_REG_ADR;
			expansion_glob_status = ReadByteI2c();

			// here we know if the current expansion module has changed in value : the analog controls changed ?
			if(expansion_glob_status & exp_gbl_status_an_changed)
			{
				// Must start reading from analog registers
				// read_address_start = EXP_CTRL_ANA_REG_ADR;
				// total size to read includes all analog registers
				// read_lenght += expansion[i].nb_an_ctrl;
				read_lenght = expansion[current_i2c_exp].nb_an_ctrl;
				// read now all analog controls of the expansion
				// ReadBytesFromExpansion(EXP_I2C_SLAVE_ADR_RAM + (i<<1), EXP_CTRL_ANA_REG_ADR, read_lenght, &current_exp_values[0]);
				EE_Address = EXP_CTRL_ANA_REG_ADR;
				ReadMultiByteI2c(&current_exp_values[0], read_lenght);

				// here compare current_exp_values with preceding exp values
				for(j=0; j < read_lenght; j++)
				{
					// Treat analog controls
					if(current_exp_values[j] != all_exp_values[all_values_offset+j])
					{
						// The value has changed : need to send to MIDI
						control_index = EXP_ANA_CTRL_INDEX + j;
			
						// Assignment mode ?
						if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
						{
							control_assign_index = control_index;
							current_exp_assign	 = current_i2c_exp;
						}
						else
						if(assign_state != ASSIGN_ACTIVE)
						{
							if(!(Z8_STATUS_4 & first_pass_z4))
							{
								// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill struct from V1.8
								FillParamCurrentStruct();
								// load current value with value to send to midi
								current_value = current_exp_values[j];	
								// Send to MIDI now
								SendControlToMidi();
								// Store new value
								all_exp_values[all_values_offset+j] = current_value;
								// Display new value
								DisplayCurrentValue();
								// Update LCD string only if required
								if(last_control_index != control_index)
								{
									// Update lcd str
									UpdateLcdString();
									last_control_index = control_index;
								}
							} // end not first pass
						} // end not assign mode
				   } // end compare all analog controls
				} // end for all analog controls
			} // end analog global status was active

			// here we know if the current expansion module has changed in value : the digital controls changed ?
			if(expansion_glob_status & exp_gbl_status_dig_changed)
			{
				// total size to read is all digital registers (must divide by 8)
				read_lenght = expansion[current_i2c_exp].nb_dig_ctrl >> 3;
				// read now all digital registers
				// ReadBytesFromExpansion(EXP_I2C_SLAVE_ADR_RAM + (i<<1), EXP_CTRL_DIG_REG_ADR, read_lenght, &current_exp_values[0]);
				EE_Address = EXP_CTRL_DIG_REG_ADR;
				ReadMultiByteI2c(&current_exp_values[0], read_lenght);


				// here compared current_exp_values with preceding exp values
				for(j=0; j < read_lenght; j++)
				{
					// init mask bit
					Mask2 = 1;
					// what dig control has changed ?
					dig_changed = all_exp_values[all_values_offset+j] ^ current_exp_values[j];
					// something has changed ?
					if(dig_changed)
					{
						// need to test 8 digital controls per byte
						for(k=0; k<8; k++)
						{
							if(dig_changed & Mask2)
							{
								if(current_exp_values[j] & Mask2)
									current_value = 127;
								else
									current_value = 0;

								// The value has changed : need to send to MIDI
								control_index = EXP_DIG_CTRL_INDEX + j + k;
					
								// Assignment mode ?
								if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
								{
									control_assign_index = control_index;
									current_exp_assign	 = current_i2c_exp;
								}
								else
								if(assign_state != ASSIGN_ACTIVE)
								{
									if(!(Z8_STATUS_4 & first_pass_z4))
									{
										// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill struct from V1.8
										FillParamCurrentStruct();
										// Send to MIDI now
										SendControlToMidi();
										// Display new value
										DisplayCurrentValue();
										// Update LCD string only if required
										if(last_control_index != control_index)
										{
											// Update lcd str
											UpdateLcdString();
											last_control_index = control_index;
										}
									} // end not first pass
								} // end not assign mode
							} // end dig control changed
							// pass to next switch
							Mask2 = Mask2 << 1;
						} // end for all 8 digital controls per byte
						// 8 dig controls have been checked, store new sw values
						all_exp_values[all_values_offset+j] = current_exp_values[j];
					} // end dig changed
				} // end for all data read
			}
		} // end expansion is present
		// pass to next expansion
		Mask = Mask << 1;
		// calculate next offset where to store analog and digital controls
		all_values_offset += (expansion[current_i2c_exp].nb_an_ctrl + (expansion[current_i2c_exp].nb_dig_ctrl >> 3));
	} // end all possible expansions
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	ConfigureAllExpansions()
// Inputs	: 	None
// Outputs	: 
// Process  : 	send all available config registers to all connected expansion modules
///////////////////////////////////////////////////////////////////////////////////////
void ConfigureAllExpansions(void)
{
	uchar Mask;
	uchar i;

	// Is there at least one expansion connected ?
	if(!STATUS_Z8_EXP) return;

	// Init Mask
	Mask = 1;

	// Configure register 0
	exp_cfg_reg[0] &= ~GROUP_TO_EXP_MAK;
	exp_cfg_reg[0] |=  current_group;
	// if analog is HOOK mode
//V1.8 expansion always hook enabled	if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm)
		exp_cfg_reg[0] |= hook_status_to_exp;
	
	// All expansion module are concerned
	for(current_i2c_exp=0; current_i2c_exp < EXPANSION_MAX_NB; current_i2c_exp++)
	{
		// Is the expansion connected ?
		if(STATUS_Z8_EXP & Mask)
		{
			// Set the current I2C address 	
			I2cCommand = EXP_I2C_SLAVE_ADR_RAM + (current_i2c_exp << 1);

			// Always configure all available configuration registers
			for(i=0; i<CONFIG_REGS_REAL_NB; i++)
			{
				EE_Address = EXP_CONFIG_REG_ADR + i;
				WriteByteI2c(exp_cfg_reg[i]);
			}
		} // end the module was connected
		// pass to next expansion
		Mask = Mask << 1;	
	} // end all expansions modules

	// be sure to stop reset to factory default if it was set to 1
	exp_cfg_reg[0] &= ~factory_default_to_exp;
}

/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ReadBytesFromExpansion()
// Inputs	: None
// Outputs	: 
// Process  : Read multiple bytes from expansion module RAM or FLASH depends on i2c_command
//			  EXP_I2C_SLAVE_ADR_RAM, EXP_I2C_SLAVE_ADR_FLASH,
///////////////////////////////////////////////////////////////////////////////////////
uchar ReadBytesFromExpansion(uchar i2c_command, uchar address, uchar lenght, far uchar *data_ptr)
{
	uchar result = I2C_NO_TIMEOUT;	
	uchar index, data_read;

	// First send the slave address of the expansion and wait for ack
	MAC_I2cStart	
  	MAC_I2cWriteByte(i2c_command)			// Try to communicate with expansion module	(slave address)
	WaitI2cTransmitEmpty();					// wait for transmit buffer empty   
	if(WaitI2cAcknoledge == I2C_NO_TIMEOUT)	// wait for ACK from expansion module	
	{
		// Send command byte 0 : address of the register to read
	  	MAC_I2cWriteByte(address)				// Try to communicate with expansion module	(slave address)
		WaitI2cTransmitEmpty();					// wait for transmit buffer empty   
		if(WaitI2cAcknoledge == I2C_NO_TIMEOUT)	// wait for ACK from expansion module
		{
			// Send command byte 1 : data lenght of the register(s) to read
		  	MAC_I2cWriteByte(lenght)				// Try to communicate with expansion module	(slave address)
			WaitI2cTransmitEmpty();					// wait for transmit buffer empty   
			if(WaitI2cAcknoledge == I2C_NO_TIMEOUT)	// wait for ACK from expansion module
			{
				// Now read all bytes from the expansion module
				for(index=0;index < lenght; index++)
				{
					if(index == lenght-1)
					{
						MAC_I2cSendNack							// Send No ack on last byte to read
			     	}
					WaitI2cFullDataReceived();					// Wait for the byte to be fully received
					MAC_I2cReadByte(data_read)					// EEProm Read data		
					data_ptr[index] = (far uchar)(data_read);	// assign data read to pointer
			    }
			}
			else result = I2C_TIMEOUT;
		}
		else result = I2C_TIMEOUT;
	}
	else result = I2C_TIMEOUT;

	// Finish with I2C transaction
	I2cStop();
	return result;
}
*/

/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WriteBytesToExpansion()
// Inputs	: None
// Outputs	: 
// Process  : write multiple bytes to expansion module RAM or FLASH depends on i2c_command
//			  EXP_I2C_SLAVE_ADR_RAM, EXP_I2C_SLAVE_ADR_FLASH,
///////////////////////////////////////////////////////////////////////////////////////
uchar WriteBytesToExpansion(uchar i2c_command, uchar address, uchar lenght, far uchar *data_ptr)
{
	uchar result = I2C_NO_TIMEOUT;	
	uchar index;

	// First send the slave address of the expansion and wait for ack
	MAC_I2cStart	
  	MAC_I2cWriteByte(i2c_command)			// Try to communicate with expansion module	(slave address)
	WaitI2cTransmitEmpty();					// wait for transmit buffer empty   
	if(WaitI2cAcknoledge == I2C_NO_TIMEOUT)	// wait for ACK from expansion module	
	{
		// Send command byte 0 : address of the register to write
	  	MAC_I2cWriteByte(address)				// Try to communicate with expansion module	(slave address)
		WaitI2cTransmitEmpty();					// wait for transmit buffer empty   
		if(WaitI2cAcknoledge == I2C_NO_TIMEOUT)	// wait for ACK from expansion module
		{
			// Send command byte 1 : data lenght of the register(s) to write
		  	MAC_I2cWriteByte(lenght)				// Try to communicate with expansion module	(slave address)
			WaitI2cTransmitEmpty();					// wait for transmit buffer empty   
			if(WaitI2cAcknoledge == I2C_NO_TIMEOUT)	// wait for ACK from expansion module
			{
				// Now write all bytes to the expansion module
				for(index=0;index < lenght; index++)
				{
					MAC_I2cWriteByte(data_ptr[index])		// Try to communicate with expansion module	(write regs)
					WaitI2cTransmitEmpty();					// wait for transmit buffer empty   
					WaitI2cAcknoledge;
			    }
			}
			else result = I2C_TIMEOUT;
		}
		else result = I2C_TIMEOUT;
	}
	else result = I2C_TIMEOUT;

	// Finish with I2C transaction
	I2cStop();
	return result;

}
*/
