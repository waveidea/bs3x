#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <ctrldef.h>
#include <common_z8_ez.h>
#include "analog.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "hardware.h"
#include "serial.h"
#include "eeprom.h"
#include "assign.h"  
#include "mackie.h"  
#include "i2c.h"
#include "realtime.h"

// orders for analog input selection
// All 32 rotaries
// All 8  sliders
// infrared
// joy x
// joy y
// crossfader
// automation pot 0
// automation pot 1
// automation pot 2


///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

    All_Controls 	Ctrl;					// All controls are defned here
far Controls_Scene	Physical[GROUP_NB];		// All physical values and difference sign stored here
far All_Params	 	Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61 structure of parameters in EEPROM V1.61
far All_Params	 	*ParamCurrent;						// V1.61 current structure of parameters in EEPROM


extern far uchar Analog[ANALOG_SOURCES_NB];

// TO REMOVE
//extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes
//	generic_buffer[0] = MIDI_SOX;
//	generic_buffer[1] = 0x12;
//	generic_buffer[2] = MIDI_EOX;
//	SendDispatchToMidi(&generic_buffer[0], 3, midi_out_2_en); 

///////////////////////////////////////////////////////////////////////////////////////

uchar current_value;							// current control value
uchar control_index;							// current control nb
uchar last_control_index;						// last control nb
uchar current_hook_bmp = NO_HOOK_BMP;			// current_hook_state;
uchar infrared;

extern rom uchar fader_txt[];
// V1.66 extern rom uchar font_num_8x16[];
// V1.66 extern rom uchar font_5x8[];
extern uchar lcd_lock_incrust_cpt;
extern rom uchar bmp_joy_border_left[];
extern rom uchar bmp_joy_border_right[];
extern rom uchar bmp_joy_x_y_txt[];
extern rom uchar bmp_lock_up[];
extern uchar lcd_current_incrust ;		// The current active incrust area is now scene
extern uchar Z8_STATUS_3;
extern volatile uchar Z8_STATUS_4;
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern uchar control_assign_index;
extern uchar assign_state;
extern rom uchar identity[]; // V1.5
extern uchar Z8_STATUS_9;	// V1.62
extern uchar Z8_STATUS_10; // V1.62
extern rom uchar bmp_shift_lock[]; // V1.62

/*
extern rom uchar adc_calibrate_txt[];
extern rom uchar please_set_txt[];
extern rom uchar left_txt[];
extern rom uchar right_txt[];
extern rom uchar push_ok_txt[];
*/
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitAnalog()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 AD converters
///////////////////////////////////////////////////////////////////////////////////////
void InitAnalog(void)
{	
	ADCCTL = 0xA0;
}

