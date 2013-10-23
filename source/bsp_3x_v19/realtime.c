#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "realtime.h"
#include "arpp.h"
#include "lfo.h"
#include "eeprom.h"
#include "serial.h"
#include "motion.h"
#include "lcd.h" // V1.8
#include "hardware.h" // V1.8
#include "smpte.h" // V1.8

//extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_9;
// extern uchar Z8_STATUS_10;
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
//extern uchar LFO_Freq_Counter;		

far uchar realtime_generated_buffer[REALTIME_BUFFER_LENGHT];
far uchar realtime_received_buffer [REALTIME_BUFFER_LENGHT];
uchar realtime_generated_start_index;
uchar realtime_generated_end_index;
uchar realtime_received_start_index;
uchar realtime_received_end_index;
extern uchar next_transport_state; // V1.8
extern uchar transport_state; // V1.8

extern uchar current_smpte_index;	// V1.8
extern uchar quarter_frame_cpt;	// V1.8
extern rom uchar smpte_frame_nb[];	// V1.8
extern far SMPTE_STRUCT smpte_code;	// V1.8
extern far uchar smpte_lock_cpt;	// V1.8
extern uchar Z8_STATUS_11; // V1.8

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckRealtimeMidiReceived()
// Inputs	: None
// Outputs	: None
// Process  : check if a realtime midi event has been received
///////////////////////////////////////////////////////////////////////////////////////
void CheckRealtimeMidi(void)
{
	uchar local_realtime_data;

	//Z8_STATUS_11 |= sending_realtime_to_midi_z11; // V1.9

	////////////////////////////////////////// CHECK IF INTERNAL SYNC
	//////////////////////////////////////////////////////////////////
	while(realtime_generated_start_index != realtime_generated_end_index) 
	{
		local_realtime_data = realtime_generated_buffer[realtime_generated_start_index];	

		// generate internal sync, send to out only if smpte mode is not active
		//if(!(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm))
		//	if(local_realtime_data == MIDI_CLOCK)
		//		SendToTx0(MIDI_CLOCK);

		// CHECK FOR ARPPEGIO INTERNAL SYNC
		// V1.5 if(CTRL_REG_1_7BITS_TO_EZ & arpp_state_z_ez1)
		if(Z8_STATUS_0_NVM & arpp_state_z0nvm)
			if(!(Z8_STATUS_0_NVM & arpp_sync_ext_z0nvm))
				CheckRealtimeArpeggio(local_realtime_data);

		// CHECK FOR LFO INTERNAL SYNC
		if(Z8_STATUS_1_NVM & lfo_state_z1nvm)
			if(!(Z8_STATUS_1_NVM & lfo_sync_ext_z1nvm))
				CheckRealtimeLfo(local_realtime_data);
		
		// CHECK FOR MOTION INTERNAL SYNC
		if(Z8_STATUS_1_NVM & motion_state_z1nvm)
			if(!(Z8_STATUS_1_NVM & motion_sync_ext_z1nvm))
				CheckRealtimeMotion(local_realtime_data);

		realtime_generated_start_index = (realtime_generated_start_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
	}

	// V1.8 Manage internal sync with SMPTE mode
	if(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm)
	// smpte mode
	{
		// must send a new quarter frame ?
		if(Z8_STATUS_11 & must_send_smpte_quarter_z11)
		{
			// unflag
			Z8_STATUS_11 &= ~must_send_smpte_quarter_z11;
			// Update smpte time
			quarter_frame_cpt++;
			if(quarter_frame_cpt >= QUARTER_FRAME)
			{
				// Increase smpte time by +1 frame and display
				IncreaseDisplaySmpteTime(1);
				// reset counter
				quarter_frame_cpt = 0;
			}
			// Send a new smpte quarter frame
			SendSmpteToMidi(UPDATE_QUARTER);
		}
	}

	////////////////////////////////////////// CHECK IF EXTERNAL SYNC
	//////////////////////////////////////////////////////////////////

	// Treat realtime MIDI from external sync
	while(realtime_received_start_index != realtime_received_end_index) 
	{
		local_realtime_data = realtime_received_buffer[realtime_received_start_index];	

		// V1.8 : when start/stop/pause/rec is received from external : update lcd bitmap
		if( (local_realtime_data == MIDI_START) || (local_realtime_data == MIDI_CONTINUE))
			next_transport_state = TRANSPORT_PLAY;
		else
		if(local_realtime_data == MIDI_STOP)
			// pause/stop choice is managed in the display function
			next_transport_state = TRANSPORT_STOP;

		// CHECK FOR ARPPEGIO INTERNAL SYNC
		// V1.5 if(CTRL_REG_1_7BITS_TO_EZ & arpp_state_z_ez1)
		if(Z8_STATUS_0_NVM & arpp_state_z0nvm)
			if(Z8_STATUS_0_NVM & arpp_sync_ext_z0nvm)
				CheckRealtimeArpeggio(local_realtime_data);

		// CHECK FOR LFO INTERNAL SYNC
		if(Z8_STATUS_1_NVM & lfo_state_z1nvm)
			if(Z8_STATUS_1_NVM & lfo_sync_ext_z1nvm)
				CheckRealtimeLfo(local_realtime_data);
		
		// CHECK FOR MOTION INTERNAL SYNC
		if(Z8_STATUS_1_NVM & motion_state_z1nvm)
			if(Z8_STATUS_1_NVM & motion_sync_ext_z1nvm)
				CheckRealtimeMotion(local_realtime_data);

		realtime_received_start_index = (realtime_received_start_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
	}

	// V1.8 : update lcd realtime transport if required
	if(next_transport_state != CTRL_UNDEF)
	if(next_transport_state != transport_state)
		UpdateLcdTransport(); 

	//Z8_STATUS_11 &= ~sending_realtime_to_midi_z11; // V1.9
}

// V1.8
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateSmpteState()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void UpdateSmpteState(uchar next_transport_state)
{
	// Start : init smpte digit to first digit
	if(next_transport_state == TRANSPORT_PLAY)
	{
		// reload timer 
		ReinitTimer2();
		// Start smpte timer : prescaler is 8 in smpte mode
		T2CTL = 0x99;
		// Send a full smpte frame
		SendSmpteToMidi(FULL_SMPTE);
	}
	else
	if(next_transport_state == TRANSPORT_STOP)
	{
		// need to pause or stop ?
		if ( (transport_state == TRANSPORT_REC) || (transport_state == TRANSPORT_PLAY))
			// PAUSE smpte timer
			T2CTL = 0x19;	// Timer 2 one shot, not started, divide per 8
		else
		{
			// Reset smpte time to zero
			InitSmpteTime();
			// Stop smpte timer
			T2CTL = 0x19;	// Timer 2 one shot, not started, divide per 8
			// Update display with new smpte time
			DisplaySmpteTime(SMPTE_ALL_DIGIT);
			// Send a full smpte frame
			SendSmpteToMidi(FULL_SMPTE);
		}
	}
	else
	if(next_transport_state == FORWARD_CTRL_INDEX)
	{
		// Increase smpte time by 5 sec and display
		IncreaseDisplaySmpteTime((uchar)(smpte_frame_nb[smpte_code.type] * SMPTE_UPDT_SEC_FROM_SW));
		// Send a full smpte frame
		smpte_lock_cpt = 0;
		SendSmpteToMidi(FULL_SMPTE);
		smpte_lock_cpt = SMPTE_LOCK_TIME;
	}
	else
	if(next_transport_state == BACKWARD_CTRL_INDEX)
	{
		// Decrease smpte time by 5 sec and display
		DecreaseDisplaySmpteTime((uchar)(smpte_frame_nb[smpte_code.type] * SMPTE_UPDT_SEC_FROM_SW));
		// Send a full smpte frame
		smpte_lock_cpt = 0;
		SendSmpteToMidi(FULL_SMPTE);
		smpte_lock_cpt = SMPTE_LOCK_TIME;
	}	
}
