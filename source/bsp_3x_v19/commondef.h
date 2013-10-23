#ifndef COMMONDEF
#define COMMONDEF

///////////////////////////////////////////////////////////
// Common definitions
///////////////////////////////////////////////////////////
//#define Z8_COMPIL 	1

typedef unsigned char uchar;
typedef unsigned int uint;
#define NULL 0

//////////////////////////////////////////////////////////////////////////////
// REGISTERS THAT MUST BE SAVED INTO NVM
//////////////////////////////////////////////////////////////////////////////

// Control register definitions
#define CTRL_REG_0_7BITS_TO_EZ_EE_IMG		(CtrlReg_EE_img[CTRL_REG_0_7BITS_TO_EZ_INDEX])
#define CTRL_REG_1_7BITS_TO_EZ_EE_IMG		(CtrlReg_EE_img[CTRL_REG_1_7BITS_TO_EZ_INDEX])
#define LAST_CTRL_REG_7BITS_TO_EZ_EE_IMG	(CtrlReg_EE_img[LAST_CTRL_REG_7BITS_TO_EZ_INDEX])
#define Z8_STATUS_0_NVM_EE_IMG				(CtrlReg_EE_img[Z8_STATUS_0_NVM_INDEX])
#define Z8_STATUS_1_NVM_EE_IMG				(CtrlReg_EE_img[Z8_STATUS_1_NVM_INDEX])
#define Z8_STATUS_2_NVM_EE_IMG				(CtrlReg_EE_img[Z8_STATUS_2_NVM_INDEX])
#define current_channel_EE_IMG				(CtrlReg_EE_img[CHANNEL_INDEX])
#define current_group_EE_IMG				(CtrlReg_EE_img[GROUP_INDEX])
#define current_scene_EE_IMG				(CtrlReg_EE_img[SCENE_INDEX])
#define infrared_hold_save_EE_IMG			(CtrlReg_EE_img[INFRARED_HOLD_INDEX])
#define next_function_EE_IMG				(CtrlReg_EE_img[NEXT_FUNCTION_INDEX])
#define current_autom_EE_IMG				(CtrlReg_EE_img[CURRENT_AUTOM_INDEX])
#define motion_part_EE_IMG					(CtrlReg_EE_img[MOTION_PART_INDEX])
#define LFO_Ampl_EE_IMG						(CtrlReg_EE_img[LFO_AMPL_INDEX])
#define LFO_Freq_EE_IMG						(CtrlReg_EE_img[LFO_FREQ_INDEX])
#define LFO_Offset_EE_IMG					(CtrlReg_EE_img[LFO_OFFSET_INDEX])
#define arp_gate_EE_IMG						(CtrlReg_EE_img[ARP_GATE_INDEX])
#define arp_accent_EE_IMG					(CtrlReg_EE_img[ARP_ACCENT_INDEX])
#define arp_rhythm_EE_IMG					(CtrlReg_EE_img[ARP_RHYTHM_INDEX])
#define arp_range_EE_IMG					(CtrlReg_EE_img[ARP_RANGE_INDEX])
#define arp_transpose_EE_IMG				(CtrlReg_EE_img[ARP_TRANSPOSE_INDEX])
#define arp_melody_EE_IMG					(CtrlReg_EE_img[ARP_MELODY_INDEX])
#define arp_dir_EE_IMG						(CtrlReg_EE_img[ARP_DIR_INDEX])
#define arp_clock_resolution_EE_IMG			(CtrlReg_EE_img[ARP_CLOCK_RESOLUTION_INDEX])
#define arp_midi_ch_EE_IMG					(CtrlReg_EE_img[ARP_CHANNEL_INDEX])
#define arp_velocity_EE_IMG					(CtrlReg_EE_img[ARP_VELOCITY_INDEX])
#define arp_swing_EE_IMG					(CtrlReg_EE_img[ARP_SWING_INDEX])
#define arp_melody_bk_nb_EE_IMG				(CtrlReg_EE_img[ARP_MELODY_BNK_INDEX])
#define arp_rhythm_bk_nb_EE_IMG				(CtrlReg_EE_img[ARP_RHYTHM_BNK_INDEX])
#define arp_accent_bk_nb_EE_IMG				(CtrlReg_EE_img[ARP_ACCENT_BNK_INDEX])
#define arp_mode_EE_IMG						(CtrlReg_EE_img[ARP_MODE_INDEX])
#define arp_offset_EE_IMG					(CtrlReg_EE_img[ARP_OFFSET_INDEX])
#define internal_tempo_EE_IMG				(CtrlReg_EE_img[INT_TEMPO_INDEX])
#define OPERATING_MODE_NVM_EE_IMG			(CtrlReg_EE_img[OPERATING_MODE_NVM_INDEX])
#define Z8_STATUS_3_NVM_EE_IMG				(CtrlReg_EE_img[Z8_STATUS_3_NVM_INDEX])
#define current_scene_arp_EE_IMG			(CtrlReg_EE_img[SCENE_ARP_INDEX])

