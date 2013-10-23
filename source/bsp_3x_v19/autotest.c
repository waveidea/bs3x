#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "serial.h"
#include "eeprom.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "hardware.h"
#include "automation.h"
#include "autotest.h"
#include "arpp.h"
#include "motion.h"
#include "realtime.h"
#include "i2c.h"
#include "expansion.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

far    uint non_assign_sw_test_mode;
far    uchar encoder_autotest_value;	// V1.5

extern uint  EE_Address;							// current EEProm address
extern uchar I2cCommand;								// current EEProm selection
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion
extern rom uchar initializing_txt[];	
extern rom uchar default_lcd_str_txt[];
// V1.66 extern rom uchar font_5x8[];
extern uchar control_index;							// current control nb
extern rom uchar default_grp_name_txt[];
//extern far All_Params	 Param;						// structure of parameters in EEPROM
extern far All_Params *ParamCurrent;		// V1.61 current structure of parameters in EEPROM
extern All_Controls  Ctrl;							// All controls are defined here
extern volatile far uchar CtrlReg		[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern far uchar CtrlReg_EE_img	[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
//extern uchar current_function;
extern far uchar all_sw_value_EE_img; // V1.64 far
extern far uchar other_sw_EE_img; // V1.64 far
extern far   uchar CURVE_USER_0_TABLE[CURVE_TABLES_LENGHT];	// user curve model 0
extern far   uchar CURVE_USER_1_TABLE[CURVE_TABLES_LENGHT];	// user curve model 1

extern far 	Motion_Struct Motion_Str[MOTION_PART_NB];

extern far uchar realtime_received_buffer[REALTIME_BUFFER_LENGHT];
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];	// NN=1 registers can be accessed from ez board

rom uchar default_mmc_transport[] = {MIDI_MMC_REC_STROBE, MIDI_MMC_PLAY, MIDI_MMC_BACKWARD, MIDI_MMC_STOP, MIDI_MMC_FORWARD };
extern uchar Z8_STATUS_8;
extern uchar current_value;							// current control value
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// V1.5 a generic far ram buffer of 64 bytes

extern uint  current_random; // V1.5
extern uchar Z8_STATUS_10;	// V1.6
extern far uchar exp_cfg_reg[CONFIG_REGS_REAL_NB];	// V1.6 All config regs to expansions
//////////////////////////////////////////////////////////////////////////////////////////////////
/*
rom uchar ok_txt[]				= "OK";
rom uchar bad_txt[]				= "BAD";
rom uchar eeprom_txt[]			= "EE[ ]";
rom uchar out1_to_in_txt[]		= "Cable OUT1->IN";
*/
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckBootStatus()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	start a autotest or factory default
//				Keys functions at boot :
//					EDIT + OK + CANCEL			:	FIRMWARE UPGRADE (prioritary)
//					EDIT + OK + GROUP			:	FACTORY DEFAULT
//					EDIT + OK + CHANNEL			:	AUTOTEST + FACTORY DEFAULT
///////////////////////////////////////////////////////////////////////////////////////
void CheckBootStatus(void)
{
	uchar index; // V1.5
	
	// V1.5 allows interrupt while factory default
//V1.6	EI();

	// V1.5 if the eeproms are not intitialized then perform a factory default at startup
	// read the control registers from eeprom  : if all bytes are FF then perform a factory default
	I2cCommand	= BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
	EE_Address = (uint)CTRL_REG_START_EE_ADR;
	ReadMultiByteI2c((far uchar *)&generic_buffer[0], CTRL_REG_REAL_NB);	// place the first 64 bytes of eeprom 0 in generic buffer 0-63
	// Check that all control registers are not equal to FF
	for(index=0 ; index < CTRL_REG_REAL_NB ; index++)
	{
		if(generic_buffer[index] != 0xFF) break;
	}

	// V1.5 All registers were equal to FF : must perform a factory default
	if(index == CTRL_REG_REAL_NB)
		FactoryDefault();

	if(EDIT_PORT_IN & EDIT_PIN)
	if(OK_PORT_IN & OK_PIN)
	{
		// allow itrs since used for eeprom locking
		//V1.5 EI();

//		if(CANCEL_PORT_IN & CANCEL_PIN)
//		{
//			Display_Upgrade();
//		}

		if(GROUP_PORT_IN & GROUP_PIN)
		{
			FactoryDefault();
			return;
		}

		if(CHANNEL_PORT_IN & CHANNEL_PIN)
		{
			AutoTest();
			//CalibrateAdc();
			//FactoryDefault();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: FactoryDefault()
// Inputs	: None
// Outputs	: None
// Process  : Reset All parameters to factory default
///////////////////////////////////////////////////////////////////////////////////////
void FactoryDefault(void)
{
	uchar index;
	uchar tmp, tmp2;

	// Indicates to expansions that they have to reset to factory default
	exp_cfg_reg[0] |= factory_default_to_exp;	// V1.6

	// First clear display
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);	

	// display Initializing text
	rom_to_lcd_text_copy(initializing_txt);
	DisplayLcdText(20, 3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */ );

	////////////////////////// INITIALIZE ALL 8 MOTION PARTS //////////////////////////////////
	for(index=0 ; index < MOTION_PART_NB ; index++)
	{
		// all motion datas are init to 0
		for(tmp=0 ; tmp < MOTION_PART_LENGHT ; tmp++)
			Motion_Str[index].motion_data_table[tmp] = tmp;
		// all motion lenght are 4 measures
		Motion_Str[index].motion_length = 7;
		// all motion control indexes are undefined	
		Motion_Str[index].motion_ctrl_index = CTRL_UNDEF;
		motion_part = index;
		// save motion to eeprom
		SaveMotionToEEprom();
	}

	// current EEProm selection
	I2cCommand 	= BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Reset all 8 switches to 0
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	Ctrl.Ctrl_sc.all_sw_value = 0;
	all_sw_value_EE_img		  = 0;	
	EE_Address  			  = (uint)ALL_SWITCHES_EE_ADR;
	WriteByteI2c(Ctrl.Ctrl_sc.all_sw_value);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Reset all other 8 switches to 0
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	Ctrl.Ctrl_sc.other_sw_value = 0;
	other_sw_EE_img		 	 	= 0;	
	EE_Address  			 	= (uint)OTHER_SWITCHES_EE_ADR;
	WriteByteI2c(Ctrl.Ctrl_sc.other_sw_value);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Reset all control registers to their default values
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	CTRL_REG_0_7BITS_TO_EZ 		= /* sync24_state_z_ez0 | */ merger_src_midi_in_state_z_ez0 /* V1.9| merger_src_usb_in_state_z_ez0 */| merger_dst_usb_out_state_z_ez0 | merger_dst_out1_state_z_ez0;
	// V1.5 CTRL_REG_1_7BITS_TO_EZ 	= 0; //arpp_state_z_ez1 ; //| arpp_dst_usb_out_z_ez1 | arpp_dst_midi_out_z_ez1; //sync24_source_z_z_ez1;
	CTRL_REG_1_7BITS_TO_EZ 			= realtime_source_int_z_ez1; 
	//LAST_CTRL_REG_7BITS_TO_EZ 	= ez_board_fully_updated_ezlast;
	// V1.5 Z8_STATUS_0_NVM			= arpp_dst_midi_out1_z0nvm | arpp_dst_midi_out2_z0nvm;
	Z8_STATUS_0_NVM					= arpp_dst_usb_out_z0nvm | arpp_dst_midi_out1_z0nvm | arpp_dst_midi_out2_z0nvm;
	Z8_STATUS_1_NVM					= /* V1.8 hook_zjump_mode_z1nvm | */ infrared_state_z1nvm | motion_loop_state_z1nvm;
	// V1.5 Z8_STATUS_2_NVM			= /* V1.3 automation_sel_0_z2nvm |*/ lfo_wave_1_z2nvm | lfo_wave_0_z2nvm ;
	Z8_STATUS_2_NVM					=  shift_lock_midi_z2nvm /* V1.8 | lfo_wave_1_z2nvm | lfo_wave_0_z2nvm */ ; // V1.8 LFO is sine default
	Z8_STATUS_3_NVM					= 0; // v1.8
	current_channel 				= 0;
	current_group 					= 0;
	current_scene 					= 0;
	infrared_hold_save				= 64;
	next_function 					= FCT_SCENE;
	current_autom	  				= AUTOMATION_LFO;
	LFO_Offset						= 63;
	LFO_Ampl						= 63;
	LFO_Freq						= 16; // V1.64 6
   	arp_clock_resolution 			= ARP_DEFAULT_CLK_RESO_INDEX;
	arp_range 						= DEFAULT_RANGE; //DBG MAX_RANGES-1; // v1.8
   	arp_melody 						= 0; //0
   	arp_dir 						= DIR_UP;
	arp_rhythm 						= 0;
	arp_accent 						= 0;   
    arp_transpose 					= DEFAULT_TRANSPOSE;
    arp_gate 						= (arp_clradio_16th >> 1);
	arp_melody_bk_nb				= MELODYBK0;
   	arp_rhythm_bk_nb 				= RHYTHMBK0;
   	arp_accent_bk_nb 				= ACCENTBK0;
   	arp_velocity 					= DEFAULT_VELOCITY;
   	arp_swing 						= DEFAULT_SWING;
   	arp_mode 						= ARP_MODE_AUTOTRIG; // v1.8
	arp_offset						= 1;
	arp_midi_ch						= 0; // V1.69
   	internal_tempo					= DEFAULT_INTERNAL_TEMPO;
	OPERATING_MODE_NVM				= 0; // V1.69 standard mode by default
	current_scene_arp				= 0; // V1.8

	// Now all controls regs are defined and clean also assign control regs to control regs ee img
	for(tmp=0; tmp<CTRL_REG_REAL_NB; tmp++)
		CtrlReg_EE_img[tmp] = CtrlReg[tmp];

	// Save all control registers in a multi byte eeprom write
	EE_Address = (uint)CTRL_REG_START_EE_ADR; 
	WriteMultiByteI2c(&CtrlReg[0], CTRL_REG_REAL_NB);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Write default group names : group 1, group 2 ..
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	rom_to_lcd_text_copy(default_grp_name_txt);
	// Program all groups names
	for(current_group=0;current_group<GROUP_NB;current_group++)
	{
		// extarct the 2 digits of current group
		tmp  = current_group;
		tmp2 = '0'; 
   		while(tmp > 9)
   		{
      		tmp -= 10;
      		tmp2++;
   		}
		lcd_text_value[9] = tmp2;
		lcd_text_value[10] = tmp + '0';
		// calculate group name address
		EE_Address = (uint)(GROUP_NAME_BASE_EE_ADR + (uint)((uint)(current_group) * GROUP_NAME_EE_LENGHT));
		// write group name to eeprom
		WriteMultiByteI2c((far uchar *)&lcd_text_value[0], GROUP_NAME_LENGHT);
	}

	// V1.8 save all default arpeggio scenes values
	for(tmp=0; tmp<SCENE_ARP_NB; tmp++)
	{
		// First calculate EE base address according to scene arp selected
		EE_Address = (uint)(ARP_SCENES_START_EE_ADR + (uint)((uint)tmp * (uint)ARP_PARAM_NB));
		// save default scenes values for arpp ?
		WriteMultiByteI2c((far uchar *)&CtrlReg[ARP_GATE_INDEX], ARP_PARAM_NB);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Write default user curves models 0 & 1 ..
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(tmp=0; tmp<CURVE_TABLES_LENGHT; tmp++)
	{
		// default user curve 0 is linear
		CURVE_USER_0_TABLE[tmp] = tmp;
		// default user curve 1 is invert linear
		CURVE_USER_1_TABLE[tmp] = 127 - tmp;
	}
	EE_Address = (uint)CURVE_USER_0_LOW_START_EE_ADR;
	WriteMultiByteI2c(&CURVE_USER_0_TABLE[0], EEPROM_PAGE_SIZE);
	EE_Address = (uint)CURVE_USER_0_HIGH_START_EE_ADR;
	WriteMultiByteI2c(&CURVE_USER_0_TABLE[EEPROM_PAGE_SIZE], EEPROM_PAGE_SIZE);
	EE_Address = (uint)CURVE_USER_1_LOW_START_EE_ADR;
	WriteMultiByteI2c(&CURVE_USER_1_TABLE[0], EEPROM_PAGE_SIZE);
	EE_Address = (uint)CURVE_USER_1_HIGH_START_EE_ADR;
	WriteMultiByteI2c(&CURVE_USER_1_TABLE[EEPROM_PAGE_SIZE], EEPROM_PAGE_SIZE);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Write default Transport functions MIDI struct and str, no lcd str for transport
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Default transport mapping is aranged for cubase transport functions MMC
	// MMC is : F0 7F ID 06 NN F7 with 
	// ID = 127 for all devices
	// NN = 0x01 Stop 0x02 Play 0x03 Deferred Play 0x04 Fast Forward 0x05 Rewind 0x06 Record Strobe (Punch In) 
	// 0x07 Record Exit (Punch out) 0x09 Pause 
	// control status 1 is (~ctrl_mode)|(scene_exclude)|(~autosend)|usb_out_en|midi_out_1_en|midi_out_2_en|(~midi_delay_msk);
	ParamCurrent->ctrl_status_1				= scene_exclude | usb_out_en | midi_out_1_en /* V1.9 | midi_out_2_en */;
	ParamCurrent->ctrl_status_2				= 0x00;		// linear curve model, no checksum
	ParamCurrent->midi_str_lenght 			= 0x06;		// default are 6 bytes for MMC				
	ParamCurrent->midi_channel_position		= 0x00;		// no midi channel on MMC message 	
	ParamCurrent->ctrl_value_position_0		= 0x00;		// no value on MMC message			
	ParamCurrent->ctrl_value_position_1		= 0x00;		// no value on MMC message		
	ParamCurrent->ctrl_value_min			= 0x00;		// Min default is 0					
	ParamCurrent->ctrl_value_max			= 0x7F;		// Max default is 127					
	ParamCurrent->checksum_start_position 	= 0x00;		// No checksum on default CC messages		
	ParamCurrent->ctrl_chain				= 0x00;		// No Control chain defined						
	// default MMC midi string 
	ParamCurrent->midi_string[0] 			= 0xF0;		// default string is MMC 
	ParamCurrent->midi_string[1] 			= 0x7F;		// default string is MMC
	ParamCurrent->midi_string[2] 			= 0x7F;		// default string is MMC id is 127 
	ParamCurrent->midi_string[3] 			= 0x06;		// default string is MMC
	ParamCurrent->midi_string[5] 			= 0xF7;		// default string is MMC

	// Transport function are located at the begining of eeprom one
	I2cCommand	= BS3X_I2C_SLAVE_ADR_EE + EEPROM_ONE_I2C_ADR;
	EE_Address = (uint)TRANSPORT_MIDI_STR_EE_BASE_ADR;

	for(control_index=0; control_index < TRANSPORT_NB; control_index++)	
	{
		ParamCurrent->midi_string[4] = default_mmc_transport[control_index];
		ParamCurrent->ctrl_status_0  =  control_index & 0x7F;		// default is CC
		WriteMultiByteI2c((far uchar *)&ParamCurrent->ctrl_status_0, MIDI_STRUCT_SIZE);
		EE_Address += (uint)MIDI_STRUCT_SIZE;
	}

 	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Write default MIDI struct and strings and LCD strings
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fill parts of control struct that will not change
	// control_status 1 is (~ctrl_mode)|(~scene_exclude)|(~autosend)|usb_out_en|midi_out_1_en|midi_out_2_en|(~midi_delay_msk);
	ParamCurrent->ctrl_status_1				= autosend | usb_out_en | midi_out_1_en /* V1.9 | midi_out_2_en */ | switch_ctrl_hook_mode;
	ParamCurrent->ctrl_status_2				= 0x00;		// linear curve model, no checksum
	ParamCurrent->midi_str_lenght 			= 0x03;		// default are 3 bytes for CCs				
	ParamCurrent->midi_channel_position		= 0x01;		// midi channel position on first byte 	
	ParamCurrent->ctrl_value_position_0		= 0x03;		// default control value on byte 3			
	ParamCurrent->ctrl_value_position_1		= 0x00;		// only one control value is default	
	ParamCurrent->ctrl_value_min			= 0x00;		// Min default is 0					
	ParamCurrent->ctrl_value_max			= 0x7F;		// Max default is 127					
	ParamCurrent->checksum_start_position 	= 0x00;		// No checksum on default CC messages		
	ParamCurrent->ctrl_chain				= 0x00;		// No Control chain defined						
	// default midi string 
	ParamCurrent->midi_string[0] 			= 0xB0;		// default string is CC : 
	ParamCurrent->midi_string[2] 			= 0x00;		// default string is CC : value byte is cleared
	// We use here the lcd_text_value as the lcd string buffer
	// copy '    CC # ___    ' to lcd_text_value
	rom_to_lcd_text_copy(default_lcd_str_txt);	

	tmp  = 0;
	tmp2 = 0;
	// Program all groups
	for(current_group=0;current_group < GROUP_NB; current_group++)
	{
		// program all controls (after transport functions)
		for(index=0; index < CONTROLS_WITH_GROUP_NB; index++)	
		{
			// offset control index since transport are not programmed here
			control_index = index + TRANSPORT_NB;

			// Fulfill remaining bytes of control structure, midi str and lcd str
			FulfillControlStructure();

			// Write control struct and midi str to ext EEPROM
			GetCtrlStructEEaddFromIndexGroup();
			WriteMultiByteI2c((far uchar *)&ParamCurrent->ctrl_status_0, MIDI_STRUCT_SIZE);

//			GetCtrlStructEEaddFromIndexGroup();
//			ReadMultiByteEEprom((far uchar *)&ParamCurrent->ctrl_status_0, MIDI_STRUCT_SIZE);
//			if(ParamCurrent->midi_channel_position != 0x01)
//				control_index = index + TRANSPORT_NB;

			// Get LCD str EE address (remove transport nb since no lcd str for transport)
			GetLcdStrEEaddFromIndexGroup();
			WriteMultiByteI2c(&lcd_text_value[0], LCD_STR_SIZE);

			// Update Initializing bargraph
			if(++tmp == INIT_BARGRAPH_STEP) 
			{
				// V1.5 make current autom leds blinking while restoring to factory default
				UpdateAutomationLeds();
				current_autom++;
				if(current_autom > AUTOMATION_NB) current_autom = AUTOMATION_MOTION;
					
				tmp2++;
				if (tmp2 > INIT_BARGRAPH_LENGHT)
				    tmp2 = INIT_BARGRAPH_LENGHT;	
				tmp =0;
				FillLcdZone(1, 5, tmp2, 1, 0x00);
			}
		} // end for all controls
	} // end for all groups

	// V1.6 : reinit initial values after factory default !
	InitInitialValues();

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: FulfillControlStructure()
// Inputs	: None
// Outputs	: None
// Process  : Fill remaining bytes of control struct
///////////////////////////////////////////////////////////////////////////////////////
void FulfillControlStructure(void)
{
	uchar offset_index_from_group=0;
	uchar tmp;

	for(tmp=0; tmp < current_group; tmp++)
		offset_index_from_group += GROUP_INDEX_OFFSET;

	// Define default control structure and write it
	ParamCurrent->ctrl_status_0 	=  (control_index + offset_index_from_group) & 0x7F;// default is CC
	ParamCurrent->midi_string[1] 	=  ParamCurrent->ctrl_status_0;								// default string is CC

	// Define default LCD string
	CreateStdLcdStrFromCtrlIndex();
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AutoTest()
// Inputs	: None
// Outputs	: None
// Process  : Reset All parameters to factory default
///////////////////////////////////////////////////////////////////////////////////////
void AutoTest(void)
{
	// PLACE Both EZ and Z8 in autotest modes
	CTRL_REG_1_7BITS_TO_EZ = autotest_mode_z_ez1 | realtime_source_int_z_ez1;

	// V1.5 When autotesting the following settings are required
	CTRL_REG_0_7BITS_TO_EZ 		=  sync24_state_z_ez0 | merger_src_midi_in_state_z_ez0 | merger_src_usb_in_state_z_ez0 | merger_dst_usb_out_state_z_ez0 | merger_dst_out1_state_z_ez0;

	// V1.5 immediately send controls regs to EZ
	// in this way MIDI_CLOCK & MIDI_CONTINUE sent below will illuminate permanelty the LEDS
	CheckCtrlRegToSendToEz();

	// AUTOTEST MODE : TURN ON ALL LEDS
	LED_MIDI_1_PORT_OUT 		|= LED_MIDI_1_PIN;		// led midi out 1 pin
	LED_MIDI_2_PORT_OUT			|= LED_MIDI_2_PIN;		// led midi out 2 pin
	LED_MOTION_PORT_OUT			|= LED_MOTION_PIN;		// led motion pin
	LED_ARP_PORT_OUT			|= LED_ARP_PIN;			// led arp pin
	LED_LFO_PORT_OUT			|= LED_LFO_PIN;			// led lfo pin
	LED_AUTOMATION_PORT_OUT		|= LED_AUTOMATION_PIN;	// led automation pin
	LED_CURRENT_AUTOM_PORT_OUT	|= 0xE0;				// leds for current autom bits 5,6,7			

	// V1.5 turn on sync-24 signal to illuminate 3 test LEDs
	SendToTx0(MIDI_CLOCK);
	SendToTx0(MIDI_CONTINUE);
	// SendToTx0(MIDI_START);

	// reset non assignable switches register
	non_assign_sw_test_mode = 0;
	
	// V1.5 init encoder_autotest_value to 64
	encoder_autotest_value = 64;

	// V1.5 : launch the expansion board test
	AutoTestExpansion();

/*
	uchar tmp, mask;
	uchar TEST_RESULT_REG_0 = NO_ERROR;
	uchar y_page_test_res  = 0;
	rom uchar *const_far_ptr;

	// When autotesting the following settings are required
	CTRL_REG_0_7BITS_TO_EZ 		=  sync24_state_z_ez0; // | merger_src_midi_in_state_z_ez0 | merger_src_usb_in_state_z_ez0 | merger_dst_usb_out_state_z_ez0 | merger_dst_out1_state_z_ez0;
	CTRL_REG_1_7BITS_TO_EZ 		= 0; //arpp_state_z_ez1 ; //| arpp_dst_usb_out_z_ez1 | arpp_dst_midi_out_z_ez1; //sync24_source_z_z_ez1;
	// send new config to ez board
	SyncToEzBoard();
	// signal to Z8 that everything is updated from EZ
	//CTRL_REG_7BITS_FROM_EZ[0] |= z8_board_fully_updated_from_ez0;

	// Clear display
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);

	///////////////////////////////////////////////////////////////////////////////////
	// [TEST 1] First autotest all 3 eeproms (TRANSPARENT TEST) ///////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	// the address 
	EE_Address = (uint)0;
	mask       = 1;
	for(tmp=0; tmp < EEPROM_NB ; tmp++)
	{
		rom_to_lcd_text_copy(eeprom_txt);
		lcd_text_value[3] = tmp + '0';
		DisplayLcdText(1, tmp, LCD_INVERT, font_5x8);
		I2cCommand  = (tmp << EEPROM_I2C_ADR_POS);
		WriteByteEEprom(AUTOTEST_VALUE);
		delay_msec(10);
		if(ReadByteEEprom() != AUTOTEST_VALUE)
		{
			TEST_RESULT_REG_0 |= mask;
			const_far_ptr = bad_txt;
		}
		else
			const_far_ptr = ok_txt;			

		rom_to_lcd_text_copy(const_far_ptr);	
		DisplayLcdText(35, tmp, LCD_INVERT, font_5x8);
		mask = mask << 1;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// [TEST 2] Check loopback cables /////////////////////////////////////////////////
	// Send / receive realtime events F8(clock), 
	///////////////////////////////////////////////////////////////////////////////////
	// MIDI OUT 1 to MIDI IN
	rom_to_lcd_text_copy(out1_to_in_txt);
	DisplayLcdText(1, 3, LCD_INVERT, font_5x8);
	// send REALTIME_BUFFER_LENGHT midi clock events
//	for(tmp=0; tmp < REALTIME_BUFFER_LENGHT ; tmp++)
		
	// check that all input buffer is filled with expected datas
	tmp=0;
	do
	{
		SendToTx0(MIDI_CLOCK);
		if(realtime_received_buffer[tmp] == MIDI_CLOCK)
		{	
			// reset buffer for next test
			realtime_received_buffer[tmp] = 0;
			tmp++;
		}
	}
	while(tmp < REALTIME_BUFFER_LENGHT);
	
	// MIDI OUT 2 to MIDI IN
	lcd_text_value[9]='2';
	DisplayLcdText(1, 4, LCD_INVERT, font_5x8);
	// send REALTIME_BUFFER_LENGHT midi clock events
//	for(tmp=0; tmp < REALTIME_BUFFER_LENGHT ; tmp++)
	// check that all input buffer is filled with expected datas
	tmp=0;
	do
	{
		SendToTx1(MIDI_CLOCK);
		if(realtime_received_buffer[tmp] == MIDI_CLOCK)
		tmp++;
	}
	while(tmp < REALTIME_BUFFER_LENGHT);

	///////////////////////////////////////////////////////////////////////////////////
	// [TEST 3]  /////////////////////////////////////////////////
	// 
	///////////////////////////////////////////////////////////////////////////////////
*/
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckSendSwSysexTstMode()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void CheckSendSwSysexTstMode(void)
{
	uchar i;
	uint mask = 1;

	// V1.5 turn on sync-24 signal to illuminate 3 test LEDs
//	SendToTx0(MIDI_CLOCK);
//	SendToTx0(MIDI_CONTINUE);

	// OK and CANCEL switches are still not tested
	// OK switch pressed ?
	if(OK_PORT_IN & OK_PIN)
	{
		if(Z8_STATUS_8 & ok_passed_0_z8)
		{
			non_assign_sw_test_mode |= TEST_MODE_OK_BIT_MASK;
			Z8_STATUS_8 &= ~ok_passed_0_z8;	
		}
	}
	else Z8_STATUS_8 |= ok_passed_0_z8;

	// CANCEL switch pressed ?
	if(CANCEL_PORT_IN & CANCEL_PIN)
	{
		if(Z8_STATUS_8 & cancel_passed_0_z8)
		{
			non_assign_sw_test_mode |= TEST_MODE_CANCEL_BIT_MASK;
			Z8_STATUS_8 &= ~cancel_passed_0_z8;	
		}
	}
	else Z8_STATUS_8 |= cancel_passed_0_z8;


	// for all non assignable switches while being in test mode
	for(i=0; i<NON_ASSIGN_SW_NB; i++)
	{
		// a non assignable switch has changed in test mode
		if(non_assign_sw_test_mode & mask)
		{
			// init both control_index and value to be sent in sysex
			control_index = TEST_MODE_NON_ASSIGN_SW_BASE_INDEX + i;
			
			if(control_index != AUTOTEST_ENCODER_UP_INDEX)
				current_value = 127;
			else
				current_value = encoder_autotest_value;

			// send the autotest sysex
			SendControlInAutotestMode(midi_out_2_en);
			// clear the flag treated
			non_assign_sw_test_mode &=  ~mask;
		}

		mask = mask << 1;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AutoTestExpansion()
// Inputs	: None
// Outputs	: None
// Process  : V1.5 
///////////////////////////////////////////////////////////////////////////////////////
void AutoTestExpansion(void)
{
	uchar autotest_value = (uchar)(current_random & 0xFF);

	// Expansion IO pin becomes an output to select expansion eeprom address (PC7)
	// PORTC DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PCADDR 	= PORT_DIR;     	// Acces to sub-reg direction
  	PCCTL 	&= ~EXPANSION_PIN; 	// Port direction 1=in 0=out : expansion pin becomes an output
	// Set the expansion pin output to 1
	EXPANSION_PORT_OUT |= EXPANSION_PIN;
	// Memory address is from 0 to 4095
	EE_Address = (uint)(current_random & 0xFFF);
	// The external expansion eeprom address is now A0A1A2 = 1 1 1
	I2cCommand   = BS3X_I2C_SLAVE_ADR_EE + EEPROM_AUTOTEST_EXPANSION_I2C_ADR;
	// Write a random byte at a random address
	current_value = 0;
	WriteByteI2c(autotest_value);
	if(!(Z8_STATUS_10 & i2c_timout_z10))
	{
	   	// wait for eeprom write completed
		delay_msec(15);
		// Compare
		if(ReadByteI2c() == autotest_value)
			current_value = 127;
	}

	control_index = AUTOTEST_EXPANSION_INDEX;
	
	// send the autotest sysex
	SendControlInAutotestMode(midi_out_1_en | usb_out_en);

	// PORTC DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PCADDR 	= PORT_DIR;     	// Acces to sub-reg direction
  	PCCTL 	|= EXPANSION_PIN; 	// Port direction 1=in 0=out : expansion pin rebecomes an input for normal mode

}

