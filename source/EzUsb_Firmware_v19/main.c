//#pragma NOIV	// Do not generate interrupt vectors 

#include <ezusb.h>
#include <ezregs.h>
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "main.h"
#include "usb.h"
#include "macros.inc"
#include "serial.h"
#include "bootloader.h"	 // V1.9

///////////////////// GLOBAL VARS ///////////////////////////////////////

// CTRL_REGS_7BITS_TO_Z8 must be sent to Z8 board when updated
unsigned char bdata CTRL_REGS_7BITS_TO_Z8[CTRL_REG_MAX_FROM_EZ_TO_Z8];
// bit 7 is always 0 since sent to Z8
// the following bits def MUST be duplicated in common_z8_ez.h file
	sbit usb_state_from_ez0_bit	 				= usb_state_from_ez0;	// DO NOT CHANGE ORDER
	sbit z8_board_fully_updated_from_ez0_bit	= z8_board_fully_updated_from_ez0;	
	sbit must_start_bootloader_from_ez0_bit		= must_start_bootloader_from_ez0;
	sbit usb_enumrated_from_ez0_bit				= usb_enumrated_from_ez0;

// Registers updated from the Z8 board (7 bits wide)
unsigned char bdata CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_NB_FROM_Z8_TO_EZ];
	sbit filter_state_z_ez0_bit 			= filter_state_z_ez0;
	sbit sync24_state_z_ez0_bit 			= sync24_state_z_ez0;
	sbit filter_type_z_ez0_bit 				= filter_type_z_ez0;
	sbit merger_src_midi_in_state_z_ez0_bit = merger_src_midi_in_state_z_ez0;
	sbit merger_src_usb_in_state_z_ez0_bit 	= merger_src_usb_in_state_z_ez0;
	sbit merger_dst_usb_out_state_z_ez0_bit = merger_dst_usb_out_state_z_ez0;
	sbit merger_dst_out1_state_z_ez0_bit 	= merger_dst_out1_state_z_ez0;

	sbit sync24_source_int_z_z_ez1_bit 		= sync24_source_int_z_z_ez1;
//V1.5	sbit sync24_source_ext_z_z_ez1_bit 		= sync24_source_ext_z_z_ez1;
//V1.5 	sbit arpp_state_z_ez1_bit 			= arpp_state_z_ez1;
	sbit ez_fw_midi_to_z8_z_ez1_bit 		= ez_fw_midi_to_z8_z_ez1;
	sbit arpp_midi_src_z_ez1_bit			= arpp_midi_src_z_ez1;
	sbit autotest_mode_z_ez1_bit			= autotest_mode_z_ez1;
	sbit realtime_source_int_z_ez1_bit 		= realtime_source_int_z_ez1;
	sbit realtime_source_ext_z_ez1_bit 		= realtime_source_ext_z_ez1;

// sync24 counters to clear lines : also shared with bootloader registers
volatile BYTE sync24_clear_clock_cpt;
volatile BYTE sync24_clear_cont_cpt;

xdata Midi_In_Struct MIDI_In_From_Con, MIDI_In_From_Usb, Serial_In_From_Z8;
idata Midi_In_Realtime_Struct MIDI_In_Realtime_From_Con, MIDI_In_Realtime_From_Usb, Realtime_From_Z8;

extern volatile BYTE bootloader_state; // V1.9

