#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "motion.h"
//#include "analog.h"
#include "eeprom.h"
#include "hardware.h"
#include "lcd_low_level.h"
#include "lcd.h"
#include "serial.h"
#include "i2c.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
far 	Motion_Struct Motion_Str[MOTION_PART_NB];
uchar 	Motion_Status_Play; 	// each bit indicates if part's playing
uchar 	Motion_Status_Rec; 		// each bit indicates if part's recording

rom uchar motion_point_nb[] = {	MOTION_PTS_PER_LENGHT, 
								MOTION_PTS_PER_LENGHT * 2,
								MOTION_PTS_PER_LENGHT * 3,
								MOTION_PTS_PER_LENGHT * 4,
								MOTION_PTS_PER_LENGHT * 5,
								MOTION_PTS_PER_LENGHT * 6,
								MOTION_PTS_PER_LENGHT * 7,
								MOTION_PTS_PER_LENGHT * 8};

extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_7;
extern uchar Z8_STATUS_9;
extern uchar Z8_STATUS_10;	// V1.7
extern uchar control_index;			// current control nb
extern uchar last_control_index;	// last control nb sent
extern uchar infrared;

extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern rom uchar full_motion_rec_bmp[];
extern rom uchar un_demi_bmp[];
extern rom uchar motion_rec_confirm_bmp[];
// V1.66 extern rom uchar font_5x8[];
extern rom uchar bmp_small_int_txt[];
extern rom uchar bmp_small_ext_txt[];
extern uchar current_value;					// current control value
extern All_Controls Ctrl;				// All controls are defned here
extern uint EE_Address;
extern uchar I2cCommand;					// current EEProm selection
//extern rom uchar *local_bmp_ptr;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckRealtimeMotion()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check if new MOTION value has to be sent
///////////////////////////////////////////////////////////////////////////////////////
void CheckRealtimeMotion(uchar realtime_data)
{
	uchar Mask;
	uchar current_motion_part;
	uchar x_pos;
	uchar motion_data_index_local;
	far uchar *far_ptr;
	uchar motion_ctrl_index_local;
	uchar mask_sw_local;

	if(realtime_data == MIDI_STOP)
	{
		Z8_STATUS_10 &= ~realtime_playing_z10;
//		// stop motion only if not recording
//		Motion_Status_Play = 0;
	}
//	else
	if( (realtime_data == MIDI_START) || (realtime_data == MIDI_CONTINUE) )
	{
		// V1.7 reset all indexes to zero when play is requested
//		Mask  = 1;
		Z8_STATUS_10 |= realtime_playing_z10;
		for(current_motion_part=0; current_motion_part < MOTION_PART_NB; current_motion_part++)
		{
			if(realtime_data == MIDI_START)
			{
				// reset all indexes to zero when play is requested
				Motion_Str[current_motion_part].motion_data_index 		= 0;
				Motion_Str[current_motion_part].motion_last_data_sent 	= CTRL_UNDEF;
				Motion_Str[current_motion_part].motion_clock_counter 	= 0;
			}
			// force play on selected motion track for START or CONTINUE
//			Motion_Status_Play |= Mask;
			// select next track
//			Mask = Mask << 1;
		}
	}
	else
	// Check the realtime MIDI message received
	// V1.7 if(realtime_data == MIDI_CLOCK)
	if((realtime_data == MIDI_CLOCK) && (Z8_STATUS_10 & realtime_playing_z10))
	{
		Mask  = 1;
		x_pos = LCD_ZONE_MOTION_STATE_X; 
		// reach all 8 motion parts
		for(current_motion_part=0; current_motion_part < MOTION_PART_NB; current_motion_part++)
		{
			// is the motion track containing data ?
			if(Motion_Str[current_motion_part].motion_ctrl_index != CTRL_UNDEF)
			{
				// clock counter is reached ?
				if(++(Motion_Str[current_motion_part].motion_clock_counter) == MOTION_CLOCK_STEP)
				{
					motion_data_index_local = Motion_Str[current_motion_part].motion_data_index;
					motion_ctrl_index_local = Motion_Str[current_motion_part].motion_ctrl_index;

					/////////////////////// MOTION CURRENTLY PLAYING //////////////////////////////
					if(Motion_Status_Play & Mask)
					{
						current_value = Motion_Str[current_motion_part].motion_data_table[motion_data_index_local];
						// Now send this control to MIDI outs only if different than preceding sent
						if(current_value != Motion_Str[current_motion_part].motion_last_data_sent)
						{
							control_index = motion_ctrl_index_local + TRANSPORT_NB;
							// Inform to sendcontroltomidi that a motion is currently played
							Z8_STATUS_9 |= sending_motion_to_midi_z9;
							// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill struct from V1.8
							FillParamCurrentStruct();
							// send now to midi
							SendControlToMidi();
							// Finished to send a motion
							Z8_STATUS_9 &= ~sending_motion_to_midi_z9;
							// Force to get midi str from eeprom
							last_control_index = control_index;					
							// save last data sent
							Motion_Str[current_motion_part].motion_last_data_sent = current_value;
							// Display small bargraph value on selected part
							DisplaySmallBargraphValue(x_pos, current_value);
						}
						// increase motion data buffer index and Loop on last motion data index depending on lenght
						if(++motion_data_index_local == motion_point_nb[Motion_Str[current_motion_part].motion_length])
						{
							Motion_Str[current_motion_part].motion_data_index =	0;
							// stop playing if not in loop mode
							if(!(Z8_STATUS_1_NVM & motion_loop_state_z1nvm))
								Motion_Status_Play &=  ~Mask;
						}
						else
							Motion_Str[current_motion_part].motion_data_index = motion_data_index_local;
					} // end playing
					else
					//////////////////// MOTION CURRENTLY RECORDING ///////////////////////////////
					if( (Motion_Status_Rec & Mask) && (!(Z8_STATUS_9 & motion_wait_rec_confirm_z9)))
					{
						// All 16 switches are treated separetely
						if(motion_ctrl_index_local >= (BW_ASSIGN_SW_CTRL_INDEX - TRANSPORT_NB))
						{
							// it is a other switch
							if(motion_ctrl_index_local < (SWITCH_BASE_CTRL_INDEX - TRANSPORT_NB))
							{
								far_ptr = (far uchar *) &Ctrl.Ctrl_sc.other_sw_value;
								mask_sw_local = 1 << (motion_ctrl_index_local-(BW_ASSIGN_SW_CTRL_INDEX-TRANSPORT_NB));
							}
							else
							{
								far_ptr = (far uchar *) &Ctrl.Ctrl_sc.all_sw_value;
								mask_sw_local = 1 << (motion_ctrl_index_local-(SWITCH_BASE_CTRL_INDEX-TRANSPORT_NB));
							}
							// Now assign the switch value
							if(*far_ptr & mask_sw_local) mask_sw_local = 127;
							else						 mask_sw_local = 0;
							Motion_Str[current_motion_part].motion_data_table[motion_data_index_local] = mask_sw_local;
						}
						// It is an analog control : assign it directly if it is not the infrared
						else
						{
							//Is it the infrared
							if(motion_ctrl_index_local == (INFRARED_CTRL_INDEX-TRANSPORT_NB)) 
								far_ptr = (far uchar *) &infrared;
//								Motion_Str[current_motion_part].motion_data_table[motion_data_index_local] = infrared & 0x7F;
							else
								// record this motion value
								far_ptr = (far uchar *) (&Ctrl.Ctrl_sc.CTRL_STRUCT_START_FIELD + motion_ctrl_index_local);
							Motion_Str[current_motion_part].motion_data_table[motion_data_index_local] = *far_ptr & 0x7F;
						}
						// Display motion recording graph
						DisplayMotionRecGraph(current_motion_part, motion_data_index_local);
						// Display motion recoding step value into the measures
	
						// increase motion data buffer index and Loop on last motion data index depending on lenght
						if(++motion_data_index_local == (motion_point_nb[Motion_Str[current_motion_part].motion_length]))
						{
							// Finish recording ?
							Motion_Str[current_motion_part].motion_data_index =	0;
							// wait recorded motion to confirm
							Z8_STATUS_9 |= motion_wait_rec_confirm_z9;
							// display confirm bmp
							DisplayBmp(35, 7, LCD_INVERT, motion_rec_confirm_bmp);
							//Motion_Status_Rec = 0;
							// reinit normal screen
							//Z8_STATUS_7 |= must_reinit_all_lcd_z7;
						}
						else
							Motion_Str[current_motion_part].motion_data_index = motion_data_index_local;
					} // end motion recording
	
					Motion_Str[current_motion_part].motion_clock_counter = 0;
				} // end clock step reached
			} // end motion control index was undefined
			Mask = Mask << 1;
			x_pos += 3;
		}	// end all 8 motions
	} // end midi clock received


}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckMotionRecording()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	check if a control motion has to be recorded
///////////////////////////////////////////////////////////////////////////////////////
void CheckMotionRecording(void)
{
	rom uchar *local_bmp_ptr;
	uchar tmp, motion_lenght_local;

	// Check if the current control is concerned by motion

	// Ignore LFO while recording a motion 
	if(control_index == LFO_CTRL_INDEX) return;
	// Ignore scenes while recording a motion
	if(Z8_STATUS_4 & sending_scene_z4) return;
	// exit if sending transport 
	if(control_index <= FORWARD_CTRL_INDEX)	return;
	// exit also if we are in menus or assign modes
//	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;


	// Check if we are already recording or if it is a new control to record
	if(Motion_Str[motion_part].motion_ctrl_index == CTRL_UNDEF)
	{
		// Start the record phase on selected control
		Motion_Str[motion_part].motion_ctrl_index 		= control_index - TRANSPORT_NB;
		// reset the motion struct parameters
		Motion_Str[motion_part].motion_data_index 		= 0;
		Motion_Str[motion_part].motion_last_data_sent 	= CTRL_UNDEF;
		Motion_Str[motion_part].motion_clock_counter 	= 0;
		// it is the beginning of motion recording : start the dedicated LCD screen
		DisplayBmp(0, 0, LCD_INVERT, full_motion_rec_bmp);
		// display recording control number
		DisplayLcdInt(8, 3, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, control_index );
		// display motion sync method
		if(Z8_STATUS_1_NVM & motion_sync_ext_z1nvm)
			local_bmp_ptr = bmp_small_ext_txt;
		else
			local_bmp_ptr = bmp_small_int_txt;
		DisplayBmp(10, 6, LCD_INVERT, local_bmp_ptr);
		// display recording motion part number
		DisplayLcdInt(106, 3, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, motion_part );
		// display recording motion part lenght
		// odd lenght is sync on full measure	
		motion_lenght_local = Motion_Str[motion_part].motion_length;
		if(motion_lenght_local & 1)
		{
			tmp = 5;
			// remove old 1/2 if exist
			FillLcdZone(103, 6, 22, 1, FILL_BLANK);
		}
		else
		{
			tmp = 0;
			DisplayBmp(114, 6, LCD_INVERT, un_demi_bmp);
		}
		DisplayLcdInt(102+tmp, 6, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, (motion_lenght_local + 1) >> 1);
		// Lock LCD from other functions
		Z8_STATUS_4 |= lcd_locked_by_function_z4;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckMotionRecording()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	check if a control motion has to be recorded
///////////////////////////////////////////////////////////////////////////////////////
void SaveMotionToEEprom(void)
{
	uint EE_Address_tmp; 	
	uchar i;
	uchar motion_part_local = motion_part;

	// First save all 64 bytes of data
	// First calculate EE base address according to scene select
	EE_Address 		= MOTION_DATA_PART_0_EE_ADR;
	EE_Address_tmp	= MOTION_LENGHT_AND_INDEX_0_EE_ADR;
	// motion are located in first eeprom (EEPROM_ZERO_I2C_ADR)
	I2cCommand = BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

	// add offset depending on part
	for(i=0; i<motion_part; i++)
	{
		EE_Address 		+= (uint) MOTION_PART_LENGHT; 
		EE_Address_tmp 	+= (uint) MOTION_LENGHT_AND_INDEX_SIZE; 
	}

	// Now write all 64 bytes of data
	WriteMultiByteI2c((far uchar *)&Motion_Str[motion_part_local].motion_data_table[0], motion_point_nb[Motion_Str[motion_part_local].motion_length]);

	// Now write lenght and control index
	EE_Address = EE_Address_tmp;
	WriteMultiByteI2c((far uchar *)&Motion_Str[motion_part_local].motion_ctrl_index, MOTION_LENGHT_AND_INDEX_SIZE);
}