/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Get7BitsADValue()
// Inputs	: 	next Channel
// Outputs	: 	return a 7 bits AD value on global register current_value
// Process  : 	Get AD converter value on selected channel, start conv on next channel
///////////////////////////////////////////////////////////////////////////////////////
void Get7BitsADValue(uchar next_channel, uchar next_mux)
{	
	//DBG
	//LED_AUTOMATION_PORT_OUT |= LED_AUTOMATION_PIN;

	// wait for conversion complete on preceding channel
#ifndef SIMUL
	while(ADCCTL & 0x80);
#endif

	//DBG
	//LED_AUTOMATION_PORT_OUT &= ~LED_AUTOMATION_PIN;

	// Change external mux value
	MUX_PORT_OUT &= 0xF8;
	MUX_PORT_OUT |= next_mux;

	// return current AD value on 7 bits
	// current_value = (ADCD_H + ADC_Offset - 127) >> 1 ;
	current_value  = ADCD_H  >> 1 ;
	current_value += ADCD_H  >> 6 ;

	// IMMEDIATELY start new conversion on next channel
	ADCCTL = 0xA0 + next_channel;
	//ADCCTL = 0xA0 + 7;

	if(current_value > 127) 
		current_value = 127;	
}
*/
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckPots()
// Inputs	: None
// Outputs	: None
// Process  : Check all 40 pots rotaries + sliders
///////////////////////////////////////////////////////////////////////////////////////
void CheckPots(void)
{
	uchar row, index, /* next_ad_channel, next_mux,*/ pot_index;	
	uchar current_pot_7F;
	uchar must_send_to_midi, must_update_lcd;
	uchar local_physical;
	uchar hook_bmp_index, group_index;

	// First pot is control index 5
	control_index = POTS_BASE_CTRL_INDEX;

	for(row = ROW0 ; row < POTS_ROW_NB; row++ )
   	{

		// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
		//CheckRealtimeMidi();

		//next_ad_channel = row;
     	for(index=0 ; index < CTRL_PER_ROW; index++) 
		{
			pot_index = control_index - POTS_BASE_CTRL_INDEX;

			// Mask Hook bit
			current_pot_7F = Ctrl.Ctrl_sc.pots[pot_index] & (~HOOK_BIT);

			// Get A/D converter value
			// Get7BitsADValue(next_ad_channel, next_mux);
			current_value = Analog[POTS_ANALOG_INDEX + pot_index];

			// filter if not first pass nor if 127
   			if((current_value != 127) && (!(Z8_STATUS_4 & first_pass_z4)))
				current_value = (current_pot_7F + current_value) >> 1;

			must_send_to_midi = FALSE;
			must_update_lcd	  = FALSE;
			hook_bmp_index    = NO_HOOK_BMP;

			// first pass ?
			if(Z8_STATUS_4 & first_pass_z4)
			{
				FillParamCurrentStruct(); // V1.61
/*
// BLOCK REMOVED IN V1.61 
				// Read midi struct header to check if autosend
				GetCtrlStructEEaddFromIndexGroup();
				ReadMultiByteI2c((far uchar *)&ParamCurrent->ctrl_status_0, MIDI_STRUCT_HEADER_SIZE);
*/
				// check autosend flag
				if(ParamCurrent->ctrl_status_1 & autosend)
					must_send_to_midi = TRUE;
				Ctrl.Ctrl_sc.pots[pot_index] = current_value | HOOK_BIT;
				// initialize physical register to physical value, sign is 0 (lower or equal)
				for(group_index=0; group_index<GROUP_NB; group_index++)
					Physical[group_index].pots[pot_index] = current_value;
			} // end first pass
			else
			{
				// pot has changed ?
				if(current_pot_7F != current_value)
				{	
					// are we in assign mode
					if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
						  control_assign_index = control_index;
					else
					if(assign_state != ASSIGN_ACTIVE)
					{
						must_update_lcd	 = TRUE;
						// V1.8 fill structure of the pot before calling SendCtrlToMidi() because we need to know the hook config now
						FillParamCurrentStruct();
						// pot has changed and jump mode
						// V1.8 if(!(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm)) replaced by 
						if(!(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode))
							must_send_to_midi = TRUE;
						// pot has changed and hook mode is active
						else
						{
							local_physical 	= Physical[current_group].pots[pot_index];
							// is the pot hooked thus ready to be sent ?
							hook_bmp_index = CheckHookPot(&Ctrl.Ctrl_sc.pots[pot_index], local_physical, current_value);
							// hooked : ready to send midi
							if(hook_bmp_index == NO_HOOK_BMP)
								must_send_to_midi = TRUE;
						} // end we are not in assign mode
					} // end pot has changed and hook mode is active
				} // end current pot has changed
			} // end not first pass

			if(must_send_to_midi) 
			{
				// must store new pot value before sending to midi since send to midi may modify it
				Ctrl.Ctrl_sc.pots[pot_index] = current_value | HOOK_BIT;
				SendControlToMidi();
			}

			if(must_update_lcd)
			{
				DisplayCurrentValue();
				// Update pots hook state only if hook mode
				// V1.8 if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm) replaced by
				if(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode)
					UpdateLcdHookState(hook_bmp_index);
				if(last_control_index != control_index)
				{
						// Update lcd str
						UpdateLcdString();
						if(must_send_to_midi) 
							last_control_index = control_index;

				}
			}
			// pass to next pot
			control_index++;
		} // end all ctrl_per_row
	} // end all rows
}	


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckInfrared()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check infrared analog input
//				Infared is 0-109, need to multiply by 0.165 : >>3 + >>4 = 0.1875 
///////////////////////////////////////////////////////////////////////////////////////
void CheckInfrared(void)	   
{
	uchar tmp;
//V1.2	rom uchar local_bmp_ptr;

	// if infrared off start AD conversion on next AD channel and exit
	if(!(Z8_STATUS_1_NVM & infrared_state_z1nvm))
	{
//		ADCCTL = 0xA0 + JOYSTICK_X_AD_CHANNEL;
		return;
	}

	// Get infrared a/d converter value
	// Get7BitsADValue(JOYSTICK_X_AD_CHANNEL, 0);
	current_value = Analog[INFRARED_ANALOG_INDEX];

	// V1.5
	// remove small noise oscillating between 0 and 06 (infrared and ribbon)
	if(current_value < MIN_INFRARED_VALUE) current_value = MIN_INFRARED_VALUE;
	current_value -= MIN_INFRARED_VALUE;

	// Filter only if not first pass
   	if((current_value != 127) && (!(Z8_STATUS_4 & first_pass_z4)))		
		current_value = (infrared + current_value) >> 1;

	// V1.5 HW 1.x
	if(identity[HW_VERSION_INDEX] < 2)
	{
		// Infared is 0-109, need to multiply by 0.165 : >>3 + >>4 = 0.1875 
		// now rescale infrared (x 1.1875)
		tmp = current_value >> 3;
		current_value += tmp;
		current_value += (tmp>>1);
	}
	else
	{	// V1.5 HW 2.x
		// ribbon is about 0-120 now rescale ribbon (x 1.0625)
		tmp = current_value >> 4;
		current_value += tmp;
	}

	if(current_value > 127)
		current_value = 127;

	if ( (current_value >= Ctrl.Ctrl_sc.infrared_hold) && (!(Z8_STATUS_4 & wait_ir_passed_zero_z4)))
		 Z8_STATUS_3 &= (~infrared_lock_midi_z3);

	if (infrared != current_value)
	{
		// update control index 
		control_index = INFRARED_CTRL_INDEX;
		
		// are we in assign mode
		if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
			  control_assign_index = control_index;
		else
		if(assign_state != ASSIGN_ACTIVE)
		{
			// update new infrared value
			infrared = current_value;
		
			if(!(Z8_STATUS_4 & first_pass_z4))
			{
				// send to midi only if not locked
				if(!(Z8_STATUS_3 & infrared_lock_midi_z3))
				{
					// V1.8 fill structure of the infrared before calling SendCtrlToMidi() because SendControlToMidi() do not fill structure anymore in V1.8
					FillParamCurrentStruct();
					SendControlToMidi();
					UpdateLcdHookState(NO_HOOK_BMP);
				}
				else
				{
					// infrared lock : display locked direction up
					if (current_value < Ctrl.Ctrl_sc.infrared_hold)
						 tmp = HOOK_UP_BMP;
					else tmp = HOOK_DOWN_BMP;	
	 		    	UpdateLcdHookState(tmp);
				}
	
				// Update display bargraph (value)
				UpdateLCDInfraredBargraph();

				// Now display numeric value
				DisplayCurrentValue();
		
				// Update LCD string only if required
				if(last_control_index != control_index)
				{
						// Update lcd str
						UpdateLcdString();
						last_control_index = control_index;
				}
		    } // end not first pass
		} // end not in assign mode
	} // end infared value changed
}	

