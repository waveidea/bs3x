#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "hardware.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "eeprom.h"
#include "automation.h"
#include "analog.h"
#include "lfo.h"
#include "serial.h"
#include "assign.h"
#include "arpp.h"
#include "motion.h"
#include "mackie.h" // V1.5
#include "i2c.h"
#include "expansion.h" // V1.6
#include "smpte.h" // V1.8

#ifdef LCD_SCREEN_COPY
#include "lcd_screencopy.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar Z8_STATUS_0;						
uchar Z8_STATUS_3;
volatile uchar Z8_STATUS_4;
uchar Z8_STATUS_5;
uchar Z8_STATUS_6;
uchar Z8_STATUS_7;
uchar Z8_STATUS_8;
uchar Z8_STATUS_9;
uchar Z8_STATUS_10;
uchar Z8_STATUS_11; // V1.8

uchar current_function;			

volatile far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 
far uchar CtrlReg_EE_img[CTRL_REG_REAL_NB];		// table of control reg for eerprom saving
far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 64 bytes
far uchar all_sw_value_EE_img;					// idem for switches which are treated seperately // V1.64 far
far uchar other_sw_EE_img;						// idem for switches which are treated seperately // V1.64 far

far uint zone_to_reinit;	 // V1.62

extern far Controls_Scene	Physical[GROUP_NB];	// All physical values stored here

extern uchar infrared;
extern uchar transport_state;
extern uchar next_transport_state;
//extern uchar current_value;							// current control value
extern uchar lcd_current_incrust;

// V1.66 extern rom uchar font_num_8x16[];
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];		// global buffer for integer to text conversion	

extern rom uchar infrared_txt[];
extern rom uchar ribbon_txt[];
extern rom uchar hold_to_txt[];
// V1.66 extern rom uchar font_5x8[];
extern rom uchar bmp_lock_down[];
extern uchar ir_sw_long_push_cpt;
extern uchar control_index;							// current control nb
extern uchar last_control_index;					// last control nb

extern uint EE_Address;					// current EEProm address
extern uchar I2cCommand;					// current EEProm selection
extern All_Controls Ctrl;				// All controls are defned here

extern uint  seconds_5_timer_cpt;		// used to create a 5 seconds timer from timer 0
extern uchar serial_in_start_index;		// a near pointer on FAR ram
extern uchar serial_in_end_index;		// a near pointer on FAR ram
extern uchar serial_in_current_index;
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];		// NN=1 registers can be accessed from ez board 
extern uchar menu_state;
extern uchar assign_state;
extern uchar current_hook_bmp ;
extern far   uchar CURVE_USER_0_TABLE[CURVE_TABLES_LENGHT];	// user curve model 0
extern far   uchar CURVE_USER_1_TABLE[CURVE_TABLES_LENGHT];	// user curve model 1
extern uchar current_curve_model;
extern uchar lcd_lock_incrust_cpt;

extern volatile uchar TX_STATUS; // V1.8
extern uchar tx0_start_index;
extern uchar tx0_end_index;
extern uchar tx1_start_index;
extern uchar tx1_end_index;
extern far uchar physical_autom_pot[(AUTOMATION_POTS_NB<<2)]; // 3 autom pots, 4 functions each
extern uchar row_analog_itr;
extern volatile uchar index_analog_itr;
extern far uchar current_analog_index; // V1.64 far
//uchar internal_tempo;

extern uchar realtime_generated_start_index;
extern uchar realtime_generated_end_index;
extern uchar realtime_received_start_index;
extern uchar realtime_received_end_index;
extern far 	 Motion_Struct Motion_Str[MOTION_PART_NB];
extern rom   uchar motion_point_nb[];
extern far   uint non_assign_sw_test_mode;
extern far 	 uint must_reinit_lcd_from_sysex;

extern rom uchar shift_bmp[]; // V1.4
extern volatile uchar mackie_bargraph_timeout; // V1.5
extern rom uchar identity[]; // V1.5

extern far uchar index_scene; // V1.6
extern far uchar index_scene_dig; // V1.6
// extern far uchar exp_cfg_reg[CONFIG_REGS_REAL_NB];	// V1.6 All config regs to expansions
//extern uchar STATUS_Z8_EXP; // V1.6
extern uchar current_i2c_exp; // V1.6

extern far All_Params	 Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61 structure of parameters in EEPROM V1.61
extern far All_Params	 *ParamCurrent;						// V1.61 current structure of parameters in EEPROM
extern uchar last_struct_index;								// V1.61
extern far uchar control_index_structs_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61
extern uchar last_lcd_struct_index;								// V1.61
extern far uchar control_index_lcd_structs_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61

// V1.6
extern uchar old_joy_x_pixel 		;
extern uchar old_joy_y_page_start  ;
extern uchar old_joy_y_page_end	;
extern uchar old_fader_x_pixel		;
extern uchar old_fader_hooked_x_pixel;
extern uchar current_curve_model ;

extern far uchar last_displayed_channel;	// V1.62
extern far uchar override_channel_cpt;		// V1.62
extern far uchar mackie_logic_sysex_id;		// V1.69	

