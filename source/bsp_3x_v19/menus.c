#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd.h"
#include "lcd_low_level.h"
#include "automation.h"
#include "hardware.h"
#include "itrs.h"
#include "eeprom.h"
#include "arpp.h"
#include "menus.h"
#include "realtime.h" // V1.69
#include "smpte.h" // V1.8
#include "menus_tables.h"
#include "assign.h"
#include "motion.h"
#include "i2c.h"
#include "expansion.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar current_menu_depth;   // current menu depth
uchar current_menu_index;   	// index into the menutable of the current selected menu
far uchar menu_index_call_stack[MENUS_MAX_DEPTH];
uchar menu_stack_index = 0;
uchar next_value;
uchar up_down_action; 
uchar menu_state;

extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];		// table of control reg for eerprom saving and menus 
extern uchar Z8_STATUS_3;								// V1.69
extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_6;
extern uchar Z8_STATUS_7;
extern uchar Z8_STATUS_8;
extern uchar Z8_STATUS_9;
// V1.66 extern rom uchar font_5x8[];
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion	
//extern rom uchar menu_param_bmp[];
extern rom uchar full_menu_bmp[];
extern rom uchar arrow_left_bmp[];
extern rom uchar arrow_left_val_bmp[];
extern rom uchar arrow_right_val_bmp[];
extern rom uchar menu_assign_top_bmp[];

//extern uchar ok_sw_lock_cpt;				// debounce ok switch
//extern uchar cancel_sw_lock_cpt;			// debounce cancel switch