//#define ADC_Offset_EE_IMG					(CtrlReg_EE_img[ADC_OFFSET_INDEX])

#define CTRL_REG_0_7BITS_TO_EZ			(CtrlReg[CTRL_REG_0_7BITS_TO_EZ_INDEX])
#define CTRL_REG_1_7BITS_TO_EZ			(CtrlReg[CTRL_REG_1_7BITS_TO_EZ_INDEX])
#define LAST_CTRL_REG_7BITS_TO_EZ		(CtrlReg[LAST_CTRL_REG_7BITS_TO_EZ_INDEX])
#define Z8_STATUS_0_NVM					(CtrlReg[Z8_STATUS_0_NVM_INDEX])
#define Z8_STATUS_1_NVM					(CtrlReg[Z8_STATUS_1_NVM_INDEX])
#define Z8_STATUS_2_NVM					(CtrlReg[Z8_STATUS_2_NVM_INDEX])
#define current_channel					(CtrlReg[CHANNEL_INDEX])
#define current_group					(CtrlReg[GROUP_INDEX])
#define current_scene					(CtrlReg[SCENE_INDEX])
#define infrared_hold_save				(CtrlReg[INFRARED_HOLD_INDEX])
#define next_function					(CtrlReg[NEXT_FUNCTION_INDEX])
#define current_autom					(CtrlReg[CURRENT_AUTOM_INDEX])
#define motion_part						(CtrlReg[MOTION_PART_INDEX])
#define LFO_Ampl						(CtrlReg[LFO_AMPL_INDEX])
#define LFO_Freq						(CtrlReg[LFO_FREQ_INDEX])
#define LFO_Offset						(CtrlReg[LFO_OFFSET_INDEX])
#define arp_gate						(CtrlReg[ARP_GATE_INDEX])
#define arp_accent						(CtrlReg[ARP_ACCENT_INDEX])
#define arp_rhythm						(CtrlReg[ARP_RHYTHM_INDEX])
#define arp_range						(CtrlReg[ARP_RANGE_INDEX])
#define arp_transpose					(CtrlReg[ARP_TRANSPOSE_INDEX])
#define arp_melody						(CtrlReg[ARP_MELODY_INDEX])
#define arp_dir							(CtrlReg[ARP_DIR_INDEX])
#define arp_clock_resolution			(CtrlReg[ARP_CLOCK_RESOLUTION_INDEX])
#define arp_midi_ch						(CtrlReg[ARP_CHANNEL_INDEX])
#define arp_velocity					(CtrlReg[ARP_VELOCITY_INDEX])
#define arp_swing						(CtrlReg[ARP_SWING_INDEX])
#define arp_melody_bk_nb				(CtrlReg[ARP_MELODY_BNK_INDEX])
#define arp_rhythm_bk_nb				(CtrlReg[ARP_RHYTHM_BNK_INDEX])
#define arp_accent_bk_nb				(CtrlReg[ARP_ACCENT_BNK_INDEX])
#define arp_mode						(CtrlReg[ARP_MODE_INDEX])
#define arp_offset						(CtrlReg[ARP_OFFSET_INDEX])
#define internal_tempo					(CtrlReg[INT_TEMPO_INDEX])
#define OPERATING_MODE_NVM				(CtrlReg[OPERATING_MODE_NVM_INDEX])
#define Z8_STATUS_3_NVM					(CtrlReg[Z8_STATUS_3_NVM_INDEX])
#define current_scene_arp				(CtrlReg[SCENE_ARP_INDEX])

//#define ADC_Offset						(CtrlReg[ADC_OFFSET_INDEX])

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// Z8_STATUS_0 bits definition
#define must_change_scene_z0		(1<<7)	// bit 5 : scene select increase
#define must_increase_scene_z0		(1<<6)	// bit 5 : current group increase
#define must_change_channel_z0		(1<<5)	// bit 5 : midi channel increase
#define must_increase_channel_z0	(1<<4)	// bit 5 : midi channel increase
#define snapshot_changed_z0	 		(1<<3)	// bit 3 : snapshot state has changed
#define must_recheck_snapshot_z0	(1<<2)	// bit 2 : debounce snapshot button
#define save_zsend_snapshot_z0		(1<<1)	// bit 1 : 1=save / 0 = send snapshot
#define snapshot_locked_z0			(1<<0)	// bit 0 : wait snapshot sw to 0 before reenable snapshot

