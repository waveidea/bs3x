#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "scene.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "itrs.h"
#include "hardware.h"
#include "motion.h" // V1.8
#include "eeprom.h"
#include "serial.h"
#include "analog.h"	// V1.6
#include "i2c.h"
#include "arpp.h" // V1.8
#include "realtime.h" // V1.9

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

far uchar index_scene;		// V1.6 scene index
far uchar index_scene_dig;	// V1.6 scene index

//V1.6 extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 64 bytes

extern far uchar snapshot_long_push_cpt;	// V1.64 far
extern uchar Z8_STATUS_0;
extern uchar Z8_STATUS_11; // V1.8
extern volatile uchar Z8_STATUS_4;
extern uchar control_index;							// current control nb
extern uchar last_control_index;					// current control nb
extern uchar current_value;							// current control value
//extern rom uchar bmp_incrust[];
extern uchar lcd_lock_incrust_cpt;
//extern far uchar lcd_text_value[MAX_TEXT_LENGHT];
extern rom uchar sending_txt[];
extern rom uchar saving_txt[];
extern rom uchar scene_txt[];
extern rom uchar snapshot_txt[];
extern rom uchar arpp_txt[]; // V1.8
// V1.66 extern rom uchar font_5x8[];
// V1.66 extern rom uchar font_num_8x16[];
//extern uchar current_scene;				// current scene select
extern uchar lcd_current_incrust;
extern uint EE_Address;
extern uchar I2cCommand;					// current EEProm selection
extern All_Controls Ctrl;				// All controls are defned here
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern far   uint non_assign_sw_test_mode;

// extern far uchar Analog[ANALOG_SOURCES_NB]; // V1.6
extern far Controls_Scene	Physical[GROUP_NB];		// V1.6 All physical values stored here 
extern unsigned char      	 arp_ref_note; // midi input note value  (ref_note = 0 is used to disable arp) V1.8

extern far All_Params	 *ParamCurrent;	// V1.8 V1.61 current structure of parameters in EEPROM

