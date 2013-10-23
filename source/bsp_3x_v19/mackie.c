#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "mackie.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "hardware.h"
#include "serial.h"
#include "eeprom.h"
#include "analog.h"
#include "realtime.h" // V1.9

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
far uchar MackieTimeCode[MACKIE_MTC_DIGIT_NB+MACKIE_7SEG_NB] = {'0','0','0','0','0'+0x40,'0','0'+0x40,'0','0'+0x40,'0','0','0'}; // when 0x40 is added, a dots follow the digit
rom uchar MackieDigitLcdPos[] = {5,11,27,33,39,47,53,61,67,75,81,87}; // Pixel Position of the digits 7S HHH,MM,SS,MSC in the group name LCD zone (7S=7segemnt)
far uchar MackieMetering[MACKIE_CHANNEL_NB] = {0,0,0,0,0,0,0,0};
volatile uchar mackie_bargraph_timeout;
rom uchar MackieSysexHeader[MACKIE_SYX_HEADER_SIZE] = { MIDI_SOX, MACKIE_SYX_ID0, MACKIE_SYX_ID1, MACKIE_SYX_ID2/* V1.69, MACKIE_SYX_ID3 */ };
far uchar mackie_logic_sysex_id;		// V1.69

// far uchar MackieLcdLines[MACKIE_LCD_LINE_LENGHT * 2];	// V1.7

extern far   uchar serial_in_buffer[SERIAL_IN_BUFF_LENGHT];	// serial in buffer
extern uchar serial_in_start_index;

//extern rom uchar font_5x8[];
// V1.66 extern rom uchar font_7seg_5x8[];
//extern rom uchar mackie_mtc_init_txt[];
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion	

