#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd_low_level.h"
#include "hardware.h"
#include "lcd.h"
#include "eeprom.h"
#include "realtime.h" // V1.9
//#include "lcd_fonts.h"
//#include "lcd_bmps.h"

///////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////////

rom uchar * current_font_ptr;	// V1.7

extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion	
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving
extern rom uchar identity[];

extern rom uchar font_5x8[];		// V1.66
extern rom uchar font_num_8x16[];	// V1.66
extern rom uchar font_7seg_5x8[];	// V1.66

//V1.66 uchar lcd_lock_all_cpt;						// allow to lock all LCD zones
//V1.66 rom uchar font_7seg_5x8[];

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitLcd()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Graphic LCD
///////////////////////////////////////////////////////////////////////////////////////
void InitLcd(void)
{
	// If PE2 controls LCD reset then first reset LCD
#ifdef PE2_CONTROL_LCD_RESET
	// V1.5 new block for CME LCD : requires reset Vih > 0.7xVdd > 3.5V
	// Set reset lcd pin low
	LCD_RESET_PORT_OUT &= (~LCD_RESET_PIN);
 	// wait 50 usec (10 us min)
	delay_usec(50);
	// depending on hardware version the lcd must be initialized differently (cme lcd requires reset input > 3.5V)
	// so the uC output at 3.3V cannot drive directly the lcd reset input
	// on cme hardware (v2.0 and higher), a pullup to 5v is placed on lcd reset input and the uC becomes an input 
	// to allow the pullup to drive lcd reset input to 5V which is > 3.5V
	if(identity[HW_VERSION_INDEX] < 2)
		// on hardware < 2.0, there is no pullup on lcd reset pin, so the output must drive high the lcd reset input
		LCD_RESET_PORT_OUT |= LCD_RESET_PIN;
	else
		// Set reset lcd pin as an input : the V1.66al pullup will set a high voltage 5V on LCD reset input
		PECTL |= LCD_RESET_PIN;

	delay_msec(50);

/*
	// Set reset lcd pin high
	LCD_RESET_PORT_OUT |= LCD_RESET_PIN;
 	// wait 10 usec
	delay_usec(10);
	// Set reset lcd pin low
	LCD_RESET_PORT_OUT &= (~LCD_RESET_PIN);
 	// wait 10 usec
	delay_usec(10);
	// Set reset lcd pin high
	LCD_RESET_PORT_OUT |= LCD_RESET_PIN;
	delay_usec(20);
*/
#endif

	// Initliaze default registers
	WriteToLcd(LCD_LEFT , LCD_COMMAND, SET_RAM_0);
	WriteToLcd(LCD_RIGHT, LCD_COMMAND, SET_RAM_0);
	WriteToLcd(LCD_LEFT , LCD_COMMAND, SET_Y);
	WriteToLcd(LCD_RIGHT, LCD_COMMAND, SET_Y);
	WriteToLcd(LCD_LEFT,  LCD_COMMAND, SET_X);
	WriteToLcd(LCD_RIGHT, LCD_COMMAND, SET_X);

	// V1.5 clear the display after initialization
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);

	// Now Turn on the display
	WriteToLcd(LCD_LEFT , LCD_COMMAND, DISPLAY_ON);
	WriteToLcd(LCD_RIGHT, LCD_COMMAND, DISPLAY_ON);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	WriteToLcd()