/*
FUNCTION REMOVED IN V1.61
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckJoystick()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check joystick X, Y and button
//				Joystick are 0-125, need to multiply by 1.016 : >>6 
///////////////////////////////////////////////////////////////////////////////////////
void CheckJoystick(void)
{
	uchar must_update_lcd_joystick_x = FALSE;
	uchar must_update_lcd_joystick_y = FALSE;
	uchar tmp;

	// Check if joystick X has changed
	// Get7BitsADValue(JOYSTICK_Y_AD_CHANNEL, 0);
	current_value = Analog[JOYSTICK_X_ANALOG_INDEX];

	// Filter only if not first pass
	if(!(Z8_STATUS_4 & first_pass_z4))
		current_value = (Ctrl.Ctrl_sc.joystick_x + current_value) >> 1;	
	// now rescale (x 1.03125)
	tmp = current_value >> 5; // V1.61 >> 6
	current_value += tmp;
	if(current_value > 127)
		current_value = 127;

	// Joystick X has changed ?
	if (Ctrl.Ctrl_sc.joystick_x != current_value)
	{
		if(!(Z8_STATUS_4 & first_pass_z4))
		{
			// update control index 
			control_index = JOYSTICK_X_CTRL_INDEX;

			// are we in assign mode
			if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
				  control_assign_index = control_index;
			else
			if(assign_state != ASSIGN_ACTIVE)
			{
				// must store new pot value before sending to midi since send to midi may modify it
				Ctrl.Ctrl_sc.joystick_x = current_value;
	
				// Send to MIDI
				SendControlToMidi();		
			
				// Update LCD string only if required
				if(last_control_index != control_index)
				{
					// Update lcd str
					UpdateLcdString();
					last_control_index = control_index;
				}
		
				must_update_lcd_joystick_x = TRUE;
			} // end not assign mode
		} // end not first pass
	}

	// Check if joystick Y has changed
	// Get7BitsADValue(CROSSFADER_AD_CHANNEL, 0);
	current_value = Analog[JOYSTICK_Y_ANALOG_INDEX];

	// filter only if not first pass
	if(!(Z8_STATUS_4 & first_pass_z4))
		current_value = (Ctrl.Ctrl_sc.joystick_y + current_value) >> 1;
	// now rescale (x 1.03125)
	tmp = current_value >> 5; // V1.61 >> 6
	current_value += tmp;
	if(current_value > 127)
		current_value = 127;
	
	// Joystick Y has canged ?
	if (Ctrl.Ctrl_sc.joystick_y != current_value)
	{
		if(!(Z8_STATUS_4 & first_pass_z4))
		{
			// update control index 
			control_index = JOYSTICK_Y_CTRL_INDEX;

			// are we in assign mode
			if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
				  control_assign_index = control_index;
			else
			if(assign_state != ASSIGN_ACTIVE)
			{
				// must store new pot value before sending to midi since send to midi may modify it
				Ctrl.Ctrl_sc.joystick_y = current_value;
	
				// Send to MIDI
				SendControlToMidi();		
		
				// Update LCD string only if required
				if(last_control_index != control_index)
				{
						// Update lcd str
						UpdateLcdString();
						last_control_index = control_index;
				}
		
				must_update_lcd_joystick_y = TRUE;
			} // end not in assign mode
		} // end not first pass
	}

	// Update Joystick LCD zone, values if required 
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;
	if( (must_update_lcd_joystick_x==TRUE)||(must_update_lcd_joystick_y==TRUE))
	{
		// If joystcick area already active then do not display it again
		if(lcd_current_incrust != LCD_INCRUST_JOYSTICK)
		{
			// Area joystick was not active, force x and y display
			must_update_lcd_joystick_x = TRUE;
			must_update_lcd_joystick_y = TRUE;

			// Display empty incrust zone
			FillLcdZone(LCD_ZONE_INCRUST_X-1, LCD_ZONE_INCRUST_Y, LCD_ZONE_INCRUST_X_PIX+1, LCD_ZONE_INCRUST_Y_PAGE, FILL_BLANK);
			
			// Show the 4 corners joystick area
			DisplayBmp(LCD_ZONE_JOY_BORDER_LEFT_X   , LCD_ZONE_JOY_BORDER_LEFT_Y   , LCD_INVERT, bmp_joy_border_left);
			DisplayBmp(LCD_ZONE_JOY_BORDER_RIGHT_X  , LCD_ZONE_JOY_BORDER_RIGHT_Y  , LCD_INVERT, bmp_joy_border_right);
			// Show X and Y text
			DisplayBmp(LCD_ZONE_JOY_BORDER_RIGHT_X+6, LCD_ZONE_JOY_BORDER_RIGHT_Y+2, LCD_INVERT, bmp_joy_x_y_txt);
		}

		UpdateLCDJoystickArea();

		// Display X and Y values if required
		if(must_update_lcd_joystick_x==TRUE)
			DisplayLcdInt(LCD_ZONE_JOY_BORDER_RIGHT_X+12, LCD_ZONE_INCRUST_Y+2, DIGITS_3|LCD_INVERT, font_5x8, Ctrl.Ctrl_sc.joystick_x);
		if(must_update_lcd_joystick_y==TRUE)
			DisplayLcdInt(LCD_ZONE_JOY_BORDER_RIGHT_X+12, LCD_ZONE_INCRUST_Y+3, DIGITS_3|LCD_INVERT, font_5x8, Ctrl.Ctrl_sc.joystick_y);

		// Joystick zone area is active now
		lcd_current_incrust = LCD_INCRUST_JOYSTICK;
		
		// Lock LCD incrust zone for 1 second
		lcd_lock_incrust_cpt = CPT_1_SEC;
	}
}
*/