// Z8_STATUS_3 bits definition
#define must_change_group_z3			(1<<7)	// group changed
#define must_increase_group_z3			(1<<6)	// current group increase
#define must_reinit_incrust_lcd_z3		(1<<5)	// must_reinit incrusted lcd zone only
#define infrared_lock_midi_z3			(1<<4)	// bit 4 : infrared lock until 0 since holded
#define ir_sw_changed_z3				(1<<3)	// bit 3 : infrared sw has been updated
#define must_recheck_ir_sw_z3			(1<<2)	// bit 2 : debounce infrared button
#define onoff_zhold_infrared_z3			(1<<1)	// bit 1 : 1=ir on/off / 0 = hold infrared
#define ir_sw_locked_z3			 		(1<<0)	// bit 0 : wait infrared sw to 0 before reenable

// Z8_STATUS_4 bits definition
#define sending_scene_z4				(1<<7)	// currrently sending scene
#define mode_changed_z4					(1<<6)	// mode has been changed
#define eeprom_locked_10ms_z4			(1<<5)	// allow to lock eeprom durring 10ms
#define first_pass_z4					(1<<4)	// allow to detect first pass
#define wait_ir_passed_zero_z4			(1<<3)  // lock ir after hold up to pass through zero 
#define fader_hook_bmp_state_z4 		(1<<2)  // 1 = fader is hooked else 0 
#define force_hook_fader_display_z4 	(1<<1)  // 1 = force hook fader state to be displayed again
#define lcd_locked_by_function_z4		(1<<0)	// 1= lcd is locked if menus/assign/motion active

// Z8_STATUS_5 bits definition
#define lfo_waiting_midi_clock_z5		(1<<7)	// currrently waiting midi clock
#define must_send_lfo_z5				(1<<6)	// must send a new lfo data
#define sending_lfo_z5					(1<<5)	// lfo is currently sent to midi
#define serial_in_state_z5				(1<<4)	// 0=waiting SOF / 1=waiting EOF
#define must_save_assign_struct_z5		(1<<3)	// 1=yes
#define must_show_ctrl_chain_z5			(1<<2)	// 1=must display control chain bmp
#define lfo_play_zstop_z5				(1<<1)	// 0=lfo waits start
#define must_update_lcd_small_lfo_z5	(1<<0)	// 1= small lcd lfo update

// Z8_STATUS_6 bits definition
#define encoder_rising_state_z6			(1<<7)	// the state of encoder B pin on A pin rising edge itr
#define must_reinit_channel_lcd_z6		(1<<6)	// 1: must redisplay midi channel V1.62
#define ok_sw_changed_z6				(1<<5)	// 1: ok sw pressed
#define autom_sw_on_passed_0_z6			(1<<4)	// 1: 
#define up_down_sw_changed_z6			(1<<3)	// 1: up OR dwn sw has been pressed
#define up_down_sw_direction_z6			(1<<2)	// 1: up pressed else down pressed
#define edit_sw_changed_z6				(1<<1)	// 1: edit sw pressed
#define random_byte_select				(1<<0)	// used to select high or low byte of random 16 bits

// Z8_STATUS_7 bits definition
#define must_reinit_all_lcd_z7			(1<<7)	// must reinit the whole lcd
#define mode_passed_0_z7				(1<<6)	// 1 if switch passed through 0					
#define down_passed_0_z7				(1<<5)	// 1 if switch passed through 0					
#define encoder_sw_passed_0_z7			(1<<4)	// 1 if switch passed through 0					
#define edit_passed_0_z7				(1<<3)	// 1 if switch passed through 0					
#define up_passed_0_z7					(1<<2)	// 1 if switch passed through 0					
#define autom_sw_type_passed_0_z7		(1<<1)	// 1 if switch passed through 0					
#define autom_sw_sel_passed_0_z7		(1<<0)	// 1 if switch passed through 0					

// Z8_STATUS_9 bits definition

#define group_sw_passed_0_z9 				(1<<7)	// bit 7 : set to 1 when group sw is relaxed V1.4
#define must_decrease_mackie_bargraph_z9	(1<<6)	// set to 1 when group sw is relaxed V1.4
#define scene_sel_down_passed_0_z9			(1<<5)	// set to 1 when group sw is relaxed V1.4
#define motion_wait_rec_confirm_z9			(1<<4)	// 1 = waiting confirm before saving motion
#define arp_updown_dir_z9					(1<<3)	// 
#define shift_state_z9						(1<<2)	// 1 = shift active 
#define arp_first_note_z9					(1<<1)	// 
#define sending_motion_to_midi_z9			(1<<0)