// Inputs	: 	LCD_DATA, LCD_COMMAND and value to write
// Outputs	: 	None
// Process  : 	First wait for busy flag
//				write to lcd data or command
//				cs1 and cs2 are high at the function input and output
///////////////////////////////////////////////////////////////////////////////////////
void WriteToLcd(uchar current_x_pixel, uchar data_type, uchar value)
{
	// Extract chip selects lcd left / right
	if(current_x_pixel > 63)
		LCD_CS1_PORT_OUT &= (~LCD_CS1_PIN); // clear cs1
	else
		LCD_CS2_PORT_OUT &= (~LCD_CS2_PIN); // clear cs2

#ifdef MICROTIPS_MTGS12864KFGHSGW01 
	// V1.3 if(Z8_STATUS_0_NVM & video_mode_z0nvm)
	if(Z8_STATUS_2_NVM & video_mode_z2nvm)
	{
		// Invert video mode if required
		if(!(data_type & LCD_INVERT))
			if(data_type & LCD_DATA)
		 		 value = ~value; 
	}
	else
	{
		if(data_type & LCD_INVERT)
			value = ~value; 
	}
#endif
#ifdef SUNLIKE_SG12864HBWBGBK 
	// Invert video mode if required
	if(data_type & LCD_INVERT)
		// V1.3 if(Z8_STATUS_0_NVM & video_mode_z0nvm)
		if(Z8_STATUS_2_NVM & video_mode_z2nvm)
			value = ~value; 
#endif

	// Wait for the LCD to become ready, wait for busy flag 
	WaitLcdBusyBit();

	// Clear RW to allows LCD write 
	LCD_RW_PORT_OUT &= (~LCD_RW_PIN);

	// Extract data type, RS was already low after busy bit checking
	if(data_type & LCD_DATA)
		LCD_RS_PORT_OUT |= LCD_RS_PIN; 		// set RS (DI) if data to write

	// Set Z8F lcd bus to output
 	PFADDR 	= PORT_DIR;     				// Acces to sub-reg direction
	PFCTL 	= 0x00;    						// Port direction 1=in 0=out

   	// place data to write on the bus
	LCD_DATA_PORT_OUT = value;

	// Pulse LCD E pin
	SetLcdEPulse();

	// Disable both cs1 and cs2 to avoid spurious writes
	LCD_CS2_PORT_OUT |= LCD_CS2_PIN; 		// set cs2	
	LCD_CS1_PORT_OUT |= LCD_CS1_PIN; 		// set cs1

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WaitLcdBusyBit()
// Inputs	: None 
// Outputs	: None
// Process  : Wait until LCD busy bit is ok
///////////////////////////////////////////////////////////////////////////////////////
void WaitLcdBusyBit(void)
{
	uchar lcd_busy;

	// Set Z8F lcd bus to input
 	PFADDR 	= PORT_DIR;     				// Acces to sub-reg direction
	PFCTL 	= 0xFF;    						// Port direction 1=in 0=out

	asm("nop");
	asm("nop");

	// clear RS (DI) to read LCD status
	LCD_RS_PORT_OUT &= (~LCD_RS_PIN); 	
	
	// set RW to high 
	LCD_RW_PORT_OUT |= LCD_RW_PIN;
	
	do {
			// Wait Tasu (140 ns)
			asm("nop");
			asm("nop");
			// set E high
			LCD_E_PORT_OUT |= LCD_E_PIN; 			// set E to high
			// wait Td = 320ns
			asm("nop");
			asm("nop");
			asm("nop");
			// asm("nop");
			lcd_busy = LCD_DATA_PORT_IN & BUSY_FLAG;
			LCD_E_PORT_OUT &= (~LCD_E_PIN); 			// set E to low		

		//	SetLcdEPulse();
	} 	
	// Now poll bit 7 (busy) until it is 0
	while(lcd_busy);	

	// wait pwel = 450ns
	asm("nop");
	asm("nop");
//	asm("nop");


/*
	// Set Z8F lcd bus to input
 	PFADDR 	= PORT_DIR;     				// Acces to sub-reg direction
	PFCTL 	= 0xFF;    						// Port direction 1=in 0=out

	asm("nop");
	asm("nop");

	// clear RS (DI) to read LCD status
	LCD_RS_PORT_OUT &= (~LCD_RS_PIN); 	
	
	// set RW to high 
	LCD_RW_PORT_OUT |= LCD_RW_PIN;

	// Wait Tasu (140 ns)
	asm("nop");
	asm("nop");
	// set E high
	LCD_E_PORT_OUT |= LCD_E_PIN; 			// set E to high

	// wait Td = 320ns
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	
	// Pulse LCD E pin
#ifndef SIMUL
	// Now poll bit 7 (busy) until it is 0
	while(LCD_DATA_PORT_IN & BUSY_FLAG);	
#endif
	LCD_E_PORT_OUT &= (~LCD_E_PIN); 			// set E to low

	// wait pwel = 450ns
	asm("nop");
	asm("nop");
//	asm("nop");
*/
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SetLcdEPulse()
// Inputs	: None
// Outputs	: None
// Process  : Set a pulse LOW-HIGH-LOW to LCD_E pin
///////////////////////////////////////////////////////////////////////////////////////
void SetLcdEPulse(void)
{
//	asm("nop");								// V1.6
	asm("nop");
	asm("nop");
	// set E high
	LCD_E_PORT_OUT |= LCD_E_PIN; 			// set E to high

	// wait pweh = 450ns
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	// set E low
	LCD_E_PORT_OUT &= (~LCD_E_PIN); 		// set E to low

	// wait pwel = 450ns
	asm("nop");
	asm("nop");
//	asm("nop");								// V1.6
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SetCurrentXY()
// Inputs	: 	X column (0-127) and Y page row (0-7) 
// Outputs	: 	None
// Process  : 	Set the current LCD zones X (0-127) and Y (page 0-7)
///////////////////////////////////////////////////////////////////////////////////////
void SetCurrentXY(uchar x_pixel, uchar y_page)
{
	uchar lr_select = LCD_LEFT;

	// check if x > 63 : RIGHT part of LCD
	if(x_pixel > 63)
	{
		lr_select = LCD_RIGHT;
		x_pixel -= 64; 
	}

	WriteToLcd(lr_select, LCD_COMMAND, SET_Y | x_pixel);    // Set current x 0-63
	WriteToLcd(lr_select, LCD_COMMAND, SET_X | y_page); 	// set current y page 0-7
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	IntToLcdText()
// Inputs	: 	value and formating
// Outputs	: 	None
// Process  : 	value must be 7 bits max 0 - 127
// 				data_format is defined as follow : 	
//					bit1   : 0 = 2 digits / 1 = 3 digits	
//////////////////////////////////////////////////////////////////////////////////////////////
void IntToLcdText(uchar value, uchar lcd_format)
{
   uchar loc_lcd_char = '0';
   uchar loc_tmp_value;
   uchar lcd_text_index = 0;

   // Assign value to all local reg 
   loc_tmp_value = value;

   // Special treatment for 3 digits data
   if(lcd_format & DIGITS_3)
   {
     if(value < 100) 
	   	lcd_text_value[lcd_text_index] = '0';
	 else
     if(value < 200) 
	 { 	
	 	lcd_text_value[lcd_text_index] = '1';
		loc_tmp_value -= 100;
	 }
	 else
	 { 	
	 	lcd_text_value[lcd_text_index] = '2';
		loc_tmp_value -= 200;
	 }

	lcd_text_index++;
  }

   // Treat now the 2 remaining digits
   while(loc_tmp_value > 9)
   {
      loc_tmp_value = loc_tmp_value - 10;
      loc_lcd_char++;
   }

   lcd_text_value[lcd_text_index++] = loc_lcd_char;
   lcd_text_value[lcd_text_index++] = loc_tmp_value + '0';	
   lcd_text_value[lcd_text_index]   = '\0'; 	// mark end of text	

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayLcdInt()
// Inputs	: 	x, y 
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayLcdInt(uchar x_pixel, uchar y_page, uchar lcd_format, /* V1.7 rom uchar * font_ptr ,*/ uchar value)
{

	// Transform value to text
	IntToLcdText(value, lcd_format);

	// Now lcd_text_value contains text to display
	DisplayLcdText(x_pixel, y_page, lcd_format /* V1.7, font_ptr */);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayLcdText()
// Inputs	: 	x, y and formating
// Outputs	: 	None
// Process  : 	Display text contained in global table lcd_text_value
//				Max lenght is 8, display untils it reaches 0
//				data_format is defined as follow :
//					bit 0 : not used
//					bit 1 : font selection : PIX_HEIGHT_16 or PIX_HEIGHT_8
//					bit 2-4 : leading pixel space before character
//					IMPORTANT RESTRICTION : 1 CAR MUST BE 2N PAGE HEIGHT
//					if font_ptr is font_7seg_5x8, all caracters >= 0x40 means a dot must be added
//					So add a dot and remove 0x40 to value
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayLcdText(uchar x_pixel, uchar y_page, uchar lcd_format /* V1.7 , rom uchar * font_ptr */)
{
	uchar lcd_text_index = 0;									// index on text table to display
//	rom uchar * font_ptr;	// V1.7
	uchar offset_per_char;
	uchar raw_y_page_per_char_2n;
	uchar raw_y_page_per_char;					// Nb of page per raw block
	uchar raw_leading_space;					// Nb of space to add before/after a raw block
	uchar font_first_char;						// for uncomplete font, allow good caracter indexing
	uchar index_x_pixel;						// raw x index 
	uchar index_y_page;							// raw page index
	unsigned int font_index;					// caracter index in the whole font
	uchar current_x_pixel;						// initial x pixel is x_pixel+ignored lead spaces
	uchar raw_total_x_pixel;					// Nb of x pixel for a raw block 
	uchar index_x_pixel_page;					// index to de-entrelace x pixel
	uchar text_char_x_index;					// First x index of each caracter to display
	uchar start_index;
//	uchar must_add_dot;
	
	// V1.7 retriewe font from flags
	current_font_ptr = font_5x8;
	if(lcd_format & FONT_NUM_8x16_FLG)
		current_font_ptr = font_num_8x16;
	else
	if(lcd_format & FONT_7SEG_5x8_FLG)
		current_font_ptr = font_7seg_5x8;

	offset_per_char 		= current_font_ptr[0];
	raw_y_page_per_char_2n 	= current_font_ptr[1];
	raw_y_page_per_char 	= 1 << raw_y_page_per_char_2n;	// Nb of page per raw block
	raw_leading_space	 	= current_font_ptr[2];					// Nb of space to add before/after a raw block
	font_first_char 		= current_font_ptr[3];					// for uncomplete font, allow good caracter indexing
	current_x_pixel 		= x_pixel + raw_leading_space;
	text_char_x_index 		= current_x_pixel,
	start_index 			= raw_leading_space;

	// save total x pixels per text caracter (2x leading spaces + nb_x_pixel_per_char)
	raw_total_x_pixel = raw_leading_space + (offset_per_char >> raw_y_page_per_char_2n);

	// Add information to lcd format
	lcd_format 		  |= LCD_DATA;

	// For each caracter to display
  	while(lcd_text_value[lcd_text_index] != '\0')
	{	
		// get car offset from the whole font
		font_index = lcd_text_value[lcd_text_index] - font_first_char;
		font_index = (font_index * offset_per_char) + FONT_START_OFFSET;

		// If one block is taller than one Y page, pass trough all pages
		for(index_y_page=0; index_y_page  < raw_y_page_per_char ; index_y_page++)
		{
			// Change Y page if required
			SetCurrentXY(text_char_x_index, (y_page + index_y_page));

			current_x_pixel    = text_char_x_index;
			index_x_pixel_page = 0;

			for(index_x_pixel=start_index; index_x_pixel<raw_total_x_pixel ; index_x_pixel++)
			{
				// Need to change LCD half part ?
				if(current_x_pixel == (LCD_X_PIXEL_MAX >> 1))
					SetCurrentXY(current_x_pixel, (y_page + index_y_page));

				// add leading spaces if required
				if(index_x_pixel < raw_leading_space)
						WriteToLcd(current_x_pixel, lcd_format, 0);	
				else
				{
					WriteToLcd(current_x_pixel, lcd_format, current_font_ptr[(font_index+index_x_pixel_page+index_y_page)]);
				 	// x pointer must be increased by 2 if 2 pages per char since raw page are entrelaced
					index_x_pixel_page += raw_y_page_per_char;
				}
				
				current_x_pixel++;
			}
	  }
	  // First caracter passed, no more need to ignore leading spaces
	  start_index = 0;
	  // LCD x pixel index for next char
	  text_char_x_index += raw_total_x_pixel;
	  // remove offset due to first char ignored leading spaces
	  if(lcd_text_index==0) text_char_x_index -= raw_leading_space;	  
	  lcd_text_index++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayBmp()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayBmp(uchar start_x_pixel, uchar start_y_page, uchar lcd_format, rom uchar * bmp_ptr)
{
	uchar index_y_page;
	uchar index_x_pixel;
	uchar x_pixel_max 	   = bmp_ptr[0] + start_x_pixel;
	uchar y_page_max   	   = bmp_ptr[1] + start_y_page;
	unsigned int bmp_index = BMP_START_OFFSET;

	// LCD is locked ?
//	if(lcd_lock_all_cpt) return;

	// Add information to lcd format
	lcd_format |= LCD_DATA;

	for(index_y_page=start_y_page; index_y_page  < y_page_max ; index_y_page++)
	{
		SetCurrentXY(start_x_pixel, index_y_page);
		for(index_x_pixel=start_x_pixel; index_x_pixel<x_pixel_max ; index_x_pixel++)
		{
			// need to change LCD half screen
			if(index_x_pixel == (LCD_X_PIXEL_MAX >> 1))
				SetCurrentXY(index_x_pixel, index_y_page);
			// now write to lcd
			WriteToLcd(index_x_pixel, lcd_format, bmp_ptr[bmp_index++]);	
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	FillLcdZone()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	used to clear display
//////////////////////////////////////////////////////////////////////////////////////////////
void FillLcdZone(uchar start_x, uchar start_y, uchar size_x, uchar size_y, uchar fill_value)
{
	uchar index_y_page;
	uchar index_x_pixel;
	uchar x_max = start_x + size_x;
	uchar y_max = start_y + size_y;

	for(index_y_page=start_y; index_y_page  < y_max ; index_y_page++)
	{
		SetCurrentXY(start_x, index_y_page);
		for(index_x_pixel=start_x; index_x_pixel < x_max ; ++index_x_pixel)
		{
			if(index_x_pixel == (LCD_X_PIXEL_MAX >> 1))
				// WriteToLcd(index_x_pixel, LCD_COMMAND, SET_Y ); // set current x pixel to 64 (0)
				SetCurrentXY(index_x_pixel, index_y_page);
			
			WriteToLcd(index_x_pixel, LCD_DATA, fill_value);	

			//if(index_x_pixel & 1)
				// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
				// CheckRealtimeMidi();
		}
	}
}