extern volatile uchar Z8_STATUS_4;
extern uchar lcd_lock_incrust_cpt;					// allow to lock LCD incrust zone
extern uchar current_value;							// current control value
extern rom uchar identity[];
extern far Controls_Scene	Physical[GROUP_NB];		// All physical values stored here 
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg 
extern All_Controls Ctrl;				// All controls are defned here


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: EnterMackieMode()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void EnterMackieMode(void)
{
	// First send a Host connection querry and wait for a valid answer within 300ms
	SendMackieSysexCommand(TX_HOST_CONNECT_QUERRY);

	// Indicates to EZ to forward all incoming MIDI events
	CheckIfEzMustForwardMidiToZ8();

	// Display "waiting host answer" in a incrust zone
	

	// Replace current group name per Mackie time code 000.00.00.000
	DisplayMackieTimeCodeFull();

	// Display Meterering of all 8 channels
	DisplayMackieMeteringFull();

	// start the timeout decay for mackie channel bargraph
	mackie_bargraph_timeout = MACKIE_BARGRAPH_DECAY;

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: LeaveMackieMode()
// Inputs	: None
// Outputs	: None
// Process  : Here we have received a RX_OFFLINE_STATUS : exit mackie mode
///////////////////////////////////////////////////////////////////////////////////////
void LeaveMackieMode(void)
{
	// stop the timeout decay for mackie channel bargraph
	mackie_bargraph_timeout = 0;

	// Replace Mackie Time code per Current group name
	UpdateLcdZones(INIT_LCD_GROUP);

	// Indicates to EZ that it may not need to forward all incoming MIDI events (if arpp is also off)
	CheckIfEzMustForwardMidiToZ8();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendMackieSysexCommand()
// Inputs	: Command ID (see defines)
// Outputs	: None
// Process  : Send a Mackie sysex command to host
//			  TX_HOST_CONNECT_QUERRY 	TX_CONFIRM_CONNECT 
//			  TX_FW_VERSION_REPLY		TX_ERROR_CONNECT
///////////////////////////////////////////////////////////////////////////////////////
void SendMackieSysexCommand(uchar command)
{
	uchar i;
	// add header + mackie_logic_sysex_id + command + eox bytes to sysex lenght
	uchar sysex_lenght = MACKIE_SYX_HEADER_SIZE + 1 + 1 + 1; // V1.69 +1

	// Send a Mackie sysex header
	generic_buffer[0] = MIDI_SOX;
	generic_buffer[1] = MACKIE_SYX_ID0;
	generic_buffer[2] = MACKIE_SYX_ID1;
	generic_buffer[3] = MACKIE_SYX_ID2;
	generic_buffer[4] = mackie_logic_sysex_id; // V1.69 MACKIE_SYX_ID3;
	// send the Mackie command ID
	generic_buffer[5] = command;
	// Depending on command : send different bytes
	if(command == TX_FW_VERSION_REPLY)
	{
		// send  VV VV VV VV VV F7 (VV=firmware version eg "1.0")
		generic_buffer[6] = 'V';
		generic_buffer[7] = identity[FW_VERSION_INDEX] + '0';
		generic_buffer[8] = '.';
		generic_buffer[9] = identity[FW_SUB_VERSION_INDEX] + '0';
		generic_buffer[10] = '0';

		sysex_lenght += MACKIE_FW_VERSION_LENGHT;
	}
	else
	{
		// here command is one of TX_HOST_CONNECT_QUERRY TX_CONFIRM_CONNECT TX_ERROR_CONNECT TX_SERIAL_NB_REPLY_MACKIE
		// send SS SS SS SS SS SS SS F7 (SS=serial number)
		generic_buffer[6] = identity[MANUF_YEAR_INDEX] + '0';
		generic_buffer[7] = identity[MANUF_MONTH_INDEX] + '0';
		generic_buffer[8] = identity[SERIAL_4_INDEX] + '0';
		generic_buffer[9] = identity[SERIAL_3_INDEX] + '0';
		generic_buffer[10] = identity[SERIAL_2_INDEX] + '0';
		generic_buffer[11] = identity[SERIAL_1_INDEX] + '0';
		generic_buffer[12] = identity[SERIAL_0_INDEX] + '0';

		sysex_lenght += MACKIE_SERIAL_NB_LENGHT;

		// if command is connection query must send chalenge code
		if(command == TX_HOST_CONNECT_QUERRY)
		{
			// send LL LL LL LL
			generic_buffer[13] = '1';
			generic_buffer[14] = '2';
			generic_buffer[15] = '3';
			generic_buffer[16] = '4';

			sysex_lenght += MACKIE_CHALLENGE_LENGHT;
		}
	}

	// send end of sysex	
	generic_buffer[sysex_lenght-1] = MIDI_EOX;
	SendDispatchToMidi(&generic_buffer[0], sysex_lenght, (midi_out_1_en | /*midi_out_2_en |*/ usb_out_en));
	
	//if(!(Z8_STATUS_11 & sending_realtime_to_midi_z11))
		CheckRealtimeMidi();

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckIfRxMackieCommand()
// Inputs	: The status Byte received
// Outputs	: None
// Process  : Interpret a mackie command received from host
///////////////////////////////////////////////////////////////////////////////////////
void CheckIfRxMackieCommand(uchar status_received)
{
	uchar tmp, tmp2, b2, b3;
	far uchar *physical_ptr;
	uchar *current_fader_ptr;
	uchar tmp3;	// V1.68

	// Here some MIDI events are in the serial in buffer, these events are also mackie commands
	// parse these commands and perform corresponding actions
	b2 = serial_in_buffer[(serial_in_start_index + 1U)&(SERIAL_IN_BUFF_LENGHT-1)];
	b3 = serial_in_buffer[(serial_in_start_index + 2U)&(SERIAL_IN_BUFF_LENGHT-1)];
	
	// Check if MIDI_PITCH_WHEEL event (Move fader to position Ei LL HH) (i=fader index, LL=LSBs, HH=MSBs)
	// V1.69 : use channel feedback only if hook mode active)
	if(Z8_STATUS_2_NVM & enable_mackie_fader_fbk_z2nvm)
// V1.8	if(Z8_STATUS_1_NVM & hook_zjump_mode_z1nvm) 
	if( (status_received & 0xF0) == MIDI_PITCH_WHEEL)
	{
		// Ei LL HH	: only consider HH since bs3x is 7 bits resolution on faders
//		b2 = serial_in_buffer[(serial_in_start_index + 1U)&(SERIAL_IN_BUFF_LENGHT-1)];
//		b3 = serial_in_buffer[(serial_in_start_index + 2U)&(SERIAL_IN_BUFF_LENGHT-1)];
		// Get fader index 0-7 or 08 if master fader (crossfader Left)
		tmp = status_received & 0x0F;

		// V1.69 Check that the fader feedback is not locked before unhooking

		if(tmp < SLIDER_NB)
		{
			current_fader_ptr = &Ctrl.Ctrl_sc.pots[ROTARY_NB + tmp];
			physical_ptr = &Physical[current_group].pots[ROTARY_NB + tmp]; // local physical value;
		}
		else
		{
			current_fader_ptr = &Ctrl.Ctrl_sc.crossfader_l;
			physical_ptr = &Physical[current_group].crossfader_l;
		}

		// arrondi 1024 pos to 128 pos
		if(b2 > 63) b3++;
		// store position of new fader received
		*physical_ptr = b3 & 0x7F;
		// un-hook pot if not equal
		if( (*current_fader_ptr & 0x7F) != *physical_ptr )
		{
			// unhook now
			*current_fader_ptr &= (~HOOK_BIT);
			// Check for sign 
			if( *current_fader_ptr > *physical_ptr )
				*physical_ptr |= POSITIVE_BIT;
		}
		else
			*current_fader_ptr |= HOOK_BIT;
		return;
	}

	// Check if CC event (Time code display or 7 segment display)
	if((status_received & 0xF0) == MIDI_CONTROL_CHANGE)
	{
		// B0 4i YY	: update time code or 7 segment single caracter
//		b2 = serial_in_buffer[(serial_in_start_index + 1U)&(SERIAL_IN_BUFF_LENGHT-1)];
//		b3 = serial_in_buffer[(serial_in_start_index + 2U)&(SERIAL_IN_BUFF_LENGHT-1)];
		if( (b2 & 0xF0) == RX_WRITE_SINGLE_MTC)
		{
			tmp = MACKIE_MTC_DIGIT_NB + MACKIE_7SEG_NB-1 - (b2 & 0x0F);
			// unalowed digit number ?
			if(tmp >= MACKIE_MTC_DIGIT_NB+MACKIE_7SEG_NB) return;
			// unalowed value ?
			// if(b3 >= LCD_7SEG_FONT_LENGHT) return;
			// save mackie time code to RAM
			MackieTimeCode[/*MackieDigitLcdMtc[digit]*/tmp] = b3;
			// Update single digit
			DisplayMackieTimeCodeDigit(tmp);
		}
		return;
	}

	// Check if CHANNEL PRESSURE event (8 channels metering bargraph)
	if((status_received & 0xF0) == MIDI_CHANN_PRESSURE)
	{
		// D0 XX : update mackie channel metering (XX = 0 hhh llll) (hhh=channel 0-7) (llll=meter value)
//		b2 = serial_in_buffer[(serial_in_start_index + 1U)&(SERIAL_IN_BUFF_LENGHT-1)];
		tmp = b2 >> 4; // get channel
		// un allowed channel ?
		if(tmp >= MACKIE_CHANNEL_NB) return;
		MackieMetering[tmp] =  b2 & 0x0F; // save value
		DisplayMackieMetering(tmp);
		return;
	}

	// Is it a mackie sysex ?
	// check if the Mackie sysex header is present ?
	tmp = 0;	
	while((serial_in_buffer[(serial_in_start_index + tmp)&(SERIAL_IN_BUFF_LENGHT-1)] 
		  	== MackieSysexHeader[tmp]) && (tmp < MACKIE_SYX_HEADER_SIZE))
			tmp++;

	// a whole Mackie sysex header was in the buffer ?
	if(tmp == MACKIE_SYX_HEADER_SIZE)
	{
		// V1.69
		if(serial_in_buffer[(serial_in_start_index + tmp)&(SERIAL_IN_BUFF_LENGHT-1)] != mackie_logic_sysex_id)
			return;

		tmp++;

		// Decode Which sysex has been received 
		///////////////////////////////////////

		// V1.68 treat byte in buffer to low mem
		tmp3 = serial_in_buffer[(serial_in_start_index + tmp)&(SERIAL_IN_BUFF_LENGHT-1)];

		// Is it a RX_DEVICE_QUERY V1.69
		if(tmp3 == RX_DEVICE_QUERY)
		{
			// The host request a device query : answer
			SendMackieSysexCommand(TX_HOST_CONNECT_QUERRY);
		}
		else
		// Is it a RX_OFFLINE_STATUS V1.69 (Logic Only, not Mackie)
		if( (tmp3 == RX_OFFLINE_STATUS_LOGIC) && (mackie_logic_sysex_id == LOGIC_MODE))
		{
			// V1.69, be sure the next mode will not be mackie nor logic
			OPERATING_MODE_NVM = MACKIE_MODE ;
			// change mode flag, the main will go out from Mackie/logic mode to next mode
			Z8_STATUS_4 |= mode_changed_z4;
		}
		else
		// Is it a RX_HOST_CONNECT_REPLY
		if(tmp3 == RX_HOST_CONNECT_REPLY)
		{
			// The host answered from a connection query : confirm
			SendMackieSysexCommand(TX_CONFIRM_CONNECT);
		}
		else
		// Is it a RX_FW_VERSION_REQUEST
		if(tmp3 == RX_FW_VERSION_REQUEST)
		{
			// The host answered from a connection query : confirm
			SendMackieSysexCommand(TX_FW_VERSION_REPLY);
		}
		else
		// Is it a RX_SERIAL_NB_REQUEST_MACKIE V1.69 Mackie only
		if(tmp3 == RX_SERIAL_NB_REQUEST_MACKIE)
		{
			// The host answered from a connection query : confirm
			SendMackieSysexCommand(TX_SERIAL_NB_REPLY_MACKIE);
		}
		else
		// Is it a RX_WRITE_MULTIPLE_MTC
		if(tmp3 == RX_WRITE_MULTIPLE_MTC)
		{
			// The host sends multiple caracters to display on mackie time code
			tmp2 = MACKIE_MTC_DIGIT_NB + MACKIE_7SEG_NB - 1; // write left to right
			while( ((serial_in_buffer[(serial_in_start_index + ++tmp)&(SERIAL_IN_BUFF_LENGHT-1)]) != MIDI_EOX) && (tmp2 >= MACKIE_7SEG_NB))
				MackieTimeCode[tmp2--] = serial_in_buffer[(serial_in_start_index + tmp)&(SERIAL_IN_BUFF_LENGHT-1)]; 
			DisplayMackieTimeCodeFull();
		}
		else
		// Is it a RX_WRITE_MULTIPLE_7SEG
		if(tmp3 == RX_WRITE_MULTIPLE_7SEG)
		{
			// The host sends multiple caracters to display on mackie time code
			MackieTimeCode[1] = serial_in_buffer[(serial_in_start_index + tmp + 1)&(SERIAL_IN_BUFF_LENGHT-1)]; 
			MackieTimeCode[0] = serial_in_buffer[(serial_in_start_index + tmp + 2)&(SERIAL_IN_BUFF_LENGHT-1)]; 
			DisplayMackieTimeCodeDigit(0);
			DisplayMackieTimeCodeDigit(1);
		}	
/*		else
		// V1.7 : Is it a RX_WRITE_LCD_TXT
		if(tmp3 == RX_WRITE_LCD_TXT)
		{
			// Get the offset in the lines to update
			tmp++;
			tmp2 = serial_in_buffer[(serial_in_start_index + tmp)&(SERIAL_IN_BUFF_LENGHT-1)];
			// Get all new texts
			while( ((serial_in_buffer[(serial_in_start_index + ++tmp)&(SERIAL_IN_BUFF_LENGHT-1)]) != MIDI_EOX))
				MackieLcdLines[tmp2++] = serial_in_buffer[(serial_in_start_index + tmp)&(SERIAL_IN_BUFF_LENGHT-1)]; 
		}
*/		
	} // end it was a mackie sysex
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMackieTimeCodeDigit()
// Inputs	: DigitNb = 0 to B (7S HHH MM SS MSC)
//			  value to display is a global var
// Outputs	: None
// Process  : UPDATE Mackie Time code digit (000.00.00.000)
//			  In Mackie mode, Mackie Time code is dispayed at group name position
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMackieTimeCodeDigit(uchar DigitNb)
{
	uchar value;
//	rom uchar *font_local;	

	// return if lcd fully locked
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// also exit if zone incrust is locked
	if(lcd_lock_incrust_cpt) return;

	value = MackieTimeCode[DigitNb];

	// convert mackie ascii to ascii real
	// if((value >= 0x40)&&(value <= 0x60)) value -= 0x40;
	// Must add a dot ?
	if(value >= 0x40)
	{
		if(value >= 0x60)
			// add a dot
			FillLcdZone(MackieDigitLcdPos[DigitNb]+8, LCD_ZONE_GROUP_NAME_Y, 1, 1, 0x7F);
		value -= 0x40;
	}

	if(value < 0x20)
		value += 0x40;
//	else
//		value -= 0x20;

	// unalowed value ?
	if(value >= LCD_7SEG_FONT_LENGHT) return;

	lcd_text_value[0] = value;
	lcd_text_value[1] = '\0';
	DisplayLcdText(MackieDigitLcdPos[DigitNb], LCD_ZONE_GROUP_NAME_Y, LCD_INVERT|FONT_7SEG_5x8_FLG/* V1.66 , font_7seg_5x8 */);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMackieTimeCodeFull()
// Inputs	: Update all digits of mackie time code + 7 segement display
// Outputs	: None
// Process  : UPDATE Mackie Time code digit (00 000.00.00.000)
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMackieTimeCodeFull(void)
{
	uchar i;

	// return if lcd fully locked
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// also exit if zone incrust is locked
	if(lcd_lock_incrust_cpt) return;

	// Clear whole group name zone
	FillLcdZone(0, LCD_ZONE_GROUP_NAME_Y, 94, 1, FILL_BLANK);

	for(i=0;i<MACKIE_MTC_DIGIT_NB+MACKIE_7SEG_NB;i++)
		DisplayMackieTimeCodeDigit(i);
		
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMackieMeteringFull()
// Inputs	: Update all 8 channels metering
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMackieMeteringFull(void)
{
	uchar i;

	// return if lcd fully locked
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Clear whole group name zone
	FillLcdZone(LCD_ZONE_MODE_X, LCD_ZONE_MODE_Y, 15, 1, FILL_BLANK);

	for(i=0;i<MACKIE_CHANNEL_NB;i++)
		DisplayMackieMetering(i);

	// Also display master channel (crossdafer)
	DisplayMackieMetering(MACKIE_MASTER_BARGRAPH);
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMackieMetering()
// Inputs	: Update a single channel metering
//			  value to display is a global var
// Outputs	: None
// Process  : In Mackie mode, Mackie channel meter is dispayed at small bmp mode position
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMackieMetering(uchar channel)
{
	uchar value_local, tmp, x_pos, channel_width;

	// return if lcd fully locked
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;
	// V1.69 also de not display if one incrust is active
	//if(lcd_current_incrust != LCD_NO_INCRUST) return;	

	if(channel == MACKIE_MASTER_BARGRAPH)
	{
		// transform 0-127 to 0-6
		tmp = current_value >> 4;
		x_pos = LCD_ZONE_MODE_X+16;
		channel_width=2;
	}
	else
	{
		x_pos = LCD_ZONE_MODE_X;
		channel_width=1;
		for(tmp = 0; tmp < channel; tmp++)
			x_pos += 2;
		
		// transform 0-12 to 0 then >> 5 = 0-4 
		tmp = MackieMetering[channel] >> 1;
	}

	if(tmp > 6) tmp = 6;

	value_local = 0x40; 
	while(tmp--)
		value_local	= (value_local >> 1) | 0x40 ;

	// display the current lfo value on lcd small graph
	FillLcdZone(x_pos, LCD_ZONE_MODE_Y, channel_width, 1, ~value_local); 
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateMackieBargraphDecay()
// Inputs	: Decrease & Update all channels metering
//			  value to display is a global var
// Outputs	: None
// Process  : In Mackie mode, Mackie channel meter is dispayed at small bmp mode position
///////////////////////////////////////////////////////////////////////////////////////
void UpdateMackieBargraphDecay(void)
{
	// Decrease all 8 Mackie channels bargraph

	uchar channel;

	// pass all 8 bargraph channels
	for(channel=0; channel<MACKIE_CHANNEL_NB; channel++)
	{
		// if metering was not 0
		if(MackieMetering[channel] > MACKIE_BARGRAPH_DECAY_STEP)
		{
			// Decrease metering value
			MackieMetering[channel] = MackieMetering[channel]-MACKIE_BARGRAPH_DECAY_STEP;
			//Update display
			DisplayMackieMetering(channel);

		} // end mackie metering was not 0
		else 
			MackieMetering[channel]=0;

	} // end all 8 channels
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendMackieMidiNote()
// Inputs	: 
// Outputs	: None
// Process  : Send a Mackie MIDI NOTE ON or any other 3 bytes message
///////////////////////////////////////////////////////////////////////////////////////
void SendMackieShortEvent(uchar event, uchar note_nb, uchar note_value)
{
	generic_buffer[0] = event;
	generic_buffer[1] = note_nb;
	generic_buffer[2] = note_value;

	SendDispatchToMidi(&generic_buffer[0], 3, (midi_out_1_en | /*midi_out_2_en |*/ usb_out_en));

	//if(!(Z8_STATUS_11 & sending_realtime_to_midi_z11))
		CheckRealtimeMidi();
}