// V1.62 #define Z8_STATUS_RX_REALTIME 				Z8_STATUS_9	
// V1.62 #define REALTIME_MSK						0xF0

// Z8_STATUS_10 bits definition

#define must_send_mackie_encoder_z10		(1<<7)	// in mackie mode : must send midi from jogwheel
#define mackie_encoder_left_zright_z10		(1<<6)	// in mackie mode : jogwheel left=1 right=0
#define i2c_timout_z10						(1<<5)	// 1 if a i2c timeout has occured
#define must_reset_exp_fact_default_z10		(1<<4)	// V1.6 1 if must reset expansions to factory default
#define shift_passed_0_z10					(1<<3)	// set to 1 when shift button passed through 0 V1.62
#define last_value_locked_0_z10				(1<<2)	// set to 1 when last value displayed was "shift lock" bmp V1.62
#define tempo_changed_z10					(1<<1)	// set to 1 when the tempo zone must be updated V1.69
#define realtime_playing_z10				(1<<0)	// set to 1 when START or CONTINUE received V1.7
// #define must_reload_lfo_freq_cpt_z10		(1<<1)	// set to 1 when must reload lfo freq from new pot value bmp V1.64


/*
#define realtime_clock_generated_z10		(1<<7)	// a midi clock has been generated 		DO NOT CHANGE ORDER (must be like z8_status_9)
#define realtime_start_generated_z10		(1<<6)	// a midi start has been generated		DO NOT CHANGE ORDER (must be like z8_status_9)		
#define realtime_stop_generated_z10			(1<<5)	// a midi stop has been generated		DO NOT CHANGE ORDER (must be like z8_status_9)		
#define realtime_continue_generated_z10		(1<<4)	// a midi continue has been generated	DO NOT CHANGE ORDER (must be like z8_status_9)
*/

// Z8_STATUS_11 bits definition V1.8

#define must_change_scene_arp_z11			(1<<7)	// bit 7 : scene select increase
#define must_increase_scene_arp_z11			(1<<6)	// bit 6 : current group increase
#define send_scene_from_prog_ch_z11			(1<<5)	// bit 5 : force sending snapshot instead of arrp scene
#define force_transport_stop_z11			(1<<4)	// a control was programmed to send stop // V1.9
#define force_transport_play_z11			(1<<3)	// a control was programmed to send play // V1.9
#define must_send_smpte_quarter_z11			(1<<2)	// 1: must send new smpte quarter frame
//#define sending_realtime_to_midi_z11		(1<<1)	// will  not check realtime after sending to midi V1.9

//#define Z8_STATUS_TX_REALTIME 				Z8_STATUS_10	

enum {	FCT_CHANNEL, FCT_SCENE, FCT_SCENE_ARP, FCT_GROUP, FCT_UNDEFINED }; // do not change order

enum {	LCD_NO_INCRUST, LCD_INCRUST_SCENE, LCD_INCRUST_JOYSTICK, LCD_INCRUST_CROSSFADER, 
		LCD_INCRUST_INFRARED_HOLD, LCD_INCRUST_EXPANSION, LCD_INCRUST_FULL_LFO, 
		LCD_INCRUST_MIDI_OVERFLOW, LCD_INCRUST_SERIAL_IN_OVERFLOW, LCD_INCRUST_PROG_UPLOAD_RESULT,
	 	LCD_INCRUST_UNVALID_SYSEX, LCD_INCRUST_COMMON_PARAM, LCD_INCRUST_FULL_ARPP, 
		LCD_INCRUST_FULL_ARPP_SHIFT, LCD_INCRUST_FULL_MOTION, LCD_INCRUST_UNVALID_MIDI_STRUCT,
	 	//V1.69
		LCD_INCRUST_TEMPO_ADJUST };

#define CPT_5_SEC			500L
#define CPT_2_5_SEC			250U	
#define CPT_2_SEC			200U
#define CPT_1_SEC			100U
#define CPT_INFINITE		255U	// V1.69

#define FALSE				0U
#define TRUE				1U

//#define MENU_STATE_NB		3U
//enum {NO_MENU, MENU_PARAM, MENU_ASSIGN};
// V1.4
enum { NO_MENU, MENU_PARAM, MENU_ASSIGN, MENU_BS3X_INFOS, MENU_STATE_NB };

#define PARAM_STRUCT_IN_RAM_MAX_NB	8U	// V1.61 : max nb of param struct in ram

#endif
