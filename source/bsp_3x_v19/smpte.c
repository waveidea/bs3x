#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "smpte.h"
#include "eeprom.h"
#include "serial.h"
#include "hardware.h"
#include "lcd.h"
#include "lcd_low_level.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

far SMPTE_STRUCT smpte_code;
uchar current_smpte_index;
uchar current_smpte_reload_cpt;
far uchar smpte_lock_cpt;
rom uchar smpte_sysex_header[] = {MIDI_SOX, 0x7F, 0x7F, 0x01, 0x01 };

// The following table is used to correct smpte timing
// Timer reload cpt is loaded with 26042, then 26042, then 26041, then 26042, ... for 24FPS
rom uint  smpte_reload_cpt[SMPTE_FRAME_RATE_NB][3] = {	26042, 26042, 26041,
														25000, 25000, 25000,
														20834, 20834, 20833,
														20834, 20834, 20833 };
rom uchar smpte_frame_nb[] = {24,25,30,30};
uchar quarter_frame_cpt;

extern far uchar CtrlReg[CTRL_REG_REAL_NB];			// v1.8
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes
//extern rom uchar font_5x8[];
extern volatile uchar Z8_STATUS_4;
//extern uchar Z8_STATUS_11;

///////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitSmpte()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void InitSmpteTime(void)
{
	smpte_code.frame 			= 0;
	smpte_code.second 			= 0;
	smpte_code.minute 			= 0;
	smpte_code.hour				= 0;
	smpte_code.type				= (Z8_STATUS_3_NVM & SMPTE_FRAME_RATE_MASK_BIT) >> SMPTE_TYPE_BIT_POS;
	quarter_frame_cpt			= 0;
	current_smpte_reload_cpt	= 0;
	current_smpte_index 		= SMPTE_FRAME_LOW_NIBBLE;
 	smpte_lock_cpt				= 0;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendSmpteToMidi()
// Inputs	: None
// Outputs	: None
// Process  : FULL_SMPTE, UPDATE_QUARTER
///////////////////////////////////////////////////////////////////////////////////////
void SendSmpteToMidi(uchar command)
{
	uchar tmp, offset, lenght;
	far uchar *ptr;

	if(smpte_lock_cpt) return;

	// Treat START_SMPTE command
	if(command == FULL_SMPTE)
	{
		// Get the frame type if changed from menus
		smpte_code.type	= (Z8_STATUS_3_NVM & SMPTE_FRAME_RATE_MASK_BIT) >> SMPTE_TYPE_BIT_POS;

		// reset the current digit of smpte to zero
		current_smpte_index = 0;

		// Send a full frame sysex SMPTE
		for(tmp=0; tmp<SMPTE_FULL_FRAME_SYSEX_HEADER_SIZE; tmp++)
			generic_buffer[tmp] = smpte_sysex_header[tmp];

		// Now add hr, min sec, frame to the sysex
		generic_buffer[SMPTE_FULL_FRAME_SYSEX_HEADER_SIZE]   = (smpte_code.type << 5) + (smpte_code.hour);
		generic_buffer[SMPTE_FULL_FRAME_SYSEX_HEADER_SIZE+1] = smpte_code.minute;
		generic_buffer[SMPTE_FULL_FRAME_SYSEX_HEADER_SIZE+2] = smpte_code.second;
		generic_buffer[SMPTE_FULL_FRAME_SYSEX_HEADER_SIZE+3] = smpte_code.frame;

		// Add EOX
		generic_buffer[SMPTE_FULL_FRAME_SYSEX_HEADER_SIZE+4] = MIDI_EOX;
		
		// Full frame lenght to send to midi
		lenght = SMPTE_FULL_FRAME_SYSEX_SIZE;
	}
	else
	// Treat UPDATE_QUARTER command
	{
		// 0xF1
		generic_buffer[0] = MIDI_TIME_CODE;
		// index is 0-7 : transform 0-4
		offset = current_smpte_index >> 1;
		// Get the frame byte to send
		ptr = &smpte_code.frame;
		tmp = *(ptr + offset);
		// If index was odd then select high nibble
		if(current_smpte_index & 1)
			tmp = (tmp >> 4);
		// Get only low nible
		tmp &= 0x0F;
		// Special case for the high digits hours
		if(current_smpte_index == SMPTE_HOURS_HIGH_NIBBLE_PLUS_TYPE)
		{
			// high digits hours contains frame rate type 
			generic_buffer[1] = 0x70 + ((smpte_code.type << SMPTE_TYPE_BIT_POS) & SMPTE_FRAME_RATE_MASK_BIT);
			// Also, bit 0 contains high digit for hours 16 to 23
			if(smpte_code.hour > 0x0F) generic_buffer[1] |= 1;
		}
		else
			generic_buffer[1] = (current_smpte_index << 4) + tmp;
		// 2 bytes to send to midi
		lenght = 2;
		// Update the smpte current digit to send
		current_smpte_index++;
		if(current_smpte_index >= SMPTE_BYTE_NB)
			current_smpte_index = 0;
	}

	//Z8_STATUS_11 |= sending_realtime_to_midi_z11; // V1.9

	// Now send the full frame or quarter frame to MIDI Outs
	SendDispatchToMidi(&generic_buffer[0], lenght, (midi_out_1_en | usb_out_en));

	//Z8_STATUS_11 &= ~sending_realtime_to_midi_z11; // V1.9
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: IncreaseDisplaySmpteTime()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void IncreaseDisplaySmpteTime(uchar frame_nb)
{
	uchar digit = 0;

	// Increase frame nb
	smpte_code.frame += frame_nb;
	// will display frame digit
	digit |= update_frame_digit;
	// must reset frame nb ?
	while(smpte_code.frame >= smpte_frame_nb[smpte_code.type])
	{
		smpte_code.frame -= smpte_frame_nb[smpte_code.type];
		smpte_code.second++;
		// will display second digit
		digit |= update_second_digit;
		// must reset second nb ?
		if(smpte_code.second >= SMPTE_SECONDS_NB_MAX)
		{
			smpte_code.second = 0;
			smpte_code.minute++;
			// will display minute digit
			digit |= update_minute_digit;
			// must reset minute nb ?
			if(smpte_code.minute >= SMPTE_MINUTES_NB_MAX)
			{
				smpte_code.minute = 0;
				smpte_code.hour++;
				// will display hour digit
				digit |= update_hour_digit;
				// must reset hour nb ?
				if(smpte_code.hour >= SMPTE_HOURS_NB_MAX)
				{
					// InitSmpteTime();
					smpte_code.hour = 0;
				}
			}
		}
	}

	// Time has been updated, display new time
	DisplaySmpteTime(digit);

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DecreaseDisplaySmpteTime()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void DecreaseDisplaySmpteTime(uchar frame_nb)
{
	uchar digit = 0;
	uchar frame_nb_tmp;

	// will display frame digit
	digit |= update_frame_digit;

	do 
	{
		if(frame_nb >= smpte_frame_nb[smpte_code.type])
		{
			frame_nb_tmp	 = smpte_frame_nb[smpte_code.type];
			frame_nb 		-= frame_nb_tmp;
		}
		else
			frame_nb_tmp	 = frame_nb;

		if(smpte_code.frame >= frame_nb_tmp)
		{
			// Decrease frame nb
			smpte_code.frame -= frame_nb_tmp;
		}
		else
		{
			smpte_code.frame = smpte_frame_nb[smpte_code.type] - (frame_nb_tmp - smpte_code.frame);
			// will display second digit
			digit |= update_second_digit;
			if(smpte_code.second == 0)
			{
				smpte_code.second = SMPTE_SECONDS_NB_MAX-1;
				// will display minute digit
				digit |= update_minute_digit;			
				if(smpte_code.minute == 0)
				{
					smpte_code.minute = SMPTE_MINUTES_NB_MAX-1;
					// will display hour digit
					digit |= update_hour_digit;
					if(smpte_code.hour == 0)
					{
						smpte_code.hour = SMPTE_HOURS_NB_MAX-1;
					}
					else
						smpte_code.hour--;
				}
				else
					smpte_code.minute--;
			}
			else
				smpte_code.second--;
		}
	}
	while(frame_nb >= smpte_frame_nb[smpte_code.type]);

	// Time has been updated, display new time
	DisplaySmpteTime(digit);

}



///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplaySmpteTime()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void DisplaySmpteTime(uchar digit)
{
/*	
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Update frame ?
	if(digit & update_frame_digit)
	{
		DisplayLcdInt(LCD_ZONE_TEMPO_X+53, LCD_ZONE_TEMPO_Y+1, DIGITS_2|LCD_INVERT, font_5x8, smpte_code.frame );
	}

	// Update seconds ?
	if(digit & update_second_digit)
	{
		DisplayLcdInt(LCD_ZONE_TEMPO_X+35, LCD_ZONE_TEMPO_Y+1, DIGITS_2|LCD_INVERT, font_5x8, smpte_code.second );
	}

	// Update minutes ?
	if(digit & update_minute_digit)
	{
		DisplayLcdInt(LCD_ZONE_TEMPO_X+17, LCD_ZONE_TEMPO_Y+1, DIGITS_2|LCD_INVERT, font_5x8, smpte_code.minute );
	}

	// Update hours ?
	if(digit & update_hour_digit)
	{
		DisplayLcdInt(LCD_ZONE_TEMPO_X, LCD_ZONE_TEMPO_Y+1, DIGITS_2|LCD_INVERT, font_5x8, smpte_code.hour );
	}
*/
}
