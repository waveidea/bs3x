#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "automation.h"
#include "analog.h"
#include "eeprom.h"
#include "itrs.h"
#include "lcd.h"
#include "lfo.h"
#include "assign.h"
#include "arpp.h"
#include "macros.h"
#include "motion.h"
#include "hardware.h"
#include "serial.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

far uchar physical_autom_pot[(AUTOMATION_POTS_NB<<2)]; // 3 autom pots, 4 functions each
uchar autom_pot[AUTOMATION_POTS_NB]; // 3 autom pots

extern uchar current_value;				// current value
extern uchar control_index;				// current control nb

extern uint EE_Address;					// current EEProm address
extern uchar I2cCommand;					// current EEProm selection
extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_6;
extern uchar Z8_STATUS_7;
extern uchar Z8_STATUS_9;

extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern uchar control_assign_index;
extern uchar assign_state;

extern uchar   		arp_ref_note; // midi input note value  (ref_note = 0 is used to disable arp)
extern uchar	    arp_clock_counter;
extern uchar		arp_ref_note; // midi input note value  (ref_note = 0 is used to disable arp)
extern rom uchar 	table_clock_resolution[ARP_CLK_RESO_NB];

extern far uchar Analog[ANALOG_SOURCES_NB];
extern volatile uchar index_analog_itr;
extern far 	Motion_Struct Motion_Str[MOTION_PART_NB];
extern uchar 	Motion_Status_Play; 	// each bit indicates if part's playing
extern uchar 	Motion_Status_Rec; 		// each bit indicates if part's recording
//extern uchar motion_sw_type_long_push;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckAutomationAnalog()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check all 3 pots for automation purpose
//				The 3 automation pots are always in HOOK mode
///////////////////////////////////////////////////////////////////////////////////////
void CheckAutomationAnalog(void)
{
	uchar tmp_value, hook_bmp_index, local_physical, i;
	uchar must_update_lcd = FALSE;

	// Ignore whole automations analog if the motion rec has started
	if((Motion_Status_Rec) && (Motion_Str[motion_part].motion_ctrl_index != CTRL_UNDEF))
		return;

	////////////////////////////////////////////////////////////////////////////////////
	////////////////////  Check if Automation pot 0 has changed ////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	// Get7BitsADValue(AUTOMATION_POT_1_AD_CHANNEL, 0U);
	current_value = Analog[AUTOM_POT_0_ANALOG_INDEX];

	if(Z8_STATUS_4 & first_pass_z4)
	{	
		autom_pot[0] = current_value | HOOK_BIT;
		// initialize automation reg physical register to physical value, sign is 0 (lower or equal)
		for(i=0; i < ((AUTOMATION_NB+1)*AUTOMATION_POTS_NB); i=i+3 )
			physical_autom_pot[i] = current_value;
	}
	else
	{
		// IF UNDER TEST : send a specific small sysex
		if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
		{
			if(current_value != (autom_pot[0] & 0x7F))
			{
				control_index = LFO_CTRL_INDEX;
				SendControlInAutotestMode(midi_out_2_en);
				autom_pot[0] = current_value;
				return;
			}
		}
	
		// check which is the current motion selected
		// MOTION SAMPLER is active
		///////////////////////////////////////////////////////
		if(current_autom == AUTOMATION_MOTION)
		{
		}
		else
		// ARPP is active
		///////////////////////////////////////////////////////
		if(current_autom == AUTOMATION_ARPP)
		{
			if(Z8_STATUS_9 & shift_state_z9)
			{
				if(current_value != (autom_pot[0] & 0x7F))
				{
					local_physical = physical_autom_pot[PHYSICAL_ARP_RANGE];
					hook_bmp_index = CheckHookPot(&autom_pot[0], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
						autom_pot[0] = current_value | HOOK_BIT;
						tmp_value = current_value >> 3U;
						if(arp_range != tmp_value)
						{
							arp_range = tmp_value;
							must_update_lcd = TRUE;	
						}
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			} // end shift active
			else
			{
				if(current_value != (autom_pot[0] & 0x7F))
				{
					local_physical = physical_autom_pot[PHYSICAL_ARP_GATE];
					hook_bmp_index = CheckHookPot(&autom_pot[0], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
						autom_pot[0] = current_value | HOOK_BIT;
						tmp_value = (uchar)((((uint)table_clock_resolution[arp_clock_resolution] * (uint)(current_value)) >> (int) 7)& 0x7F);
						if(arp_gate != tmp_value)
						{
							if(arp_clock_counter >= arp_gate)
							{
		       					if(arp_ref_note) 
									mute_note(); // send note off message 
	   						}
							arp_gate = tmp_value; 
							must_update_lcd = TRUE;	
						}
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
	
			// Update display with modified arpeggio param 
			if(must_update_lcd)
				DisplayFullArppLcdIncrust(FORCE_GATE_RANGE_DISPLAY);	
		}
		else
		// LFO is active : change here LFO Amplitude value 0-63
		///////////////////////////////////////////////////////
		{
			if((current_value != 127) && (!(Z8_STATUS_4 & first_pass_z4)))
			   	current_value = ((autom_pot[0] & 0x7F) + current_value) >> 1U;
	
			if(current_value != (autom_pot[0] & 0x7F))
			{
				// are we in assign mode
				if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
					  control_assign_index = LFO_CTRL_INDEX;
				else
				if(assign_state != ASSIGN_ACTIVE)
				{
					local_physical = physical_autom_pot[PHYSICAL_LFO_AMPL];
					hook_bmp_index = CheckHookPot(&autom_pot[0], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
						autom_pot[0] = current_value | HOOK_BIT;
						tmp_value = current_value >> 1U;
						if(LFO_Ampl != tmp_value)
						{
							LFO_Ampl = tmp_value;
							DisplayFullLfoLcdIncrust(FORCE_AMPL_DISPLAY);
						}
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	////////////////////  Check if Automation pot 1 has changed ////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	// Get7BitsADValue(AUTOMATION_POT_2_AD_CHANNEL, 0);
	current_value = Analog[AUTOM_POT_1_ANALOG_INDEX];

	if(Z8_STATUS_4 & first_pass_z4)
	{	
		autom_pot[1] = current_value | HOOK_BIT;
		// initialize automation reg physical register to physical value, sign is 0 (lower or equal)
		for(i=1; i < ((AUTOMATION_NB+1)*AUTOMATION_POTS_NB)+1; i=i+3 )
			physical_autom_pot[i] = current_value;
	}
	else
	{
		// IF UNDER TEST : send a specific small sysex
		if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
		{
			if(current_value != (autom_pot[1] & 0x7F))
			{
				control_index = LFO_CTRL_INDEX+1;
				SendControlInAutotestMode(midi_out_2_en);
				autom_pot[1] = current_value;
				return;
			}
		}

		// check which is the current motion selected
		if(current_autom == AUTOMATION_MOTION)
		{
			// V1.7 Allow changing part lenght only if motion part is undefined
			if((Motion_Status_Rec) && (Motion_Str[motion_part].motion_ctrl_index == CTRL_UNDEF))
			{
				// Motion sampler is active : change here current motion part length
				{
					if(current_value != (autom_pot[1] & 0x7F))
					{
						// are we in assign mode
						if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
							  control_assign_index = LFO_CTRL_INDEX;
						else
						if(assign_state != ASSIGN_ACTIVE)
						{
							local_physical = physical_autom_pot[PHYSICAL_MOTION_LENGTH];
							hook_bmp_index = CheckHookPot(&autom_pot[1], local_physical, current_value);
							if(hook_bmp_index == NO_HOOK_BMP)
							{
								Motion_Str[motion_part].motion_length = current_value >> 4; // motion lenght is 0-7
								autom_pot[1] = current_value | HOOK_BIT;
								DisplayFullMotionLcdIncrust(FORCE_MOTION_LENGTH_DISPLAY);
							}
							UpdateLcdHookState(hook_bmp_index);
						}
					}
				}
			} // end the motion part was undefined
		}	// end current function is motion
		else
		if(current_autom == AUTOMATION_ARPP)
		{
			if(Z8_STATUS_9 & shift_state_z9)
			{
				if(current_value != (autom_pot[1] & 0x7F))
				{
					local_physical = physical_autom_pot[PHYSICAL_ARP_TRANSPOSE];
					hook_bmp_index = CheckHookPot(&autom_pot[1], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
				  		if((arp_ref_note) && (arp_clock_counter <= arp_gate))
				   	   		mute_note(); //tbd should I wait for arp_gate time before muting ???
				   		arp_transpose = current_value;
						autom_pot[1] = current_value | HOOK_BIT;
						must_update_lcd = TRUE;	
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
			else
			{
				if(current_value != (autom_pot[1] & 0x7F))
				{
					local_physical = physical_autom_pot[PHYSICAL_ARP_ACCENT];
					hook_bmp_index = CheckHookPot(&autom_pot[1], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
				   		arp_accent = current_value; 
						autom_pot[1] = current_value | HOOK_BIT;
						must_update_lcd = TRUE;	
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
	
			// Update display with modified arpeggio param 
			if(must_update_lcd)
				DisplayFullArppLcdIncrust(FORCE_ACC_TRANSP_DISPLAY);	
		}
		else
		// LFO is active : change here LFO Frequency value
		{
			if((current_value != 127) && (!(Z8_STATUS_4 & first_pass_z4)))
			   	current_value = ((autom_pot[1] & 0x7F) + current_value) >> 1;
	
			if(current_value != (autom_pot[1] & 0x7F))
			{
				// are we in assign mode
				if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
					  control_assign_index = LFO_CTRL_INDEX;
				else
				if(assign_state != ASSIGN_ACTIVE)
				{
					local_physical = physical_autom_pot[PHYSICAL_LFO_FREQ];
					hook_bmp_index = CheckHookPot(&autom_pot[1], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
/*						// LFO was internally synched
						if((autom_pot[1] & 0x7F) > 63)
						{
							// is it now externaly synched ?
							if(current_value < 64)
								// it is automatically stopped until a MIDI start is received
								Z8_STATUS_5 &= ~lfo_play_zstop_z5;
						}
*/						// lfo freq is 0-63 while pot is 0-127
						// V1.64 LFO_Freq = current_value >> 4; 
						// V1.7 LFO_Freq = current_value >> 1; 
						// V1.7 convert 0-127 to 0-15
						LFO_Freq = current_value >> 3; 
						autom_pot[1] = current_value | HOOK_BIT;
						// V1.64 ReloadLfoFreqCpt();
						// V1.64 Z8_STATUS_10 |= must_reload_lfo_freq_cpt_z10;
						DisplayFullLfoLcdIncrust(FORCE_FREQ_DISPLAY);
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	////////////////////  Check if Automation pot 2 has changed ////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	// Get7BitsADValue(ROW0, 0);
	current_value = Analog[AUTOM_POT_2_ANALOG_INDEX];

	if(Z8_STATUS_4 & first_pass_z4)
	{	
		autom_pot[2] = current_value | HOOK_BIT;
		// initialize automation reg physical register to physical value, sign is 0 (lower or equal)
		for(i=2; i < ((AUTOMATION_NB+1)*AUTOMATION_POTS_NB)+2; i=i+3 )
			physical_autom_pot[i] = current_value;
	}
	else
	{
		// IF UNDER TEST : send a specific small sysex
		if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
		{
			if(current_value != (autom_pot[2] & 0x7F))
			{
				control_index = LFO_CTRL_INDEX+2;
				SendControlInAutotestMode(midi_out_2_en);
				autom_pot[2] = current_value;
				return;
			}
		}

		if(current_autom == AUTOMATION_MOTION)
		{
			// Motion sampler is active : change here current motion part
			{
				// allow motion part chaning only if not recording 
				if(!(Motion_Status_Rec))
				{
					if(current_value != (autom_pot[2] & 0x7F))
					{
						// are we in assign mode
						if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
							  control_assign_index = LFO_CTRL_INDEX;
						else
						if(assign_state != ASSIGN_ACTIVE)
						{
							local_physical = physical_autom_pot[PHYSICAL_MOTION_PART];
							hook_bmp_index = CheckHookPot(&autom_pot[2], local_physical, current_value);
							if(hook_bmp_index == NO_HOOK_BMP)
							{
								motion_part = current_value >> 4; // motion part is 0-7
								autom_pot[2] = current_value | HOOK_BIT;
								DisplayFullMotionLcdIncrust(FORCE_MOTION_PART_DISPLAY);
							}
							UpdateLcdHookState(hook_bmp_index);
						}
					}
				}
			}
		}
		else
		if(current_autom == AUTOMATION_ARPP)
		{
			if(Z8_STATUS_9 & shift_state_z9)
			{
				if(current_value != (autom_pot[2] & 0x7F))
				{
					local_physical = physical_autom_pot[PHYSICAL_ARP_MELODY];
					hook_bmp_index = CheckHookPot(&autom_pot[2], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
						autom_pot[2] 	= current_value | HOOK_BIT;
						tmp_value = current_value >> 2U;
						if(tmp_value >= MAX_MELODIES)
							tmp_value = MAX_MELODIES-1;
						if(arp_melody != tmp_value)
						{
							// reset parameters to avoid endless notes
							if((arp_ref_note) && (arp_clock_counter <= arp_gate)) 
								mute_note(); //tbd should I wait for arp_gate time before muting ???
	  						set_direction(); // reset arp_index
	  						//NB we don't reset pulse_index here... tbd
			   				arp_melody 		= tmp_value;
							must_update_lcd = TRUE;	
						}
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
			else
			{
				if(current_value != (autom_pot[2] & 0x7F))
				{
					local_physical = physical_autom_pot[PHYSICAL_ARP_RYTHM];
					hook_bmp_index = CheckHookPot(&autom_pot[2], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
						arp_rhythm      = current_value; 
						autom_pot[2]    = current_value | HOOK_BIT; 
						must_update_lcd = TRUE;	
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
	
			// Update display with modified arpeggio param 
			if(must_update_lcd)
				DisplayFullArppLcdIncrust(FORCE_RYTHM_MELODY_DISPLAY);	
	
		}
		else
		// LFO is active : change here LFO offset value 0-63
		{
			if(current_value != (autom_pot[2] & 0x7F) )
			{
				// are we in assign mode
				if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
					  control_assign_index = LFO_CTRL_INDEX;
				else
				if(assign_state != ASSIGN_ACTIVE)
				{
					local_physical = physical_autom_pot[PHYSICAL_LFO_OFFSET];
					hook_bmp_index = CheckHookPot(&autom_pot[2], local_physical, current_value);
					if(hook_bmp_index == NO_HOOK_BMP)
					{
						autom_pot[2] = current_value | HOOK_BIT; 
						tmp_value = (current_value >> 1) + LFO_MIN_OFFSET;
						if(LFO_Offset != tmp_value)
						{
							LFO_Offset = tmp_value;
							DisplayFullLfoLcdIncrust(FORCE_OFFSET_DISPLAY);
						}
					}
					UpdateLcdHookState(hook_bmp_index);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckAutomationSw()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check all automation sw and start actions
///////////////////////////////////////////////////////////////////////////////////////
void CheckAutomationSw(void)
{
	uchar pin_state, mux_port_out_local;

	MAC_DISABLE_ITR_ADC
	mux_port_out_local = index_analog_itr;
	pin_state = PARAM_SW_1_IN & PARAM_SW_1_PIN;
	MAC_ENABLE_ITR_ADC

	if(mux_port_out_local == AUTOM_SW_TYPE)
	{
		// check if sw is pressed
		if(pin_state)
		{
			if(Z8_STATUS_7 & autom_sw_type_passed_0_z7)
			{
//				Z8_STATUS_9 |=  autom_sw_type_passed_1_z9;
				Z8_STATUS_7 &= ~autom_sw_type_passed_0_z7;	
				AutomationSwTypePressed();	
			}
		}
		else 
//		{
			Z8_STATUS_7 |= autom_sw_type_passed_0_z7;
			// short push detection for motion sw type
/*			if(current_autom == AUTOMATION_MOTION)
				if(Z8_STATUS_1_NVM & motion_state_z1nvm)
					if(!(Motion_Status_Rec)) // & (1 << motion_part)))
						if(Z8_STATUS_9 & autom_sw_type_passed_1_z9)
						{
							Z8_STATUS_9 &= ~autom_sw_type_passed_1_z9;
							motion_sw_type_long_push = 0;
							// launch a play/stop motion on selected part
							Motion_Status_Play ^= (1 << motion_part);
							DisplayFullMotionLcdIncrust(FORCE_MOTION_STATES_DISPLAY);
						}
		}
*/	}
	else
	if(mux_port_out_local == AUTOM_SW_SEL)
	{
	   	if(pin_state)
		{
			if(Z8_STATUS_7 & autom_sw_sel_passed_0_z7)
			{
				AutomationSwSelPressed();
				Z8_STATUS_7 &= ~autom_sw_sel_passed_0_z7;	
			}
		}
		else Z8_STATUS_7 |= autom_sw_sel_passed_0_z7;
	}

	else
	if(mux_port_out_local == AUTOM_SW_ON)
	{
	  	if(pin_state)
		{
			if(Z8_STATUS_6 & autom_sw_on_passed_0_z6)
			{
				AutomationSwOnOffPressed();
				Z8_STATUS_6 &= ~autom_sw_on_passed_0_z6;	
			}
		}
		else Z8_STATUS_6 |= autom_sw_on_passed_0_z6;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateAutomationLeds()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	depending on automations selected : show current automation led
///////////////////////////////////////////////////////////////////////////////////////
void UpdateAutomationLeds(void)
{
	uchar tmp ; //= 0x20;
	uchar index;

	// do not change LEDs states if the unit is in autotest mode
	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
		return;

	// Here update automations leds : PE5, PE6, PE7
//	for(index=0; index < current_autom ; index++)
//		tmp = tmp << 1;

	if(current_autom == AUTOMATION_MOTION)
		tmp = 0x20;
    else
	if(current_autom == AUTOMATION_LFO)
		tmp = 0x80;
	else
		tmp = 0x40;

	// Clear previous selected automation led (bits 5, 6, 7)
	LED_CURRENT_AUTOM_PORT_OUT &= 0x1F;

	// Update current selected automation led
	LED_CURRENT_AUTOM_PORT_OUT |= tmp;

	// turnoff automation led if all 3 automs are off
	if(!(Z8_STATUS_1_NVM & motion_state_z1nvm))
	{
		// V1.5 if(!(CTRL_REG_1_7BITS_TO_EZ & arpp_state_z_ez1))
		if(!(Z8_STATUS_0_NVM & arpp_state_z0nvm))
		{
			if(!(Z8_STATUS_1_NVM & lfo_state_z1nvm))
			{
				LED_AUTOMATION_PORT_OUT &= ~LED_AUTOMATION_PIN;
				return;
			}
		}
	}

	// otherwise turnoff automation led
	LED_AUTOMATION_PORT_OUT |= LED_AUTOMATION_PIN;	
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	AutomationSwTypePressed()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	different behaviours according to selected automation
//				motion 	: short push : start/stop playback / long push : start record
//				lfo		: short push only : current wave is updated (shift for direction)
//				arp 	: to define with fred
///////////////////////////////////////////////////////////////////////////////////////
void AutomationSwTypePressed(void)
{
	uchar tmp;
//	rom uchar * local_bmp_ptr;

	// V1.4 : If autotest mode send the apprpriate sysex
	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
	{
		control_index = AUTOTEST_AUTOM_SW_TYPE_INDEX;
		SendControlInAutotestMode(midi_out_2_en);
		return;
	}

	// check which is the current motion selected
	if(current_autom == AUTOMATION_MOTION)
	{
		// depending on push type (no shift / shift), then play/record automation
		if(Z8_STATUS_1_NVM & motion_state_z1nvm)
		{
			// SHIFT IS ACTIVE ?
			//if(motion_sw_type_long_push >= CPT_2_SEC)
			tmp = 1 << motion_part;
			if((Z8_STATUS_9 & shift_state_z9)&&(!Motion_Status_Rec))
			{
				// A RECORD MOTION IS REQUESTED
				//motion_sw_type_long_push = 0;
				// launch a record on selected motion part
				Motion_Status_Rec  |=  tmp;
				Motion_Status_Play &= ~tmp;
				// reset the control index to record motion
				Motion_Str[motion_part].motion_ctrl_index = CTRL_UNDEF; 
				DisplayFullMotionLcdIncrust(FORCE_MOTION_REC_DISPLAY);
			}
			else
			{
				// A PLAY MOTION IS REQUESTED
				// V1.7 reset all indexes to zero when play is requested
				Motion_Str[motion_part].motion_data_index 		= 0;
				Motion_Str[motion_part].motion_last_data_sent 	= CTRL_UNDEF;
				Motion_Str[motion_part].motion_clock_counter 	= 0;
				// end v1.7
				// rearm flag for motion sw type only
				// Z8_STATUS_7 |= autom_sw_type_passed_0_z7;
				if(!(Motion_Status_Rec)) // & (1 << motion_part)))
				{
					//Z8_STATUS_9 &= ~autom_sw_type_passed_1_z9;
					//motion_sw_type_long_push = 0;
					// launch a play/stop motion on selected part
					Motion_Status_Play ^= tmp;
					DisplayFullMotionLcdIncrust(FORCE_MOTION_STATES_DISPLAY);
				}
			}
		}
	}
	else
	if(current_autom == AUTOMATION_ARPP)
	{
		// shift activates arp mode
		if(Z8_STATUS_9 & shift_state_z9)
		{
			arp_mode++;
			if(arp_mode >= ARPP_MODE_NB) 
			   arp_mode	= 0;

			// reinit arpp according to new mode
			set_mode();

   			// Update display with modified arpeggio param 
			DisplayFullArppLcdIncrust(FORCE_ARP_MODE_DISPLAY);
		}
		else
		{
			// no shift must increase arp dir
			arp_dir++;
			if(arp_dir >= ARP_DIR_MAX) 
			   arp_dir	= 0;
	
	   		set_direction(); // reset arp_index
		
			// Update display with modified arpeggio param 
			DisplayFullArppLcdIncrust(FORCE_ARP_DIR_DISPLAY);	
   		}
		
	}
	else
	// LFO is active : change here selected waveform type
	{
		// tmp = (Z8_STATUS_2_NVM & LFO_WAVE_MSK_Z2_NVM) >> LFO_WAVE_MSK_POS;
		tmp = (Z8_STATUS_2_NVM >> LFO_WAVE_MSK_POS) & (LFO_WAVE_NB - 1);
		// depending on SHIFT : increase or decrease
		if(Z8_STATUS_9 & shift_state_z9)
			tmp--;
		else
			tmp++;
		
		tmp &= (LFO_WAVE_NB - 1);
		
		// reaffect new wave to control register
		Z8_STATUS_2_NVM &= ~LFO_WAVE_MSK_Z2_NVM;
		Z8_STATUS_2_NVM = Z8_STATUS_2_NVM | ((uchar)(tmp) << LFO_WAVE_MSK_POS);

		// Update display with modified LFO wave selection 
		DisplayFullLfoLcdIncrust(FORCE_WAV_DISPLAY);	
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	AutomationSwOnOffPressed()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
///////////////////////////////////////////////////////////////////////////////////////
void AutomationSwOnOffPressed(void)
{
	uint lcd_zone_to_update=0;

	// V1.4 : If autotest mode send the apprpriate sysex
	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
	{
		control_index = AUTOTEST_AUTOM_SW_ON_INDEX;
		SendControlInAutotestMode(midi_out_2_en);
		return;
	}

	// check which is the current motion selected
	if(current_autom == AUTOMATION_MOTION)
	{
		// toggle the motion state only if not recording
		if(!Motion_Status_Rec)
		{
			// MOTION is active, toggle its state
			Z8_STATUS_1_NVM ^= motion_state_z1nvm;
			lcd_zone_to_update = (uint)INIT_LCD_MOTION_STATE;
		}
	}
	else
	if(current_autom == AUTOMATION_ARPP)
	{
		// ARPP is active, turn it on/off
		// V1.5 CTRL_REG_1_7BITS_TO_EZ ^= arpp_state_z_ez1;
		Z8_STATUS_0_NVM ^= arpp_state_z0nvm;
		lcd_zone_to_update = (uint)INIT_LCD_ARPP_STATE;
		// indicate to EZ board to formward MIDI datas
		CheckIfEzMustForwardMidiToZ8();
	}
	else
	{
		// LFO is active, turn it on/off
		Z8_STATUS_1_NVM ^= lfo_state_z1nvm;
		// if now on, reload frequency counter
		if(Z8_STATUS_1_NVM & lfo_state_z1nvm)
			ReloadLfoFreqCpt();
		lcd_zone_to_update = (uint)INIT_LCD_LFO_WAVE_STATE;
	}

	UpdateAutomationLeds();

	// Update display with modified automation
	UpdateLcdZones(lcd_zone_to_update);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	AutomationSwSelPressed()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
///////////////////////////////////////////////////////////////////////////////////////
void AutomationSwSelPressed(void)
{
	uchar old_automation;

	// V1.4 : If autotest mode send the apprpriate sysex
	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
	{
		control_index = AUTOTEST_AUTOM_SW_SEL_INDEX;
		SendControlInAutotestMode(midi_out_2_en);
		return;
	}

	// if we are currently record a motion exit
	if(Motion_Status_Rec) return;

	old_automation = current_autom;

	// depending on SHIFT : increase or decrease
	if(Z8_STATUS_9 & shift_state_z9)
	{
		// need to decrease
//		if(current_autom)
//		   current_autom--;
//		else
//		   current_autom = AUTOMATION_NB-1;	
		if(current_autom == AUTOMATION_LFO)
			current_autom = AUTOMATION_ARPP;
		else
		if(current_autom == AUTOMATION_ARPP)
			current_autom = AUTOMATION_MOTION;
	    else
			current_autom = AUTOMATION_LFO;
	}
	else
	{
		// current_autom++;
		//if(current_autom == AUTOMATION_NB)
		//   current_autom = 0;
		if(current_autom == AUTOMATION_LFO)
			current_autom = AUTOMATION_MOTION;
		else
		if(current_autom == AUTOMATION_MOTION)
			current_autom = AUTOMATION_ARPP;
	    else
			current_autom = AUTOMATION_LFO;
	}

	// the selected automation has changed : UN-HOOK all 3 automation pots ()
	UnhookAutomationPots(old_automation, current_autom);
	
	UpdateAutomationLeds();
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UnhookAutomationPots()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
///////////////////////////////////////////////////////////////////////////////////////
void UnhookAutomationPots(uchar old_automation, uchar new_automation)
{
	uchar i;
	uchar old_automation_pot_index, new_automation_pot_index;

	// get the old automation pot index
	old_automation_pot_index = old_automation  * AUTOMATION_POTS_NB;
	new_automation_pot_index = new_automation  * AUTOMATION_POTS_NB;

	for(i=0; i < AUTOMATION_POTS_NB; i++)
	{
		// store old physical positions to physical regs if already hooked
		if(autom_pot[i] & 0x80)
			physical_autom_pot[old_automation_pot_index+i] = autom_pot[i] & 0x7F;

		// now update new group sign and store it to physical registers 
		if( (autom_pot[i] & 0x7F) <= (physical_autom_pot[new_automation_pot_index+i] & 0x7F) )
  			 physical_autom_pot[new_automation_pot_index+i] &= 0x7F;
		else
			 physical_autom_pot[new_automation_pot_index+i] |= 0x80;

		// and un-hook all pots
		autom_pot[i] &= (~HOOK_BIT);
	}
}