//extern rom uchar arrow_right_bmp[];
extern uchar assign_state;
extern rom uchar control_txt[];
extern uchar control_assign_index;
//extern far All_Params Param;				// structure of parameters in EEPROM
//extern far All_Params *ParamCurrent;		// V1.61 current structure of parameters in EEPROM
extern far All_Params	 Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61 structure of parameters in EEPROM V1.61
extern uchar control_index;					// current control nb
extern uchar last_control_index;			// last control nb
extern uchar Motion_Status_Rec; 			// each bit indicates if part's recording
extern far uint non_assign_sw_test_mode;
extern far uchar current_exp_assign;		// V1.6
extern uchar current_i2c_exp;				// V1.6
extern rom uchar tempo_txt[];				// V1.69
extern uchar lcd_current_incrust; 			// V1.69
extern rom uchar bmp_bpm_txt[];				// V1.69
extern uchar lcd_lock_incrust_cpt;			// V1.69

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckMenuSwitches()
// Inputs	: None
// Outputs	: None
// Process  : Check if a menu related sw has been pressed
///////////////////////////////////////////////////////////////////////////////////////
void CheckMenuSwitches(void)
{
	if(!Motion_Status_Rec)
	{
		// EDIT switch pressed ?
		if(EDIT_PORT_IN & EDIT_PIN)
		{
			if(Z8_STATUS_7 & edit_passed_0_z7)
			{
				Z8_STATUS_6 |= edit_sw_changed_z6;
				Z8_STATUS_7 &= ~edit_passed_0_z7;	
			}
		}
		else Z8_STATUS_7 |= edit_passed_0_z7;
	
		// edit changed ?
		if(Z8_STATUS_6 & edit_sw_changed_z6)
		{
			// if we are in test mode then update the dedicated register
			if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
				non_assign_sw_test_mode |= TEST_MODE_EDIT_BIT_MASK;
		
			Z8_STATUS_6 &= (~edit_sw_changed_z6);
			// Need to enter next menu mode (NO_MENU, MENU_PARAM, ...)
			if(++menu_state >= MENU_STATE_NB)
			{
				ExitMenusMode();
				return;
			}
	
			if(menu_state == MENU_PARAM)
				MenuEnter();
			else
			if(menu_state == MENU_ASSIGN)
				AssignEnter();
			else
			// V1.4 : a new screen with bs3x infos added
			if(menu_state == MENU_BS3X_INFOS)
				DisplayBs3xInformations();
		}
	
		// no menu switch active if not in menus or assign mode
		// V1.69 if OK clicked while in normal mode : give a direct access to tempo
		if(menu_state == NO_MENU)
		{
			// OK switch pressed 
			if(OK_PORT_IN & OK_PIN)
			{
				if(Z8_STATUS_8 & ok_passed_0_z8)
				{
					DisplayTempo();
					Z8_STATUS_8 &= ~ok_passed_0_z8;	
				}
			}
			else
 		    // remove the tempo incrust zone if it was displayed
			{
				if(lcd_current_incrust == LCD_INCRUST_TEMPO_ADJUST)
				{
					// Update new tempo value when OK button is relaxed if tempo mode is delayed
					if(!(Z8_STATUS_2_NVM & tempo_change_mode_z2nvm))
					{
						// V1.8 depending on midi sync type, reinit timer2 (smpte) or timer3 (midi clock)
						// smpte sync
						//ReinitTimer2();
						// midi clock sync
						ReinitTimer3();
					}

					// reinit the incrust zone
					Z8_STATUS_3 |= must_reinit_incrust_lcd_z3;
					// stop counting down from infinite
					lcd_lock_incrust_cpt = 0;
				}
				Z8_STATUS_8 |= ok_passed_0_z8;
			}
			return;
		}
	} // end motion not currently recording

	////////////////////////////////////////////////////////////////////////
	// here we are in menu or assign mode : check ok/cancel/encoder switches
	////////////////////////////////////////////////////////////////////////

	// CANCEL switch pressed ?
	if(CANCEL_PORT_IN & CANCEL_PIN)
	{
		if(Z8_STATUS_8 & cancel_passed_0_z8)
		{
			ButtonCancelClick();
			Z8_STATUS_8 &= ~cancel_passed_0_z8;	
		}
	}
	else Z8_STATUS_8 |= cancel_passed_0_z8;

	// the following sw are used if not waiting tweak ctrl to assign
	if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
		return;

	// nothing to do after cancel if not waiting for a record motion confirm
	if( (Motion_Status_Rec) && (!(Z8_STATUS_9 & motion_wait_rec_confirm_z9))) return;
		
	// V1.4 : nothing to do while displaying bs3x infos
	if(menu_state == MENU_BS3X_INFOS)
		return;

	// OK switch pressed 
	if(OK_PORT_IN & OK_PIN)
	{
		if(Z8_STATUS_8 & ok_passed_0_z8)
		{
			ButtonOkClick();
			Z8_STATUS_8 &= ~ok_passed_0_z8;	
		}
	}
	else Z8_STATUS_8 |= ok_passed_0_z8;

	// nothing to do after cancel if recording a motion part
	if(Motion_Status_Rec) return;

	// the following sw are used if not waiting confirm assign mode
	if((assign_state == ASSIGN_WAIT_CONFIRM))//||(assign_state == ASSIGN_WAIT_TWEAK_CTRL))
		return;

	// OK is also obtained from encoder sw when menu mode active
	if(Z8_STATUS_6 & ok_sw_changed_z6)
	{
		ButtonOkClick();
		Z8_STATUS_6 &= (~ok_sw_changed_z6);
	}

	// UP or DOWN SWITCH PRESSED ?
	if(Z8_STATUS_6 & up_down_sw_changed_z6)
	{
		Z8_STATUS_6 &= (~up_down_sw_changed_z6);

		// UP switch pressed ?
		if(Z8_STATUS_6 & up_down_sw_direction_z6)
			ChangeCurrentMenu(UP_PRESSED);
		else
			ChangeCurrentMenu(DOWN_PRESSED);				
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: MenuEnter()
// Inputs	: menu_state
// Outputs	: None
// Process  : Enter into menus mode, check menus_state value
///////////////////////////////////////////////////////////////////////////////////////
void MenuEnter(void)
{	
	// return to top level depth
	current_menu_depth = TOP_LEVEL_MENU;

	// Display first menu param global bitmap
	//DisplayBmp(LCD_MENUS_TITLE_BMP_X, LCD_MENUS_TITLE_BMP_Y, LCD_INVERT, local_bmp_ptr);
	DisplayBmp(0, 0, LCD_INVERT, full_menu_bmp);

	// display control in assign mode
	if(menu_state == MENU_ASSIGN)
	{
		DisplayBmp(0, 0, LCD_INVERT, menu_assign_top_bmp);
		//rom_to_lcd_text_copy(control_txt);
		//DisplayLcdText(3, 1, LCD_INVERT, font_5x8);	
		DisplayLcdInt(27, 1, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, control_assign_index);
		//DisplayLcdInt(27, 1, DIGITS_2|LCD_INVERT, font_5x8, current_group);
		current_menu_index = MENUS_ASSIGN_FIRST_INDEX; 
	}
	else
		current_menu_index = 0; 

	// lock lcd updates from controls
	Z8_STATUS_4 |= lcd_locked_by_function_z4;

	// Show menus
	DisplayMenus();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMenus()
// Inputs	: None
// Outputs	: None
// Process  : display current menu index name according to menu depth & index
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMenus(void)
{
        // Display current menu line
        AddLcdMenuLine(current_menu_depth);

        // Now display menu value
        DisplayMenuValue();

        // Display selected line
        DisplaySelectedLine();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetBitPositionFromMask()
// Inputs	: READ or WRITE reg, plus the bits mask
// Outputs	: None
// Process  : Enter into menus mode
///////////////////////////////////////////////////////////////////////////////////////
uchar GetBitPositionFromMask(uchar bits_mask, uchar action)
{
        uchar pos = 0;
        uchar msk = 1;
        while(!(bits_mask & msk))
        {
                msk = msk<<1;
                pos++;
        }
        // if action is to read the reg value, then return position of the bit
        if(action == READ_REG_VALUE)
           return pos;

        // if action is to write the reg value, then return single bit toogle info
        // or the bit position if multi bit mask to update
        // Check if it was a single bit mask
        msk = msk << 1;
        // if next msk is 0, then single bit to toggle
        if(bits_mask & msk)
           return pos;
        else
           return MUST_TOGGLE_SINGLE_BIT;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: MenuGetValue()
// Inputs	: None
// Outputs	: None
// Process  : Return the current menu associated value
///////////////////////////////////////////////////////////////////////////////////////
uchar MenuGetValue(void)
{
        uchar value;
        uchar bit_pos;
		//MENU *local_ptr;

        if(menutable[current_menu_index].table_id == TABLE_GLOBAL_PARAMS)
        {
                // A full reg must be read
                if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                        value = CtrlReg[menutable[current_menu_index].value_index];
                else
                {
                        // A Bit Masked reg must be read
                        bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, READ_REG_VALUE);
                        value   = ((CtrlReg[menutable[current_menu_index].value_index]) & (menutable[current_menu_index].bit_pos_msk)) >> bit_pos;
                }
        }
        // Insert here other tables to modify
		else
        if(menutable[current_menu_index].table_id == TABLE_CTRL_ASSIGN)
        {
				//local_ptr = &Param;

                // A full reg must be read
                if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                        value = *((far uchar *)(&Param[0].ctrl_status_0 + (uint)(menutable[current_menu_index].value_index)));
                else
                {
                        // A Bit Masked reg must be read
                        bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, READ_REG_VALUE);
                        value   = (*(far uchar *)(&Param[0].ctrl_status_0 + (uint)(menutable[current_menu_index].value_index)) & (menutable[current_menu_index].bit_pos_msk)) >> bit_pos;
                }
		}
		
	return value;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: MenuSetValue()
// Inputs	: None
// Outputs	: None
// Process  : Set the current selected value for the current menu
///////////////////////////////////////////////////////////////////////////////////////
void MenuSetValue(uchar next_value)
{
        uchar value;
        uchar bit_pos;
//		far uchar *local_ptr;

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////// MENUS VARIAVBLES ////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
        if(menutable[current_menu_index].table_id == TABLE_GLOBAL_PARAMS)
        {
                // A full reg must be written
                if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                        CtrlReg[menutable[current_menu_index].value_index] = next_value;
                else
                {
                        // A Bit Masked reg must be written
                        bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, WRITE_REG_VALUE);
                        // Must toggle single bit ?
                        if(bit_pos == MUST_TOGGLE_SINGLE_BIT)
                          CtrlReg[menutable[current_menu_index].value_index] ^= menutable[current_menu_index].bit_pos_msk;
                        else
                        {
                                // Must update a Multi bit masked value
                                CtrlReg[menutable[current_menu_index].value_index] &= (~menutable[current_menu_index].bit_pos_msk);
                                CtrlReg[menutable[current_menu_index].value_index] |= (next_value << bit_pos);
                        }
                }
				//////////////////////////////////////////////////////////////
				// specific actions to perform when a menu variable is changed
				//////////////////////////////////////////////////////////////
				// arpeggio resolution
				if(current_menu_index == ARPP_CLK_RESO_INDEX)
					set_resolution();
				else
				if(current_menu_index == ARPP_SWING_INDEX)
					set_swing();
				else
				if(current_menu_index == ARPP_MODE_INDEX)
					set_mode();
				else
				if( (current_menu_index == ARPP_STATE_INDEX) ||
					(current_menu_index == LFO_STATE_INDEX) ||
					(current_menu_index == MOTION_STATE_INDEX))
					UpdateAutomationLeds();
				else
				if(current_menu_index == INTERNAL_TEMPO_INDEX)
					// reinit timer for midi clock sync						
					ReinitTimer3();
				else					
				if(	(current_menu_index == INT_SYNC_STATE_INDEX) ||
					(current_menu_index == INT_SYNC_MODE_INDEX) || // V1.8
					(current_menu_index == SMPTE_FRAME_RATE_INDEX) ) // V1.8
					{
						// reinit timer for smpte sync
						ReinitTimer2();
						// reinit timer for midi clock sync						
						ReinitTimer3();
					}
				else // V1.6 : when updating group or hook mode : need to inform expansions
				if( (current_menu_index == GROUP_SELECT_INDEX) /* V1.8 || (current_menu_index == ANALOG_MODE_INDEX) */)
					ConfigureAllExpansions();
				else // V1.7 : when updating operating mode : need to prepare change
				if(current_menu_index == OPERATING_MODE_INDEX)
				{
					UpdateOperatingMode();
				}

        }
        // Insert here other tables to modify
		else
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////// CONTROL ASSIGN //////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
        if(menutable[current_menu_index].table_id == TABLE_CTRL_ASSIGN)
        {
//				local_ptr = &ParamCurrent->ctrl_status_0;

                // A full reg must be written
                if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                        *((far uchar *)(&Param[0].ctrl_status_0 + (uint)(menutable[current_menu_index].value_index))) = next_value;
                else
                {
                        // A Bit Masked reg must be written
                        bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, WRITE_REG_VALUE);
                        // Must toggle single bit ?
                        if(bit_pos == MUST_TOGGLE_SINGLE_BIT)
                          *((far uchar *)(&Param[0].ctrl_status_0 + (uint)(menutable[current_menu_index].value_index))) ^= menutable[current_menu_index].bit_pos_msk;
                        else
                        {
                                // Must update a Multi bit masked value
                                *((far uchar *)(&Param[0].ctrl_status_0 + (uint)(menutable[current_menu_index].value_index))) &= (~menutable[current_menu_index].bit_pos_msk);
                                *((far uchar *)(&Param[0].ctrl_status_0 + (uint)(menutable[current_menu_index].value_index))) |= (next_value << bit_pos);
                        }
                }
        }

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMenuValue()
// Inputs	: None
// Outputs	: None
// Process  : Display on LCD bottom the current menu value or ">>>" if submenu exists
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMenuValue(void)
{
        uchar menu_value, tmp, center_offset;
		rom uchar *local_ptr;

		// remove old menu value
		FillLcdZone(LCD_MENUS_VALUE_X, LCD_MENUS_VALUE_Y, LCD_MENUS_MAX_X_PIXEL, 1, 0);

        // Is it a menu with affected value or submenus below this menu ?
        if(menutable[current_menu_index].SubMenuBaseIndex == UNDEFINED_VALUE)
        {
                if(menutable[current_menu_index].display_type == DISP_STRING)
                {
					// Get offset to add to retriewe the string (read fat)
   					local_ptr = menutable[current_menu_index].MenuValueStr;
					tmp = (uchar) (local_ptr[next_value]);
					rom_to_lcd_text_copy(local_ptr + tmp);
					// Get offset to add to center menu value
					tmp=0;
					center_offset=48;
					while(lcd_text_value[tmp++])
						center_offset -= 3;
					// tmp nows contains value string lenght
					
					DisplayLcdText(LCD_MENUS_VALUE_X+center_offset, LCD_MENUS_VALUE_Y, LCD_NORMAL|FONT_5x8_FLG/* V1.66 , font_5x8 */);
				}
                else
				{	// DISP_NUMERIC or DISP_NUMERIC_PLUS_1 or DISP_NUMERIC_PLUS_30
					menu_value = next_value;
                	if(menutable[current_menu_index].display_type == DISP_NUMERIC_PLUS_1)
						menu_value++;
					else
                	if(menutable[current_menu_index].display_type == DISP_NUMERIC_PLUS_30)
						menu_value += 30;
         			
					DisplayLcdInt(LCD_MENUS_VALUE_X+37, LCD_MENUS_VALUE_Y, DIGITS_3|LCD_NORMAL|FONT_5x8_FLG/* V1.66 , font_5x8 */, menu_value);
				}
		}
        else
		{
				lcd_text_value[0]  = '>';
				lcd_text_value[1]  = '>';
				lcd_text_value[2]  = '>';
				lcd_text_value[3]  = '\0';
                DisplayLcdText(LCD_MENUS_VALUE_X+37, LCD_MENUS_VALUE_Y, LCD_NORMAL|FONT_5x8_FLG/* V1.66 , font_5x8 */);
		}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: RemoveLcdMenuLine()
// Inputs	: None
// Outputs	: None
// Process  : A value has been validated or cancel is pressed, remove last depth
///////////////////////////////////////////////////////////////////////////////////////
void RemoveLcdMenuLine(void)
{
	uchar offset = 0;

	if(menu_state == MENU_ASSIGN)
		offset = 1;

	FillLcdZone(LCD_MENUS_BASE_X, LCD_MENUS_BASE_Y+current_menu_depth+offset, LCD_MENUS_MAX_X_PIXEL, 1, FILL_BLANK);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AddLcdMenuLine()
// Inputs	: current menu line index (0-3)
// Outputs	: None
// Process  : Add or modify a menu line
///////////////////////////////////////////////////////////////////////////////////////
void AddLcdMenuLine(uchar current_lcd_line)
{
        uchar tmp;
        //char lcd_str[30];
		uchar offset_x_submenu=0;
		uchar offset = 0;

		if(menu_state == MENU_ASSIGN)
			offset = 1;

        // create string right shifts according to the menu depth
        for(tmp=0; tmp<current_menu_depth; tmp++)
            // lcd_str[tmp] = '-';
			offset_x_submenu += 5;

		RemoveLcdMenuLine();
		rom_to_lcd_text_copy(menutable[current_menu_index].MenuStr);
		DisplayLcdText(LCD_MENUS_BASE_X+offset_x_submenu, LCD_MENUS_BASE_Y+current_lcd_line+offset, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */);

		// Display top level menu bmp if current line is top level menu depth
		if(menu_state != MENU_ASSIGN)
			if(!current_menu_depth)
				DisplayBmp(LCD_MENUS_TOP_LEVEL_BMP_X, LCD_MENUS_TOP_LEVEL_BMP_Y, LCD_INVERT, menutable[current_menu_index].GlobalMenuBmp);

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplaySelectedLine()
// Inputs	: None
// Outputs	: None
// Process  : display a bmp which show the current active menu line
///////////////////////////////////////////////////////////////////////////////////////
void DisplaySelectedLine(void)
{
//	uchar sel_line_y_page;
	uchar offset = 0;

	if(menu_state == MENU_ASSIGN)
		offset = 1;

	// first clear current selected value 
	//DisplayBmp(2, LCD_MENUS_VALUE_Y, LCD_INVERT, bmp_clr_menu_sel_val);
	// First clear all possible selected line
	FillLcdZone(2, LCD_MENUS_BASE_Y+offset, LCD_SELECT_LINE_ARROW_X_MAX, MENUS_MAX_DEPTH-offset, FILL_BLANK);
	FillLcdZone(1, LCD_MENUS_VALUE_Y, 4, 1, FILL_BLANK);
	FillLcdZone(123, LCD_MENUS_VALUE_Y, 4, 1, FILL_BLANK);

     if(up_down_action == CHANGE_MENU_VALUE)
     {        
  	 	//sel_line_y_page = LCD_MENUS_VALUE_Y;
		DisplayBmp(1, LCD_MENUS_VALUE_Y, LCD_INVERT, arrow_left_val_bmp);
		DisplayBmp(123, LCD_MENUS_VALUE_Y, LCD_INVERT, arrow_right_val_bmp);
     }
	 else
     {
	 	// sel_line_y_page = current_menu_depth+LCD_MENUS_BASE_Y;
		DisplayBmp(2, current_menu_depth+LCD_MENUS_BASE_Y+offset, LCD_INVERT, arrow_left_bmp);
     }
	 // Display left arrow on selected line
//	 DisplayBmp(2, sel_line_y_page, LCD_INVERT, arrow_left_bmp);
//	 DisplayBmp(LCD_MENUS_BASE_X+LCD_MENUS_MAX_X_PIXEL, sel_line_y_page, LCD_INVERT, arrow_right_bmp);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ButtonCancelClick()
// Inputs	: None
// Outputs	: None
// Process  : Cancel key has been pressed
///////////////////////////////////////////////////////////////////////////////////////
void ButtonCancelClick(void)
{
	// we are now recording a motion
	if(Motion_Status_Rec) 
	{
		Motion_Status_Rec = 0; 
		Z8_STATUS_9 &= ~motion_wait_rec_confirm_z9;
		Z8_STATUS_7 |= must_reinit_all_lcd_z7;
		return;
	}

	// exit menus if current depth is 0
	if( (current_menu_depth==TOP_LEVEL_MENU) || (assign_state == ASSIGN_WAIT_CONFIRM))
	{
		ExitMenusMode();
		return;
	}

	// Here we are in a menu mode
	if(up_down_action == CHANGE_MENU_VALUE)
	{
		// here we were changing menu value, need to cancel only
		up_down_action = CHANGE_MENU_INDEX;
		// retriewe unmodifed value
		next_value = MenuGetValue();
	}
	else
	// here we were navigating into menus, need to decrease menu depth
	if(current_menu_depth)
	{
		RemoveLcdMenuLine();
		current_menu_depth--;
		current_menu_index = menu_index_call_stack[--menu_stack_index];
		// need to redisplay top level menu bmp ?
		if(menu_state != MENU_ASSIGN)
			if(current_menu_depth==TOP_LEVEL_MENU)
				DisplayBmp(LCD_MENUS_TOP_LEVEL_BMP_X, LCD_MENUS_TOP_LEVEL_BMP_Y, LCD_INVERT, menutable[current_menu_index].GlobalMenuBmp);

	}
	DisplayMenuValue();
	DisplaySelectedLine();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ButtonOkClick()
// Inputs	: None
// Outputs	: None
// Process  : OK key has been pressed
///////////////////////////////////////////////////////////////////////////////////////
void ButtonOkClick(void)
{

	// are we waiting a recorded motion to save to eeprom ?
	if( (Motion_Status_Rec) && (Z8_STATUS_9 & motion_wait_rec_confirm_z9))
	{
		// Save the recorded motion to external eeprom
		SaveMotionToEEprom();
		// finish recording
		Z8_STATUS_9 &= ~motion_wait_rec_confirm_z9;
		Motion_Status_Rec = 0; 
		Z8_STATUS_7 |= must_reinit_all_lcd_z7;
		return;
	}

	// do we have to confirm the assign mode ?
	if(assign_state == ASSIGN_WAIT_CONFIRM)
	{
		assign_state = ASSIGN_WAIT_TWEAK_CTRL;	
		// clear lcd zone for next message
		FillLcdZone(LCD_ZONE_ASSIGN_TEXT_X, LCD_ZONE_ASSIGN_TEXT_Y, LCD_ZONE_ASSIGN_TEXT_X_LENGHT, 3, FILL_BLANK);
		// redisplay line of menu assign
		FillLcdZone(LCD_ZONE_ASSIGN_TEXT_X, LCD_ZONE_ASSIGN_TEXT_Y+2, LCD_ZONE_ASSIGN_TEXT_X_LENGHT, 1, 0x9F);
		// display message "tweak control to assign
		rom_to_lcd_text_copy(control_txt);
		DisplayLcdText(LCD_ZONE_ASSIGN_TEXT_X+2, LCD_ZONE_ASSIGN_TEXT_Y+1, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */);	
		control_assign_index = CTRL_UNDEF;
		// clear any pending switch action
		Z8_STATUS_6 &= (~(up_down_sw_changed_z6 | ok_sw_changed_z6));
		return;
	}

    // ok pressed, save new value if we were updating values
    if(up_down_action == CHANGE_MENU_VALUE)
    {
        MenuSetValue(next_value);
		// reload update value since may be different
	    next_value = MenuGetValue();
        // automatically return to last menu depth
        up_down_action = CHANGE_MENU_INDEX;
		DisplayMenuValue();
        DisplaySelectedLine();
		// if we are assigning : the struct has been modified
		if(menu_state == MENU_ASSIGN)
			Z8_STATUS_5 |= must_save_assign_struct_z5;
        return ;
    }

    // is there an associated sub menu : go into next submenu ?
    if(menutable[current_menu_index].SubMenuBaseIndex != UNDEFINED_VALUE)
    {
			// here we may pass from top level to sub-menu
			// may need to remove top level menu bmp
			if(menu_state != MENU_ASSIGN)
				if(!current_menu_depth)
					FillLcdZone(LCD_MENUS_TOP_LEVEL_BMP_X, LCD_MENUS_TOP_LEVEL_BMP_Y, LCD_MENUS_BMP_X_MAX, LCD_MENUS_BMP_Y_MAX, FILL_BLANK);
            // go to next menu depth level
            current_menu_depth++;
            // push caller current_index to stack
            menu_index_call_stack[menu_stack_index++] = current_menu_index;
            // get sub menu base index from struct
            current_menu_index = menutable[current_menu_index].SubMenuBaseIndex;
            // up & down switches are still changing menus index
            up_down_action = CHANGE_MENU_INDEX;
            // get sub menu value
            next_value = MenuGetValue();
            DisplayMenus();
    }
    // else go into value change field
    else
    {
            // now up & down switches are changing menus value
            up_down_action = CHANGE_MENU_VALUE;
            next_value = MenuGetValue();
            DisplaySelectedLine();
    }
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ChangeCurrentMenu()
// Inputs	: None
// Outputs	: None
// Process  : up or down switch is pressed : change menu value or menu index
///////////////////////////////////////////////////////////////////////////////////////
void ChangeCurrentMenu(uchar direction)
{
      uchar first_index_depth=current_menu_index;
      uchar last_index_depth=current_menu_index;
      uchar current_menu_index_saved = current_menu_index;

      // need to change current menu or current menu value ?
      if(up_down_action == CHANGE_MENU_VALUE)
      {
        if(direction==UP_PRESSED)
        {
                // here need to change menu value
                next_value++;
                if(next_value >= menutable[current_menu_index].value_max)
                next_value = 0;
        }
        else
        {
                if(next_value)
                     next_value--;
                else
                     next_value = menutable[current_menu_index].value_max-1;
        }
        DisplayMenuValue();
      }
      else
      {
         // retriewe first index on the current menu depth
         while(current_menu_index)
         {
                if(menutable[--current_menu_index].NextMenuExist)
                        first_index_depth = current_menu_index;
                else break;
         }

         // restore current menu index
         current_menu_index = current_menu_index_saved;

         // retriewe last index on the current menu depth
         while(menutable[current_menu_index].NextMenuExist)
         {
                last_index_depth++;
                current_menu_index++;
         }

         // restore current menu index
         current_menu_index = current_menu_index_saved;

        if(direction==UP_PRESSED)
        {
                 // get index UP on same menu depht
                 if(current_menu_index > first_index_depth)
                        current_menu_index--;
                else
                        current_menu_index = last_index_depth;
        }
        else
        {
               // get index DOWN on same menu depht
              if(current_menu_index < last_index_depth)
                 current_menu_index++;
              else
                 current_menu_index = first_index_depth;
        }

        next_value = MenuGetValue();
        // Display current menu line
        AddLcdMenuLine(current_menu_depth);
        // Now display menu value
        DisplayMenuValue();
     }
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ExitMenusMode()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void ExitMenusMode(void)
{
		// need to save struct before exiting ?
		if(assign_state == ASSIGN_ACTIVE)
		{
			if(Z8_STATUS_5 & must_save_assign_struct_z5)
			{
				control_index = control_assign_index;
				current_i2c_exp = current_exp_assign;	// V1.6
				last_control_index = CTRL_UNDEF;
				// save full structure
				GetCtrlStructEEaddFromIndexGroup();				
				WriteMultiByteI2c((far uchar *)&Param[0].ctrl_status_0, MIDI_STRUCT_SIZE);
				// Initialize all 16 switches push/toggle from ext eeprom since may have been modified
			    GetCurrentGroupSwitchModesFromEE();
				// V1.61 reinit all param and lcd structs already preloaded in RAM
				InitParamStructsInRam();
			}
		}

		menu_state 		= NO_MENU;
		assign_state	= ASSIGN_OFF;		
		Z8_STATUS_7 |= must_reinit_all_lcd_z7;
}