extern rom uint  smpte_reload_cpt[SMPTE_FRAME_RATE_NB][3];	// V1.8
extern far SMPTE_STRUCT smpte_code; // V1.8
extern uchar current_smpte_reload_cpt; // V1.8
extern far uchar smpte_lock_cpt; // V1.8
//volatile uint tempo_reload_cpt; // V1.8

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: rom_to_data_copy()
// Inputs	: None
// Outputs	: None
// Process  : Copy a rom table to ram location
///////////////////////////////////////////////////////////////////////////////////////
void rom_to_lcd_text_copy(rom uchar *src)
{
	uchar i=0;

	while (*src)
	   lcd_text_value[i++] = *src++;
	lcd_text_value[i] = '\0';
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitIos()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 IO ports direction and default values and alternate funtions
///////////////////////////////////////////////////////////////////////////////////////
void InitIos(void)
{
	// PORTA DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PAADDR 	= PORT_DIR;   		// Acces to sub-reg direction
  	PACTL 	= 0x90;    			// Port direction 1=in 0=out
	PAADDR 	= PORT_ALT_FCT;     // Acces to sub-reg alternate_functions
  	PACTL 	= 0xF0;    			// Port alternate functions enable 1=in 0=out (i2c, serial 0)
   	PAOUT	= 0X38;     		// Default out values on port, cs1 is high, rw low, e low

	// PORTB DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
	// all portB is alternate function, do not init direction nor default values
	PBADDR 	= PORT_ALT_FCT;     // Acces to sub-reg alternate_functions
  	PBCTL 	= 0xFF;    			// Port alternate functions enable 1=in 0=out

	// PORTC DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PCADDR 	= PORT_DIR;     	// Acces to sub-reg direction
  	PCCTL 	= 0xDF;    			// Port direction 1=in 0=out
	// Port alternate functions not used on this port
   	PCOUT	= 0X20;     		// Default out values on port, cs2 is high

	// PORTD DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PDADDR 	= PORT_DIR;     	// Acces to sub-reg direction
  	PDCTL 	= 0xDF;    			// Port direction 1=in 0=out
    PDADDR 	= PORT_ALT_FCT; 	// Acces to sub-reg alternate_functions
  	PDCTL 	= 0x20;    			// Port alternate functions enable 1=in 0=out
   	// No need to default out values on port

	// PORTE DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PEADDR 	= PORT_DIR;    		// Acces to sub-reg direction
  	PECTL 	= 0x03;    			// Port direction 1=in 0=out
	// No alternate functions on this port
   	PEOUT	= 0X00;     		// Default out values on port

	// PORTF DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
	// Default port dir is input (connected to LCD)
	// No alternate functions on this port

	// PORTG DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PGADDR 	= PORT_DIR;    		// Acces to sub-reg direction
  	PGCTL 	= 0x78;    			// Port direction 1=in 0=out
   	// No alternate functions on this port
	PGOUT	= 0X00;     		// Default out values on port

	// PORTH DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
	// all portH is alternate function, do not init direction nor default values
	PHADDR 	= PORT_ALT_FCT;     // Acces to sub-reg alternate_functions
  	PHCTL 	= 0xFF;    			// Port alternate functions enable 1=in 0=out
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitTimers()
// Inputs	: None
// Outputs	: None
// Process  : Initialize all timers 
///////////////////////////////////////////////////////////////////////////////////////
void InitTimers(void)
{
	// Init Timer 0 : time base timer, continuous, overload at 10ms
	// used to debounce switches
	T0RH  = 0x06;	// Timer 0 reload value High (1562d for 10ms)
	T0RL  = 0x1A;	// Timer 0 reload value Low  (1562d for 10ms)
	T0CTL = 0xB9;	// Timer 0 continuous, auto reload, divide per 128

	// Init Timer 1 : time base timer, single shot, overload at 10ms (lock eeprom)
	// used to lock eeprom
	ReinitTimer1();
	T1CTL = 0x38;	// Timer 1 single shot, not started, divide per 128

	// Timer 2 : used to generate internal tempo smpte mode
	ReinitTimer2();

	// V1.8 Timer 3 : used to generate internal tempo midi clock mode
	ReinitTimer3();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ReinitTimer1()
// Inputs	: None
// Outputs	: None
// Process  : Initialize timer 1 
///////////////////////////////////////////////////////////////////////////////////////
void ReinitTimer1(void)
{
	// Start timer 1 value is 0
	T1H   	= 0x00;	// Timer 1 start value High (1562d for 10ms)
	T1L   	= 0x00;	// Timer 1 start value Low  (1562d for 10ms)
	// Reload timer 1 counter
	T1RH   	= 0x06;	// Timer 1 reload value High (1562d for 10ms)
	T1RL   	= 0x1A;	// Timer 1 reload value Low  (1562d for 10ms)
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	ReinitTimer2()
// Inputs	:	None
// Outputs	: 	None
// Process  : 	Initialize timer 2
//				internal_tempo is 0-220, treat it as 30 - 250 (add 30)
//				tempo to ticks = bpm x 24 / 60 = bpm x 0.4
//				timer2 reload counter = (20000000/(bpm x 0.4))/128
//				timer2 reload counter = 156250 / (bpm x 0.4) = 390625 / bpm
// 
//				LATER SUPPORT FOR 0.1 step on tempo :
//				ex : 133.2 :	calculate tmp1 for 133 = 2937.03
//								calculate tmp2 for 134 = 2915.11
//								step = (tmp1 - tmp2) / 10 = 2.192
//								remove n times step for 0.n tempo
//								on the example above remove 4 from 2937
///////////////////////////////////////////////////////////////////////////////////////
void ReinitTimer2(void)
{	
	uint tempo_reload_cpt_local;
	
	// stop timer 2
	T2CTL = 0x39;	// Timer 2 continuous, not started, divide per 128

	// realtime state ON ?
	if(CTRL_REG_1_7BITS_TO_EZ & realtime_source_int_z_ez1)
	{		
		// Start timer 2 value is 0
		T2H   	= 0x00;	// Timer 2 start value High 
		T2L   	= 0x01;	// Timer 2 start value Low  

		// Depending on sync mode; reload timer 2 with different values
		// if midi sync is configured as smpte sync
		if(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm)
		// SMPTE Mode
		{
			// Depending on frame rate, load timer with different values
			tempo_reload_cpt_local = (uint)smpte_reload_cpt[smpte_code.type][current_smpte_reload_cpt];
			T2RH   	= (uchar)(tempo_reload_cpt_local >> 8);	// Timer 2 reload value High (1562d for 10ms)
			T2RL   	= (uchar)(tempo_reload_cpt_local & 0xFF);	// Timer 2 reload value Low  (1562d for 10ms)
			// Stop timer in smpte mode
			//T2CTL = 0x39;	// Timer 2 continuous, not started, divide per 128
		}

	}
}

// V1.8 : timer 3 used for internal midi clock sync
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	ReinitTimer3()
// Inputs	:	None
// Outputs	: 	None
// Process  : 	Initialize timer 3
//				internal_tempo is 0-220, treat it as 30 - 250 (add 30)
//				tempo to ticks = bpm x 24 / 60 = bpm x 0.4
//				timer2 reload counter = (20000000/(bpm x 0.4))/128
//				timer2 reload counter = 156250 / (bpm x 0.4) = 390625 / bpm
// 
//				LATER SUPPORT FOR 0.1 step on tempo :
//				ex : 133.2 :	calculate tmp1 for 133 = 2937.03
//								calculate tmp2 for 134 = 2915.11
//								step = (tmp1 - tmp2) / 10 = 2.192
//								remove n times step for 0.n tempo
//								on the example above remove 4 from 2937
///////////////////////////////////////////////////////////////////////////////////////
void ReinitTimer3(void)
{
	uint tempo_reload_cpt_local;

	// stop timer 3
	T3CTL = 0x39;	// Timer 3 continuous, not started, divide per 128
	
	// realtime state ON ?
	if(CTRL_REG_1_7BITS_TO_EZ & realtime_source_int_z_ez1)
	{		
		// Start timer 3 value is 0
		T3H   	= 0x00;	// Timer 2 start value High 
		T3L   	= 0x01;	// Timer 2 start value Low  

		// Depending on sync mode; reload timer 3 with different values
		// if midi sync is configured as MIDI clock
//		if(!(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm))
		{
			// Calcultate timer overflow
			tempo_reload_cpt_local = 390625L / (internal_tempo + 30);
			// Reload timer 3 counter
			T3RH   	= (uchar)(tempo_reload_cpt_local >> 8);	// Timer 3 reload value High (1562d for 10ms)
			T3RL   	= (uchar)(tempo_reload_cpt_local & 0xFF);	// Timer 3 reload value Low  (1562d for 10ms)
			// In Midi clock mode : start timer
			T3CTL = 0xB9;	// Timer 3 continuous, started, divide per 128
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	delay_usec()
// Inputs	: 	delay_time in usec
// Outputs	: 	None
// Process  : 	generic delay function in usec
//				1 cycle is 50ns @ 20 MHz
//				1 instruction is 2 cycles = 100 ns
///////////////////////////////////////////////////////////////////////////////////////
void delay_usec(uchar delay_time)
{
#ifndef SIMUL
	uchar i;
	for(i=0;i<delay_time;i++)		// i x 1usec delay
	{
		asm("nop"); asm("nop");		// This is the 1usec delay
		asm("nop"); asm("nop");
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: delay_msec()
// Inputs	: delay_time in msec
// Outputs	: None
// Process  : generic delay function in msec
///////////////////////////////////////////////////////////////////////////////////////
void delay_msec(unsigned int delay_time)
{
#ifndef SIMUL
	unsigned int i;
	uchar j;

	for(i=0;i<delay_time;i++)		// i x 1usec delay
	  for(j=0;j<4;j++)				// this is the 1 msec delay
	  	delay_usec(250);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckParamChangedFromItr()
// Inputs	: None
// Outputs	: None
// Process  : Check if some params have to be updated from ITRs
///////////////////////////////////////////////////////////////////////////////////////
void CheckParamChangedFromItr(void)
{
	uchar 		old_group = current_group;
	uchar 		index;
	far uchar 	*ptr_far;
	uchar 	  	*ptr;	
	uchar 		old_automation;	
	uchar 		new_automation;	
	uchar 	  	step = 1;
	uchar 		shift_changed = FALSE;
	uchar		tmp; // v1.1
	
	zone_to_reinit = 0L; // V1.62

	////////////////////////////////////////////////////////////////////
	/////// CHECK IF MUST REINIT LCD ZONES FROM INCOMING SYSEX (COMMON PARAM)
	// Need to update LCD after a common param programming, not all zone reinitialized since
    // lcd zone incrust is reinitialized for common param result upload/download
	if(must_reinit_lcd_from_sysex)
	{
		// V1.62 UpdateLcdZones( (uint)(must_reinit_lcd_from_sysex) );
		zone_to_reinit = (uint) must_reinit_lcd_from_sysex;
		must_reinit_lcd_from_sysex = 0;
	}

	////////////////////////////////////////// CHECK IF SHIFT HAS CHANGED
	/////////////////////////////////////////////////////////////////////
	// V1.62 : SHIFT can be push or toggle mode
	if(Z8_STATUS_0_NVM & shift_toggle_mode_z0nvm)
	{
		// Shift is pressed
		if(SHIFT_PORT_IN & SHIFT_PIN)			
		{
			// does the button passed through zero ?
			if(Z8_STATUS_10 & shift_passed_0_z10)
			{
				Z8_STATUS_10 &= ~shift_passed_0_z10;
				shift_changed = TRUE;
			}
		}
		else
			Z8_STATUS_10 |= shift_passed_0_z10;
	}
	else
	{
		if(SHIFT_PORT_IN & SHIFT_PIN)	
		{
			if(!(Z8_STATUS_9 & shift_state_z9))
				shift_changed = TRUE;
		}
		else
		{
			if(Z8_STATUS_9 & shift_state_z9)
				shift_changed = TRUE;
		}
	}

	// shift has changed : need to unhook the automation pots that works with shift
	if(shift_changed)
	{
		if(current_autom == AUTOMATION_ARPP)
		{
			// shift was active
			if(Z8_STATUS_9 & shift_state_z9)
			{
				old_automation = AUTOMATION_ARPP_SHIFT;
				new_automation = AUTOMATION_ARPP;
			}
			else
			{
				old_automation = AUTOMATION_ARPP;
				new_automation = AUTOMATION_ARPP_SHIFT;
			}

			// the selected automation has changed : UN-HOOK all 3 automation pots ()
			UnhookAutomationPots(old_automation, new_automation);
		}

		// toggle shift state
		Z8_STATUS_9 ^= shift_state_z9;

		// V1.4 display / delete Small SHIFT bitmap
		// V1.5 display/delete shift bitmap only if lcd available
		if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
		{
			if(Z8_STATUS_9 & shift_state_z9)
				DisplayBmp(LCD_ZONE_SHIFT_BMP_X, LCD_ZONE_SHIFT_BMP_Y, LCD_INVERT, shift_bmp);
			else
				FillLcdZone(LCD_ZONE_SHIFT_BMP_X, LCD_ZONE_SHIFT_BMP_Y, 3, 2, FILL_BLANK);
		}
	
		// V1.5 : shift button is used to get a LCD screencopy via sysexs
#ifdef LCD_SCREEN_COPY
		SendLcdScreenCopyToSysexs();
#endif

		// if we are in test mode then update the dedicated register
		if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
			// only when shift becomes active
			if(Z8_STATUS_9 & shift_state_z9)
				non_assign_sw_test_mode |= TEST_MODE_SHIFT_BIT_MASK;
		
		// V1.5 : In mackie mode the shift button must send a MIDI Note event
		// Mackie mode ?
		//V1.67 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS == MACKIE_MODE)
		//if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) == MACKIE_MODE_TEST_POS)
		if( (OPERATING_MODE_NVM == MACKIE_MODE) || 
			(OPERATING_MODE_NVM == LOGIC_MODE))
		{
			// shift pressed send note 7F, shift released send note 0
			uchar note_value = 0;
			if(Z8_STATUS_9 & shift_state_z9) note_value = 0x7F;
			SendMackieShortEvent(MIDI_NOTE_ON, MACKIE_SHIFT_ID, note_value);
		}

	} // end shift changed

	// if shift active Then all function dec/incr step is 10
	if(Z8_STATUS_9 & shift_state_z9)	
		step = SHIFT_STEP_SIZE;		

	////////////////////////////// CHECK IF SWITCHES ARE PASSED THROUGH 0
	/////////////////////////////////////////////////////////////////////
	// treat only 1/8 time statistically this swicth = debounce  
	if(!index_analog_itr)
	{
		if(!(UP_SW_PORT_IN & UP_SW_PIN))
			Z8_STATUS_7 |= up_passed_0_z7;
		if(!(DOWN_SW_PORT_IN & DOWN_SW_PIN))
			Z8_STATUS_7 |= down_passed_0_z7;
		if(!(ENCODER_SW_PORT_IN & ENCODER_SW_PIN))
			Z8_STATUS_7 |= encoder_sw_passed_0_z7;
	//	if(!(EDIT_PORT_IN & EDIT_PIN))
	//		 Z8_STATUS_7 |= edit_passed_0_z7;
	//	if(!(MODE_PORT_IN & MODE_PIN))
	//		 Z8_STATUS_7 |= mode_passed_0_z7;
	}

	//////////////////////////////// CHECK IF MIDI CHANNEL CHANGED
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_0 & must_change_channel_z0)
	{
		Z8_STATUS_0 &= (~must_change_channel_z0);
		// Update channel value according to its direction
		if(Z8_STATUS_0 & must_increase_channel_z0)
			current_channel += step;
		else
			current_channel -= step;

		current_channel &= (CHANNEL_NB-1);

		// V1.7 : In mackie mode the midi channel button must send a MIDI Note event (Fader bank change)
		// Mackie mode ?
		if( (OPERATING_MODE_NVM == MACKIE_MODE) || 
			(OPERATING_MODE_NVM == LOGIC_MODE))
		{
			// Send a mackie fader bank change Left or Right
			uchar id_value = MACKIE_FADER_BNK_LEFT_ID; 
			if(Z8_STATUS_0 & must_increase_channel_z0)
			 id_value = MACKIE_FADER_BNK_RIGHT_ID;
			// Send a mackie note on
			SendMackieShortEvent(MIDI_NOTE_ON, id_value, 0x7F);
			// Send a mackie note off
			SendMackieShortEvent(MIDI_NOTE_ON, id_value, 0x00);
		}


		// Update LCD channel value
		// DisplayLcdInt(LCD_ZONE_CHANNEL_X, LCD_ZONE_CHANNEL_Y, DIGITS_2|LCD_INVERT, font_num_8x16, current_channel );
		// V1.62 UpdateLcdZones( INIT_LCD_CHANNEL );
		zone_to_reinit |= (uint) INIT_LCD_CHANNEL;
	}

	// V1.8
	//////////////////////////////////// CHECK MUST INCREASE SMPTE TIME
	///////////////////////////////////////////////////////////////////
//	if(Z8_STATUS_11 & must_inc_1_smpte_frame_z11)
//	{
//		// Increase smpte time by 4 frames and display
//		IncreaseDisplaySmpteTime(4);
//		// Send a full smpte frame
//		smpte_lock_cpt = 0;
//		SendSmpteToMidi(FULL_SMPTE);
//		// reset flag
//		Z8_STATUS_11 &= ~must_inc_1_smpte_frame_z11;
//	}

	// V1.8	
	//////////////////////////////////// CHECK MUST DECREASE SMPTE TIME
	///////////////////////////////////////////////////////////////////
//	if(Z8_STATUS_11 & must_dec_1_smpte_frame_z11)
//	{
//		T2CTL = 0x19;
//		// Decrease smpte time by 1 frame and display
//		DecreaseDisplaySmpteTime(4);
//		// Send a full smpte frame
//		smpte_lock_cpt = 0;
//		SendSmpteToMidi(FULL_SMPTE);
//		smpte_lock_cpt = SMPTE_LOCK_TIME;
//		// reset flag
//		Z8_STATUS_11 &= ~must_dec_1_smpte_frame_z11;
//		if(transport_state == TRANSPORT_PLAY)
//		 	T2CTL = 0x99;
//	}


	/////////////////////////////////////// CHECK IF GROUP CHANGED
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_3 & must_change_group_z3)
	{
		Z8_STATUS_3 &= (~must_change_group_z3);
		// Update channel value according to its direction
		if(Z8_STATUS_3 & must_increase_group_z3)
		{
			tmp = 1;
			current_group += step;
			if(current_group >= GROUP_NB)
				current_group -= GROUP_NB;
		}
		else
		{
			tmp = 0;
			if(current_group < step)
				current_group = GROUP_NB-step;
		    else
				current_group -= step;
		}

/* Block removed in V1.7 : now bank changes are done using the MIDI channel buttons
		// V1.5 : In mackie mode the group button must send a MIDI Note event (Fader bank change)
		// Mackie mode ?
		if( (OPERATING_MODE_NVM == MACKIE_MODE) || 
			(OPERATING_MODE_NVM == LOGIC_MODE))
		{
			// Send a mackie fader bank change Left or Right
			uchar id_value = MACKIE_FADER_BNK_LEFT_ID; 
			if(Z8_STATUS_3 & must_increase_group_z3)
			 id_value = MACKIE_FADER_BNK_RIGHT_ID;
			// Send a mackie note on
			SendMackieShortEvent(MIDI_NOTE_ON, id_value, 0x7F);
			// Send a mackie note off
			SendMackieShortEvent(MIDI_NOTE_ON, id_value, 0x00);
		}
*/

		//else // V1.5 : In mackie mode : Hook is managed by the host
		// if analog is HOOK mode, initialize new hooked pots
// V1.8		if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm)
		{
			// Force hook fader state to be re-displayed if required
			Z8_STATUS_4 |= force_hook_fader_display_z4;

			// Now save current controls to physical data
			ptr		= &Ctrl.Ctrl_sc.CTRL_STRUCT_START_FIELD;	
			// ignore all 16 switches since treated seperately
			for(index=0; index < CTRL_STRUCT_BASE_SW_INDEX; index++)
			{			
				ptr_far = &Physical[old_group].CTRL_STRUCT_START_FIELD;
				// store physical positions to physical regs if already hooked
				if(ptr[index] & 0x80)
				   ptr_far[index] = ptr[index] & 0x7F;
				// now update new group sign and store it to physical registers 
				ptr_far = &Physical[current_group].CTRL_STRUCT_START_FIELD;
				if( (ptr[index] & 0x7F) <= (ptr_far[index] & 0x7F) )
					ptr_far[index] &= 0x7F;
				else
					ptr_far[index] |= 0x80;
				// un-hook all pots
				ptr[index] &= (~HOOK_BIT);
			}
		}

		// Reinit all 16 switches toggle/push modes from external eeprom
		GetCurrentGroupSwitchModesFromEE();
		// Reload new other_sw_value from next group
		Physical[old_group].other_sw_value = Ctrl.Ctrl_sc.other_sw_value;
		Ctrl.Ctrl_sc.other_sw_value = Physical[current_group].other_sw_value;
		// Reload new all_sw_value from next group
		Physical[old_group].all_sw_value = Ctrl.Ctrl_sc.all_sw_value;
		Ctrl.Ctrl_sc.all_sw_value = Physical[current_group].all_sw_value;

		// Update LCD group value, and new switches states
		// V1.62 UpdateLcdZones( INIT_LCD_GROUP | INIT_LCD_SW_STATE);
		zone_to_reinit |= (uint)(INIT_LCD_GROUP | INIT_LCD_SW_STATE);

		// V1.1 Send a sysex indicating the group has changed when in Reason mode
		// V1.5 Are we in standard or Reason mode ?
		// V1.69 if(!(Z8_STATUS_2_NVM & operating_mode1_z2nvm))
			// V1.5 yes : are we in Reason mode ?
			// V1.69 if(Z8_STATUS_2_NVM & operating_mode0_z2nvm)
			if(OPERATING_MODE_NVM == REASON_MODE) 
				SendUploadGenericBufferToMidi(prog_upload_sysxid0, 1, GROUP_INDEX, tmp);

		// group changed, this will force to update lcd str, and reload midi structure
		last_control_index = CTRL_UNDEF;

		InitParamStructsInRam();

		// V1.6 : when group changes : must inform all expansions
		ConfigureAllExpansions();
		
	} // end must change group

	//////////////////////////////// CHECK IF SCENE SELECT CHANGED
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_0 & must_change_scene_z0)
	{
		Z8_STATUS_0 &= (~must_change_scene_z0);
		// Update scene value according to its direction
		if(Z8_STATUS_0 & must_increase_scene_z0)
		{
			current_scene += step;
			if(current_scene >= SCENE_NB)
				current_scene -= SCENE_NB;
		}
		else
		{
			if(current_scene < step)
				current_scene = SCENE_NB - step;
		    else
				current_scene -= step;
		}

		// Update LCD scene value
		// DisplayLcdInt(LCD_ZONE_SCENE_X, LCD_ZONE_SCENE_Y, DIGITS_2|LCD_INVERT, font_num_8x16, current_scene );
		// V1.62 UpdateLcdZones( INIT_LCD_SCENE );
		zone_to_reinit |= (uint) INIT_LCD_SCENE;
	}

	//////////////////////////////// CHECK IF SCENE ARPP CHANGED 
	//////////////////////////////////////////////////////////////
	// new in V1.8
	if(Z8_STATUS_11 & must_change_scene_arp_z11)
	{
		Z8_STATUS_11 &= (~must_change_scene_arp_z11);
		// Update scene arp value according to its direction
		if(Z8_STATUS_11 & must_increase_scene_z0)
		{
			current_scene_arp += step;
			if(current_scene_arp >= SCENE_ARP_NB)
				current_scene_arp -= SCENE_ARP_NB;
		}
		else
		{
			if(current_scene_arp < step)
				current_scene_arp = SCENE_ARP_NB - step;
		    else
				current_scene_arp -= step;
		}

		// Update LCD scene arp value
		zone_to_reinit |= (uint) INIT_LCD_SCENE;
	}

	//////////////////////////////// CHECK IF INFRARED BUTTON CHANGED
	/////////////////////////////////////////////////////////////////
	if(Z8_STATUS_3 & ir_sw_changed_z3)
	{
		// Infrared on/off or infrared hold value ?
		if(Z8_STATUS_3 & onoff_zhold_infrared_z3)
		{
			// here must toggle infrared state
			Z8_STATUS_1_NVM ^= infrared_state_z1nvm;

			// Update lcd with new state
			// V1.62 UpdateLcdZones( INIT_LCD_INFRARED );
			zone_to_reinit |= (uint) INIT_LCD_INFRARED;
		}
		else
		// Hold infarred only if on
		if(Z8_STATUS_1_NVM & infrared_state_z1nvm)
		{
			// here must hold infrared value
			Ctrl.Ctrl_sc.infrared_hold = infrared;
			infrared_hold_save		   = infrared;

			// infrared hold pressed, lock until it returns to zero
			Z8_STATUS_3 |= infrared_lock_midi_z3;
			Z8_STATUS_4 |= wait_ir_passed_zero_z4;

			// Following display only if no menu active
			if(menu_state == NO_MENU)
			{
				// If crossfader area already active then do not display it again
				if(lcd_current_incrust != LCD_INCRUST_INFRARED_HOLD)
				{
					// Display incrust zone
					//DisplayBmp(LCD_ZONE_INCRUST_X, LCD_ZONE_INCRUST_Y, LCD_INVERT, bmp_incrust);
					// Display incrust title
					if(identity[HW_VERSION_INDEX] < 2)
						rom_to_lcd_text_copy(infrared_txt);
					else
						rom_to_lcd_text_copy(ribbon_txt);
					//DisplayLcdText(LCD_ZONE_INCRUST_X+6, LCD_ZONE_INCRUST_Y, LCD_NORMAL, font_5x8);	
					DisplayEmptyIncrust();
					// display Hold to text
					rom_to_lcd_text_copy(hold_to_txt);
					DisplayLcdText(LCD_ZONE_INCRUST_X+10, LCD_ZONE_INCRUST_Y+3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );
				}
	
				// Dislpay infared hold value
				DisplayLcdInt(LCD_ZONE_INCRUST_X+60, LCD_ZONE_INCRUST_Y+2, DIGITS_3|LCD_INVERT|FONT_NUM_8x16_FLG/* V1.66 , font_num_8x16 */ , Ctrl.Ctrl_sc.infrared_hold );
	
				// infrared hold area is active now
				lcd_current_incrust = LCD_INCRUST_INFRARED_HOLD;
		
				// Lock LCD incrust zone for 2 seconds
				lcd_lock_incrust_cpt = CPT_2_SEC;
	
				// Infrared locked until pass through 0 : show down hokk direction
				DisplayBmp(LCD_ZONE_HOOK_DIRECTION_X, LCD_ZONE_HOOK_DIRECTION_Y, LCD_INVERT, bmp_lock_down);
			}

		} // end infrared hold value changed

		// reinit counter and flags
		ir_sw_long_push_cpt = 0;	
		Z8_STATUS_3 &= (~ir_sw_changed_z3);

		// update non assignable test mode register if required
		if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
			non_assign_sw_test_mode |= TEST_MODE_HOLD_BIT_MASK;

	}

	//////////////////////////////////// CHECK IF VIDEO MODE CHANGED
	////////////////////////////////////////////////////////////////
	// Update new video mode only if not in menu
	if(menu_state == NO_MENU)
	{
		// V1.3 if(Z8_STATUS_0_NVM & video_mode_z0nvm)
		if(Z8_STATUS_2_NVM & video_mode_z2nvm)
		{
			if(!(Z8_STATUS_2_NVM & new_video_mode_z2nvm))
			{
				//V1.3 Z8_STATUS_0_NVM &= (~video_mode_z0nvm);
				Z8_STATUS_2_NVM &= (~video_mode_z2nvm);
				// V1.3 update full LCD since video mode changed
				Z8_STATUS_7 |= must_reinit_all_lcd_z7;
			}
		}
		else
		{
			if(Z8_STATUS_2_NVM & new_video_mode_z2nvm)
			{
				//V1.3 Z8_STATUS_0_NVM |= video_mode_z0nvm;
				Z8_STATUS_2_NVM |= video_mode_z2nvm;
				// V1.3 update full LCD since video mode changed
				Z8_STATUS_7 |= must_reinit_all_lcd_z7;
			}
		}
	}

	//////////////////////////////////// CHECK IF LCD FULL RE-INIT
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_7 & must_reinit_all_lcd_z7)
	{
		// no reinit after this one
		Z8_STATUS_7 &= (~must_reinit_all_lcd_z7);
		// lcd no more locked by menus
		Z8_STATUS_4 &= (~lcd_locked_by_function_z4);
		// be sure hook bmp is default to no hook
		current_hook_bmp = NO_HOOK_BMP;
		// be sure to re-update curve model
		current_curve_model = CTRL_UNDEF;
		// V1.62 be sure to update midi channel
		last_displayed_channel = CTRL_UNDEF; // V1.62
		// be sure to reupdate transports from current active
		if(transport_state == TRANSPORT_PAUSE)	
		{
			// be sure to reupdate pause bitmap
			next_transport_state = TRANSPORT_STOP;
			transport_state = TRANSPORT_PLAY;	
		}
		else 
		{
			next_transport_state = transport_state;
			transport_state = CTRL_UNDEF; 
		}
		// Clear display
		FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);
		// Reinit LCD normal mode
		zone_to_reinit |= (uint)(
						INIT_LCD_CHANNEL 		|	INIT_LCD_EXPANSION	|	INIT_LCD_FUNCTION		|
						INIT_LCD_SCENE			|	INIT_LCD_GROUP		|	INIT_LCD_AUTOMATION_TXT | // V1.8 INIT_LCD_FUNCTION will call INIT_LCD_SCENE
						INIT_LCD_LFO_WAVE_STATE	|	INIT_LCD_ARPP_STATE |	INIT_LCD_SYNC24     	|
						INIT_LCD_MOTION_STATE	|	INIT_LCD_SW_STATE 	|	INIT_LCD_INFRARED 		|
						INIT_LCD_USB			|	INIT_LCD_TRANSPORT 	|	INIT_LCD_IR_GRAPH		|
						INIT_LCD_MODE);
/* // V1.62 
		UpdateLcdZones( (uint)(
						INIT_LCD_CHANNEL 		|	INIT_LCD_EXPANSION	|	INIT_LCD_FUNCTION		|
						INIT_LCD_SCENE			|	INIT_LCD_GROUP		|	INIT_LCD_AUTOMATION_TXT |
						INIT_LCD_LFO_WAVE_STATE	|	INIT_LCD_ARPP_STATE |	INIT_LCD_SYNC24     	|
						INIT_LCD_MOTION_STATE	|	INIT_LCD_SW_STATE 	|	INIT_LCD_INFRARED 		|
						INIT_LCD_USB			|	INIT_LCD_TRANSPORT 	|	INIT_LCD_IR_GRAPH		|
						INIT_LCD_MODE	
						));
*/
		// V1.8 : be sure to update scene arp/snapshot
		DisplaySceneTypeBitmap();
		// Also reinit 3 digits value
		//DisplayLcdInt(LCD_ZONE_VALUE_X, LCD_ZONE_VALUE_Y, DIGITS_3|LCD_INVERT, font_num_8x16, 0 );
		DisplayCurrentValue();
		// Also display infrared bargraph and hold values
		UpdateLCDInfraredBargraph();
		lcd_current_incrust = LCD_NO_INCRUST;
	}

	///////////////////////////////// CHECK IF LCD INCRUST RE-INIT
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_3 & must_reinit_incrust_lcd_z3)
	{
		// in any case, joystick area is no more active
		//Z8_STATUS_3 &= (~joystick_lcd_area_active);
		lcd_current_incrust = LCD_NO_INCRUST;
		Z8_STATUS_3 &= (~must_reinit_incrust_lcd_z3);
		// reinit incrust zone only if not in menus mode
		if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
		{
			// Clear display over incrust zone only
			FillLcdZone(LCD_ZONE_INCRUST_X, LCD_ZONE_INCRUST_Y, LCD_ZONE_INCRUST_X_PIX, LCD_ZONE_INCRUST_Y_PAGE, FILL_BLANK);
			// Reinit LCD normal mode
			zone_to_reinit |= (uint)(INIT_LCD_GROUP				|	INIT_LCD_AUTOMATION_TXT	|	INIT_LCD_FUNCTION		|
									 INIT_LCD_LFO_WAVE_STATE	|	INIT_LCD_ARPP_STATE		|	INIT_LCD_MOTION_STATE	|	
									 INIT_LCD_SW_STATE	);
/* // V1.62 
			UpdateLcdZones( (uint)(
							INIT_LCD_GROUP			|	INIT_LCD_AUTOMATION_TXT	|	INIT_LCD_FUNCTION		|
							INIT_LCD_LFO_WAVE_STATE	|	INIT_LCD_ARPP_STATE		|	INIT_LCD_MOTION_STATE	|	
							INIT_LCD_SW_STATE
							));
*/
		}
	}

	//////////////////////////////////////// CHECK IF MODE CHANGED
	//////////////////////////////////////////////////////////////
	// treat only 1/8 time statistically this swicth = debounce  
	if(!index_analog_itr)
	{
		if(MODE_PORT_IN & MODE_PIN)
		{
			if(Z8_STATUS_7 & mode_passed_0_z7)
			{
				Z8_STATUS_4 |= mode_changed_z4;
				Z8_STATUS_7 &= ~mode_passed_0_z7;	
			}
		}
		else Z8_STATUS_7 |= mode_passed_0_z7;
	
		// mode changed ?
		if(Z8_STATUS_4 & mode_changed_z4)
		{
			Z8_STATUS_4 &= (~mode_changed_z4);
			// V1.69 tmp = (Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS;
			
			// V1.5 test mode : update non assignable test mode register if required
			if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
			{
				non_assign_sw_test_mode |= TEST_MODE_MODE_BIT_MASK;
				// V1.5 In test mode, always select Standard mode
				tmp = 0;
			}
			else
			{
				// update mode
				tmp = (OPERATING_MODE_NVM /* & OPERATING_MODE_MSK_Z3_NVM */) + 1U;
				// update mode from 0 to 4
				if(tmp >= OPERATING_MODE_NB)
					tmp = 0;
			}
			
			// now update new mode
			// V1.69 Z8_STATUS_2_NVM &= ~OPERATING_MODE_MSK_Z2_NVM;
			// V1.69 Z8_STATUS_2_NVM |= (tmp << OPERATING_MODE_MSK_POS);
		/*	OPERATING_MODE_NVM &= ~OPERATING_MODE_MSK_Z3_NVM;
			OPERATING_MODE_NVM |= tmp ;
		*/
			OPERATING_MODE_NVM = tmp ;

			// V1.7 : make some stuff if the mode has changed
			UpdateOperatingMode();

			// Be sure the mode on lcd will be updated
			zone_to_reinit |= (uint)(INIT_LCD_MODE);

			// be sure to re-update curve model
			current_curve_model = CTRL_UNDEF;
			UpdateLcdCurveModel(current_curve_model);

			/* Block removed in V1.7 : replaced by the above function
			// V1.5 1.69 Enter mackie/logic mode if mackie control is the new selected mode
			if(tmp == LOGIC_MODE)
			{
				mackie_logic_sysex_id = MACKIE_SYX_ID3_LOGIC;
				EnterMackieMode();
			}
			else
			if(tmp == MACKIE_MODE)
			{
				mackie_logic_sysex_id = MACKIE_SYX_ID3;				
				EnterMackieMode();
			}
			else
			if( (tmp == USER_MODE) || (tmp == STANDARD_MODE))
				// preceding mode was Mackie control or logic control
				LeaveMackieMode();

			// mode changed, this will force to update lcd str, and reload midi structure
			last_control_index = CTRL_UNDEF;
	
			// V1.61 reinit all param and lcd structs already preloaded in RAM
			InitParamStructsInRam();
		*/
		}
	}

	////////////////////////// CHECK IF SERIAL IN OVERFLOW OCCURED
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_8 & serial_in_overflow_z8)
	{
		DisplayInputOverflowIncrust();
		Z8_STATUS_8 &= (~serial_in_overflow_z8);
	}

	/////////////////////////// CHECK IF MUST UPDATE SMALL LCD LFO
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_5 & must_update_lcd_small_lfo_z5)
	{
		// V1.62 UpdateLcdZones( INIT_LCD_LFO_WAVE_STATE );
		zone_to_reinit |= (uint)(INIT_LCD_LFO_WAVE_STATE);
		Z8_STATUS_5 &= (~must_update_lcd_small_lfo_z5);
	}
	
	/////////////////////// CHECK IF MUST DECREASE MACKIE BARGRAPH
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_9 & must_decrease_mackie_bargraph_z9)
	{
		// Update Mackie bargraph
		UpdateMackieBargraphDecay();

		Z8_STATUS_9 &= (~must_decrease_mackie_bargraph_z9);
		// reload timeout counter if mackie mode active
		//V1.67 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS == MACKIE_MODE)
		// if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) == MACKIE_MODE_TEST_POS)
		if( (OPERATING_MODE_NVM == MACKIE_MODE) || 
			(OPERATING_MODE_NVM == LOGIC_MODE))
			mackie_bargraph_timeout = MACKIE_BARGRAPH_DECAY;
	}

	///////// IN MACKIE MODE : JOGWHEEL SENDS EVENTS IF ENCODER SW IS PRESSED
	/////////////////////////////////////////////////////////////////////////
	// V1.5 : if encoder sw is active and jogwheel tweaked, then send a midi event
	if(Z8_STATUS_10 & must_send_mackie_encoder_z10)
	{
		Z8_STATUS_10 &= ~must_send_mackie_encoder_z10;
		if(Z8_STATUS_10 & mackie_encoder_left_zright_z10)
			SendMackieShortEvent(MIDI_CONTROL_CHANGE, MACKIE_JOG_WHEEL_ID, JOG_WHEEL_LEFT);
		else
			SendMackieShortEvent(MIDI_CONTROL_CHANGE, MACKIE_JOG_WHEEL_ID, JOG_WHEEL_RIGHT);
	}

	///////// RESET BS3X MIDI CHANNEL IF IT WAS OVERRIDED BY A CONTROL WITH A FIXED CHANNEL
	/////////////////////////////////////////////////////////////////////////
	// V1.62 : 
	if(Z8_STATUS_6 & must_reinit_channel_lcd_z6)
	{
		// V1.62 UpdateLcdZones(INIT_LCD_CHANNEL); 		
		zone_to_reinit |= (uint)(INIT_LCD_CHANNEL);
		Z8_STATUS_6 &= ~must_reinit_channel_lcd_z6;
	}

	// V1.62 reinit LCD zones only one time in this function
	if((uint)zone_to_reinit)
		UpdateLcdZones((uint)zone_to_reinit); 

	// V1.69
	///////// CHECK IF TEMPO ZONE MUST BE UPDATED WITH NEW TEMPO VALUE
	/////////////////////////////////////////////////////////////////////////
	if(Z8_STATUS_10 & tempo_changed_z10)
	{
//		tempo_reload_cpt = (uint)(390625L / (uint)(internal_tempo + 30)); // V1.8
//		Z8_STATUS_11= must_reload_tempo_timer_z11; // V1.8
		DisplayTempo();
		// no more tempo to display
		Z8_STATUS_10 &= ~tempo_changed_z10;
	}

}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitInitialValues()
// Inputs	: None
// Outputs	: None
// Process  : Initialize all initial values 
///////////////////////////////////////////////////////////////////////////////////////
void InitInitialValues(void)
{
	uchar index;

	// Must be initialized in the same way of EZ board 
	CTRL_REG_7BITS_FROM_EZ[0] = 0;

	must_reinit_lcd_from_sysex = 0;
	lcd_lock_incrust_cpt = 0; // V1.62

	// adc indexes initial values
	row_analog_itr	 		= ROW0;
	index_analog_itr 		= 0;
	current_analog_index	= 0;

	// realtime buffers indexes
	realtime_generated_start_index	= 0;
	realtime_generated_end_index	= 0;
	realtime_received_start_index	= 0;
	realtime_received_end_index 	= 0;

	I2cCommand	= BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

	// Initialize all other switches values 
	EE_Address = (uint)OTHER_SWITCHES_EE_ADR;
	Ctrl.Ctrl_sc.other_sw_value = ReadByteI2c();
	other_sw_EE_img = Ctrl.Ctrl_sc.other_sw_value;

	// Initialize all switches values 
	EE_Address = (uint)ALL_SWITCHES_EE_ADR;
	Ctrl.Ctrl_sc.all_sw_value = ReadByteI2c();
	all_sw_value_EE_img = Ctrl.Ctrl_sc.all_sw_value;

	// Load user defined curves models from external eeprom at startup
	// need to read 128 bytes curves as 2 blocks of 64 bytes (eeprom page size)
	EE_Address = (uint)CURVE_USER_0_LOW_START_EE_ADR;
	ReadMultiByteI2c((far uchar *)&CURVE_USER_0_TABLE[0], EEPROM_PAGE_SIZE);
	EE_Address = (uint)CURVE_USER_0_HIGH_START_EE_ADR;
	ReadMultiByteI2c((far uchar *)&CURVE_USER_0_TABLE[EEPROM_PAGE_SIZE], EEPROM_PAGE_SIZE);
	// same for user curve 1
	EE_Address = (uint)CURVE_USER_1_LOW_START_EE_ADR;
	ReadMultiByteI2c((far uchar *)&CURVE_USER_1_TABLE[0], EEPROM_PAGE_SIZE);
	EE_Address = (uint)CURVE_USER_1_HIGH_START_EE_ADR;
	ReadMultiByteI2c((far uchar *)&CURVE_USER_1_TABLE[EEPROM_PAGE_SIZE], EEPROM_PAGE_SIZE);

	// read all control registers in a single bloc read, skip ctrl_reg[0] which is identity
	EE_Address = (uint)CTRL_REG_START_EE_ADR;
	ReadMultiByteI2c(&CtrlReg[0], CTRL_REG_REAL_NB);

	// V1.5 be sure to avoid booting in test mode since this flag is saved to external eeprom (should not be)
	CTRL_REG_1_7BITS_TO_EZ &= ~autotest_mode_z_ez1;

	// V1.5 : If Mackie mode then pass to standard mode (mackie mode must be initiated from mode button
	// V1.69 index = (Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS;
		// V1.69 if(index == MACKIE_MODE)
			// V1.69 Z8_STATUS_2_NVM &= ~OPERATING_MODE_MSK_Z2_NVM;

	// Now define limits of the different control registers	read
	// Initialize current active function 
//	next_function 	  = current_function;
	if(next_function >= FCT_UNDEFINED)
	   next_function  = FCT_CHANNEL; 
	current_function  = FCT_UNDEFINED;

    // Initialize midi channel 
	current_channel &= (CHANNEL_NB-1);

	// Initialize group 
	if(current_group >= GROUP_NB)
   	   current_group = 0;

	// Initialize scene 
	if(current_scene >= SCENE_NB)
		current_scene = 0;

	// Initialize scene arp V1.8
	if(current_scene_arp >= SCENE_ARP_NB)
		current_scene_arp = 0;

	// V1.69
	index = OPERATING_MODE_NVM /* & OPERATING_MODE_MSK_Z3_NVM */;
	// if EEPROM is not properly initialized, select standard mode also select std mode if mackie or logic modes)
	// (mackie and logic modes must be initiated from mode button)
	if( (index >= OPERATING_MODE_NB) || (index == LOGIC_MODE) || (index == MACKIE_MODE))
	{
		// OPERATING_MODE_NVM &= ~OPERATING_MODE_MSK_Z3_NVM;
		OPERATING_MODE_NVM = 0;

	}

	// Initialize infrared hold value to 0 
	Ctrl.Ctrl_sc.infrared_hold = infrared_hold_save & 0x7F;

	// Initialize current automation selected
	if(current_autom >= AUTOMATION_NB)
		current_autom = 0;

	// Now all controls regs are defined and clean also assign control regs to control regs ee img
	for(index=0; index<CTRL_REG_REAL_NB; index++)
		CtrlReg_EE_img[index] = CtrlReg[index];

/*
	// calibrate adc offset is not calibrated yet
	if(!((ADC_Offset > 122) && (ADC_Offset < 132)))
		 CalibrateAdc();
*/
	// initialize LFO counter is default LFO is on
	if(Z8_STATUS_1_NVM & lfo_state_z1nvm)
		ReloadLfoFreqCpt();

	// Initialize control index and last control index 
	control_index 		= 0;
	last_control_index 	= CTRL_UNDEF;

	// no incrust on LCD at beginning
	lcd_current_incrust = LCD_NO_INCRUST;

	// Initialize transport 
	transport_state 		= TRANSPORT_STOP;
//	next_transport_state	= CTRL_UNDEF;

	// Initialize automation leds states
	UpdateAutomationLeds();

	// Initialize 5 seconds timer
	seconds_5_timer_cpt = CPT_5_SEC;

	// first pass flag is on at the begining
	Z8_STATUS_4  = first_pass_z4;
	Z8_STATUS_5  = 0;
	Z8_STATUS_6  = 0;
	Z8_STATUS_7  = 0xFF; // all sw passed through 0 + reinit whole lcd;
	Z8_STATUS_8  = ok_passed_0_z8 | cancel_passed_0_z8;
	Z8_STATUS_11 = 0; // V1.8

	TX_STATUS			= 0;
	tx0_start_index    	= 0;
	tx0_end_index   	= 0;
	tx1_start_index    	= 0;
	tx1_end_index   	= 0;

	// Initialize all 16 switches push/toggle from ext eeprom
    GetCurrentGroupSwitchModesFromEE();

	// Init menus 
	menu_state = NO_MENU;
	assign_state = ASSIGN_OFF;

	// Init arpeggio
	InitArpeggiator();

	// Read all 8 motion parts
	// motion are located in first eeprom (EEPROM_ZERO_I2C_ADR)
	I2cCommand 	= BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
	
	EE_Address	= (uint)MOTION_LENGHT_AND_INDEX_0_EE_ADR;
	for(index=0 ; index<MOTION_PART_NB ; index++)
	{
		ReadMultiByteI2c((far uchar *)&Motion_Str[index].motion_ctrl_index, MOTION_LENGHT_AND_INDEX_SIZE);
		EE_Address 	+= (uint)MOTION_LENGHT_AND_INDEX_SIZE;
	}

	EE_Address	= MOTION_DATA_PART_0_EE_ADR;
	for(index=0 ; index<MOTION_PART_NB ; index++)
	{
		ReadMultiByteI2c((far uchar *)&Motion_Str[index].motion_data_table[0], motion_point_nb[Motion_Str[index].motion_length]);
		EE_Address 	+= (uint)MOTION_PART_LENGHT;
	}

	// V1.6 : not sending scene
	index_scene = 0;
	index_scene_dig = 0;

	// V1.6 Initialize expansion config registers : config_reg_0 bit 6 (must reset is initialy set to zero)
//	exp_cfg_reg[0] = current_group;
	// if analog is HOOK mode
//	if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm)
//		exp_cfg_reg[0] |= hook_status_to_exp;

	// V1.61
	ParamCurrent = &Param[0];
	last_struct_index = 0;	// V1.61
	last_lcd_struct_index = 0; // V1.61
	InitParamStructsInRam();

	// V1.6
	old_joy_x_pixel 			= 30;
	old_joy_y_page_start  		= 3;
	old_joy_y_page_end			= 3;
	old_fader_x_pixel			= 60;
	old_fader_hooked_x_pixel	= 60;
	current_curve_model 		= CTRL_UNDEF;

	last_displayed_channel 		= CTRL_UNDEF; 
	override_channel_cpt		= 0;		// V1.62

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckCurrentActiveFunction()
// Inputs	: None
// Outputs	: None
// Process  : Check which function is active (group/scene/channel)
///////////////////////////////////////////////////////////////////////////////////////
void CheckCurrentActiveFunction(void)
{
	// V1.4 ***** TREAT THESE 3 SWITCHES IN AUTO TEST MODE ******
	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
	{
		// CHANNEL switch pressed ?
		if(CHANNEL_PORT_IN & CHANNEL_PIN)
		{
			if(Z8_STATUS_8 & channel_passed_0_z8)
			{
				non_assign_sw_test_mode |= TEST_MODE_CHANNEL_BIT_MASK;
				Z8_STATUS_8 &= ~channel_passed_0_z8;	
			}
		}
		else Z8_STATUS_8 |= channel_passed_0_z8;

		// GROUP switch pressed ?
		if(GROUP_PORT_IN & GROUP_PIN)
		{
			if(Z8_STATUS_9 & group_sw_passed_0_z9)
			{
				non_assign_sw_test_mode |= TEST_MODE_GROUP_BIT_MASK;
				Z8_STATUS_9 &= ~group_sw_passed_0_z9;	
			}
		}
		else Z8_STATUS_9 |= group_sw_passed_0_z9;

		// SCENE SEL switch pressed ?
		if(SCENE_SEL_PORT_IN & SCENE_SEL_PIN)
		{
			if(Z8_STATUS_9 & scene_sel_down_passed_0_z9)
			{
				non_assign_sw_test_mode |= TEST_MODE_SCENE_SEL_BIT_MASK;
				Z8_STATUS_9 &= ~scene_sel_down_passed_0_z9;	
			}
		}
		else Z8_STATUS_9 |= scene_sel_down_passed_0_z9;
			

		// if we are in test mode then update the dedicated register
		//V1.4 non_assign_sw_test_mode |= (uint)(1 << next_function);
	}

	// ***** NOW TREAT THESE 3 SWITCHES IN NORMAL MODE ******
	// check if midi channel is active
	if(CHANNEL_PORT_IN & CHANNEL_PIN)
		next_function = FCT_CHANNEL;
	else
	// check if group is active
	if(GROUP_PORT_IN & GROUP_PIN)
		next_function = FCT_GROUP;
	else
	// check if scene select is active
//	if(SCENE_SEL_PORT_IN & SCENE_SEL_PIN)
//		next_function = FCT_SCENE;
	if(SCENE_SEL_PORT_IN & SCENE_SEL_PIN) // V1.8 added scenes ARP
	{
		if(Z8_STATUS_9 & scene_sel_down_passed_0_z9)
		{
			// reset the flag passed through zero
			Z8_STATUS_9 &= ~scene_sel_down_passed_0_z9;	
			// scene snapshot or scene arp ?
			if(current_function == FCT_SCENE)
			{
				next_function = FCT_SCENE_ARP;
				Z8_STATUS_3_NVM |= scene_mode_z3nvm;
			}
			else
			{
				next_function = FCT_SCENE;
				Z8_STATUS_3_NVM &= ~scene_mode_z3nvm;
			}
		}
	}
	else
	{
		Z8_STATUS_9 |= scene_sel_down_passed_0_z9;
		// none of these switches was pressed
		return;
	}

	// Check if it changed from before
	if(next_function != current_function)
	{
		// Update LCD with new active function
		UpdateLcdFunction();

 		// update flag states
		// current_function = next_function;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckCtrlRegToSendToEz()
// Inputs	: None
// Outputs	: None
// Process  : check if a new EZBOARD param has to be sent to EZ board
///////////////////////////////////////////////////////////////////////////////////////
void CheckCtrlRegToSendToEz(void)
{
	uchar ctrl_reg_index;

	// V1.2 control regs are located in first eeprom
	I2cCommand  = BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR; 

	// If CTRL_REG_N_7BITS_TO_EZ has changed, need to send it to the EZ board
	for(ctrl_reg_index = CTRL_REG_TO_EZ_START_INDEX; ctrl_reg_index <= LAST_CTRL_REG_7BITS_TO_EZ_INDEX; ctrl_reg_index++)
	{
		if(CtrlReg_EE_img[ctrl_reg_index] != CtrlReg[ctrl_reg_index])
		{
			// get eeprom current control reg address			
			EE_Address = (uint)((uint)CTRL_REG_START_EE_ADR + (uint)ctrl_reg_index);
			// write new reg to eeprom
			WriteByteI2c(CtrlReg[ctrl_reg_index]);
			// update old reg with new reg value 
			CtrlReg_EE_img[ctrl_reg_index] = CtrlReg[ctrl_reg_index];

			// Also send to EZ board
			SendCtrlRegToEzBoard(ctrl_reg_index);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckCtrlRegToSave5Sec()
// Inputs	: None
// Outputs	: None
// Process  : Each 5 seconds : check if controls reg have to be saved to EEPROM
///////////////////////////////////////////////////////////////////////////////////////
void CheckCtrlRegToSave5Sec(void)
{
	uchar ctrl_reg_index;

	// 5 seconds passed ?
	if(!seconds_5_timer_cpt)
	{
		// V1.8 check if ez must fw pure midi to z8
		CheckIfEzMustForwardMidiToZ8();

		// EEprom is the first one for the ctrl registers 
		I2cCommand	= BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

		// Switches are treated differently
		if(Ctrl.Ctrl_sc.all_sw_value != all_sw_value_EE_img)
		{
			// update EEprom register with register itself
			all_sw_value_EE_img = Ctrl.Ctrl_sc.all_sw_value;
			// select EE_adress
			EE_Address = (uint)ALL_SWITCHES_EE_ADR;
			// Save to EEprom new register value
			WriteByteI2c(all_sw_value_EE_img);
		}

		// Other Switches are treated differently
		if(Ctrl.Ctrl_sc.other_sw_value != other_sw_EE_img)
		{
			// update EEprom register with register itself
			other_sw_EE_img = Ctrl.Ctrl_sc.other_sw_value;
			// select EE_adress
			EE_Address = (uint)OTHER_SWITCHES_EE_ADR;
			// Save to EEprom new register value
			WriteByteI2c(other_sw_EE_img);
		}

		// Now check all other control registers
		// skip both control regs to EZ board since treated separately
		for(ctrl_reg_index = Z8_STATUS_0_NVM_INDEX; ctrl_reg_index < CTRL_REG_REAL_NB; ctrl_reg_index++)
		{
			if(CtrlReg_EE_img[ctrl_reg_index] != CtrlReg[ctrl_reg_index])
			{
				// get eeprom current control reg address
				EE_Address = (uint)(CTRL_REG_START_EE_ADR + (uint)ctrl_reg_index);
				// write new reg to eeprom
				WriteByteI2c(CtrlReg[ctrl_reg_index]);
				// update old reg with new reg value 
				CtrlReg_EE_img[ctrl_reg_index] = CtrlReg[ctrl_reg_index];
			}
		}

		// reload 5 seconds counter
		seconds_5_timer_cpt = CPT_5_SEC;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SyncToEzBoard()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void SyncToEzBoard(void)
{
	uchar index;

	// Initialize serial In indexes
	serial_in_start_index 	= 0;
	serial_in_end_index		= 1;
	serial_in_current_index	= 0;

	// turn on itrs from rx0 from ez board
	U0CTL1 = 0x00; 					// itr on rx, brg normal, 8 bits, no infrared

	// be sure LAST_CTRL_REG_7BITS_TO_EZ indicates to EZ board it has been fully initialized
//	LAST_CTRL_REG_7BITS_TO_EZ |= ez_board_fully_updated_ezlast;
	LAST_CTRL_REG_7BITS_TO_EZ =EZ_FULLY_UPDATED;

//	SendCtrlRegToEzBoard(0);
	// Send Controls CTRL_REG_X_7BITS_TO_EZ_INDEX dedicated to EZBOARD
	LED_MIDI_1_PORT_OUT  |= LED_MIDI_1_PIN;
	LED_MIDI_2_PORT_OUT  |= LED_MIDI_2_PIN;
	
	for(index=CTRL_REG_TO_EZ_START_INDEX; index <= LAST_CTRL_REG_7BITS_TO_EZ_INDEX; index++)
		SendCtrlRegToEzBoard(index);

	// Wait for the registers sent by the EZ board
	do 
	{
		CheckSerialInBuffer();

		// Uncomment the following line to skip EZ-Z8 synchronization
#ifdef _DEBUG
	 CTRL_REG_7BITS_FROM_EZ[0] |= z8_board_fully_updated_from_ez0;
#endif
	} 
	while (!(CTRL_REG_7BITS_FROM_EZ[0] & z8_board_fully_updated_from_ez0));

	// turn off the leds only if not in auto test mode
	if(!(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1))
	{
		LED_MIDI_1_PORT_OUT &= ~LED_MIDI_1_PIN;
		LED_MIDI_2_PORT_OUT &= ~LED_MIDI_2_PIN;
	}
}

// V1.61
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitParamStructsInRam()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void InitParamStructsInRam(void)
{
	uchar index;

	// undefine all already loaded structs in ram (index similar but structures differents)
	for(index=0; index<PARAM_STRUCT_IN_RAM_MAX_NB; index++)
	{
		control_index_structs_in_ram[index] = CTRL_UNDEF;
		control_index_lcd_structs_in_ram[index] = CTRL_UNDEF;
	}
}

// V1.7
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitParamStructsInRam()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void UpdateOperatingMode(void)
{
	uchar tmp = OPERATING_MODE_NVM;

	// V1.5 1.69 Enter mackie/logic mode if mackie control is the new selected mode
	if(tmp == LOGIC_MODE)
	{
		mackie_logic_sysex_id = MACKIE_SYX_ID3_LOGIC;
		EnterMackieMode();
	}
	else
	if(tmp == MACKIE_MODE)
	{
		mackie_logic_sysex_id = MACKIE_SYX_ID3;				
		EnterMackieMode();
	}
	else
	if( (tmp == USER_MODE) || (tmp == STANDARD_MODE))
		// preceding mode was Mackie control or logic control
		LeaveMackieMode();

	// mode changed, this will force to update lcd str, and reload midi structure
	last_control_index = CTRL_UNDEF;

	// V1.61 reinit all param and lcd structs already preloaded in RAM
	InitParamStructsInRam();
}

/*
// V1.8
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: LoadHookModesFromEEprom()
// Inputs	: None
// Outputs	: None
// Process  : read all analog hook config and store in RAM
///////////////////////////////////////////////////////////////////////////////////////
void LoadHookModesFromEEprom(void)
{
}
*/
