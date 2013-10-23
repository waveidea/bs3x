//#pragma NOIV	// Do not generate interrupt vectors
 
// **************************** SERIAL.C *********************************
// All code relating to serial ports 0 and 1
// ***********************************************************************

#include <ezusb.h>
#include <ezregs.h>
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "macros.inc"
#include "serial.h"
#include "bootloader.h"
#include "itr.h"

// GLOBAL VARS
idata unsigned char TX_To_Midi_Out_Con_Buff[SERIAL_TX_BUFF_LENGTH];
xdata unsigned char TX_To_Serial_Z8_Buff[SERIAL_TX_BUFF_LENGTH]; // V1.8 idata

unsigned char tx_to_midi_out_con_start_index;
unsigned char tx_to_midi_out_con_end_index;
bit tx_to_midi_out_con_busy;
bit tx_to_midi_out_con_full;

unsigned char tx_to_serial_z8_start_index;
unsigned char tx_to_serial_z8_end_index;
bit tx_to_serial_z8_busy;
bit tx_to_serial_z8_full;

// FROM MAIN.C
extern xdata Midi_In_Struct  MIDI_In_From_Con, MIDI_In_From_Usb;
extern bit must_start_bootloader_from_ez0_bit;
extern bit ez_fw_midi_to_z8_z_ez1_bit;
// V1.5 extern bit arpp_state_z_ez1_bit;
// V1.5 extern bit arpp_midi_src_z_ez1_bit;

// FROM IDENTITY.C
extern unsigned char code identity[];

// FROM BOOTLER.C
extern volatile BYTE bootloader_state;
extern BYTE fw_bl_src;

unsigned char code wid_sysex_header[] = {
MIDI_SOX, WID_SYX_ID0, WID_SYX_ID1, WID_SYX_ID2, WID_SYX_ID3, WID_SYX_ID4 
};

/* ******************************************************************** */
/* Routine: Init_Serial_Ports											*/
/* Process: SERIAL PORT 0 & 1 INITIALIZATIONS, based on 24Mhz Xtal		*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* Process: Serial ports 0 uses Timer 2 @ 31250 b/sec					*/
/*			Serial ports 1 uses Timer 1 @ 57600 b/sec					*/
/* ******************************************************************** */
void Init_Serial_Ports(void)
{

//             K * 24 000 000
//Baud-Rate= ------------------------ = ((SMOD[7]+1) * 62500) / (256-TH1)
//             32 * 12 * (256- TH1) 

	// TIMERS 0 & 1 COMMON
    TMOD  = 0x21;       	   	// TMOD: timer 1, mode 2, 8-bit reload 
								//		 timer 0, mode 1, 16bits counter 

	// TIMER O : 2ms counter
    TH0  = (TIMER0_RELOAD >> 8);  		// Setup reload value
    TL0  = (TIMER0_RELOAD & 0xFF);		// Setup overflow value
	ET0  = 1;							// enable timer 0 interrupt    
	TR0  = 0;                   		// TR0:  timer 0 is off for now

	// TIMER 1 : BAUD RATE GENERATOR FOR SERIAL PORTS 1
    // 62500 Bauds rate registers
//V1.2    TH1  = 256-SERIAL_TO_Z8_BAUD_RELOAD;  	// Setup Baudrate reload value
//V1.2    TL1  = 256-SERIAL_TO_Z8_BAUD_RELOAD;  	// Setup Baudrate overflow value
    TH1  = 256-SERIAL_TO_OCD_Z8_BAUD_RELOAD;  	// Setup Baudrate reload value
    TL1  = 256-SERIAL_TO_OCD_Z8_BAUD_RELOAD;  	// Setup Baudrate overflow value
    TCON = 0x40;							// clear timer 1 itrs flags 
//	TR1  = 1;                   		  	// TR1:  timer 1 run
	

	// TIMER 2 : BAUD RATE GENERATOR FOR SERIAL PORTS 0
    // 32150 Bauds rate registers
//    TH2  	= MSB(MIDI_OUT_1_BAUD_RELOAD);  // Setup Baudrate reload value
//    TL2  	= LSB(MIDI_OUT_1_BAUD_RELOAD);	// Setup Baudrate overflow value
    RCAP2H  = MSB(MIDI_OUT_1_BAUD_RELOAD);	// Setup Baudrate reload value
    RCAP2L  = LSB(MIDI_OUT_1_BAUD_RELOAD);	// Setup Baudrate overflow value
   	T2CON = 0x34;							// Set timer 2 for serial port 0
//    TR2  = 1;                   			// TR2:  timer 2 is on

   	// Configure serial port 0 registers
	PCON  |= 0x80;      /* SMOD0=1 baud rate double enabled */
	SCON0 = 0x50;		/* SCON: mode 1, 8-bit UART, enable rcvr (0x50) */

	// Configure serial port 1 registers
	EICON |= 0x80;      /* SMOD1=1 baud rate double enabled */
	SCON1 = 0x50;		/* SCON1: mode 1, 8-bit UART, enable rcvr (0x50) */

	// Turn on serial port 0 : TO/FROM MIDI IN / OUT1 CONNECTORS
	ES0 = 1;			/* enable serial port 0 interrupts */
	// Turn on serial port 1 : TO/FROM Z8 SERIAL PORT
	ES1 = 1;			/* enable serial port 1 interrupts */
}