// ***************************************************************************
// Routine	: Check_Midi_In_Buffer()
// Input	: 
// Output	: None
// Process	: There are "pure" midi data in the buffer
// ***************************************************************************
void Check_Midi_In_Buffer(Midi_In_Struct *midi_struct, bit merger_src_enable)
{
	BYTE Start_Index_LOCAL;
	BYTE End_Index_LOCAL;

	Start_Index_LOCAL = midi_struct->start_index;
	EA = 0;
	End_Index_LOCAL   = midi_struct->end_index;
	EA = 1;

	// Check if something must be sent to Z8 Board (WID sysex or pure midi if arpeggio on)
	Check_MIDI_Buff_Send_To_Z8_Board(midi_struct, End_Index_LOCAL);

	// Check if current MIDI In buffer has to be merged
	if(merger_src_enable)
	{
		// merge only if midi filter is off OR midi filter realtime only is on 
		if( (!filter_state_z_ez0_bit) || (!filter_type_z_ez0_bit) )
		{
			// Merge to MIDI out 1 ?
			if(merger_dst_out1_state_z_ez0_bit)
			{
				// Merge to USB out CN0 ?
				if(merger_dst_usb_out_state_z_ez0_bit)
						Create_Send_USB_Frame_From_MIDI_Buffer(CN0, midi_struct, End_Index_LOCAL);
		
				// Send all bytes to MIDI Out Connector
				while (Start_Index_LOCAL != End_Index_LOCAL)
				{
					Send_To_Midi_Out_Con(midi_struct->buffer[Start_Index_LOCAL]);
					Start_Index_LOCAL = (Start_Index_LOCAL + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT -1);
				}
			} // end merge to midi out con 
	
		} // end midi filter allows merge
	} // end merger is on

	// Update treated pointers
	midi_struct->start_index = End_Index_LOCAL;
}

