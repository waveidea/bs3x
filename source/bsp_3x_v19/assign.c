#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd.h"
#include "lcd_low_level.h"
#include "assign.h"
#include "hardware.h"
#include "eeprom.h"
#include "menus.h"
#include "i2c.h"
#include "serial.h"

uchar assign_state;
far uchar current_exp_assign;				// V1.6
uchar control_assign_index;					// current control nb to assign
extern uchar control_index;					// current control nb
extern uchar last_control_index;			// last control nb

// extern uchar menu_state;
extern uchar Z8_STATUS_5;
extern rom uchar full_menu_assign_bmp[];
//extern rom uchar control_txt[];
//extern rom uchar font_5x8[];
 
// extern far All_Params Param;				// structure of parameters in EEPROM
//extern far All_Params *ParamCurrent;		// V1.61 current structure of parameters in EEPROM
extern far All_Params	 Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61 structure of parameters in EEPROM V1.61
//extern rom uchar full_menu_bmp[];
extern uchar current_i2c_exp;				// V1.6

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AssignEnter()
// Inputs	: menu_state
// Outputs	: None
// Process  : Enter into assign menus mode
///////////////////////////////////////////////////////////////////////////////////////
void AssignEnter(void)
{	
	DisplayBmp(0, 0, LCD_INVERT, full_menu_assign_bmp);
	// wait for the user to confirm
	assign_state = ASSIGN_WAIT_CONFIRM;
	Z8_STATUS_5 &= ~must_save_assign_struct_z5;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AssignEnter()
// Inputs	: menu_state
// Outputs	: None
// Process  : Enter into assign menus mode
///////////////////////////////////////////////////////////////////////////////////////
void AssignWaitControl(void)
{
	// continue only if waiting control to assign 
	if(assign_state != ASSIGN_WAIT_TWEAK_CTRL)
		return;

	// the control has been tweaked
	if(control_assign_index == CTRL_UNDEF)
		return;

	control_index = control_assign_index;
	current_i2c_exp = current_exp_assign;
	last_control_index = CTRL_UNDEF;
	// set the new assign mode
	assign_state = ASSIGN_ACTIVE;

//	FillParamCurrentStruct(); // V1.61

// BLOCK REMOVED IN V1.61 
	// Load full structure
	GetCtrlStructEEaddFromIndexGroup();
	// Read struct full or status only from eeprom 
	ReadMultiByteI2c((far uchar *)&Param[0].ctrl_status_0, MIDI_STRUCT_SIZE);

	// Show assign menus
	MenuEnter();
}

