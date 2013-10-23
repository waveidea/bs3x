#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lfo.h"
//#include "analog.h"
#include "hardware.h"
#include "eeprom.h"
#include "serial.h"
#include "lcd.h"
#include "lcd_low_level.h"

//rom uchar LFO_SINE[]     = {0x08,0x18,0x30,V1.66,0x5A,0x68,0x74,0x7F};
//rom uchar LFO_TRIANGLE[] = {0x08,0x18,0x2A,0x3A,0x4C,0x5C,0x6E,0x7F};
// V1.7 rom uchar LFO_SINE_TABLE[]     	= {0x04, 0x0C, 0x18, 0x23, 0x2D, 0x34, 0x3A, 0x3F};
// V1.7 rom uchar LFO_TRIANGLE_TABLE[] 	= {0x04, 0x0C, 0x15, 0x1D, 0x26, 0x2E, 0x37, 0x3F};

rom uchar LFO_SINE_TABLE[]     	= {0x05, 0x21, 0x2E, 0x37, 0x3C, 0x3F};
rom uchar LFO_TRIANGLE_TABLE[] 	= {0x05, 0x10, 0x1C, 0x28, 0x33, 0x3F};

uchar LFO_Freq_Counter;		
uchar LFO_Wave_Index;      	// 0 to 23
uint  current_random;