// ***************************************************************************
// Routine	: Check_Serial_In_From_Z8_Buffer()
// Input	: 
// Output	: None
// Process	: this buffer contains midi data + other registers [all framed]
// ***************************************************************************
void Check_Serial_In_From_Z8_Buffer(void)
{
	BYTE Start_Index_LOCAL;
	BYTE End_Index_LOCAL_minus_one, End_Index_LOCAL;
	BYTE frame_status, frame_type;
//	BYTE data_local; // V1.6

	Start_Index_LOCAL = Serial_In_From_Z8.start_index;

	// v1.1 assign end index - 1 to a local variable
	EA = 0;
	End_Index_LOCAL				=  Serial_In_From_Z8.end_index;
	EA = 1;
	End_Index_LOCAL_minus_one 	= (End_Index_LOCAL - 1)&(SERIAL_IN_FROM_Z8_BUFF_LENGHT-1);

	// Detect if there is a valid frame into the buffer
	//v1.1 while(Start_Index_LOCAL != Serial_In_From_Z8.end_index)
	while(Start_Index_LOCAL != End_Index_LOCAL)
	{
		// assign end index - 1 to a local variable
//		EA = 0;
//v1.1	End_Index_LOCAL				=  Serial_In_From_Z8.end_index;
//v1.1	End_Index_LOCAL_minus_one 	= (End_Index_LOCAL - 1)&(SERIAL_IN_FROM_Z8_BUFF_LENGHT-1);
//		EA = 1;
		// Check valid frame is present (if SOF and EOF are presents)
		if(Serial_In_From_Z8.buffer[Start_Index_LOCAL] 	       == SOF_TO_EZ)
		// if(Serial_In_From_Z8.buffer[(End_Index_LOCAL_minus_one-1)&(SERIAL_IN_FROM_Z8_BUFF_LENGHT-1)] == EOF_TO_EZ)
		if(Serial_In_From_Z8.buffer[End_Index_LOCAL_minus_one] == EOF_TO_EZ)
		{
			// Detect here which frame type has been received
			frame_status  = Serial_In_From_Z8.buffer[(Start_Index_LOCAL + 1)&(SERIAL_IN_FROM_Z8_BUFF_LENGHT-1)];
			frame_type = frame_status & FRAME_FROM_Z8_TO_EZ_TYPE_MSK;
			///////////////////////////////////// NEW CONTROL REG RECEIVED /////////////////////////
			///// 0xFF 0b000rrrrr 0b0vvvvvvv 0xFE (r=reg select 0-31) (v=reg value 0-127) //////////
			////////////////////////////////////////////////////////////////////////////////////////
			if(frame_type == FRAME_TO_EZ_IS_CTRL_REG)
			{
				// here we received a new control reg from Z8 board
				// data_local = Serial_In_From_Z8.buffer[(Start_Index_LOCAL + 2)&(SERIAL_IN_FROM_Z8_BUFF_LENGHT-1)]; // V1.6 
				
				// Bits 4-0 are the control reg address to update
				// CTRL_REGS_7BITS_FROM_Z8[(frame_status & CTRL_REG_Z8_TO_EZ_ADDR_MSK)] = data_local; // V1.6 Serial_In_From_Z8.buffer[(Start_Index_LOCAL + 2)&(SERIAL_IN_FROM_Z8_BUFF_LENGHT-1)];
				CTRL_REGS_7BITS_FROM_Z8[(frame_status & CTRL_REG_Z8_TO_EZ_ADDR_MSK)] = Serial_In_From_Z8.buffer[(Start_Index_LOCAL + 2)&(SERIAL_IN_FROM_Z8_BUFF_LENGHT-1)];

				// V1.6 each time the last control reg to ez is received then send sync back to Z8
				// This is used when the Z8 is in factory default, Sync to EZ occurs later and the Z8 still need to receive
				// registers from EZ 
/*				if( (frame_status & CTRL_REG_Z8_TO_EZ_ADDR_MSK) == LAST_CTRL_REG_7BITS_TO_EZ_INDEX)
				if( data_local == EZ_FULLY_UPDATED)
				{
					Send_Single_Framed_byte_To_Z8(CTRL_REG_0_7B_FROM_EZ_INDEX, (CTRL_REGS_7BITS_TO_Z8[0] | (1<<z8_board_fully_updated_from_ez0_bit_pos)));
					// z8_board_fully_updated_from_ez0_bit = 1;
				}
*/
				// V1.4 If autotest mode is active then turn on the USB led
				if(autotest_mode_z_ez1_bit) USB_LED_PORT_OUT |= USB_LED_PIN;

				// update start index
				Start_Index_LOCAL =  (Start_Index_LOCAL + CTRL_REG_Z8_FRAME_LENGHT) & (SERIAL_IN_FROM_Z8_BUFF_LENGHT -1 );
			} // end we received a control reg
			else
			///////////////////////////////////// NEW MIDI FRAME RECEIVED /////////////////////////
			///// 0xFF 0b001u12xx 0b0lllllll vv vv vv vv 0xFE (u=usb out en, 1/2=midi out1/2 en
			///// lllll = midi message lenght / vv = midi mess bytes
			////////////////////////////////////////////////////////////////////////////////////////
			if(frame_type == FRAME_TO_EZ_IS_MIDI)
			{
				// decode where to send this midi message from Z8 (out1, usb1, usb2)
				// Need to send to midi out 0 ?
				if(frame_status & send_to_out1)
				{
					// need to send to usb out 1 ?
					if(frame_status & send_to_usb) 
						Create_Send_USB_Frame_From_MIDI_Buffer(CN0, &Serial_In_From_Z8, End_Index_LOCAL);

					// Send all bytes to MIDI Out Connector
					//Start_Index_LOCAL =  (Start_Index_LOCAL + 3) & (SERIAL_IN_FROM_Z8_BUFF_LENGHT - 1);
					while (Start_Index_LOCAL != End_Index_LOCAL)
					{
						// V1.9 Check both MIDI in and USB in realtime buffers 
						//Check_Realtime();

						// need to skip sof from z8 board ?
						if(Serial_In_From_Z8.buffer[Start_Index_LOCAL] == SOF_TO_EZ)
							Start_Index_LOCAL =  (Start_Index_LOCAL + 3) & (SERIAL_IN_FROM_Z8_BUFF_LENGHT - 1);
						// send midi data to out connector
						Send_To_Midi_Out_Con(Serial_In_From_Z8.buffer[Start_Index_LOCAL]);
						// increase index
						Start_Index_LOCAL =  (Start_Index_LOCAL + 1) & (SERIAL_IN_FROM_Z8_BUFF_LENGHT - 1);
						// need to skip eof from z8 board ?
						if(Serial_In_From_Z8.buffer[Start_Index_LOCAL] == EOF_TO_EZ)
							Start_Index_LOCAL =  (Start_Index_LOCAL + 1) & (SERIAL_IN_FROM_Z8_BUFF_LENGHT - 1);
					}
				}

				// Check both MIDI in and USB in realtime buffers 
				Check_Realtime();

				// send to usb out 2 ?
				if(frame_status & send_to_usb) 
					if(frame_status & send_to_out2)
						Create_Send_USB_Frame_From_MIDI_Buffer(CN1, &Serial_In_From_Z8, End_Index_LOCAL);
				
				// all buffer now treated
				//v1.1 Start_Index_LOCAL =  End_Index_LOCAL;
				//v1.2 
				Start_Index_LOCAL =  End_Index_LOCAL;
			} // end frame is midi
		} // end frame is valid
	} // end start <> end index		

	// the buffer has now been treated
	Serial_In_From_Z8.start_index = Start_Index_LOCAL;
}


