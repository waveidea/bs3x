#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "digital.h"
#include "lcd.h"
#include "serial.h"
//#include "analog.h"
#include "eeprom.h"
#include "hardware.h"
#include "assign.h"  
#include "macros.h"  
#include "realtime.h" 
 
///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar transport_state;
uchar next_transport_state;
uchar whole_sw_modes[NB_BYTES_FOR_SW];
uchar whole_sw_toggled[NB_BYTES_FOR_SW];
uchar transport_passed_0;

// V1.4
/*
rom uchar reorg_other_sw[8] = {		REC_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX, 
									PLAY_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX, 
									BW_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX,
									STOP_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX,
									FW_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX,
									JOYSTICK_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX,
									FOOTSWITCH_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX,
									ENCODER_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX
									};
*/
rom uchar reorg_other_sw[TRANSPORT_NB] = {	REC_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX, 
											PLAY_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX, 
											BW_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX,
											STOP_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX,
											FW_ASSIGN_SW_CTRL_INDEX-BW_ASSIGN_SW_CTRL_INDEX
											};

extern uchar control_index;				// current control nb
extern uchar last_control_index;		// current control nb
extern All_Controls Ctrl;				// All controls are defned here
extern uchar current_value;				// current control value
extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_9;
extern uchar Z8_STATUS_11; // V1.9
extern far uchar realtime_generated_buffer[REALTIME_BUFFER_LENGHT];
extern uchar realtime_generated_end_index;

extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eeprom saving
extern uchar menu_state;
extern uchar control_assign_index;
extern uchar assign_state;
extern volatile uchar index_analog_itr;
//extern uchar pin_state_param_sw0_itr;
//extern uchar pin_state_param_sw1_itr;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckTransport()
// Inputs	: None
// Outputs	: None
// Process  : Check transport functions from switches
///////////////////////////////////////////////////////////////////////////////////////
void CheckTransport(void)
{
	uchar Mask, /* index, */ mux_port_out_local, pin_state;

	next_transport_state = CTRL_UNDEF;

	// V1.9, play/stop can be trigged from controls programmed with start/stop
	// only available in user/mackie/logic modes
	if(OPERATING_MODE_NVM >= MACKIE_MODE)
	{
		if(Z8_STATUS_11 & force_transport_play_z11)
		{
			next_transport_state = TRANSPORT_PLAY;
			current_value = 127;
			Z8_STATUS_11 &= ~force_transport_play_z11;
		}
		else
		if(Z8_STATUS_11 & force_transport_stop_z11)
		{
			next_transport_state = STOP_CTRL_INDEX;
			// will not pass through pause (play->stop)
			transport_state = TRANSPORT_PAUSE;
			current_value = 127;
			Z8_STATUS_11 &= ~force_transport_stop_z11;
		}
	}

	// still undefeined next transport state ?
	if(next_transport_state == CTRL_UNDEF)
	{
		// Transport are only activated when SHIFT NOT ACTIVE
		if(Z8_STATUS_9 & shift_state_z9) return; 

		// check if sw is pressed
		MAC_DISABLE_ITR_ADC
		mux_port_out_local = index_analog_itr;
		pin_state = PARAM_SW_1_IN & PARAM_SW_1_PIN;			
		MAC_ENABLE_ITR_ADC

		// check transport muxes 3 to 7
		if(mux_port_out_local >=  TRANSPORT_MUX_OFFSET) 
		{
			// Mask = (uchar)((1U << TRANSPORT_MUX_OFFSET) << index); 
			Mask = 1 << mux_port_out_local;
			//next_transport_state = CTRL_UNDEF;

			if(pin_state)
			{
				// V1.3 transport also send value : ONLY THIS VALUE, never 0
				current_value = 127;
				// sw already passed trhough 0 ?
				if(transport_passed_0 & Mask)
				{
					// next_transport_state = index; // 3-7 becomes 0-4
					next_transport_state = mux_port_out_local - TRANSPORT_MUX_OFFSET;
					transport_passed_0 &= ~Mask;
				}
			}
			else
			{
				transport_passed_0 |= Mask;
			}
		}		
	}
	
	// send to midi only if transport state changed Check if transport state changed
	if(next_transport_state != CTRL_UNDEF)
	{
		control_index = REC_CTRL_INDEX + next_transport_state;	
	
		// are we in assign mode
		if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
			  control_assign_index = control_index;
		else
		if(assign_state != ASSIGN_ACTIVE)
		{	
			// if internal sync is on : transports also send MIDI_START, MIDI_STOP
			if(CTRL_REG_1_7BITS_TO_EZ & realtime_source_int_z_ez1)
			{
				// START pressed
				if(next_transport_state == PLAY_CTRL_INDEX)
				{
					// V1.8 : send start event only in midi clock mode
					if(!(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm))
						SendToTx0(MIDI_START);
					// signal START to arpp if arp is on
//							if(CTRL_REG_1_7BITS_TO_EZ & arpp_state_z_ez1)
//							Z8_STATUS_9 |= realtime_start_received_z9;
					// Z8_STATUS_10 |= realtime_start_generated_z10;
					realtime_generated_buffer[realtime_generated_end_index] = MIDI_START;
					realtime_generated_end_index = (realtime_generated_end_index + 1) & (REALTIME_BUFFER_LENGHT - 1);

				}
				else
				// STOP pressed
				if(next_transport_state == STOP_CTRL_INDEX)
				{
					// V1.8 : send stop event only in midi clock mode
					if(!(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm))
						SendToTx0(MIDI_STOP);
					// signal STOP to arpp if arp is on
//							if(CTRL_REG_1_7BITS_TO_EZ & arpp_state_z_ez1)
//								Z8_STATUS_9 |= realtime_stop_received_z9;
					// Z8_STATUS_10 |= realtime_stop_generated_z10;
					realtime_generated_buffer[realtime_generated_end_index] = MIDI_STOP;
					realtime_generated_end_index = (realtime_generated_end_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
				}

				// V1.8 if midi sync is configured as SMPTE
				if(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm)
					// update SMPTE state according to new transport button pressed
					UpdateSmpteState(next_transport_state);

			} // end internal sync is on

			// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill struct from V1.8
			FillParamCurrentStruct();

			// Then Send MIDI event : different from all ctrls since transport only used in USER mode
			SendControlToMidi();

			// update lcd for START STOP REC PAUSE, ignore FW BW
			if( (next_transport_state != BACKWARD_CTRL_INDEX) && (next_transport_state != FORWARD_CTRL_INDEX))
				 UpdateLcdTransport();

			last_control_index = control_index;
		} // end not in assign mode
	}
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckSendDisplayCurrentSw()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check, send and display a single sw, generic function called by all 16 sw
//				Also manage the Toggle/Push modes
//			  	Need to recalculate index for display a transport_assign sw since not connected
//			  	in the same way on front panel and 4051 mux 
///////////////////////////////////////////////////////////////////////////////////////
// void CheckSendDisplayCurrentSw(unsigned char volatile far *port, uchar pin, uchar sw_id, uchar index)
void CheckSendDisplayCurrentSw(uchar pin_state, uchar sw_id, uchar index)
{
	uchar Mask = 1U << index;
	uchar switch_changed = FALSE;
	uchar *current_sw_reg;

	if(sw_id == OTHER_SW_ID)
		current_sw_reg	= &Ctrl.Ctrl_sc.other_sw_value;
	else
		current_sw_reg	= &Ctrl.Ctrl_sc.all_sw_value;

	// switches are considered as pots
	current_value = 0;

	// Toggle mode ? 
	// V1.7 : never use toggle switch in reason mode
	if( (whole_sw_modes[sw_id] & Mask) && (OPERATING_MODE_NVM != REASON_MODE))
	{
		if(pin_state)									      	// Switch high ?
	    {
			if(whole_sw_toggled[sw_id] & Mask)				  	// Switch passed through 0 ?
			{
				*current_sw_reg ^= Mask;   						// Toggle switch value
				whole_sw_toggled[sw_id] &= (~Mask);    		  	// Need to repass through 0
				switch_changed = TRUE;				         	// Need to send it out to MIDI
				if(*current_sw_reg & Mask)  					// What is the value to Send ?
					current_value = 127;
			}
	    }
	    else											        // Switch is currently low
	    	whole_sw_toggled[sw_id] |= Mask;					// then switch passed through 0
	 } // end toggle button mode
	 else
	 {
	 	// PushButton Mode
		if(!(*current_sw_reg & Mask))
		{ 	
			// sw was 0, and now it is 1
			if(pin_state)
			{
				current_value = 127;
				switch_changed = TRUE;
				*current_sw_reg |= Mask;
			}
		 }
		 else
		 {
			// sw was 1, and now it is 0
			if(!(pin_state))
			{
				switch_changed = TRUE;
				*current_sw_reg &= (~Mask);
			}
		 } // end sw was 1
	} // end push button mode

	// If switch has changed, send it to midi
	if((switch_changed)&&(!(Z8_STATUS_4 & first_pass_z4)))
	{
		// are we in assign mode
		if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
			  control_assign_index = control_index;
		else
		if(assign_state != ASSIGN_ACTIVE)
		{
			// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill struct from V1.8
			FillParamCurrentStruct();
			// Send pot to midi		
			SendControlToMidi();	
		
			if(last_control_index != control_index)
			{
					// Update lcd str
					UpdateLcdString();
	
					last_control_index = control_index;
			}
	
			if(sw_id == ALL_SW_ID)
				UpdateSwState(index);
			else
				UpdateOtherSwState(index);
				// V1.4 UpdateOtherSwState(reorg_other_sw[index]);
	
			DisplayCurrentValue();
		} // end not in assign mode
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckSwitches()
// Inputs	: None
// Outputs	: None
// Process  : Check all 8 programmables switches
///////////////////////////////////////////////////////////////////////////////////////
void CheckSwitches(void)
{
	uchar index;
	uchar mux_port_out_local, pin_state;

	MAC_DISABLE_ITR_ADC
	mux_port_out_local = index_analog_itr;		
	pin_state = PARAM_SW_0_IN & PARAM_SW_0_PIN;		
	MAC_ENABLE_ITR_ADC

	// First switch is control index ;nb
	control_index = SWITCH_BASE_CTRL_INDEX + mux_port_out_local;
	// check if sw changed
	CheckSendDisplayCurrentSw(pin_state, ALL_SW_ID, mux_port_out_local);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckOtherSwitches()
// Inputs	: None
// Outputs	: None
// Process  : Check Footswitch, encoder switch and Joystick switch, and transport_assign if shift active
///////////////////////////////////////////////////////////////////////////////////////
void CheckOtherSwitches(void)
{
	uchar index;
	uchar mux_port_out_local, pin_state;

	// first check all 5 assigned transport switches
	// only if SHIFT ACTIVE
	if(Z8_STATUS_9 & shift_state_z9)
	{
			MAC_DISABLE_ITR_ADC
			mux_port_out_local = index_analog_itr;			
			pin_state = PARAM_SW_1_IN & PARAM_SW_1_PIN;			
			MAC_ENABLE_ITR_ADC

			// check transport muxes 3 to 7
			if(mux_port_out_local >=  TRANSPORT_MUX_OFFSET) 
			{
				// First other switch is BAWKWARD 
				control_index = BW_ASSIGN_SW_CTRL_INDEX + (mux_port_out_local - TRANSPORT_MUX_OFFSET);

				// check if sw changed
				// CheckSendDisplayCurrentSw(&PARAM_SW_1_IN, PARAM_SW_1_PIN, OTHER_SW_ID, index);
				//V1.4 CheckSendDisplayCurrentSw(pin_state, OTHER_SW_ID, mux_port_out_local - TRANSPORT_MUX_OFFSET);
				CheckSendDisplayCurrentSw(pin_state, OTHER_SW_ID, reorg_other_sw[mux_port_out_local - TRANSPORT_MUX_OFFSET]);
			}
	} // end if SHIFT is active.

	// pass to next other switches
	control_index = JOYSTICK_SW_CTRL_INDEX;
	index 		  = TRANSPORT_NB;

	///////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// Check Joystick SW /////////////////////////////
	pin_state = JOYSTICK_SW_PORT_IN & JOYSTICK_SW_PIN;
	CheckSendDisplayCurrentSw(pin_state, OTHER_SW_ID, index);
	// pass to next switch
	control_index++;
	index++;

	///////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// Check Footswitch SW /////////////////////////////
	pin_state = FOOTSWITCH_PORT_IN & FOOTSWITCH_PIN;
	CheckSendDisplayCurrentSw(pin_state, OTHER_SW_ID, index);
	// pass to next switch
	control_index++;
	index++;

	///////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// Check encoder SW /////////////////////////////
	// check encoder button only if not in menus and not in mackie mode
	if(menu_state != MENU_PARAM)
	// V1.5 : In mackie mode the group button must send a MIDI Note event
	// Mackie mode ?
	//V1.67 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS != MACKIE_MODE)
	// if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) != MACKIE_MODE_TEST_POS)
	if( (OPERATING_MODE_NVM != MACKIE_MODE) && 
		(OPERATING_MODE_NVM != LOGIC_MODE))
	{
		pin_state = ENCODER_SW_PORT_IN & ENCODER_SW_PIN;
		CheckSendDisplayCurrentSw(pin_state, OTHER_SW_ID, index);
	}
}