// V1.7 extern rom uchar font_num_8x16[];
extern uchar current_value;
extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_9;
//extern uchar Z8_STATUS_10;
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];		// table of control reg for eerprom saving
extern All_Controls	Ctrl;					// All controls are defned here
extern uchar control_index;					// current control nb
extern uchar last_control_index;			// last control nb
//V1.7 extern uchar lfo_graph_index;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckRealtimeLfo()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
///////////////////////////////////////////////////////////////////////////////////////
void CheckRealtimeLfo(uchar realtime_data)
{
/*	uchar *realtime_reg_local;

	// the realtime register is intitialized to internal (Z8_STATUS_Z10)
	realtime_reg_local = &Z8_STATUS_TX_REALTIME;
	// is the lfo externaly synched
	if(Z8_STATUS_1_NVM & lfo_sync_ext_z1nvm )
		// the realtime register is external (Z8_STATUS_Z9)
		realtime_reg_local = &Z8_STATUS_RX_REALTIME;
*/
	
	// MIDI clock from ext or int present ?
	// if(*realtime_reg_local & realtime_clock_received_z9)
	if(realtime_data == MIDI_CLOCK)
	{
		Z8_STATUS_5 &= ~lfo_waiting_midi_clock_z5;
		// is the LFO playing
		if(Z8_STATUS_5 & lfo_play_zstop_z5)
		{
			// finished to count ?
			if(--LFO_Freq_Counter == 0)	
	   			Z8_STATUS_5 |= must_send_lfo_z5;	
		} // end lfo playing
	} // end MIDI clock from ext or int present
	else
	{
		// is the LFO playing
		if(Z8_STATUS_5 & lfo_play_zstop_z5)
		{
			// if(*realtime_reg_local & realtime_stop_received_z9)
			if(realtime_data == MIDI_STOP)
			{
				Z8_STATUS_5 &= (~lfo_play_zstop_z5);
				Z8_STATUS_5 |= must_update_lcd_small_lfo_z5;
			}
		} // end lfo playing
		else
		{ // LFO is stopped, if START or CONTINUE is present it becomes playing
			// if( (*realtime_reg_local & realtime_start_received_z9)||(*realtime_reg_local & realtime_continue_received_z9))
			if( (realtime_data == MIDI_START) || (realtime_data == MIDI_CONTINUE) )
			{
				// V1.7 reset the LFO to first index when START or CONTINUE received
				if(realtime_data == MIDI_START)
					LFO_Wave_Index = 0;

				Z8_STATUS_5 |= lfo_play_zstop_z5;
				Z8_STATUS_5 |= must_update_lcd_small_lfo_z5;
			}
		} // end lfo is stopped
	} // end event received is realstime and different than MIDI clock
	
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckLfoToSend()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Check if new LFO value has to be sent
///////////////////////////////////////////////////////////////////////////////////////
void CheckLfoToSend(void)	   
{
	// exit if LFO is off
   	if(!(Z8_STATUS_1_NVM & lfo_state_z1nvm))
    	return;

	// test for LFO flag
	if(Z8_STATUS_5 & must_send_lfo_z5)
	{
	  	// If ext sync and clock not received then exit
	  	// if( (LFO_Freq < 64) && (Z8_STATUS_5 & lfo_waiting_midi_clock_z5))
		// if( (Z8_STATUS_1_NVM & lfo_sync_ext_z1nvm) && (Z8_STATUS_5 & lfo_waiting_midi_clock_z5))
		if( Z8_STATUS_5 & lfo_waiting_midi_clock_z5)
	 		return;

		// Reset flag		
   	  	Z8_STATUS_5 &= ~must_send_lfo_z5;

      	// Get value to send from sine/triangle table
      	GetLfoWaveValue();

      	// inc LFO Wave Index from 0 to 31
      	// V1.7 LFO_Wave_Index = (LFO_Wave_Index + 1) & (LFO_NB_PT_PERIOD-1);
		LFO_Wave_Index++;
	 	if(LFO_Wave_Index == LFO_NB_PT_PERIOD) 
			LFO_Wave_Index = 0;

      	// Replace old LFO value with new and send if required
      	if(current_value != Ctrl.lfo)
      	{
         	Z8_STATUS_5 |= sending_lfo_z5;
         	control_index = LFO_CTRL_INDEX;

         	// replace old value if different
        	Ctrl.lfo = current_value ;
	
		  	// V1.8 fill structure of the pot before calling SendCtrlToMidi() because SendControlToMidi() no longer fill struct from V1.8
			FillParamCurrentStruct();

         	// Now send LFO to MIDI out
         	SendControlToMidi();

			//DisplayCurrentValue();
			DisplaySmallBargraphValue(LCD_ZONE_LFO_WAVE_STATE_X+14, current_value);

			if(last_control_index != control_index)
			{
				// Update lcd str
				UpdateLcdString();
				last_control_index = control_index;
			}

      	   	Z8_STATUS_5 &= ~sending_lfo_z5;
      }
	  else
	  // square ? display all square graph points even if equal
	  if(Z8_STATUS_2_NVM & lfo_wave_1_z2nvm)
		if(!(Z8_STATUS_2_NVM & lfo_wave_0_z2nvm))
		   DisplaySmallBargraphValue(LCD_ZONE_LFO_WAVE_STATE_X+14, current_value);

      // Reload LFO frequency counter
      ReloadLfoFreqCpt();

	  // increase lfo small graph index 0-127
//V1.7	  lfo_graph_index = (lfo_graph_index + 1) & 0x7F;

    }  // end if must_send_lfo

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	GetLfoWaveValue()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Get current LFO wave value, stored in global var current_value
///////////////////////////////////////////////////////////////////////////////////////
void GetLfoWaveValue(void)
{
   	uchar wav_table_index;
   	uchar wav_sign  = LFO_WAV_POS;
   	uchar wav_slope = LFO_WAV_SLOPE_POS; 

   	// Transform wave index (0-31 : 1 period) into wav table index (0-7 : 1/4 period)
   	// Transform wave index (0-23 : 1 period) into wav table index (0-5 : 1/4 period) V1.7
   	// V1.7 wav_table_index = LFO_Wave_Index & LFO_WAV_TAB_INDEX_MAX;
	wav_table_index = LFO_Wave_Index;
	while(wav_table_index > LFO_WAV_TAB_INDEX_MAX)
		wav_table_index -= (LFO_WAV_TAB_INDEX_MAX+1);

   	// Get the wave sign
   	if(LFO_Wave_Index >= LFO_Q1_END)  			// 16-31 : Q2, Q3	
   	{
	    // sign is negative
	    wav_sign  = LFO_WAV_NEG;
	    if(LFO_Wave_Index >= LFO_Q2_END) 		// 24-31 : Q3
	    	// must invert index
	        wav_slope = LFO_WAV_SLOPE_NEG;
   	}
   	else                               			// 0-15 : Q0, Q1
   	{
	    if(LFO_Wave_Index >= LFO_Q0_END)
	    	// must invert index
	        wav_slope = LFO_WAV_SLOPE_NEG;   	// Q1
   	}

   	// invert index if Q1 Q3 (0-7 becomes 7-0)
   	if(wav_slope == LFO_WAV_SLOPE_NEG)
      	wav_table_index = LFO_WAV_TAB_INDEX_MAX - wav_table_index;

   	// Now Get waveform
	// SQUARE ?
	if(Z8_STATUS_2_NVM & lfo_wave_1_z2nvm)
	{
		// RANDOM
		if(Z8_STATUS_2_NVM & lfo_wave_0_z2nvm)
			current_value = (uchar) current_random;
		else
			// SQUARE
			current_value = LFO_Ampl;
	}
	else
	{
		// TRIANGLE ?
		if(Z8_STATUS_2_NVM & lfo_wave_0_z2nvm)
			current_value = LFO_TRIANGLE_TABLE[wav_table_index];
		else
			current_value = LFO_SINE_TABLE[wav_table_index];
		
		// Divide waveform level according to amplitude requested, 6Bits result 
		current_value = ((uint)LFO_Ampl * current_value) >> 6;
	}

   // Add Offset & Sign and truncate to 0 - 127
   if(wav_sign  == LFO_WAV_POS)
   {
      current_value += LFO_Offset;
	  if(current_value > 127)
         current_value = 127;
   }
   else
   {
      current_value = LFO_Offset - current_value;
	  if(current_value > 127)
         current_value = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	ReloadLfoFreqCpt()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Reload LFO freq counter according to sync mode and selected frequency
///////////////////////////////////////////////////////////////////////////////////////
void ReloadLfoFreqCpt(void)
{
 // V1.64  uchar tmp; 

	// exit if LFO is off
   	if(!(Z8_STATUS_1_NVM & lfo_state_z1nvm))
    	return;

	// Internal of external sync ?
//	if(LFO_Freq < 64)
//	{
		// lfo_int_ext_sync       = EXT_LFO_SYNC;
		Z8_STATUS_5 |= lfo_waiting_midi_clock_z5;
		// tmp = LFO_Freq >> 3;     // 0-63 becomes 0-7
		
		// 0-63 ->nb clock tick step
		// V1.7 LFO_Freq_Counter = (63-LFO_Freq) + 2;

		// new in v1.7 : LFO freq analog is 0-15 : convert it to clock ticks step as follow
		// Sync = 0.5 measures : 48  clocks ticks received : 1 LFO sample each  2 clock ticks : freq analog pot = 0
		// Sync = 1.0 measures : 96  clocks ticks received : 1 LFO sample each  4 clock ticks : freq analog pot = 1
		// Sync = 1.5 measures : 144 clocks ticks received : 1 LFO sample each  6 clock ticks : freq analog pot = 2
		// Sync = 2.0 measures : 192 clocks ticks received : 1 LFO sample each  8 clock ticks : freq analog pot = 3
		// Sync = 2.5 measures : 240 clocks ticks received : 1 LFO sample each 10 clock ticks : freq analog pot = 4
		// ....
		// Sync = 4.0 measures : 384 clocks ticks received : 1 LFO sample each 16 clock ticks : freq analog pot = 
		// ...
		// Sync = 7.5 measures : 720 clocks ticks received : 1 LFO sample each 30 clock ticks
		// Sync = 8.0 measures : 768 clocks ticks received : 1 LFO sample each 32 clock ticks

		// convert 0-15 to 30-0
		// LFO_Freq_Counter  = (15-LFO_Freq) << 1;
		LFO_Freq_Counter  = LFO_Freq << 1;
		// convert 30-0 to 32-2
		LFO_Freq_Counter += 2;

/* V1.64		
		tmp = LFO_Freq;
		LFO_Freq_Counter = 191; // V1.64 192 (63x3 + 2)
		while(tmp)
		{
			// V1.64 LFO_Freq_Counter = LFO_Freq_Counter >> 1; // 192,96,48,24,12,6,3,3
			LFO_Freq_Counter -= 3; // 191,188,185,...,2
			tmp--;
		}
				
		// V1.64  if(LFO_Freq_Counter < 3) LFO_Freq_Counter = 3;
*/	// end block removed V1.64

//	}
//	else
	// LFO IS INTERNALLY SYNCRONIZED
//	{
		//lfo_int_ext_sync       = INT_LFO_SYNC;
//		Z8_STATUS_5 &= ~lfo_waiting_midi_clock_z5;
//		LFO_Freq_Counter       = (~( (LFO_Freq - 64) << 1) & 0x7F) + 1; //  64-127 becomes 128-1
//	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	rand_7b()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Generate a pseudo random number
///////////////////////////////////////////////////////////////////////////////////////
uint rand(void)
{
	unsigned long tmp;

  	tmp = (current_random * 1103515245) + 12345;
  	//return ((unsigned int)(current_random/65536) & 32767); //  mod 32768;
	return ((uint)(tmp >> 8) & 32767); //  mod 32768;
}

/*
// -----------------------------------------------------
// Pseudo Random Number generator using
// a 15 Bit Linear Feedback Shift Register
// (taps at bit 14 and bit 0)
// 25.10.2000 Tim Boescke - t.boescke@tuhh.de
// The length of the pseudo random sequence is 32767.
// A 16 Bit LFSR could be used to increase this to 65535
// 
// -----------------------------------------------------
// The seed register has to be loaded with a value not
// equal to zero!

RandomInit15:
	clr	RndSeed2
	ser	RndSeed1	
	ret

Random15:
	mov	rndTemp,RndSeed1	
	sbrc	RndSeed2,6		
	com	RndTemp		;bit 0 eor bit 14
	lsr	RndTemp
	rol	rndSeed1
	rol	rndSeed2
	ret
*/