// ***************************************************************************
// Routine	: Check_Overflow_Flags()
// Input	: 
// Output	: None
// Process	: Check Overflow flags and send results to lcd Z8, results are sent
//			  sequencialy so we avoid message missing	
// ***************************************************************************
void Check_Overflow_Flags(void)
{
	// check realtime from Midi in connector overflow flags
	if(MIDI_In_Realtime_From_Con.input_status & realtime_overflow)
	{
		// send to Z8 board
		Send_Single_Framed_byte_To_Z8(/*v1.1 FRAME_IS_COMMON_PARAM,*/ LCD_MESSAGE_TO_Z8_INDEX, midi_in_realtime_overflow_z8);
		// clear overflow flag
		MIDI_In_Realtime_From_Con.input_status &= ~realtime_overflow;
	}

	// check Midi in connector overflow flags
	if(MIDI_In_From_Con.input_status & input_overflow)
	{
		// send to Z8 board
		Send_Single_Framed_byte_To_Z8(/*v1.1 FRAME_IS_COMMON_PARAM,*/ LCD_MESSAGE_TO_Z8_INDEX, midi_in_overflow_z8);
		// clear overflow flag
		MIDI_In_From_Con.input_status &= ~input_overflow;
	}

	if(usb_state_from_ez0_bit)
	{
		// check realtime from USB in overflow flags
		if(MIDI_In_Realtime_From_Usb.input_status & realtime_overflow)
		{
			// send to Z8 board
			Send_Single_Framed_byte_To_Z8(/*v1.1 FRAME_IS_COMMON_PARAM,*/ LCD_MESSAGE_TO_Z8_INDEX, usb_in_realtime_overflow_z8);
			// clear overflow flag
			MIDI_In_Realtime_From_Usb.input_status &= ~realtime_overflow;
		}
	
		// check Usb in overflow flags
		if(MIDI_In_From_Usb.input_status & input_overflow)
		{
			// send to Z8 board
			Send_Single_Framed_byte_To_Z8(/*v1.1 FRAME_IS_COMMON_PARAM,*/ LCD_MESSAGE_TO_Z8_INDEX, usb_in_overflow_z8);
			// clear overflow flag
			MIDI_In_From_Usb.input_status &= ~input_overflow;
		}
	}
}


// ***************************************************************************
// Routine	: Check_Realtime()
// Input	: 
// Output	: None
// Process	: Check if Realtime MIDI has to be merged/treated 
// ***************************************************************************
void Check_Realtime(void)
{
	if(bootloader_state != BOOTLOADER_OFF) return;

	// Check Realtime From Z8
	Check_Realtime_Buffers(&Realtime_From_Z8, 1);

	// Check Realtime MIDI Input buffer
	Check_Realtime_Buffers(&MIDI_In_Realtime_From_Con, merger_src_midi_in_state_z_ez0_bit);

	if(usb_state_from_ez0_bit)
	{
		// Check Realtime USB MIDI Input buffer 
		Check_Realtime_Buffers(&MIDI_In_Realtime_From_Usb, merger_src_usb_in_state_z_ez0_bit);
	}
}