// V1.61
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckJoystickOneAxisSendMidi()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check joystick X, Y and button is checked in another function since digital
//				Joystick are 0-125, need to multiply by 1.016 : >>6 
///////////////////////////////////////////////////////////////////////////////////////
uchar CheckJoystickOneAxisSendMidi(uchar joystick_axis_analog_index)
{
	uchar must_update_lcd_joystick;
	uchar *current_joystick_ptr;
	far uchar *current_physical_ptr;
	uchar local_physical;
	uchar difference_sign, local_diff_sign;
	uchar hook_bmp_index;
	uchar tmp, current_joystick_7F, must_send_to_midi;

	if(joystick_axis_analog_index == JOYSTICK_X_ANALOG_INDEX)
	{
		current_joystick_ptr = &Ctrl.Ctrl_sc.joystick_x;
		current_physical_ptr = &Physical[current_group].joystick_x;
	}
	else
	{
		current_joystick_ptr = &Ctrl.Ctrl_sc.joystick_y;
		current_physical_ptr = &Physical[current_group].joystick_y;
	}

	// Mask Hook bit
	current_joystick_7F = *current_joystick_ptr & (~HOOK_BIT);

	// Check if joystick X has changed
	current_value = Analog[joystick_axis_analog_index];

	// Filter only if not first pass
	if(!(Z8_STATUS_4 & first_pass_z4))
		current_value = (current_joystick_7F + current_value) >> 1;	
	// now rescale (x 1.03125)
	tmp = current_value >> 5; // V1.61 >> 6
	current_value += tmp;
	if(current_value > 127)
		current_value = 127;

	must_send_to_midi 		 = FALSE;
	must_update_lcd_joystick = FALSE;
	hook_bmp_index	  		 = NO_HOOK_BMP;

	// FIRST PASS : check if autosend and also init hook mode
	if(Z8_STATUS_4 & first_pass_z4)
	{
		FillParamCurrentStruct(); // V1.61

		// check autosend flag
		if(ParamCurrent->ctrl_status_1 & autosend)
			must_send_to_midi = TRUE;
		*current_joystick_ptr =  current_value | HOOK_BIT;
		// initialize physical register to physical value, sign is 0 (lower or equal)
		for(tmp=0; tmp<GROUP_NB; tmp++)
		{
			if(joystick_axis_analog_index == JOYSTICK_X_ANALOG_INDEX)
				Physical[tmp].joystick_x = current_value;
			else
				Physical[tmp].joystick_y = current_value;
		}
	} // end first pass	
	else
	{
		// joystick x has changed ?
		if (current_joystick_7F != current_value)
		{	
			// are we in assign mode
			if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
			{
				control_assign_index = control_index;			
			}
			else
			if(assign_state != ASSIGN_ACTIVE)
			{
				must_update_lcd_joystick = TRUE;
				// V1.8 fill structure of the pot before calling SendCtrlToMidi() because we need to know the hook config now
				FillParamCurrentStruct();
				// joystick x has changed and jump mode
				// V1.8 if(!(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm)) replaced by
				if(!(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode))
					must_send_to_midi = TRUE;
				// joystick x has changed and hook mode is active
				else
				{
					local_physical 	= *current_physical_ptr;
					// is the pot hooked thus ready to be sent ?
					hook_bmp_index = CheckHookPot(current_joystick_ptr, local_physical, current_value);
					// hooked : ready to send midi
					if(hook_bmp_index == NO_HOOK_BMP)
						must_send_to_midi = TRUE;
				} // end pot has changed and hook mode is active
			} // end not in assign mode
		} // end current pot has changed
	} // end not first pass

	if(must_send_to_midi) 
	{
		*current_joystick_ptr = current_value  | HOOK_BIT;

		// Send Joystick current axis
		SendControlToMidi();		
	}

	// Update LCD string only if required
	if(must_update_lcd_joystick)
	{
		if(last_control_index != control_index)
		{
			// Update lcd str
			UpdateLcdString();
			last_control_index = control_index;
		}
	
		// Update pots hook state only if hook mode
		// V1.8 if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm) replaced by
		if(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode)
			UpdateLcdHookState(hook_bmp_index);
	}

	return must_update_lcd_joystick;
}