// ***************************************************************************
// Routine	: Send_To_Midi_Out_Con()
// Input	: data_to_send
// Output	: None
// Process	: Send a single byte to MIDI Out connector
// ***************************************************************************
void Send_To_Midi_Out_Con(BYTE data_to_send)
{
	while( tx_to_midi_out_con_full );

	EA = 0;	// Disable 8051 Interrupts

	if( !tx_to_midi_out_con_busy )
	{
		tx_to_midi_out_con_busy = TRUE;
		SBUF0 = data_to_send;
    }
    else 
	{
		TX_To_Midi_Out_Con_Buff[tx_to_midi_out_con_end_index++ & (SERIAL_TX_BUFF_LENGTH-1)] = data_to_send;
		if( ((tx_to_midi_out_con_end_index ^ tx_to_midi_out_con_start_index) & (SERIAL_TX_BUFF_LENGTH-1)) == 0 )
			tx_to_midi_out_con_full = TRUE;
    }	

	EA = 1;	// Re-Enable 8051 interrupts
}

// ***************************************************************************
// Routine	: Check_MIDI_Buff_Send_To_Z8_Board()
// Input	: 
// Output	: None
// Process	: Send MIDI In buffer OR USB In Buffer to Z8 if 
//				- BS3X dedicated incoming sysex
//				- arpp or mackie or logic control is On and not a sysex has been received
// ***************************************************************************
void Check_MIDI_Buff_Send_To_Z8_Board(Midi_In_Struct *midi_struct, BYTE end_index_local)
{
	BYTE start_index_local;
//	BYTE end_index_local;
	BYTE midi_mess_size;
	BYTE midi_mess_first_byte;
	BYTE sysex_index;
	BYTE tmp;
	bit /*v1.1 must_send_midi_to_z8,*/ must_send_sysex_to_z8, must_send_to_z8;

	start_index_local = midi_struct->start_index;
//	EA = 0;
//	end_index_local   = midi_struct->end_index;
//	EA = 1;

	//v1.1 if(start_index_local == end_index_local) return;
	while(start_index_local != end_index_local)
	{
		//v1.1 must_send_midi_to_z8 	= FALSE;
		must_send_sysex_to_z8 	= FALSE;
		must_send_to_z8			= FALSE;
	
		// v1.1 copy first midi byte to a local memory
		midi_mess_first_byte	= midi_struct->buffer[start_index_local];
		EA=0;
		midi_mess_size = Get_MIDI_Msg_Size(midi_mess_first_byte) + 1;
		EA=1;	
		
		// a sysex is into the buffer ?
		if(midi_mess_size == 0)		
		{
			sysex_index = start_index_local;
			while( (midi_struct->buffer[sysex_index] != MIDI_EOX) && (sysex_index != end_index_local))
			{
				midi_mess_size++;
				sysex_index = (sysex_index + 1)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1);
			}
			midi_mess_size++;
		}

		// Check if BSP SYSEX 
	/*	if(midi_struct->buffer[start_index_local]   									== MIDI_SOX)
		if(midi_struct->buffer[(start_index_local+1)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] 	== WID_SYX_ID0)	
		if(midi_struct->buffer[(start_index_local+2)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] 	== WID_SYX_ID1)	
		if(midi_struct->buffer[(start_index_local+3)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] 	== WID_SYX_ID2)	
		if(midi_struct->buffer[(start_index_local+4)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] 	== WID_SYX_ID3)	
		if(midi_struct->buffer[(start_index_local+5)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] 	== WID_SYX_ID4)
	*/
		// Check it is a wid dedicated sysex (check if all wid sysex header is present)
		sysex_index = 0;	
		while((midi_struct->buffer[(start_index_local + sysex_index)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] 
			  	== wid_sysex_header[sysex_index]) && (sysex_index < WID_SYX_HEADER_SIZE))
				sysex_index++;
	
		// a whole wid sysex header was in the buffer ?
		if(sysex_index == WID_SYX_HEADER_SIZE)
		{
			// also check this sysex is an incoming sysex for the 3X, as opposed to a outgoing sysex
			if(!(midi_struct->buffer[(start_index_local+WID_SYX_HEADER_SIZE)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] & sysex_direction_sysxid0))
			{
				// check if eox present
				//v1.1 if(midi_struct->buffer[(end_index_local-1)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)]	== MIDI_EOX)
				if(midi_struct->buffer[(start_index_local+midi_mess_size-1)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)]	== MIDI_EOX)
				{
						/////////////////////////// It is a Wave Idea BS3X INCOMING SYSEX ////////////////////////////////////
						
						// are we bootloading ? NO : Check if must start bootloader OR forward this sysex to Z8 board
						if(bootloader_state == BOOTLOADER_OFF)
						{
							// Check if must start bootloader while we are in normal mode
							if( (midi_struct->buffer[(start_index_local+WID_SYX_HEADER_SIZE)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] == (/*v1.1 FRAME_IS_FIRMWARE*/FRAME_IS_FIRMWARE_OR_PURE_MIDI | acknoledge_sysxid0 | checksum_check_sysxid0))
							  // data lenght = 127 is used to start bootloader
							  &&(midi_struct->buffer[(start_index_local+WID_SYX_HEADER_SIZE+1)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1)] == DATA_LENGHT_START_BOOTLOADER) )
							  	// flag the bootloader to start it and exit
								must_start_bootloader_from_ez0_bit = TRUE;
							// send to z8 this incoming sysex
							else
							{
								must_send_sysex_to_z8 = TRUE; // it is a non bootloader wid sysex
								must_send_to_z8 = TRUE;
							}
						} // end normal mode
						else
					    /////////////////////////// HERE WE ARE IN BOOTLOADER MODE /////////////////////////
						//if(bootloader_state == WAIT_FW_DATA)
						{
						 	//DEBUGJDS turn on USB led
							USB_LED_PORT_OUT |= USB_LED_PIN;
			
				   			// determine the bootloader source if not yet known
							if(fw_bl_src == FW_BL_SRC_UNKNOWN)
							{
								if((Midi_In_Struct *) midi_struct == (Midi_In_Struct *) &MIDI_In_From_Usb)
									fw_bl_src = FW_BL_SRC_USB_IN;
								else
									fw_bl_src = FW_BL_SRC_MIDI_IN;
							}
			
							// BOOTLOADER MODE SYSEX RECEIVED
							DecodeFirmwareSysex(midi_struct);
			
							USB_LED_PORT_OUT &= ~USB_LED_PIN;
			
							// the whole sysex has now been treated
							// no circular buffer while upgrading firmware
							midi_struct->start_index 	= 0;
							midi_struct->end_index	 	= 0;
							midi_struct->current_index	= 0;
							
							return;
						} 	// end bootloader mode;
				} // end eox present
			} // end incoming wid sysex
		} // end it was a wid sysex (header fully recognized)
			
		
		// send all midi to z8 if arpeggio on and src corresponding enabled
		// if sysex_index is different than 0 then another NON-wid sysex has been received, do not forward it to z8
		// if it was a wid sysex ignore the following
		if(!must_send_sysex_to_z8)	// it was not a wid sysex
		{
// V1.5			if(!sysex_index)		// it was not a sysex at all (F0 was not the first byte)
// V1.5			{
				// V1.5 if(arpp_state_z_ez1_bit)	// the arpp is ON
				if(ez_fw_midi_to_z8_z_ez1_bit)	// Z8 wants to receive all MIDI events (arpp is on or mackie control enabled)
				{
					must_send_to_z8 = TRUE; // V1.5

/*					// v1.1 send to z8 arpp only if note on or note off
					if( (midi_mess_first_byte <= (MIDI_NOTE_ON + 0x0F)) &&
						(midi_mess_first_byte >= MIDI_NOTE_OFF) )
					{
						// are we treating the selected arpeggio source input
						if((Midi_In_Struct *) midi_struct == (Midi_In_Struct *) &MIDI_In_From_Usb)
						{
							// we are treating the usb in buffer here
							if(arpp_midi_src_z_ez1_bit)
							{
								//v1.1 must_send_midi_to_z8 = TRUE;
								must_send_to_z8 = TRUE;
							}
						}
						else
						// v1.1 the following line should not be necessary, anyway ...
						//if((Midi_In_Struct *) midi_struct == (Midi_In_Struct *) &MIDI_In_From_Con)
						{
							// we are treating the midi in buffer here
							if(!arpp_midi_src_z_ez1_bit)
							{
								//v1.1 must_send_midi_to_z8 = TRUE;
								must_send_to_z8 = TRUE;
							}
						}
					} // end it was a note on/off
*/
				} // end Z8 wants to received all MIDI events
// V1.5		} // end it was not a sysex at all
//			else
				// it was a NON wid sysex : do not forward to z8 and return 
//				return;
		} // end it was not a wid sysex
	
		// do data have to be forwarded to z8 board ?
		if(must_send_to_z8)
		{
			// First send header bytes to Z8 board, SOF
			Send_To_Serial_Z8(SOF_TO_EZ);
			
			// a wid sysex has to be forwarded to z8; skip sysex header 
			if(must_send_sysex_to_z8)
			{
				// then next byte to send is the usefull bytes of received sysex skip (f0 00 20 4f 00 01 )
				start_index_local = (start_index_local + WID_SYX_HEADER_SIZE)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1);
				// skip eox f7
				// v1.1 end_index_local   =	(end_index_local-1)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1);
				// a sysex is present, loop will end when byte = EOX, set the loop val to its maximum
				midi_mess_size = midi_mess_size - WID_SYX_HEADER_SIZE - 1;

			} // end must forward a sysex
			else
			{
				// must send pure midi to Z8 board (id = 2)
				// V1.5 also indicates to Z8 the MIDI In source (USB or MIDI in connector)
				tmp = FRAME_IS_FIRMWARE_OR_PURE_MIDI;
				if((Midi_In_Struct *) midi_struct == (Midi_In_Struct *) &MIDI_In_From_Usb)
					tmp |= pure_midi_from_usb_sysxid0;
				
				Send_To_Serial_Z8(tmp);	
			}

			// Then send all bytes, skip eox
			//v1.1 while(start_index_local != end_index_local)
			// use sysex_index since not anymore used in this loop
			sysex_index = (start_index_local + midi_mess_size)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1);
			while( start_index_local != sysex_index)
			{
				// Forward midi bytes to Z8 board
				Send_To_Serial_Z8(midi_struct->buffer[start_index_local]);
				// inc index
				start_index_local =  (start_index_local + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
			}
					
			// a wid sysex has to be forwarded to z8; skip sysex header 
			if(must_send_sysex_to_z8)
				// skip eox
				start_index_local =  (start_index_local + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);

			// Finish by sending end of frame EOF bytes to Z8 board
			Send_To_Serial_Z8(EOF_TO_EZ);
		}
		else
			start_index_local =  (start_index_local + midi_mess_size) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);			
	}

	// This buffer has now been processed, update circular pointer
	// this is done into the caller
	// midi_struct->start_index = start_index_local;
	
}