// ***************************************************************************
// Routine	: Check_Realtime_Buffers()
// Input	: 
// Output	: None
// Process	: Check if incoming Realtime MIDI has to be merged/treated 
// ***************************************************************************
void Check_Realtime_Buffers(Midi_In_Realtime_Struct *midi_struct, bit merger_src_enable)
{
	BYTE realtime_start_index_local = midi_struct->realtime_start_index;
	BYTE realtime_data_local;	

	// if realtime data is Active sense 0xFE or 0xFF then do not send to Z8 board
	// if midi filter is On then exit
	// if merger on MIDI out 1 is enabled then immediately send to MIDI out 1
	// if merger on USB  out 1 is enabled then forward incoming midi events
	// always send realtime to z8
	// send realtime to outputs midi & usb if respective mergers destination are on 
	
	// for each byte into the realtimer buffer
	while(realtime_start_index_local != midi_struct->realtime_end_index)
	{
		realtime_data_local = midi_struct->realtime[realtime_start_index_local];

		// check for possible merges to midi out 1 and usb out 1
		if(!filter_state_z_ez0_bit)
		{
			// no midi filter, is the merger source on ?
			if(merger_src_enable)
			{
				// is the merger destination usb out 1 active ?
				if(usb_state_from_ez0_bit)
				// V1.69 if(merger_dst_usb_out_state_z_ez0_bit)
				if((merger_dst_usb_out_state_z_ez0_bit) || (midi_struct == &Realtime_From_Z8))
					Create_Send_Realtime_USB_Frame_From_MIDI_Buffer( CN0, realtime_data_local);

				// is the merger destination midi out 1 active ?
				// V1.69 if(merger_dst_out1_state_z_ez0_bit)
				if( (merger_dst_out1_state_z_ez0_bit) || (midi_struct == &Realtime_From_Z8))
					Send_To_Midi_Out_Con(realtime_data_local);
			} // end merger source is active
		} // end midi filter is off
	
		// Check if must send to Z8 board : MUST FILTER ACTIVE SENSE HERE FE and also FF (since SOF & EOF)
		// also send realtime to Z8 only if realtime source is not Z8 (to avoid loopback)
		if(midi_struct != &Realtime_From_Z8)
			if(realtime_data_local < MIDI_ACTIVE_SENSE)
				// Realtime MIDI is NOT encapsuled into frames when sent to Z8 board
				Send_To_Serial_Z8(realtime_data_local);


		// Now check if Sync24 Must be updated
/* removed block in V1.9
		// V1.5 new block Now check if Sync24 Must be updated
		if(sync24_state_z_ez0_bit)
		{
			// Realtime message comming from Internal sync ?
			if(midi_struct == &Realtime_From_Z8)
			{
				// internal sync ?
				if(!sync24_source_int_z_z_ez1_bit)	
					Update_Sync24(realtime_data_local);		
			}
			else
			// Realtime messages comming from external 
			{
				// external sync ?
				if(sync24_source_int_z_z_ez1_bit)	
					Update_Sync24(realtime_data_local);		
			}
		} // end sync24 is enabled
 */
		realtime_start_index_local = (realtime_start_index_local + 1) & (MIDI_IN_REALTIME_BUFF_LENGHT-1);
	} // end realtime buffer contains data

	// Update treated pointers
	midi_struct->realtime_start_index = realtime_start_index_local;
}

// ***************************************************************************
// Routine	: Update_Sync24()
// Input	: char that contains realtime event
// Output	: None
// Process	: Parse realtime event received and transform to SYNC24 Output
// ***************************************************************************
void Update_Sync24(unsigned char data_received)
{
	if(data_received == MIDI_CLOCK)
	{
		// Here MIDI Clock is supposed to be low, send a pulse of 4 ms
		// Timer 0 (2ms) is used to reach 4 ms required duration
		// loading counter with 3 will result in 4 to 6 ms counter
		SYNC24_CLOCK_PORT_OUT  |=  SYNC24_CLOCK_PIN;// Set CLOCK = 1
		// Now must clear the above pin in 5 ms
		sync24_clear_clock_cpt = 3; // 6 ms
	}
	else
	if(data_received == MIDI_START)
		SYNC24_RUN_PORT_OUT  |=  SYNC24_RUN_PIN; 	// Set RUN = 1
	else
	if(data_received == MIDI_STOP)
		SYNC24_RUN_PORT_OUT  &= ~SYNC24_RUN_PIN; 	// Set RUN = 0
	else
	if(data_received == MIDI_CONTINUE)
	{
		SYNC24_RUN_PORT_OUT  |=  SYNC24_RUN_PIN; 	// Set RUN = 1
		SYNC24_CONT_PORT_OUT |=  SYNC24_CONT_PIN;	// Set CONT = 1
		// Now must clear the above pin in 4 ms
		sync24_clear_cont_cpt = 3; // 6ms
	}
}

 /*
// ***************************************************************************
// Routine	: Check_Realtime_From_Z8()
// Input	: 
// Output	: None
// Process	: Check if Realtime MIDI has to be sent from Z8 internal midi sync 
// ***************************************************************************
void Check_Realtime_From_Z8(void)
{

}
*/