enum {SCENE_SNAP, SCENE_ARP};

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckScene()
// Inputs	: None
// Outputs	: None
// Process  	: Check scene functions from switch snapshot
///////////////////////////////////////////////////////////////////////////////////////
void CheckScene(void)
{
	//enum {SCENE_SNAP, SCENE_ARP};

	uchar tmp; // V1.8
	uchar scene_type;

	// V1.6 : already sending scene ?
	if(index_scene)
	{
		SendSceneToMidi();
		return;
	}

	if(Z8_STATUS_0 & snapshot_changed_z0)
	{
		// Display snapshot incrust only if not already active
		if(lcd_current_incrust != LCD_INCRUST_SCENE)
		{
			// get the type of scene to save/send
			if((!(Z8_STATUS_11 & send_scene_from_prog_ch_z11)) && (Z8_STATUS_3_NVM & scene_mode_z3nvm)) 
				  scene_type = SCENE_ARP;
			else  scene_type = SCENE_SNAP;

			// V1.8 : display snapshot or arpp scene
			if(scene_type == SCENE_ARP) 
				// Display incrust title arpp
				rom_to_lcd_text_copy(arpp_txt);
			else
				// Display incrust title snapshot
				rom_to_lcd_text_copy(snapshot_txt);
			
			//DisplayLcdText(LCD_ZONE_INCRUST_X+6, LCD_ZONE_INCRUST_Y, LCD_NORMAL, font_5x8);					
			DisplayEmptyIncrust();

			// display scene text
			if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
 		    {
				rom_to_lcd_text_copy(scene_txt);
				DisplayLcdText(LCD_ZONE_INCRUST_X+21, LCD_ZONE_INCRUST_Y+3, LCD_INVERT|FONT_5x8_FLG);
			}
		}

		if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
		{
			// Save or Send scene ?
			if(Z8_STATUS_0 & save_zsend_snapshot_z0)
				rom_to_lcd_text_copy(saving_txt);
			else
				rom_to_lcd_text_copy(sending_txt);
	
			// display saving / sending text
			DisplayLcdText(LCD_ZONE_INCRUST_X+16, LCD_ZONE_INCRUST_Y+2, LCD_INVERT|FONT_5x8_FLG);
	
			// The current active incrust area is now scene
			lcd_current_incrust = LCD_INCRUST_SCENE;
	
			// Dislpay scene number V1.8
			if(scene_type == SCENE_ARP) 
				tmp = current_scene_arp;
			else
				tmp = current_scene;
			
			// display the scene
			DisplayLcdInt(LCD_ZONE_INCRUST_X+66, LCD_ZONE_INCRUST_Y+2, DIGITS_2|LCD_INVERT|FONT_NUM_8x16_FLG, tmp );
		}

		// scenes are located in first eeprom (EEPROM_ZERO_I2C_ADR)
		I2cCommand = BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

		// Lock LCD incrust zone for 2.5 seconds (must be placed before mute_note())
		lcd_lock_incrust_cpt = CPT_2_SEC;

		// NOW SEND OR SAVE CURRENT SELECTED SCENE
		if(scene_type == SCENE_ARP) // V1.8 arpp now features scenes
		{
			// First calculate EE base address according to scene arp selected
			EE_Address = (uint)(ARP_SCENES_START_EE_ADR + (uint)((uint)current_scene_arp * (uint)ARP_PARAM_NB));
			// save or send ?
			if(Z8_STATUS_0 & save_zsend_snapshot_z0)
				// write the whole scene data, starting at infrared address
				WriteMultiByteI2c((far uchar *)&CtrlReg[ARP_GATE_INDEX], ARP_PARAM_NB);
			else
			{
				// mute last note before getting new scene
				if(arp_ref_note) mute_note();
				// update the arpp with new scene arp selected
				ReadMultiByteI2c((far uchar *)&CtrlReg[ARP_GATE_INDEX], ARP_PARAM_NB);
			}
		}
		else
		{
			// First calculate EE base address according to scene select
			EE_Address = (uint)(SCENE_BASE_EE_ADR + (uint)((uint)current_scene * (uint)SCENE_OFFSET));
			// save or send ?
			if(Z8_STATUS_0 & save_zsend_snapshot_z0)
				// write the whole scene data, starting at infrared address
				WriteMultiByteI2c((far uchar *)&Ctrl.Ctrl_sc.CTRL_STRUCT_START_FIELD, SCENE_BLOC_SIZE);
			else
			{
				index_scene 	= 0;	// V1.6
				index_scene_dig	= 0;	// V1.6
				SendSceneToMidi();
			}
		}
		

		// reinit counter and flags
		Z8_STATUS_0  &= ~snapshot_changed_z0;
		snapshot_long_push_cpt = 0;	

		if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
			non_assign_sw_test_mode |= TEST_MODE_SNAPSHOT_BIT_MASK;

	} // end snapshot changed
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendSceneToMidi()
// Inputs	: None
// Outputs	: None
// Process  : EEaddress and I2cCommand are already ok at the input of the function
// V1.6 	: send only one control per scene in the main loop : allows to send realtime events between scenes
// V1.6 	: Update all controls values from scenes values, un-hook pots
///////////////////////////////////////////////////////////////////////////////////////
void SendSceneToMidi(void)
{
	uchar Mask;
	far uchar 	*ptr_far;
	uchar 	  	*ptr;	

	// V1.4 : do not send scenes in autotest mode
	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
		return;

	// We are currently sending a scene
	Z8_STATUS_4 |= sending_scene_z4;		

	// First calculate EE base address according to scene select
	EE_Address = (uint)(SCENE_BASE_EE_ADR + (uint)((uint)current_scene * (uint)SCENE_OFFSET));
	// scenes are located in first eeprom (EEPROM_ZERO_I2C_ADR)
	I2cCommand = BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

	// update control index for analog controls
	control_index = TRANSPORT_NB + index_scene;

	// point on current scene value for control index
	EE_Address = (uint)(EE_Address + (uint)index_scene);

	// read the saved scene from eeprom for 1 control only
	current_value = ReadByteI2c();

	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	//CheckRealtimeMidi();

	// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill struct from V1.8
	FillParamCurrentStruct();

	// Are we sending analog controls or digital ?
	if(index_scene < SCENE_BLOC_SIZE-2)
	{
		current_value &= 0x7F;
	
		// V1.6 : TO DO : UNHOOK ALL ANALOG CONTROLS EXPECT JOSYTICK
// V1.8	if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm) replaced by
		if(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode)
		{
			// Hook mode enabled : un-hook current analog control and save scene value as current control value
			// Un hook current analog control
			ptr		= &Ctrl.Ctrl_sc.CTRL_STRUCT_START_FIELD;
			ptr[index_scene] &= ~HOOK_BIT ;
			// store scene in physical value, also update sign
			ptr_far = &Physical[current_group].CTRL_STRUCT_START_FIELD; 
			if(ptr[index_scene] <= current_value)
				ptr_far[index_scene] = current_value;
			else
				ptr_far[index_scene] = current_value | HOOK_BIT;
		}

		index_scene++;
	} // end analog control
	else
	{	
		// Here we are currently sending digital controls for scene
		Mask = 1 << index_scene_dig;
		
		control_index = TRANSPORT_NB + index_scene + index_scene_dig;

		// Normal switches or all other switches ?
		if(index_scene == SCENE_BLOC_SIZE-2)
		{
			// here are all other switches
			if(current_value & Mask)
			{
				current_value = 127;
				Ctrl.Ctrl_sc.other_sw_value |= Mask;
			}
			else
			{
				current_value = 0;
				Ctrl.Ctrl_sc.other_sw_value &= ~Mask;
			}

			// Display new switches values			
			UpdateOtherSwState(index_scene_dig);
		} // end other sw 
		else
		{
			// here are all normal switches
			if(current_value & Mask)
			{
				current_value = 127;
				Ctrl.Ctrl_sc.all_sw_value |= Mask;
			}
			else
			{
				current_value = 0;
				Ctrl.Ctrl_sc.all_sw_value &= ~Mask;
			}

			// Display new switches values	
			UpdateSwState(index_scene_dig);
		} // end normal sw 

		// Pass to next digital control
		index_scene_dig++;
		// must toggle from all other sw to normal sw ?
		if(index_scene_dig == 8)
		{
			index_scene_dig	= 0;
			index_scene++;
		}
	} // end digital control
	
	// Now send only one control for scene
	SendControlToMidi();
	
	// next control to send is not defined yet
	last_control_index = CTRL_UNDEF;
	
	// All controls sent ?
	if(index_scene == SCENE_BLOC_SIZE)
	{
		// reset the index 
		index_scene = 0;
		// reset the flag which forces sending snapshot from prog change V1.8
		if(Z8_STATUS_11 & send_scene_from_prog_ch_z11)
		{
			// reset the flag
			Z8_STATUS_11 &= ~send_scene_from_prog_ch_z11;
			// redisplay scene
			UpdateLcdZones(INIT_LCD_FUNCTION); // INIT_LCD_FUNCTION also call INIT_LCD_SCENE
		}
	}

	// No more sending scenes
	Z8_STATUS_4  &= ~sending_scene_z4;
}

/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendSceneToMidi()
// Inputs	: None
// Outputs	: None
// Process  : EEaddress and I2cCommand are already ok at the input of the function
///////////////////////////////////////////////////////////////////////////////////////
void SendSceneToMidi(void)
{
	uchar index_scene;
	uchar scene_sw_tmp;
	uchar mask;

	// V1.4 : do not send scenes in autotest mode
	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
		return;

	// We are currently sending a scene
	Z8_STATUS_4 |= sending_scene_z4;		

	// read saved scene from EEprom and send it to midi output(s)
	ReadMultiByteEEprom((far uchar *)(&generic_buffer[0]), SCENE_BLOC_SIZE);

	control_index = TRANSPORT_NB;
	// skip the other_sw_value and all_sw_value
	for(index_scene=0; index_scene < (SCENE_BLOC_SIZE-2); index_scene++)	
	{
		current_value = generic_buffer[index_scene] & 0x7F;
//		current_value = ReadByteEEprom() & 0x7F;
		SendControlToMidi();
		// the above function modify EE_Address and I2cCommand
		//EE_Address = ++local_ee_address;
		//I2cCommand  = EEPROM_ZERO_I2C_ADR;
		// Force to get midi str from eeprom
		last_control_index = CTRL_UNDEF;
		control_index++;
	}

	// other_sw_value are treated seperately
	mask = 1;
	for(index_scene=0; index_scene < OTHER_SW_NB; index_scene++)	
	{
		current_value = 0;
		if( (generic_buffer[SCENE_BLOC_SIZE-2])	& mask)
		//if(scene_sw_tmp & mask)
			current_value = 127;
		SendControlToMidi();
		// Force to get midi str from eeprom
		last_control_index = CTRL_UNDEF;
		// pass to next switch
		control_index++;
		mask = mask << 1;
	}

	// all_sw_value are treated seperately
	mask = 1;
	for(index_scene=0; index_scene < SW_NB; index_scene++)	
	{
		current_value = 0;
		if( (generic_buffer[SCENE_BLOC_SIZE-1])	& mask)
			current_value = 127;
		SendControlToMidi();
		// Force to get midi str from eeprom
		last_control_index = CTRL_UNDEF;
		// pass to next switch
		control_index++;
		mask = mask << 1;
	}
	
	Z8_STATUS_4  &= ~sending_scene_z4;
}
*/