// ***************************************************************************
// Routine	: Send_To_Serial_Z8()
// Input	: data_to_send
// Output	: None
// Process	: Send a single byte to TX2 (bsp z8 board)
// ***************************************************************************
void Send_To_Serial_Z8(BYTE data_to_send)
{
	while( tx_to_serial_z8_full );

	EA = 0;	// Disable 8051 Interrupts

	if( !tx_to_serial_z8_busy )
	{
		tx_to_serial_z8_busy = TRUE;
		SBUF1 = data_to_send;
    }
    else 
	{
		TX_To_Serial_Z8_Buff[tx_to_serial_z8_end_index++ & (SERIAL_TX_BUFF_LENGTH-1)] = data_to_send;
		if( ((tx_to_serial_z8_end_index ^ tx_to_serial_z8_start_index) & (SERIAL_TX_BUFF_LENGTH-1)) == 0 )
			tx_to_serial_z8_full = TRUE;
    }	

	EA = 1;	// Re-Enable 8051 interrupts

}

// ***************************************************************************
// Routine	: Send_Single_Framed_byte_To_Z8()
// Input	: 
// Output	: None
// Process	: Send a single byte to Z8 board, byte is framed
//			  v1.1 : this function always sends a FRAME_IS_COMMON_PARAM byte
// ***************************************************************************
void Send_Single_Framed_byte_To_Z8(/*v1.1 BYTE SYSEX_ID0,*/ BYTE SYSEX_ID2, BYTE byte_to_send)
{
	// First send header bytes to Z8 board, SOF
	Send_To_Serial_Z8(SOF_TO_EZ);
	
	// Now send frame type
//v1.1	Send_To_Serial_Z8(SYSEX_ID0);
	 Send_To_Serial_Z8(FRAME_IS_COMMON_PARAM);
	
	// Now send lenght (unnecessary in fact) (always 1 for single byte)
	Send_To_Serial_Z8(0x01);

	// Now send address
	Send_To_Serial_Z8(SYSEX_ID2);

	// Now send dummy : always 0 for single byte
	Send_To_Serial_Z8(0);

	// Now send data
	Send_To_Serial_Z8(byte_to_send);

	// Now send end of frame
	Send_To_Serial_Z8(EOF_TO_EZ);

}