// V1.61
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckJoystick()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check joystick X, Y and button is checked in another function since digital
//				Joystick are 0-125, need to multiply by 1.016 : >>6 
///////////////////////////////////////////////////////////////////////////////////////
void CheckJoystick(void)
{
	uchar must_update_lcd_joystick_x;
	uchar must_update_lcd_joystick_y;

	control_index = JOYSTICK_X_CTRL_INDEX;
	must_update_lcd_joystick_x = CheckJoystickOneAxisSendMidi(JOYSTICK_X_ANALOG_INDEX);

	control_index = JOYSTICK_Y_CTRL_INDEX;
	must_update_lcd_joystick_y = CheckJoystickOneAxisSendMidi(JOYSTICK_Y_ANALOG_INDEX);

	// Update Joystick LCD zone, values if required 
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;
	if( (must_update_lcd_joystick_x)||(must_update_lcd_joystick_y))
	{
		// If joystcick area already active then do not display it again
		if(lcd_current_incrust != LCD_INCRUST_JOYSTICK)
		{
			// Area joystick was not active, force x and y display
			must_update_lcd_joystick_x = TRUE;
			must_update_lcd_joystick_y = TRUE;

			// Display empty incrust zone
			FillLcdZone(LCD_ZONE_INCRUST_X-1, LCD_ZONE_INCRUST_Y, LCD_ZONE_INCRUST_X_PIX+1, LCD_ZONE_INCRUST_Y_PAGE, FILL_BLANK);
			
			// Show the 4 corners joystick area
			DisplayBmp(LCD_ZONE_JOY_BORDER_LEFT_X   , LCD_ZONE_JOY_BORDER_LEFT_Y   , LCD_INVERT, bmp_joy_border_left);
			DisplayBmp(LCD_ZONE_JOY_BORDER_RIGHT_X  , LCD_ZONE_JOY_BORDER_RIGHT_Y  , LCD_INVERT, bmp_joy_border_right);
			// Show X and Y text
			DisplayBmp(LCD_ZONE_JOY_BORDER_RIGHT_X+6, LCD_ZONE_JOY_BORDER_RIGHT_Y+2, LCD_INVERT, bmp_joy_x_y_txt);
		}

		UpdateLCDJoystickArea();

		// V1.62 display bmp SHIFT locked if required
		CheckDisplayCurrentValueLocked();

		// Display X and Y values if required
		if(must_update_lcd_joystick_x)
			DisplayLcdInt(LCD_ZONE_JOY_BORDER_RIGHT_X+12, LCD_ZONE_INCRUST_Y+2, DIGITS_3|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, Ctrl.Ctrl_sc.joystick_x & (~HOOK_BIT));
		if(must_update_lcd_joystick_y)
			DisplayLcdInt(LCD_ZONE_JOY_BORDER_RIGHT_X+12, LCD_ZONE_INCRUST_Y+3, DIGITS_3|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, Ctrl.Ctrl_sc.joystick_y & (~HOOK_BIT));

		// Joystick zone area is active now
		lcd_current_incrust = LCD_INCRUST_JOYSTICK;
		
		// Lock LCD incrust zone for 1 second
		lcd_lock_incrust_cpt = CPT_1_SEC;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckCrossFader()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Crossfader check changes
///////////////////////////////////////////////////////////////////////////////////////
void CheckCrossFader(void)	   
{
	uchar current_crossfader_7F;
	uchar must_send_to_midi, must_update_lcd;
	uchar local_physical;
	uchar difference_sign, local_diff_sign, group_index;
	uchar hook_bmp_index;
	
	// Mask Hook bit
	current_crossfader_7F = Ctrl.Ctrl_sc.crossfader_r & (~HOOK_BIT);

	// Get7BitsADValue(AUTOMATION_POT_0_AD_CHANNEL, 0);
	current_value = Analog[CROSSFADER_ANALOG_INDEX];

	// apply a digital filter if not first pass
   	if( (current_value != 127) && (!(Z8_STATUS_4 & first_pass_z4)))
	     current_value = (current_crossfader_7F + current_value) >> 1;
	
	control_index = CROSSFADER_R_CTRL_INDEX;

	must_send_to_midi = FALSE;
	must_update_lcd	  = FALSE;
	hook_bmp_index	  = NO_HOOK_BMP;

	if(Z8_STATUS_4 & first_pass_z4)
	{
		FillParamCurrentStruct(); // V1.61

		// check autosend flag
		if(ParamCurrent->ctrl_status_1 & autosend)
			must_send_to_midi = TRUE;
		Ctrl.Ctrl_sc.crossfader_r = 	  current_value | HOOK_BIT;
		Ctrl.Ctrl_sc.crossfader_l = (127-current_value) | HOOK_BIT;
		// initialize physical register to physical value, sign is 0 (lower or equal)
		for(group_index=0; group_index<GROUP_NB; group_index++)
		{
			Physical[group_index].crossfader_r = current_value;
			Physical[group_index].crossfader_l = current_value;		// ?????????
		}
	} // end first pass
	else
	{
		// crossader has changed ?
		if(current_crossfader_7F != current_value)
		{	
			// are we in assign mode
			if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
			{
				// if cf moved from right to left then assign left
				if(current_value < current_crossfader_7F)
				  control_assign_index = CROSSFADER_R_CTRL_INDEX;
				else
				  control_assign_index = CROSSFADER_L_CTRL_INDEX;
			}
			else
			if(assign_state != ASSIGN_ACTIVE)
			{
				must_update_lcd	 = TRUE;
				// V1.8 fill structure of the pot before calling SendCtrlToMidi() because we need to know the hook config now
				FillParamCurrentStruct();
				// crossfader has changed and jump mode
				// V1.8 if(!(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm)) replaced by
				if(!(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode))
					must_send_to_midi = TRUE;
				// crossfader has changed and hook mode is active
				else
				{
					local_physical 	= Physical[current_group].crossfader_r;
					// is the pot hooked thus ready to be sent ?
					hook_bmp_index = CheckHookPot(&Ctrl.Ctrl_sc.crossfader_r, local_physical, current_value);
					// hooked : ready to send midi
					if(hook_bmp_index == NO_HOOK_BMP)
						must_send_to_midi = TRUE;
				} // end pot has changed and hook mode is active
			} // end not in assign mode
		} // end current pot has changed
	} // end not first pass

	if(must_send_to_midi) 
	{
		Ctrl.Ctrl_sc.crossfader_r =      current_value  | HOOK_BIT;
		Ctrl.Ctrl_sc.crossfader_l = (127-current_value) | HOOK_BIT;
		// First send crossfader RIGHT
		//control_index 	= CROSSFADER_R_CTRL_INDEX;

		// Now current value is crossfaded
		//current_value = Ctrl.Ctrl_sc.crossfader_r & (~HOOK_BIT);

		// In mackie mode, crossfader right is not sent
		// V1.67 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS != MACKIE_MODE)
		if( (OPERATING_MODE_NVM != MACKIE_MODE) && (OPERATING_MODE_NVM != LOGIC_MODE))	
		{
			// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
			//CheckRealtimeMidi();
			// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill structure from v1.8
			//FillParamCurrentStruct();
			// Send CROSSFADER_R to MIDI
			SendControlToMidi();		
		}
	}

	if(must_update_lcd)
	{
		//current_value = Ctrl.Ctrl_sc.crossfader_r & (~HOOK_BIT);

		// V1.5 : In mackie mode the crossfader area incrust zone is not displayed
		// Mackie mode ?
		//V1.67 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS == MACKIE_MODE)
		// if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) == MACKIE_MODE_TEST_POS)
		if( (OPERATING_MODE_NVM  == MACKIE_MODE) || (OPERATING_MODE_NVM  == LOGIC_MODE))
		{
			DisplayCurrentValue();
			DisplayMackieMetering(MACKIE_MASTER_BARGRAPH); 
			// V1.66 in mackie mode crossfader hook state is diplayed like a pot
			// V1.8 if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm) replaced by
			if(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode)
				UpdateLcdHookState(hook_bmp_index);

		}
		else // not mackie mode
		{
			if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
			{
				// If crossfader area already active then do not display it again
				if(lcd_current_incrust != LCD_INCRUST_CROSSFADER)
				{
					// Display incrust zone
					//DisplayBmp(LCD_ZONE_INCRUST_X, LCD_ZONE_INCRUST_Y, LCD_INVERT, bmp_incrust);
					// Display incrust title
					rom_to_lcd_text_copy(fader_txt);
					//DisplayLcdText(LCD_ZONE_INCRUST_X+6, LCD_ZONE_INCRUST_Y, LCD_NORMAL, font_5x8);		
					DisplayEmptyIncrust();
					// Display crossfader template
					//if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
						FillLcdZone(LCD_ZONE_FADER_START, LCD_ZONE_FADER_PAGE, (uchar)(LCD_ZONE_FADER_END-LCD_ZONE_FADER_START), 1, 0xF3);
					// Force hook fader state to be re-displayed only if hook mode
					// V1.8 if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm) replaced by
					if(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode)
						Z8_STATUS_4 |= force_hook_fader_display_z4;
				}
	

				// Update crossfader bmp 
				UpdateLCDCrossfaderArea();

				// Display X and Y values if required
				DisplayLcdInt(LCD_ZONE_FADER_START-1, LCD_ZONE_INCRUST_Y+3, DIGITS_3|LCD_INVERT|FONT_5x8_FLG, current_value);

				DisplayLcdInt(LCD_ZONE_FADER_END-18 , LCD_ZONE_INCRUST_Y+3, DIGITS_3|LCD_INVERT|FONT_5x8_FLG, 127-current_value);

				// Joystick zone area is active now
				// Z8_STATUS_3 |= crossfader_lcd_area_active;
				lcd_current_incrust = LCD_INCRUST_CROSSFADER;
				// Lock LCD incrust zone for 1 second
				lcd_lock_incrust_cpt = CPT_1_SEC;

				// Update crossfader hook state only if hook mode and not mackie mode
				// V1.8 if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm) replaced by
				if(ParamCurrent->ctrl_status_1 & switch_ctrl_hook_mode)
				{
					// V1.66 if crossfader was not hooked yet, update bmp
					if(hook_bmp_index)
						hook_bmp_index = HOOK_FADER_BMP;
					UpdateLcdFaderHookState(hook_bmp_index);
				}
			}

			// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
			CheckRealtimeMidi();

		} // end not mackie mode
		

		// V1.62 display bmp SHIFT locked if required
		CheckDisplayCurrentValueLocked();
	}

	// LEFT CROSS FADER NOW
	if(must_send_to_midi)
	{
		// Be sure last crossfader displayed is crossfader RIGHT
		//last_control_index = control_index;
		// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
		//CheckRealtimeMidi();	
			
		// now select crossfader left
		control_index 	= CROSSFADER_L_CTRL_INDEX;
		// Now return to initiial current value
		current_value = Ctrl.Ctrl_sc.crossfader_l & (~HOOK_BIT);
		// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() do not fill structure anymore in V1.8
		FillParamCurrentStruct();
		// Send CROSSFADER_L to MIDI
		SendControlToMidi();	

		//CheckRealtimeMidi();
	}

	if(must_update_lcd)
	{
		// Update LCD string only if required : single crossfader str displayed (Left)
		if(last_control_index != control_index)
		{
			// Update lcd str
			UpdateLcdString();
			//if(must_send_to_midi) // V1.61
			last_control_index = control_index;
		}
	}
}

/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CalibrateAdc()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	calibarte adc offset using crossfader
///////////////////////////////////////////////////////////////////////////////////////
void CalibrateAdc(void)
{
	uchar offset_low, offset_high;
	uchar ok_pin_passed_0 = TRUE;
	uchar pass = 0;
 
	// First clear display
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);	
	// diplay text
	rom_to_lcd_text_copy(adc_calibrate_txt);
	DisplayLcdText(5, 1, LCD_INVERT, font_5x8);

	rom_to_lcd_text_copy(please_set_txt);
	DisplayLcdText(1, 3, LCD_INVERT, font_5x8);
	rom_to_lcd_text_copy(fader_txt);
	DisplayLcdText(64, 3, LCD_INVERT, font_5x8);
	rom_to_lcd_text_copy(push_ok_txt);
	DisplayLcdText(40, 4, LCD_INVERT, font_5x8);

	while(pass < 2)
	{
		// left
		if(pass==0)
			rom_to_lcd_text_copy(left_txt);
		else
			rom_to_lcd_text_copy(right_txt);
			
		DisplayLcdText(1, 4, LCD_INVERT, font_5x8);

		// lauchn analog conv on crossfader
		ADCCTL = 0xA0 + CROSSFADER_AD_CHANNEL;

		// wait for conversion complete
#ifndef SIMUL
		while(ADCCTL & 0x80);
#endif

		DisplayLcdInt(90, 6, DIGITS_3|LCD_INVERT, font_5x8, (ADCD_H >> 1));

		
			// ok switch pressed ?
			if(OK_PORT_IN & OK_PIN)
			{
				if(ok_pin_passed_0)
				{
					ok_pin_passed_0 = FALSE;
					// expect min value
					if(pass == 0)
						offset_low 	= ADCD_H >> 1;
					else
						offset_high = (0xFF - ADCD_H) >> 1;
					delay_msec(500);
					pass++;
				}
			}
			else ok_pin_passed_0 = TRUE;
	} // end while

	if(offset_high)
		ADC_Offset = 127 + offset_high;
	else
	if(offset_low)
		ADC_Offset = 127 - offset_low;
	else
		ADC_Offset = 127;

	// ADC_Offset will be written to eeprom in 5sec max

}

*/


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckHookPot()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	return NO_HOOK_BMP, HOOK_DOWN_BMP, HOOK_UP_BMP
///////////////////////////////////////////////////////////////////////////////////////
uchar CheckHookPot(uchar *param_value, uchar physical_value, uchar new_pot_value)
{		
	uchar local_physical;
	uchar difference_sign, local_diff_sign;
	uchar current_pot_7F = *param_value & (~HOOK_BIT);
			
	// is the pot already hooked ?
	if(*param_value & HOOK_BIT)
		return(NO_HOOK_BMP);
	// pot changed, hook mode, still not hooked
	else
	{
		// pot will become hooked when difference sign will change
		local_physical 	= physical_value & (~HOOK_BIT);
		
		// is the pot equal to physical value ?
		if(current_pot_7F == local_physical)
			return(NO_HOOK_BMP);
		else
		{
			difference_sign	= physical_value & HOOK_BIT;
			if(current_value < local_physical)
				local_diff_sign = NEGATIVE_BIT;
			else
				local_diff_sign = POSITIVE_BIT;

			// is the pot < physical value ?
			if(local_diff_sign != difference_sign)
				return(NO_HOOK_BMP);
			else
			{
				*param_value = new_pot_value;
				if(local_diff_sign) return(HOOK_DOWN_BMP);
				else				return(HOOK_UP_BMP);
			}
		} // end pot not equal to physical value
	} // end pot changed, hook mode, still not hooked
}

