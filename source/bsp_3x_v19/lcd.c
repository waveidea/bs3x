#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd.h"
#include "lcd_low_level.h"
#include "lcd_fonts.h"
#include "lcd_texts.h"
#include "lcd_bmps.h"
#include "macros.h"
#include "hardware.h"
#include "eeprom.h"
#include "analog.h"
#include "serial.h"
#include "arpp.h"
#include "motion.h"
#include "mackie.h" // V1.5
#include "i2c.h"
#include "realtime.h" // V1.9

///////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////////
far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion	
uchar old_joy_x_pixel; 		// V1.6	= 30;
uchar old_joy_y_page_start;  // V1.6	= 3;
uchar old_joy_y_page_end;	// V1.6	= 3;
uchar old_fader_x_pixel;	// V1.6		= 60;
uchar old_fader_hooked_x_pixel; //V1.6	= 60;
uchar lcd_current_incrust;
uchar current_curve_model 	; // V1.6	= CTRL_UNDEF;
//V1.7 uchar lfo_graph_index;
far uchar last_displayed_channel;	// V1.62

far uchar lcd_strings_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB][LCD_STR_SIZE];	// all lcd strings preloaded in RAM V1.61

//rom uchar *local_bmp_ptr;

extern uchar Z8_STATUS_0;
extern uchar Z8_STATUS_3;
extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_6;
extern uchar Z8_STATUS_8;
extern uchar Z8_STATUS_9;
extern uchar Z8_STATUS_10;
extern uchar encoder_value;
extern uchar transport_state, next_transport_state;
extern uchar current_value;							// current control value
extern rom uchar identity[];
//extern uchar lcd_lock_all_cpt;						// allow to lock all LCD zones
extern uchar lcd_lock_incrust_cpt;					// allow to lock LCD incrust zone
extern uchar current_function;			
extern uint EE_Address;					// current EEProm address
extern uchar I2cCommand;							// current EEProm selection
extern uchar control_index;						// current control index
extern All_Controls Ctrl;						// All controls are defned here
//extern far All_Params	Param;						// structure of parameters in EEPROM
extern far All_Params *ParamCurrent;		// V1.61 current structure of parameters in EEPROM
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving
extern uchar current_hook_bmp ;
extern far Controls_Scene	Physical[GROUP_NB];	// All physical values and difference sign stored here
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];		// NN=1 registers can be accessed from ez board 
extern far 	Motion_Struct Motion_Str[MOTION_PART_NB];
extern uchar Motion_Status_Play; 		// each bit indicates if part's playing
extern uchar Motion_Status_Rec; 		// each bit indicates if part's recording

extern far uchar STATUS_Z8_EXP; // V1.5

// TO REMOVE
// extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes


/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: Display_Upgrade()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void Display_Upgrade(void)
{
	// clear lcd
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);
	// display upgarde txt
	rom_to_lcd_text_copy(upgrading_txt);
	DisplayLcdText(20, 3, LCD_INVERT, font_5x8);

}
*/

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateLcdZones()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Graphic LCD zones
///////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdZones(int update_zones)
{
	rom uchar * local_bmp_ptr;
//	far uchar *local_ram_far_ptr;
	uchar i;

	// return if lcd fully locked
//	if(lcd_lock_all_cpt) return;
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Re init LCD channel if required
	if(update_zones & INIT_LCD_CHANNEL)
	{
		if(last_displayed_channel != current_channel)
		{
			DisplayLcdInt(LCD_ZONE_CHANNEL_X, LCD_ZONE_CHANNEL_Y, DIGITS_2|LCD_INVERT|FONT_NUM_8x16_FLG/* V1.66 , font_num_8x16 */, current_channel+1 );
			last_displayed_channel = current_channel;	
		}
	}

	// Re init LCD scene if required
	if(update_zones & INIT_LCD_SCENE)
	{
		if(Z8_STATUS_3_NVM & scene_mode_z3nvm)
			i = current_scene_arp;
		else
			i = current_scene;

		// V1.8 DisplayLcdInt(LCD_ZONE_SCENE_X, LCD_ZONE_SCENE_Y, DIGITS_2|LCD_INVERT|FONT_NUM_8x16_FLG, current_scene );
		DisplayLcdInt(LCD_ZONE_SCENE_X+3, LCD_ZONE_SCENE_Y+1, DIGITS_2|LCD_INVERT|FONT_5x8_FLG, i);
	}

	// Check if zone incrust is locked
	if(!lcd_lock_incrust_cpt)
	{	
		// Re init LCD function if required
		if(update_zones & INIT_LCD_FUNCTION)
			UpdateLcdFunction();

		// Re init LCD group if required
		if(update_zones & INIT_LCD_GROUP)
		{
			// Display group number
			DisplayLcdInt(LCD_ZONE_GROUP_X, LCD_ZONE_GROUP_Y, DIGITS_2|LCD_INVERT|FONT_NUM_8x16_FLG/* V1.66 , font_num_8x16 */, current_group );

			// V1.5 In mackie mode the group name is replaced by Time code : make nothing
			// V1.69 if((!(Z8_STATUS_2_NVM & operating_mode1_z2nvm)) || (Z8_STATUS_2_NVM & operating_mode0_z2nvm))
			if( (OPERATING_MODE_NVM != MACKIE_MODE) && 
				(OPERATING_MODE_NVM != LOGIC_MODE))
			{
				// Now Display group name
#ifdef TEST 	// V1.5
				rom_to_lcd_text_copy(test_txt);
#else
				EE_Address  = (uint)(GROUP_NAME_BASE_EE_ADR + (uint)((uint)GROUP_NAME_EE_LENGHT * (uint)current_group));
				I2cCommand	= BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
				// ReadMultiByteI2c((lcd_text_value+1), GROUP_NAME_LENGHT);
				ReadMultiByteI2c((far uchar *)&lcd_text_value[1], GROUP_NAME_LENGHT);
#endif
				lcd_text_value[0]=FONT_6x8_BDR_LEFT_INV;
				lcd_text_value[GROUP_NAME_LENGHT+1]=FONT_6x8_BDR_RIGHT_INV;
				lcd_text_value[GROUP_NAME_LENGHT+2]='\0';
				DisplayLcdText(LCD_ZONE_GROUP_NAME_X, LCD_ZONE_GROUP_NAME_Y, LCD_NORMAL|FONT_5x8_FLG/* V1.66 , font_5x8 */ );		
			} // end not mackie mode
			else
			{
				// V1.5 Mackie mode : redisplay Mackie Time code 
				DisplayMackieTimeCodeFull();
			}

			// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
			//CheckRealtimeMidi();
		} 
		
		// Re init LCD "LFO ARPP MOTION" TXT if required (only text titles)
		if(update_zones & INIT_LCD_AUTOMATION_TXT)
			DisplayBmp(LCD_ZONE_AUTOMATION_TXT_X, LCD_ZONE_AUTOMATION_TXT_Y, LCD_INVERT, bmp_autom_titles_txt);
	
		// Re init LCD LFO WAVE if required : if lfo is off, display "off" instead of wave
		if(update_zones & INIT_LCD_LFO_WAVE_STATE)
		{
			// LFO is ON : update selected wave
			if(Z8_STATUS_1_NVM & lfo_state_z1nvm)
			{
				// Display TRIG if LFO is synched ext and still not started
				// if( (LFO_Freq < 64)&&(!(Z8_STATUS_5 & lfo_play_zstop_z5)) )
				// if( (Z8_STATUS_1_NVM & lfo_sync_ext_z1nvm) && (!(Z8_STATUS_5 & lfo_play_zstop_z5)) )
				if(!(Z8_STATUS_5 & lfo_play_zstop_z5)) 
						local_bmp_ptr =  bmp_lfo_trig;
				else
				{
					if(Z8_STATUS_2_NVM & lfo_wave_1_z2nvm)
					{
						if(Z8_STATUS_2_NVM & lfo_wave_0_z2nvm)
							local_bmp_ptr =  bmp_lfo_random;
						else	
							local_bmp_ptr =  bmp_lfo_square;
					}
					else
					{
						if(Z8_STATUS_2_NVM & lfo_wave_0_z2nvm)
							local_bmp_ptr =  bmp_lfo_triangle;
						else
							local_bmp_ptr =  bmp_lfo_sine;
					}
				}
			}	
			else
			{
				// LFO is off display "off" instead of waveform
				local_bmp_ptr = bmp_mini_off_txt;
				// also remove end points from previous wave graph since OFF is 12 pix and graph is 16 pix
				FillLcdZone(LCD_ZONE_LFO_WAVE_STATE_X+12, LCD_ZONE_LFO_WAVE_STATE_Y, 4, 1, FILL_BLANK);
			}

			// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
			//CheckRealtimeMidi();

			DisplayBmp(LCD_ZONE_LFO_WAVE_STATE_X, LCD_ZONE_LFO_WAVE_STATE_Y, LCD_INVERT, local_bmp_ptr);
		}
	
		// Re init LCD ARPP TXT + state if required
		if(update_zones & INIT_LCD_ARPP_STATE)
		{
			// Re init LCD ARPP STATE
			i = 0; // V1.7 : the offset
			// V1.5 if(CTRL_REG_1_7BITS_TO_EZ & arpp_state_z_ez1)
			if(Z8_STATUS_0_NVM & arpp_state_z0nvm)
			{
				local_bmp_ptr = bmp_mini_on_txt;
				i = 2;
			}
			else
				local_bmp_ptr = bmp_mini_off_txt;
	
			DisplayBmp(LCD_ZONE_ARPP_STATE_X-i, LCD_ZONE_ARPP_STATE_Y, LCD_INVERT, local_bmp_ptr);
		}
	
		// Re init LCD MOTION if required : update both "motion" txt + off if motion is off
		// or empty bargraph if on 
		if(update_zones & INIT_LCD_MOTION_STATE)
		{
			// Re init LCD MOTION STATE
			if(Z8_STATUS_1_NVM & motion_state_z1nvm)
			{
				i = 0;
				// bargraph and off txt have diffrent X pixel sizes, need to offset
				local_bmp_ptr = bmp_motion_bargraph;
			}
			else
			{
				i = 5;
				// bargraph and off txt have diffrent X pixel sizes, need to offset				
				local_bmp_ptr = bmp_mini_off_txt;
			}
			DisplayBmp(LCD_ZONE_MOTION_STATE_X+i, LCD_ZONE_MOTION_STATE_Y, LCD_INVERT, local_bmp_ptr);
		}

		// Re init LCD ALL SWITCHES STATES if required
		if(update_zones & INIT_LCD_SW_STATE)
		{	
			for(i=0; i<SW_NB ; i++)
			{
				UpdateSwState(i);
				UpdateOtherSwState(i);
			}
		}
	} // end incrust zone not active

	// Re init LCD infrared if required
	if(update_zones & INIT_LCD_INFRARED)
	{
		// V1.5
		if(identity[HW_VERSION_INDEX] < 2)
		{
			if(Z8_STATUS_1_NVM & infrared_state_z1nvm)
				local_bmp_ptr = bmp_infrared_on;
			else
				local_bmp_ptr = bmp_infrared_off;
		}
		else
		{
			if(Z8_STATUS_1_NVM & infrared_state_z1nvm)
				local_bmp_ptr = bmp_ribbon_on;
			else
				local_bmp_ptr = bmp_ribbon_off;
		}

		DisplayBmp(LCD_ZONE_INFRARED_X, LCD_ZONE_INFRARED_Y, LCD_INVERT, local_bmp_ptr);
	}

	// Re init LCD usb if required
	if(update_zones & INIT_LCD_USB)
	{
		if(CTRL_REG_7BITS_FROM_EZ[0] & usb_state_from_ez0)
			local_bmp_ptr = bmp_usb_on;
		else
			local_bmp_ptr = bmp_usb_off;

		DisplayBmp(LCD_ZONE_USB_X, LCD_ZONE_USB_Y, LCD_INVERT, local_bmp_ptr);
	}

	// Re init LCD sync24 if required
	if(update_zones & INIT_LCD_SYNC24)
	{
		if(CTRL_REG_0_7BITS_TO_EZ & sync24_state_z_ez0)
			local_bmp_ptr = bmp_sync24_on;
		else
			local_bmp_ptr = bmp_sync24_off;

		DisplayBmp(LCD_ZONE_SYNC24_X, LCD_ZONE_SYNC24_Y, LCD_INVERT, local_bmp_ptr);
	}

	// Re init LCD expansion if required
	if(update_zones & INIT_LCD_EXPANSION)
	{
		// V1.5 if(Z8_STATUS_6 & expansion_state_z6)
		if(STATUS_Z8_EXP)
			local_bmp_ptr = bmp_exp_on;
		else
			local_bmp_ptr = bmp_exp_off;

		DisplayBmp(LCD_ZONE_EXPANSION_X, LCD_ZONE_EXPANSION_Y, LCD_INVERT, local_bmp_ptr);
	}

	// Re init LCD TRANSPORT if required
	if(update_zones & INIT_LCD_TRANSPORT)
		UpdateLcdTransport();

	// Re init LCD INFRARED BARGRAPH if required
	if(update_zones & INIT_LCD_IR_GRAPH)
		DisplayBmp(LCD_ZONE_IR_GRAPH_X, LCD_ZONE_IR_GRAPH_Y, LCD_INVERT, bmp_ir_bargraph);

	// Re init LCD MODE if required
	if(update_zones & INIT_LCD_MODE)
	{
		uchar must_display_mackie_metering = FALSE;

		// Also display mode in LCD string zone V1.5
/* V1.69
		if(Z8_STATUS_2_NVM & operating_mode1_z2nvm)
		{
			local_bmp_ptr = bmp_usr_txt;
			// here it is mackie or user mode
			if(Z8_STATUS_2_NVM & operating_mode0_z2nvm)
				rom_to_lcd_text_copy(user_defined_txt);
			else
			{
				rom_to_lcd_text_copy(mackie_mode_txt);
				must_display_mackie_metering = TRUE;
			}
		}
		else // here it is standard or reason mode
		{ // v1.1
			local_bmp_ptr = bmp_std_txt;
			if(Z8_STATUS_2_NVM & operating_mode0_z2nvm)
				rom_to_lcd_text_copy(reason_mode_txt);
			else
				rom_to_lcd_text_copy(standard_mode_txt);
		}
*/
		i = OPERATING_MODE_NVM /* & OPERATING_MODE_MSK_Z3_NVM */;
		local_bmp_ptr = bmp_std_txt;

		if(i == STANDARD_MODE)
			rom_to_lcd_text_copy(standard_mode_txt);
		else
		if(i == USER_MODE)
		{
			rom_to_lcd_text_copy(user_defined_txt);
			local_bmp_ptr = bmp_usr_txt;
		}
		else
		if(i == REASON_MODE)
			rom_to_lcd_text_copy(reason_mode_txt);
		else
		// Here it is mackie or logic modes
		{
			must_display_mackie_metering = TRUE;

			if(i == LOGIC_MODE)
				rom_to_lcd_text_copy(logic_mode_txt);
			else
				rom_to_lcd_text_copy(mackie_mode_txt);
		}

		DisplayLcdText(LCD_ZONE_STRING_X, LCD_ZONE_STRING_Y, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );

		// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
		//CheckRealtimeMidi();

		// Display mackie metering or small mode bmp
		if(must_display_mackie_metering)
			DisplayMackieMeteringFull();
		else
			DisplayBmp(LCD_ZONE_MODE_X, LCD_ZONE_MODE_Y, LCD_INVERT, local_bmp_ptr);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateOtherSwState()
// Inputs	: 	index on sw to display (0-7)
// Outputs	: 	None
// Process  : 	transport sw are not connected in the same order on the front panel & on the
//				mux, need to reorganize display
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateOtherSwState(uchar sw_index)
{
	uchar sw_mask=1;
	uchar index, x_pixel_bmp = LCD_ZONE_OTHER_SW_STATE_X;
	rom uchar *local_bmp_ptr;

	// return if lcd fully locked
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// also exit if zone incrust is locked
	if(lcd_lock_incrust_cpt) return;

	for(index=0; index<sw_index; index++)
	{
		// create a mask on the sw to display
		sw_mask = sw_mask << 1;

		// calculate offset of bitmap
		x_pixel_bmp += SW_STATE_WIDTH;
	}

	// if transport assigned switches (0-4) then show them at right position
	if(sw_index < TRANSPORT_NB)
	{
		x_pixel_bmp += 36;

		if(Ctrl.Ctrl_sc.other_sw_value & sw_mask)
			local_bmp_ptr = bmp_sw_on;
		else
			local_bmp_ptr = bmp_sw_off;
	}
	else
	{
		x_pixel_bmp -= 60;
		if(sw_index == JOYSTICK_SW)
		{
			if(Ctrl.Ctrl_sc.other_sw_value & sw_mask)
				local_bmp_ptr = bmp_joy_sw_on;
			else
				local_bmp_ptr = bmp_joy_sw_off;
		}
		else
		if(sw_index == FOOTSWITCH)
		{
			if(Ctrl.Ctrl_sc.other_sw_value & sw_mask)
				local_bmp_ptr = bmp_foot_sw_on;
			else
				local_bmp_ptr = bmp_foot_sw_off;
		}
		else
		// SHOULD BE ENCODER SW
		{
			if(Ctrl.Ctrl_sc.other_sw_value & sw_mask)
				local_bmp_ptr = bmp_sw_on;
			else
				local_bmp_ptr = bmp_sw_off;
		}
	}

	DisplayBmp(x_pixel_bmp, LCD_ZONE_OTHER_SW_STATE_Y, LCD_INVERT, local_bmp_ptr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateSwState()
// Inputs	: 	index on sw to display (0-7)
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateSwState(uchar sw_index)
{
	uchar sw_mask=1;
	uchar index, x_pixel_bmp = LCD_ZONE_SW_STATE_X;
	rom uchar *local_bmp_ptr;

	// return if lcd fully locked
//	if(lcd_lock_all_cpt) return;
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// also exit if zone incrust is locked
	if(lcd_lock_incrust_cpt) return;

	for(index=0; index<sw_index; index++)
	{
		// create a mask on the sw to display
		sw_mask = sw_mask << 1;

		// calculate offset of bitmap
		x_pixel_bmp += SW_STATE_WIDTH;
	}

	if(Ctrl.Ctrl_sc.all_sw_value & sw_mask)
		local_bmp_ptr = bmp_sw_on;
	else
		local_bmp_ptr = bmp_sw_off;

	DisplayBmp(x_pixel_bmp, LCD_ZONE_SW_STATE_Y, LCD_INVERT, local_bmp_ptr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdTransport()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update bmps for transports fucntions according to new sw pressed and old state
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdTransport(void)
{
	rom uchar *local_bmp_ptr;

	// LCD is locked ?
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	if(next_transport_state == TRANSPORT_STOP)
	{
		// Here must highlight STOP or PAUSE transport state
		if ( (transport_state == TRANSPORT_REC) || (transport_state == TRANSPORT_PLAY))
		{
			local_bmp_ptr = bmp_pause;
			next_transport_state = TRANSPORT_PAUSE;
		}
		else
			local_bmp_ptr = bmp_stop;
	}
	else
	{
		if(next_transport_state == TRANSPORT_PLAY)
			// Here must highlight PLAY transport state
			local_bmp_ptr 	= bmp_play;
		else
		if(next_transport_state == TRANSPORT_REC)
			// Here must highlight REC transport state
			local_bmp_ptr 	= bmp_rec;
	}

	if( (next_transport_state != BACKWARD_CTRL_INDEX) && (next_transport_state != FORWARD_CTRL_INDEX)) 
		DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, local_bmp_ptr);
	
	transport_state = next_transport_state ;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdFunction()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update bmps for active fucntion according to new sw pressed and old state
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdFunction(void)
{
	rom uchar *local_bmp_ptr; // V1.8
	uchar x_pixel_to_update = LCD_ZONE_GROUP_X;
    uchar y_page_to_update  = LCD_ZONE_GROUP_Y;

	// LCD is locked or partialy locked ?
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;
	if(lcd_lock_incrust_cpt) return;

	// DELETE OLD FUNCTION ACTIVE BORDERS
	if(current_function == FCT_CHANNEL)
	{
		x_pixel_to_update = LCD_ZONE_CHANNEL_X;
		y_page_to_update  = LCD_ZONE_CHANNEL_Y;
	}
	else
	if(current_function == FCT_GROUP)
	{
		x_pixel_to_update = LCD_ZONE_GROUP_X;
		y_page_to_update  = LCD_ZONE_GROUP_Y;
	}
	else
	// V1.8 if(current_function == FCT_SCENE)
	{
		x_pixel_to_update = LCD_ZONE_SCENE_X;
		y_page_to_update  = LCD_ZONE_SCENE_Y;
	}

	// delete left corner
	FillLcdZone(x_pixel_to_update-1, y_page_to_update, 2, 2, FILL_BLANK);
	// Write right corner
	FillLcdZone(x_pixel_to_update+18, y_page_to_update, 2, 2, FILL_BLANK);

	// DISPLAY NEW FUNCTION ACTIVE BORDERS
	if(next_function == FCT_GROUP)
	{
		x_pixel_to_update = LCD_ZONE_GROUP_X;
		y_page_to_update  = LCD_ZONE_GROUP_Y;
	}
	else
	if(next_function == FCT_CHANNEL)
	{
		x_pixel_to_update = LCD_ZONE_CHANNEL_X;
		y_page_to_update  = LCD_ZONE_CHANNEL_Y;
	}
	else // V1.8 scenes arp added : new bloc
	{
		// update bitmap depending on scene type
		DisplaySceneTypeBitmap();
		// also update scene nb
		UpdateLcdZones(INIT_LCD_SCENE);
		// will update brackets
		x_pixel_to_update = LCD_ZONE_SCENE_X;
		y_page_to_update  = LCD_ZONE_SCENE_Y;
	}

	// Write left corner
	DisplayBmp(x_pixel_to_update-1, y_page_to_update, LCD_INVERT, bmp_border_left);
	// Write right corner
	DisplayBmp(x_pixel_to_update+18, y_page_to_update, LCD_INVERT, bmp_border_right);

	// update flag states
	current_function = next_function;
}

/*
/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayBargraphMotion()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update bmps for motion bargraph from global var bargraph_motion (0-23)
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayBargraphMotion(void)
{
	// LCD is locked ?
//	if(lcd_lock_all_cpt) return;
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayRomVersion()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	display rom version while in home screen
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayRomVersion(uchar x, uchar y)
{
	// lcd_text_value[0] = FW_VERSION+'0'; //identity[FW_VERSION_INDEX]+'0';
	lcd_text_value[0] = identity[FW_VERSION_INDEX] + '0';
	lcd_text_value[1] = '.';
	// lcd_text_value[2] = FW_SUB_VERSION+'0'; //identity[FW_SUB_VERSION_INDEX]+'0';
	lcd_text_value[2] = identity[FW_SUB_VERSION_INDEX] + '0';
	lcd_text_value[3] = '\0';
	DisplayLcdText(x, y, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLCDJoystickArea()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Plot the joystick X and Y position on the area
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLCDJoystickArea(void)
{
	uchar current_vert_pix, current_vert_pix_page1, current_vert_pix_page2, tmp; // , index_x_pix;
	unsigned long y_axis_pos;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// First delete old joystick area
	FillLcdZone(old_joy_x_pixel, old_joy_y_page_start, LCD_JOY_POS_PIX_SQUARE, old_joy_y_page_end-old_joy_y_page_start+1, FILL_BLANK);

	////////////////////////////// CALCULATE Y POSITION //////////////////////////////

	// Reinit old y page start
	old_joy_y_page_start = 0;

	// Y area is from 0 to 31, while joystick is 0 to 127
	tmp = (Ctrl.Ctrl_sc.joystick_y & (~HOOK_BIT)) >> 2; // V1.61 mask hook bit

	// Initial value is 07 (3 vertical pix on bottom)
//	y_axis_pos = LCD_JOY_INITIAL_VERTICAL_VALUE;
	y_axis_pos = 0xE0000000;

	// increase the 3 vertical pix according to joystick_y value
	while(tmp--)
		y_axis_pos = y_axis_pos >> 1;

	if(y_axis_pos < ((1<<LCD_JOY_POS_PIX_SQUARE)-1)) y_axis_pos = ((1<<LCD_JOY_POS_PIX_SQUARE)-1);

	////////////////////////////// CALCULATE X POSITION //////////////////////////////	
	
	// X area is from 0 to 47, joystick X which is is 0 to 127, (factor is x 0.375)
	tmp = (Ctrl.Ctrl_sc.joystick_x & (~HOOK_BIT)) >> 2; // V1.61 mask hook bit
	old_joy_x_pixel  = (tmp) + LCD_ZONE_JOY_MIN_X;
	old_joy_x_pixel += (tmp >> 1);
	

	// Are the 3 vert pix in the very low part of y_axis_pos
	current_vert_pix = (uchar) (y_axis_pos & 0x000000FF);
	if(current_vert_pix)
	{
		old_joy_y_page_start 	= 2;
		old_joy_y_page_end   	= 2;
		current_vert_pix_page1	= current_vert_pix;
	}

	current_vert_pix = (uchar) ((y_axis_pos & 0x0000FF00)>>8);
	if(current_vert_pix)
	{
		old_joy_y_page_end = 3;
		if(!old_joy_y_page_start) 
		{ 
			old_joy_y_page_start = 3;
			current_vert_pix_page1	= current_vert_pix;
		}
		else
			current_vert_pix_page2	= current_vert_pix;
	}

	current_vert_pix = (uchar) ((y_axis_pos & 0x00FF0000)>>16);
	if(current_vert_pix)
	{
		old_joy_y_page_end   = 4;
		if(!old_joy_y_page_start) 
		{ 
			old_joy_y_page_start = 4;			
			current_vert_pix_page1	= current_vert_pix;
		}
		else
			current_vert_pix_page2	= current_vert_pix;
	}

	current_vert_pix = (uchar) ((y_axis_pos & 0xFF000000)>>24);
	if(current_vert_pix)
	{
		old_joy_y_page_end   = 5;		
		if(!old_joy_y_page_start) 
		{ 
			old_joy_y_page_start = 5;
			current_vert_pix_page1	= current_vert_pix;
		}
		else
			current_vert_pix_page2	= current_vert_pix;
	}
	
	// NOW DISPLAY JOSYTICK POSITION
	FillLcdZone(old_joy_x_pixel, old_joy_y_page_start, LCD_JOY_POS_PIX_SQUARE, 1, ~current_vert_pix_page1);

	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	//CheckRealtimeMidi();

	// display second page if required
	if(old_joy_y_page_start != old_joy_y_page_end)
		FillLcdZone(old_joy_x_pixel, old_joy_y_page_end, LCD_JOY_POS_PIX_SQUARE, 1, ~current_vert_pix_page2);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	RescaleIrBargraphValue()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Update Infrared bargraph
//				Bargraph infrared is 0-54 (x 0.425 to rescale)=(0.5-0.0625)
//////////////////////////////////////////////////////////////////////////////////////////////
uchar RescaleIrBargraphValue(uchar value, uchar *y_page_full)
{
	uchar rescaled_value;

	rescaled_value  =  value >> 1;				// x 0.5
	rescaled_value -= (rescaled_value >> 3);	// x 0.0625
	if(rescaled_value > LCD_IR_BARGRAPH_MAX) 
	   rescaled_value = LCD_IR_BARGRAPH_MAX;

   	// get number of full pages
	while(rescaled_value >= 8)
	{
		rescaled_value -= 8;
		*y_page_full += 1;
	}

	return rescaled_value;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLCDInfraredBargraph()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Update Infrared bargraph
//				Bargraph infrared is 0-50 (x 0.393 to rescale)=()
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLCDInfraredBargraph(void)
{
	uchar tmp2;
	uchar bargraph_value;
	uchar full_y_page = 0;
	uchar must_display_remaining = FALSE;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// first clear old infrared bargraph value
	FillLcdZone(LCD_ZONE_IR_GRAPH_X+1, LCD_ZONE_IR_GRAPH_Y, 2, LCD_ZONE_IR_GRAPH_Y_PAGE+1, FILL_BLANK);
	// reshow top border since deleted above
	FillLcdZone(LCD_ZONE_IR_GRAPH_X+1, LCD_ZONE_IR_GRAPH_Y, 2, 1, 0xFD);

	// calculate bargraph value
	bargraph_value = RescaleIrBargraphValue(current_value, &full_y_page);

	// bargraph_value contains remaining pix to display
	// display all full y pages
	if(full_y_page)
		FillLcdZone(LCD_ZONE_IR_GRAPH_X+1, LCD_ZONE_IR_GRAPH_Y_PAGE-full_y_page+1, 2, full_y_page, (~FILL_BLANK));

	// Now display reamaining pixels if not full (tmp contains remain)
	if(bargraph_value)
	{
		tmp2 = 0x80;
		while(bargraph_value--)
			tmp2 = (tmp2 >> 1) | 0x80;

 		must_display_remaining = TRUE;
	}	
	else
	// Display infrared HOLD value once infrared returned to zero
	if(!current_value)
	{
		// infared = 0, flag it
		Z8_STATUS_4 &= (~wait_ir_passed_zero_z4);
			
		bargraph_value = RescaleIrBargraphValue(Ctrl.Ctrl_sc.infrared_hold, &full_y_page);

		// get remaining pixels height
		tmp2 = 0x80;
		if(bargraph_value)
		{
			while(bargraph_value--)
				tmp2 = (tmp2 >> 1);
		}

		must_display_remaining = TRUE;
	}
	
	// Must diplay remaining pixels or hold value
	if( must_display_remaining )
	{
		// if last page, then add top border
		if(full_y_page==LCD_ZONE_IR_GRAPH_Y_PAGE)
			tmp2 |= 2;
	
		FillLcdZone(LCD_ZONE_IR_GRAPH_X+1, LCD_ZONE_IR_GRAPH_Y_PAGE-full_y_page, 2, 1, ~tmp2);
	}
	
	// Now display numeric value
	// V1.6 DisplayCurrentValue(); // removed since stack overflow ?
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdString()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	! control index should be from first pot (05) to the end
//				Since transports (index 0-4) doesnt have lcd str, need to remove TRANNPORT_NB	
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdString(void)
{
	uchar i;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

/* V1.69
	// v1.1 v1.5 in Reason mode : the string is NOT updated
	if(!(Z8_STATUS_2_NVM & operating_mode1_z2nvm))
	{
		// here it is standard or reason mode
		if(Z8_STATUS_2_NVM & operating_mode0_z2nvm) 
			// reason mode
			return;
		else
			// Standard mode : dynamically create string : PROGRAM/CONTROL CHANGE # NNN
			CreateStdLcdStrFromCtrlIndex();
	}
	else
	// user defined or Mackie mode : get LCD string from external EEPROM
	// V1.5 if(Z8_STATUS_2_NVM & operating_mode1_z2nvm)
	{
		FillLcdCurrentStruct();
	}
	// Standard mode : dynamically create string : PROGRAM/CONTROL CHANGE # NNN
	// V1.5 else
	// V1.5	CreateStdLcdStrFromCtrlIndex();
*/
	// V1.69
	i = OPERATING_MODE_NVM /* & OPERATING_MODE_MSK_Z3_NVM */;

	if(i == STANDARD_MODE)
		CreateStdLcdStrFromCtrlIndex();
	else
	if(i == REASON_MODE)
		return;
	else
		// Here it is mackie or logic or user modes
		FillLcdCurrentStruct();

	DisplayLcdText(LCD_ZONE_STRING_X, LCD_ZONE_STRING_Y, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLCDCrossfaderArea()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Plot the crossfader new position area
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLCDCrossfaderArea(void)
{
//	uchar fader_x_pos;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// first delete old fader position
	FillLcdZone(old_fader_x_pixel, LCD_ZONE_FADER_PAGE, LCD_ZONE_FADER_KNOB_SIZE, 1, 0xF3);
	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	//CheckRealtimeMidi();
	// Then calculate new fader position
	// fader_x_pos = current_value /* V1.6 & (~HOOK_BIT))*/ >> 1; // V1.6 + (LCD_ZONE_FADER_START + 1);
	// update old_fader_x_pos
	old_fader_x_pixel = (current_value >> 1)  + (LCD_ZONE_FADER_START + 1);
	// display it
	FillLcdZone(/* V1.6 fader_x_pos*/old_fader_x_pixel, LCD_ZONE_FADER_PAGE, LCD_ZONE_FADER_KNOB_SIZE, 1, 0xC0);
	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	//CheckRealtimeMidi();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CreateStdLcdStrFromCtrlIndex()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	create a std lcd str from control index, result in lcd_text_value[]
//////////////////////////////////////////////////////////////////////////////////////////////
void CreateStdLcdStrFromCtrlIndex(void)
{
	uchar tmp0, tmp1, tmp2;

	// First transform control index into a 3 digits
	IntToLcdText((ParamCurrent->ctrl_status_0 & (~NOTE_CTRL_CHANGE)), DIGITS_3);
	// save this 3 digit text
	tmp0 = lcd_text_value[0];
	tmp1 = lcd_text_value[1];		
	tmp2 = lcd_text_value[2];		
	// Write Program or control depending on standard control type
	rom_to_lcd_text_copy(default_lcd_str_txt);
	// here current_lcd_text contains "    CC # ___   "
	if(ParamCurrent->ctrl_status_0 & NOTE_CTRL_CHANGE)
	{
		// Now it is Note On : need to display Nt instead of CC
		lcd_text_value[4] = 'N';
		lcd_text_value[5] = 't';
	}
	
	lcd_text_value[9]  = tmp0;
	lcd_text_value[10] = tmp1;		
	lcd_text_value[11] = tmp2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdHookState()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	delete, show up or down direction hook state bmp
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdHookState(uchar next_hook_state)
{
	rom uchar *local_bmp_ptr;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// control chain is treated seperately
	if((next_hook_state == NO_HOOK_BMP)&&(Z8_STATUS_5 & must_show_ctrl_chain_z5))
	{
		Z8_STATUS_5 &= ~must_show_ctrl_chain_z5;
		next_hook_state = CTRL_CHAIN_BMP;
	}

	// Update lcd hook state and control chain only if required
	if(current_hook_bmp != next_hook_state)
	{
		current_hook_bmp = next_hook_state;

		if(next_hook_state == HOOK_DOWN_BMP)
			local_bmp_ptr = bmp_lock_down;
		else
		if(next_hook_state == HOOK_UP_BMP)
			local_bmp_ptr = bmp_lock_up;
		else
		if(next_hook_state == CTRL_CHAIN_BMP)
		{
			local_bmp_ptr = bmp_ctrl_chain;
		}
		else
		{	// here NO_HOOK_BMP was received
			FillLcdZone(LCD_ZONE_HOOK_DIRECTION_X, LCD_ZONE_HOOK_DIRECTION_Y, 7, 2, FILL_BLANK);
			return;
		}

		// show bmp hook up direction
		DisplayBmp(LCD_ZONE_HOOK_DIRECTION_X, LCD_ZONE_HOOK_DIRECTION_Y, LCD_INVERT, local_bmp_ptr);

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdFaderHookState()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	delete, show fader hook state bmp
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdFaderHookState(uchar next_hook_state)
{
	uchar fader_x_pos;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// update hook fader state if state changed or forced due to fader incrust zone removed
	if(((Z8_STATUS_4 & fader_hook_bmp_state_z4) != next_hook_state) || 
	    (Z8_STATUS_4 & force_hook_fader_display_z4))
	{
		// remove force to display flag
		Z8_STATUS_4 &= ~force_hook_fader_display_z4;
		// first delete old fader hooked position
		FillLcdZone(old_fader_hooked_x_pixel, LCD_ZONE_FADER_PAGE, LCD_ZONE_FADER_KNOB_SIZE-2, 1, 0xF3);

		// toggle fader hook bmp state
		Z8_STATUS_4 ^= fader_hook_bmp_state_z4;
		// must show fader hooked 
		if(next_hook_state)
		{
			// Then calculate new fader position
			fader_x_pos = ((Physical[current_group].crossfader_l & (~HOOK_BIT)) >> 1) + (LCD_ZONE_FADER_START + 1);
			// update old_fader_x_pos
			old_fader_hooked_x_pixel = fader_x_pos;
			// display it
			FillLcdZone(fader_x_pos, LCD_ZONE_FADER_PAGE, LCD_ZONE_FADER_KNOB_SIZE-2, 1, 0xC0);
			// Display small locked text
			DisplayBmp(LCD_ZONE_FADER_START+24, LCD_ZONE_FADER_PAGE+1, LCD_INVERT, bmp_locked_txt);
		}
		// No fader hook state to show
		else
		{
			// delete locked txt
			FillLcdZone(LCD_ZONE_FADER_START+24, LCD_ZONE_FADER_PAGE+1, 21, 1, FILL_BLANK);
		}
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayFullLfoLcdIncrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayFullLfoLcdIncrust(uchar force_display)
{
	rom uchar * local_bmp_ptr;
//	uchar local_freq;
	uchar tmp; // V1.7

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// If lfo full incrust already active, update only new param
	if(lcd_current_incrust != LCD_INCRUST_FULL_LFO)
	{
		// Display incrust title
		rom_to_lcd_text_copy(lfo_txt);
		// Display incrust zone
		DisplayEmptyIncrust();
		// Show bmp ampl freq offset wav
		DisplayBmp(LCD_ZONE_INCRUST_X+3, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, bmp_lfo_all_txt);
	}

	// Need to update lfo amplitude value
	if( (lcd_current_incrust != LCD_INCRUST_FULL_LFO)||(force_display==FORCE_AMPL_DISPLAY) )
		DisplayLcdInt(LCD_ZONE_LFO_INCRUST_AMPL_X, LCD_ZONE_INCRUST_Y+3, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, LFO_Ampl);

	// Need to update lfo freq value
	if( (lcd_current_incrust != LCD_INCRUST_FULL_LFO)||(force_display==FORCE_FREQ_DISPLAY) )
	{	
		// External sync
		// if(LFO_Freq < 64)
		if(Z8_STATUS_1_NVM & lfo_sync_ext_z1nvm)
//		{
			local_bmp_ptr = bmp_small_ext_txt;
//			local_freq	  = LFO_Freq >> 3;
//		}
		else
//		{
			local_bmp_ptr =  bmp_small_int_txt;
//		local_freq	  =  LFO_Freq - 64;
//		}
		DisplayBmp(LCD_ZONE_LFO_INCRUST_FREQ_X, LCD_ZONE_INCRUST_Y+2, LCD_INVERT, local_bmp_ptr);
		// Display frequency value

//V1.7		DisplayLcdInt(LCD_ZONE_LFO_INCRUST_FREQ_X-1, LCD_ZONE_INCRUST_Y+3, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, LFO_Freq);
		// New block in V1.7 : display nb of measures
		// odd lenght is sync on full measure	
		if(LFO_Freq & 1)
		{
			tmp = 5;
			// remove old 1/2 if exist
			FillLcdZone(LCD_ZONE_LFO_INCRUST_FREQ_X-6, LCD_ZONE_INCRUST_Y+3, 23, 1, FILL_BLANK);
		}
		else
		{
			tmp = 0;
			DisplayBmp(LCD_ZONE_LFO_INCRUST_FREQ_X+6, LCD_ZONE_INCRUST_Y+3, LCD_INVERT, un_demi_bmp);
		}

		DisplayLcdInt(LCD_ZONE_LFO_INCRUST_FREQ_X-6+tmp, LCD_ZONE_INCRUST_Y+3, DIGITS_2|LCD_INVERT|FONT_5x8_FLG, (LFO_Freq + 1) >> 1);
		// end of new block v1.7
	}

	// Need to update lfo offset value
	if( (lcd_current_incrust != LCD_INCRUST_FULL_LFO)||(force_display==FORCE_OFFSET_DISPLAY) )
		DisplayLcdInt(LCD_ZONE_LFO_INCRUST_OFFS_X, LCD_ZONE_INCRUST_Y+3, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, LFO_Offset);

	// Need to update lfo wave selected value
	if( (lcd_current_incrust != LCD_INCRUST_FULL_LFO)||(force_display==FORCE_WAV_DISPLAY) )
	{	
		// Now display current active waveform bmp
		if(Z8_STATUS_2_NVM & lfo_wave_1_z2nvm)
		{
			if(Z8_STATUS_2_NVM & lfo_wave_0_z2nvm)
				local_bmp_ptr =  bmp_lfo_random;
			else	
				local_bmp_ptr =  bmp_lfo_square;
		}
		else
		{
			if(Z8_STATUS_2_NVM & lfo_wave_0_z2nvm)
				local_bmp_ptr =  bmp_lfo_triangle;
			else
				local_bmp_ptr =  bmp_lfo_sine;
		}
		DisplayBmp(LCD_ZONE_LFO_INCRUST_WAV_X, LCD_ZONE_INCRUST_Y+3, LCD_INVERT, local_bmp_ptr);
	}

	// LCD LFO incrust zone area is active now
	lcd_current_incrust = LCD_INCRUST_FULL_LFO;

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayInputOverflowIncrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayInputOverflowIncrust(void)
{
	rom uchar *local_txt_ptr;
	rom uchar *local_txt_ptr2=NULL;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Display incrust title
	rom_to_lcd_text_copy(info_txt);
	// Display incrust zone
	DisplayEmptyIncrust();

	if(Z8_STATUS_8 & serial_in_overflow_z8)
	{
		lcd_current_incrust = LCD_INCRUST_SERIAL_IN_OVERFLOW;
		local_txt_ptr = serial_in_txt;
	}
	else	
	{
		lcd_current_incrust = LCD_INCRUST_MIDI_OVERFLOW;
		if(Z8_STATUS_8 & OVERFLOW_FROM_USB_MSK)
			local_txt_ptr  = usb_in_txt;
		if(Z8_STATUS_8 & OVERFLOW_FROM_MIDI_MSK)
			local_txt_ptr  = midi_in_txt;
		if(Z8_STATUS_8 & OVERFLOW_REALTIME_MSK)
			local_txt_ptr2 = realtime_txt;
	}

	rom_to_lcd_text_copy(local_txt_ptr);
	DisplayLcdText(LCD_ZONE_INCRUST_X+11, LCD_ZONE_INCRUST_Y+2, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	

	if(local_txt_ptr2)
	{
		rom_to_lcd_text_copy(local_txt_ptr2);
		DisplayLcdText(LCD_ZONE_INCRUST_X+38, LCD_ZONE_INCRUST_Y+2, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	
	}

	rom_to_lcd_text_copy(overflow_txt);
	DisplayLcdText(LCD_ZONE_INCRUST_X+11, LCD_ZONE_INCRUST_Y+3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayEmptyIncrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	the global var lcd_text must contains incrust title,alos lock incrust zone during 2sec
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayEmptyIncrust(void)
{

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Display incrust zone
	DisplayBmp(LCD_ZONE_INCRUST_X, LCD_ZONE_INCRUST_Y, LCD_INVERT, bmp_incrust);

	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	//CheckRealtimeMidi();

	// Display incrust title
	DisplayLcdText(LCD_ZONE_INCRUST_X+6, LCD_ZONE_INCRUST_Y, LCD_NORMAL|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	

	// Lock LCD incrust zone for 2 seconds
	// lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckDisplayCurrentValueLocked()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Display current control value locked by SHIFT on the LCD zone value V1.62
//////////////////////////////////////////////////////////////////////////////////////////////
void CheckDisplayCurrentValueLocked(void)
{
	uchar must_display_lock_bmp = FALSE;

	// V1.62 : if SHIFT button locks data then display locked
	if(Z8_STATUS_9 & shift_state_z9)
	{
		// here Shift is active
		if(Z8_STATUS_2_NVM & shift_lock_midi_z2nvm)
		{
			must_display_lock_bmp = TRUE;
		
			// here shift is active and used to lock midi
			if(!(Z8_STATUS_10 & last_value_locked_0_z10))
			{
				Z8_STATUS_10 |= last_value_locked_0_z10;
				DisplayBmp(LCD_ZONE_VALUE_X, LCD_ZONE_VALUE_Y, LCD_INVERT, bmp_shift_lock);
			}
		}
	}

	// Was it displayed before ?
	if(Z8_STATUS_10 & last_value_locked_0_z10)
		// Must not display lock bmp
		if(!must_display_lock_bmp)
		{
			// Unlock bitmap
			Z8_STATUS_10 &= ~last_value_locked_0_z10;
			// Write 000 on current value
			DisplayLcdInt(LCD_ZONE_VALUE_X, LCD_ZONE_VALUE_Y, DIGITS_3|LCD_INVERT|FONT_NUM_8x16_FLG/* V1.66 , font_num_8x16 */, 0);
		}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayCurrentValue()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Display current control value on the LCD zone value
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayCurrentValue(void)
{
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;
	
	// V1.62 Display locked bmp if required
	CheckDisplayCurrentValueLocked();

	if(!(Z8_STATUS_10 & last_value_locked_0_z10))
		DisplayLcdInt(LCD_ZONE_VALUE_X, LCD_ZONE_VALUE_Y, DIGITS_3|LCD_INVERT|FONT_NUM_8x16_FLG/* V1.66 , font_num_8x16 */, current_value);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdCurveModel()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Display current curve model bmp
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdCurveModel(uchar next_curve_model)
{
	rom uchar *local_bmp_ptr;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Force linear curve mode in standard and reason mode
	// V1.69 if(!(Z8_STATUS_2_NVM & operating_mode1_z2nvm))
	if( (OPERATING_MODE_NVM == STANDARD_MODE) ||
		(OPERATING_MODE_NVM == REASON_MODE))	
		next_curve_model = CURVE_LINEAR;

	if(next_curve_model != current_curve_model)
	{
		if(next_curve_model == CURVE_LINEAR)
			local_bmp_ptr = bmp_curve_linear;
		else
		if(next_curve_model == CURVE_INV_LIN)
			local_bmp_ptr = bmp_curve_inv_linear;
		else
		if(next_curve_model == CURVE_LOG)
			local_bmp_ptr = bmp_curve_log;
		else
		if(next_curve_model == CURVE_INV_LOG)
			local_bmp_ptr = bmp_curve_inv_log;
		else
		if(next_curve_model == CURVE_RANDOM)
			local_bmp_ptr = bmp_curve_random;
		else
		if(next_curve_model == CURVE_USER_0)
			local_bmp_ptr = bmp_curve_user_0;
		else
		if(next_curve_model == CURVE_USER_1)
			local_bmp_ptr = bmp_curve_user_1;

		DisplayBmp(LCD_ZONE_CURVE_X, LCD_ZONE_CURVE_Y, LCD_INVERT, local_bmp_ptr);
		current_curve_model = next_curve_model;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Display_Invalid_Sysex_Incrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Called when an invalid or unexpected sysex has been received
//////////////////////////////////////////////////////////////////////////////////////////////
void Display_Invalid_Sysex_Incrust(void)
{
	// will be incrust title
	rom_to_lcd_text_copy(info_txt);

	if(lcd_current_incrust != LCD_INCRUST_UNVALID_SYSEX)
	{
		DisplayEmptyIncrust();
		// display error on received sysex
		rom_to_lcd_text_copy(corrupted_sysex_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+2, LCD_ZONE_INCRUST_Y+2, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
		rom_to_lcd_text_copy(received_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+15, LCD_ZONE_INCRUST_Y+3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
	}

	lcd_current_incrust = LCD_INCRUST_UNVALID_SYSEX;	

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Display_Invalid_Midi_Struct_Incrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Called when an invalid midi structure is read from eeprom
//////////////////////////////////////////////////////////////////////////////////////////////
void Display_Invalid_Midi_Struct_Incrust(void)
{
	// will be incrust title
	rom_to_lcd_text_copy(info_txt);

	if(lcd_current_incrust != LCD_INCRUST_UNVALID_MIDI_STRUCT)
	{
		DisplayEmptyIncrust();
		// display error on received sysex
		rom_to_lcd_text_copy(corrupted_struct_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+2, LCD_ZONE_INCRUST_Y+2, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
	}

	lcd_current_incrust = LCD_INCRUST_UNVALID_MIDI_STRUCT;	

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayFullArppLcdIncrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayFullArppLcdIncrust(uchar force_display)
{
	rom uchar * local_bmp_ptr;
	uchar local_param1, local_param2, local_param3;
	uchar current_incrust_local;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// shift not active
	if(!(Z8_STATUS_9 & shift_state_z9))
	{
		current_incrust_local = LCD_INCRUST_FULL_ARPP;
		local_bmp_ptr		  = bmp_arpp_all_txt;
		local_param1		  = arp_rhythm;
		local_param2		  = arp_accent;
		local_param3		  = arp_gate;
	}
	else
	{
		current_incrust_local = LCD_INCRUST_FULL_ARPP_SHIFT;
		local_bmp_ptr		  = bmp_arpp_all_shift_txt;
		local_param1		  = arp_melody;
		local_param2		  = arp_transpose;
		local_param3		  = arp_range+1;
	}

	// If lfo full incrust already active, update only new param
	if(lcd_current_incrust != current_incrust_local)
	{
		// Display incrust title
		rom_to_lcd_text_copy(arpp_txt);
		// Display incrust zone
		DisplayEmptyIncrust();
		// Show bmp ampl freq offset wav
		DisplayBmp(LCD_ZONE_INCRUST_X+3, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, local_bmp_ptr);
	}


	// Need to update arpp param 1 value
	if( (lcd_current_incrust != current_incrust_local)||(force_display==FORCE_RYTHM_MELODY_DISPLAY) )
		DisplayLcdInt(LCD_ZONE_LFO_INCRUST_OFFS_X-6, LCD_ZONE_INCRUST_Y+3, DIGITS_3|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, local_param1);

	// Need to update arpp param 2 value
	if( (lcd_current_incrust != current_incrust_local)||(force_display==FORCE_ACC_TRANSP_DISPLAY) )
		DisplayLcdInt(LCD_ZONE_LFO_INCRUST_FREQ_X-9, LCD_ZONE_INCRUST_Y+3, DIGITS_3|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, local_param2);

	// Need to update arpp param 2 value
	if( (lcd_current_incrust != current_incrust_local)||(force_display==FORCE_GATE_RANGE_DISPLAY) )
		DisplayLcdInt(LCD_ZONE_LFO_INCRUST_AMPL_X-6, LCD_ZONE_INCRUST_Y+3, DIGITS_3|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, local_param3);

	// Need to update arpp dir selected value
	if( (lcd_current_incrust != current_incrust_local)||(force_display==FORCE_ARP_DIR_DISPLAY)||(force_display==FORCE_ARP_MODE_DISPLAY) )
	{	
		// display dir if no shift
		if(!(Z8_STATUS_9 & shift_state_z9))
		{
			// Now display current active waveform bmp
			if(arp_dir == DIR_UP)
				local_bmp_ptr =  bmp_arp_dir_up;
			else	
			if(arp_dir == DIR_DOWN)
				local_bmp_ptr =  bmp_arp_dir_down;
			else	
			if(arp_dir == DIR_UPDOWN)
				local_bmp_ptr =  bmp_arp_dir_up_down;
			else	
			if(arp_dir == DIR_RANDOM)
				local_bmp_ptr =  bmp_arp_dir_rand;
			else	
			if(arp_dir == DIR_CUSTOM1)
				local_bmp_ptr =  bmp_arp_dir_cust1;
			else	
			if(arp_dir == DIR_CUSTOM2)
				local_bmp_ptr =  bmp_arp_dir_cust2;
		}
		else
		// display mode since shift active
		{
			// Now display current active waveform bmp
			if(arp_mode == ARP_MODE_NORMAL)
				local_bmp_ptr =  bmp_arp_mode_normal;
			else	
			if(arp_mode == ARP_MODE_HOLD)
				local_bmp_ptr =  bmp_arp_mode_hold;
			else	
			if(arp_mode == ARP_MODE_AUTOTRIG)
				local_bmp_ptr =  bmp_arp_mode_autotrig;
			else	
			if(arp_mode == ARP_MODE_KEYBOARD)
				local_bmp_ptr =  bmp_arp_mode_keyb;
		}
		
		DisplayBmp(LCD_ZONE_LFO_INCRUST_WAV_X, LCD_ZONE_INCRUST_Y+3, LCD_INVERT, local_bmp_ptr);
	}

	// LCD LFO incrust zone area is active now
	lcd_current_incrust = current_incrust_local;

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplaySmallBargraphValue()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	display a small bmp of lcd graph on 5x16 pix
//				y axis : value = 0-127 -> 0-4 : current_value >> 5 + >> 6
//				x axis : timestamp 0-15 lfo_graph_index
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplaySmallBargraphValue(uchar x_pos, uchar value)
{
	uchar value_local, tmp;
	
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;
	if(lcd_lock_incrust_cpt) return;

	// transform 0-127 to 160 then >> 5 = 0-4 
	tmp = value >> 4; // Ctrl.lfo
	if(tmp > 6) tmp = 6;

	value_local = 0x40; 
	while(tmp--)
		value_local	= (value_local >> 1) | 0x40 ;

	// display the current lfo value on lcd small graph
	FillLcdZone(x_pos, LCD_ZONE_LFO_WAVE_STATE_Y, 2, 1, ~value_local); 
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayFullMotionLcdIncrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayFullMotionLcdIncrust(uchar force_display)
{
//V1.2	rom uchar * local_bmp_ptr;
	uchar must_display_motion_flags;
	uchar motion_part_local;
	uchar x_pos, tmp, motion_lenght_local;

	// exit if screen is locked by menus/assigns only if not in motion rec screen (this)
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) 
		if(lcd_current_incrust != LCD_INCRUST_FULL_MOTION) 
			return;

	must_display_motion_flags   = 0;

	// If motion full incrust already active, update only new param
	if(lcd_current_incrust != LCD_INCRUST_FULL_MOTION)
	{
		// Display incrust title
		rom_to_lcd_text_copy(motion_txt);
		// Display incrust zone
		DisplayEmptyIncrust();
		// Show bmp whole motion sampler incrust
		DisplayBmp(LCD_ZONE_INCRUST_X+20, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, bmp_motion_incrust_txt);
		// must re-display everything
		must_display_motion_flags |= (must_display_part | must_display_length);
		if(force_display != FORCE_MOTION_REC_DISPLAY)
			must_display_motion_flags |= (must_display_all_states | must_display_part_bar);	
		else
			must_display_motion_flags |= must_display_rec_text;	
	}
	else
	{
		// Need to update motion part value only
		if(force_display == FORCE_MOTION_PART_DISPLAY)
			must_display_motion_flags |= (must_display_part | must_display_length);
		else
		// Need to update motion part length only
		if(force_display == FORCE_MOTION_LENGTH_DISPLAY)
			must_display_motion_flags |= must_display_length;
		else
		// Need to update motion states only
		if(force_display == FORCE_MOTION_STATES_DISPLAY)
			must_display_motion_flags |= must_display_single_state;
		else
		// Need to update motion states only
		if(force_display == FORCE_MOTION_REC_DISPLAY)
			must_display_motion_flags |= must_display_rec_text;
	}	

	//	if( (lcd_current_incrust != LCD_INCRUST_FULL_MOTION)||(force_display==FORCE_MOTION_PART_DISPLAY) )
	if(must_display_motion_flags & must_display_part)
	{
		// display part number 0-7
		DisplayLcdInt(LCD_ZONE_INCRUST_X+21, LCD_ZONE_INCRUST_Y+2, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, motion_part);
		// if not recording remove and display a bar under the motions states
		FillLcdZone(LCD_ZONE_INCRUST_X + 10, LCD_ZONE_INCRUST_Y+4, 80, 1, 0xCF);
		// Now display small bar under selected motion part
		x_pos = LCD_ZONE_INCRUST_X + 10;
		for(tmp=0; tmp < motion_part; tmp++)
			x_pos += 10;
	
		FillLcdZone(x_pos, LCD_ZONE_INCRUST_Y+4, 4, 1, 0xC9);
	}

	// Need to update motion lenght
	//	if( (lcd_current_incrust != LCD_INCRUST_FULL_MOTION)||(force_display==FORCE_MOTION_LENGTH_DISPLAY) )
	if(must_display_motion_flags & must_display_length)
	{
		motion_lenght_local = Motion_Str[motion_part].motion_length;
		// odd lenght is sync on full measure	
		if(motion_lenght_local & 1)
		{
			tmp = 5;
			// remove old 1/2 if exist
			FillLcdZone(48, LCD_ZONE_INCRUST_Y+2, 23, 1, FILL_BLANK);
		}
		else
		{
			tmp = 0;
			DisplayBmp(LCD_ZONE_INCRUST_X+60, LCD_ZONE_INCRUST_Y+2, LCD_INVERT, un_demi_bmp);
		}
		DisplayLcdInt(LCD_ZONE_INCRUST_X+48+tmp, LCD_ZONE_INCRUST_Y+2, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, (motion_lenght_local + 1) >> 1);
	}

	// Need to update motion play/rec state of all 8 motion parts
	//	if( (lcd_current_incrust != LCD_INCRUST_FULL_MOTION)||(force_display==FORCE_MOTION_STATES_DISPLAY) )
	if(must_display_motion_flags & must_display_all_states)
	{
		for(motion_part_local=0; motion_part_local<MOTION_PART_NB ; motion_part_local++)
			DisplayMotionSmallState(motion_part_local);
	}
	else
	// Need to update motion play/rec state of a single motion part (incrust already active)
	if(must_display_motion_flags & must_display_single_state)
		DisplayMotionSmallState(motion_part);
	else
	// Need to update motion rec text
	if(must_display_motion_flags & must_display_rec_text)
		DisplayBmp(LCD_ZONE_INCRUST_X+3, LCD_ZONE_INCRUST_Y+3, LCD_INVERT, motion_wait_rec_bmp);

	if(Motion_Status_Rec)
	{
		// Lock LCD incrust zone forever since next screen is global 128x64
		lcd_lock_incrust_cpt = 0;
		// Lock LCD from other functions
		Z8_STATUS_4 |= lcd_locked_by_function_z4;
	}
	else
		// Lock LCD incrust zone for 2 seconds
		lcd_lock_incrust_cpt = CPT_2_SEC;

	// LCD LFO incrust zone area is active now
	lcd_current_incrust = LCD_INCRUST_FULL_MOTION;

}


/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayMotionSmallState()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayMotionSmallState(uchar motion_part_display)
{
	uchar tmp;
	rom uchar * local_bmp_ptr;
	uchar Mask = 1;
	uchar x_pos = LCD_ZONE_INCRUST_X + 10;
	
	for(tmp=0; tmp < motion_part_display; tmp++)
	{
		x_pos += 10;
		Mask = Mask << 1;
	}
	
	// display playing bmp ?
//V1.7	if(Motion_Status_Play & Mask)
//V1.7		local_bmp_ptr = small_play_bmp; // bmp is play
	if(Motion_Status_Play & Mask)
	{
		if(Z8_STATUS_10 & realtime_playing_z10)
			local_bmp_ptr = small_play_bmp; // bmp is play
		else
			local_bmp_ptr = small_pause_bmp; // bmp is pause V1.7
	}
	else
	if(!(Motion_Status_Rec & Mask))
		local_bmp_ptr = small_stop_bmp; // bmp is stop

	// display the bmp now
	DisplayBmp(x_pos, LCD_ZONE_INCRUST_Y+3, LCD_INVERT, local_bmp_ptr);

}


/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayMotionRecGraph()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayMotionRecGraph(uchar current_motion_part, uchar data_index)
{
	uchar local_value;
	uchar local_y_page = 5;
	uchar local_lcd_value = 0x80;
//	uchar nb_x_pix;
//	uchar local_motion_length = Motion_Str[current_motion_part].motion_length;

	// get the nb of x pixel per sampling point
//	nb_x_pix = 8;
//	while(local_motion_length)
//	{
//		nb_x_pix = nb_x_pix >> 1;
//		local_motion_length--;
//	}

	/////////////////////////////// DISPLAY MOTION VALUES ON A GRAPH ////////////////////
	// transform 0-127 into 0-31
	local_value = Motion_Str[current_motion_part].motion_data_table[data_index] >> 2;

	while(local_value)
	{
		if(local_lcd_value == 1)
		{
			local_y_page--;
			local_lcd_value = 0x80;
		}
		else
			local_lcd_value = local_lcd_value >> 1;
		local_value--;
	}

	// add border if required
//	if(local_y_page==5)
//		local_lcd_value |= 0x80;

	// Display now the value;
	FillLcdZone(32 + data_index, local_y_page, 1, 1, ~local_lcd_value);

	/////////////////////////////// DISPLAY MOTION ELAPSED REC TIME ////////////////////
	FillLcdZone(32, 6, data_index, 1, 0x44);
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayBs3xInformations()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	show all the bs3x informations
//////////////////////////////////////////////////////////////////////////////////////////////
// V1.4
void DisplayBs3xInformations(void)
{
	// lock lcd updates from controls
	Z8_STATUS_4 |= lcd_locked_by_function_z4;

	// show home screen only to get borders
	DisplayBmp(0, 0, LCD_INVERT, bmp_home);

	// clear lcd home screen exepted borders
	FillLcdZone(5, 3, 120, 4, FILL_BLANK);

	// redraw small lost border
	FillLcdZone(3, 7, 122, 1, 0x5F);
	FillLcdZone(125, 7, 1, 1, 0x40);

	// display infos txt
//	rom_to_lcd_text_copy(bs3x_info_txt);
//	lcd_text_value[0] =FONT_6x8_BDR_LEFT_INV;
//	lcd_text_value[10]=FONT_6x8_BDR_RIGHT_INV;
//	DisplayLcdText(5, 1, LCD_NORMAL, font_5x8);
	// display firmware txt
	rom_to_lcd_text_copy(firmware_txt);
	DisplayLcdText(7, 3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
	// display firmware txt
	rom_to_lcd_text_copy(hardware_txt);
	DisplayLcdText(7, 4, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
	// display firmware txt
	rom_to_lcd_text_copy(serial_txt);
	DisplayLcdText(7, 5, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
	// display firmware txt
	rom_to_lcd_text_copy(date_txt);
	DisplayLcdText(7, 6, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );

	//	Now dislpay informations
	// show firmware version
	DisplayRomVersion(72, 3);

	// show hardware version
	lcd_text_value[0] = identity[HW_VERSION_INDEX] + '0';
	lcd_text_value[2] = identity[HW_SUB_VERSION_INDEX] + '0';
	DisplayLcdText(72, 4, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	

	// show serial number
	lcd_text_value[0] = identity[SERIAL_4_INDEX] + '0';
	lcd_text_value[1] = '-'; 
	lcd_text_value[2] = identity[SERIAL_3_INDEX] + '0';
	lcd_text_value[3] = identity[SERIAL_2_INDEX] + '0';
	lcd_text_value[4] = identity[SERIAL_1_INDEX] + '0';
	lcd_text_value[5] = identity[SERIAL_0_INDEX] + '0';
	lcd_text_value[6] = '\0';
	DisplayLcdText(72, 5, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	

	// show Date
	DisplayLcdInt(72, 6, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ , identity[MANUF_MONTH_INDEX]);
	lcd_text_value[0] = '/';
	lcd_text_value[1] = '\0'; 
	DisplayLcdText(85, 6, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );	
	DisplayLcdInt(92, 6, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ , identity[MANUF_YEAR_INDEX]);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayTempo()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update the tempo zone
//////////////////////////////////////////////////////////////////////////////////////////////
// V1.69
void DisplayTempo(void)
{
	rom uchar *local_bmp_ptr;

	if(lcd_current_incrust != LCD_INCRUST_TEMPO_ADJUST)
	{
		// will be incrust title
		rom_to_lcd_text_copy(tempo_txt);
		// Display empty incrust zone
		DisplayEmptyIncrust();
		// display BPM text
		DisplayBmp(LCD_ZONE_INCRUST_X+43, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, bmp_bpm_txt); 
		// display if internal sequcenr is on/off
		if(CTRL_REG_1_7BITS_TO_EZ & realtime_source_int_z_ez1)
			local_bmp_ptr = bmp_small_int_txt;
		else
			local_bmp_ptr = bmp_small_ext_txt;
		// display sync mode
		DisplayBmp(LCD_ZONE_INCRUST_X+30, LCD_ZONE_INCRUST_Y+3, LCD_INVERT, local_bmp_ptr);
		// display update mode continuous or single shot
		if(Z8_STATUS_2_NVM & tempo_change_mode_z2nvm)
			local_bmp_ptr = bmp_tempo_cont;
		else
			local_bmp_ptr = bmp_tempo_oneshot;
		// display sync mode
		DisplayBmp(LCD_ZONE_INCRUST_X+72, LCD_ZONE_INCRUST_Y+2, LCD_INVERT, local_bmp_ptr);

		// affect new incrust zone
		lcd_current_incrust = LCD_INCRUST_TEMPO_ADJUST;
		// lock tempo incrust until OK relaxed
		lcd_lock_incrust_cpt = CPT_INFINITE;
	}

	// Update new tempo value immediately if tempo mode is immediate
	if(Z8_STATUS_2_NVM & tempo_change_mode_z2nvm)
	{
		// V1.8 depending on midi sync type, reinit timer2 (smpte) or timer3 (midi clock)
		// smpte sync
		//ReinitTimer2();
		// midi clock sync
		ReinitTimer3();
	}

	// display current tempo value
	DisplayLcdInt(LCD_ZONE_INCRUST_X+10, LCD_ZONE_INCRUST_Y+1, DIGITS_3|LCD_INVERT|FONT_NUM_8x16_FLG, internal_tempo+30);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplaySceneTypeBitmap()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update the bitmap zone for scenes
//////////////////////////////////////////////////////////////////////////////////////////////
// V1.8
void DisplaySceneTypeBitmap(void)
{
	rom uchar *local_bmp_ptr;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	if(Z8_STATUS_3_NVM & scene_mode_z3nvm)
	{
		local_bmp_ptr = bmp_scene_arp;
	}	
	else
	{
		local_bmp_ptr = bmp_scene_snapshot;
	}

	// update small bmp according to scene_arp or scene_snapshot
	DisplayBmp(LCD_ZONE_SCENE_X+4, LCD_ZONE_SCENE_Y, LCD_INVERT, local_bmp_ptr);
}
