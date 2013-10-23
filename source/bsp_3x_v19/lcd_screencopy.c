#ifdef LCD_SCREEN_COPY

#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd.h"
#include "lcd_low_level.h"
#include "lcd_screencopy.h"
#include "hardware.h"
#include "serial.h"

// THIS FILE IS ONLY USED IN LCD_SCREENCOPY CONFIGURATION OF Z8 COMPILER
// IT ALLOWS TO SEND SCREEN COPY OF THE LCD TO SYSEXS ON MIDI OUT 2 ONLY
// SYSEXs FORMAT IS :

// F0 00 20 4F 00 50 + X_OFFSET + Y_LINE + 64 NIBBLES OF PIXELS F7 (MSB first)
// with X_OFFSET = 0 : pixel  0 to 31  sent as 64 Nibbles
// with X_OFFSET = 1 : pixel 32 to 63  sent as 64 Nibbles
// with X_OFFSET = 2 : pixel 64 to 95  sent as 64 Nibbles
// with X_OFFSET = 3 : pixel 96 to 127 sent as 64 Nibbles
// with Y_LINE   = 0 to 7 (represent LCD line)


///////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////////

far uchar lcd_pix[32];
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes


void SendLcdScreenCopyToSysexs(void)
{
	uchar pix_offset;
	uchar lcd_line;
	uchar pix_pos;
	uchar current_x_pix;

	for(lcd_line=0; lcd_line<LCD_Y_PAGE_MAX ;lcd_line++)
	{
		current_x_pix = 0;
		SetCurrentXY(current_x_pix, lcd_line);
		for(pix_offset=0; pix_offset<4 ;pix_offset++) // 4 * 64 nibbles = 128 bytes
		{
			for(pix_pos=0; pix_pos<32 ;pix_pos++) // 4 * 64 nibbles = 128 bytes
			{
				// treat 32 pixels here
				if(current_x_pix == 0)
				{
					SetCurrentXY(current_x_pix, lcd_line);
					lcd_pix[pix_pos] = ReadFromLcd(current_x_pix); // dummy
				}
				else
				if(current_x_pix == 64)
				{
					SetCurrentXY(current_x_pix, lcd_line);
					lcd_pix[pix_pos] = ReadFromLcd(current_x_pix); // dummy
				}

				lcd_pix[pix_pos] = ReadFromLcd(current_x_pix);
				current_x_pix++;
			}
			SendLCDSysexToMidiOut2(lcd_line, pix_offset);
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	WriteToLcd()
// Inputs	: 	
///////////////////////////////////////////////////////////////////////////////////////
uchar ReadFromLcd(uchar current_x_pixel)
{
	uchar value;	

	// Extract chip selects lcd left / right
	if(current_x_pixel > 63)
		LCD_CS1_PORT_OUT &= (~LCD_CS1_PIN); // clear cs1
	else
		LCD_CS2_PORT_OUT &= (~LCD_CS2_PIN); // clear cs2

	// wait pweh = 450ns
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	// Wait for the LCD to become ready, wait for busy flag 
	WaitLcdBusyBit();

	// wait pweh = 450ns
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

	// Clear RW to allows LCD write 
//	LCD_RW_PORT_OUT &= (~LCD_RW_PIN);

	// Set Z8F lcd bus to output
 	PFADDR 	= PORT_DIR;     				// Acces to sub-reg direction
	PFCTL 	= 0xFF;    						// Port direction 1=in 0=out

	// Extract data type, RS was already low after busy bit checking
	LCD_RS_PORT_OUT |= LCD_RS_PIN; 		// set RS (DI) if data to write

	// Pulse LCD E pin
	asm("nop");
	asm("nop");
	// set E high
	LCD_E_PORT_OUT |= LCD_E_PIN; 			// set E to high

	// wait pweh = 450ns
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

   	// place data to write on the bus
	value = LCD_DATA_PORT_IN;

	// set E low
	LCD_E_PORT_OUT &= (~LCD_E_PIN); 			// set E to low

	// wait pwel = 450ns
	asm("nop");
	asm("nop");


	// Disable both cs1 and cs2 to avoid spurious writes
	LCD_CS1_PORT_OUT |= LCD_CS1_PIN; 		// set cs1
	LCD_CS2_PORT_OUT |= LCD_CS2_PIN; 		// set cs2

	return(value);
}


void SendLCDSysexToMidiOut2(uchar lcd_line, uchar pix_offset)
{
	generic_buffer[0] = MIDI_SOX;
	generic_buffer[1] = WID_SYX_ID0;
	generic_buffer[2] = WID_SYX_ID1;
	generic_buffer[3] = WID_SYX_ID2;
	generic_buffer[4] = WID_SYX_ID3;
	generic_buffer[5] = WID_SYX_ID4;
	generic_buffer[6] = 0x50;
	generic_buffer[7] = pix_offset; 
	generic_buffer[8] = lcd_line;
	// Now copy all LCD bytes to nibbles
	Byte_To_Generic_Nibbles(9, 64, &lcd_pix[0]);
	// Send EOX
	generic_buffer[73] = MIDI_EOX;
	SendDispatchToMidi(&generic_buffer[0],74 , midi_out_2_en); 
}

#endif
