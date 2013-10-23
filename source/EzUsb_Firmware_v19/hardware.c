//#pragma NOIV	// Do not generate interrupt vectors
 
// **************************** HARDWARE.C *********************************
// All code relating to ezusb hardware
// *************************************************************************

#include <ezusb.h>
#include <ezregs.h>
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "hardware.h"
#include "serial.h"
#include "bootloader.h"
#include "main.h"
#include "usb.h"

// GLOBAL VARS
extern unsigned char timeout_cpt;

// From MAIN.C
extern xdata Midi_In_Struct  	MIDI_In_From_Con, MIDI_In_From_Usb, Serial_In_From_Z8;
extern idata Midi_In_Realtime_Struct  MIDI_In_Realtime_From_Con, MIDI_In_Realtime_From_Usb, Realtime_From_Z8;
extern unsigned char bdata 		CTRL_REGS_7BITS_TO_Z8[CTRL_REG_MAX_FROM_EZ_TO_Z8];
extern unsigned char bdata 		CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_NB_FROM_Z8_TO_EZ];
extern bit z8_board_fully_updated_from_ez0_bit;

// FROM SERIAL.C
extern unsigned char tx_to_midi_out_con_start_index;
extern unsigned char tx_to_midi_out_con_end_index;
extern bit tx_to_midi_out_con_busy;
extern bit tx_to_midi_out_con_full;
extern unsigned char tx_to_serial_z8_start_index;
extern unsigned char tx_to_serial_z8_end_index;
extern bit tx_to_serial_z8_busy;
extern bit tx_to_serial_z8_full;

//extern bit usb_bootloader_requested;

// FROM BOOTLOADER.C
extern volatile BYTE bootloader_state;
extern BYTE fw_bl_src;

/* ******************************************************************** */
/* Routine: Init_Variables												*/
/* Process: Init Variables default states								*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void Init_Variables(void)
{
	bootloader_state 			 = BOOTLOADER_OFF; 
//	usb_bootloader_requested	 = FALSE;

	tx_to_midi_out_con_full 	 = 0;
	tx_to_serial_z8_full    	 = 0;
	tx_to_midi_out_con_busy 	 = 0;
	tx_to_serial_z8_busy 		 = 0;

	tx_to_midi_out_con_start_index = 0;
	tx_to_serial_z8_start_index    = 0;
	tx_to_midi_out_con_end_index   = 0;
	tx_to_serial_z8_end_index 	   = 0;

	MIDI_In_From_Con.merger_state 					= WAIT_FOR_CORRECT_DATA;
	MIDI_In_From_Con.current_index					= 0;
	MIDI_In_From_Con.start_index					= 0;
	MIDI_In_From_Con.end_index						= 0;	
	MIDI_In_From_Con.input_status					= 0;
	MIDI_In_Realtime_From_Con.realtime_start_index	= 0;
	MIDI_In_Realtime_From_Con.realtime_end_index	= 0;
	MIDI_In_Realtime_From_Con.input_status			= 0;

	MIDI_In_From_Usb.merger_state 					= NORMAL;
	MIDI_In_From_Usb.current_index					= 0;
	MIDI_In_From_Usb.start_index					= 0;
	MIDI_In_From_Usb.end_index						= 0;
	MIDI_In_From_Usb.input_status					= 0;
	MIDI_In_Realtime_From_Usb.realtime_start_index	= 0;
	MIDI_In_Realtime_From_Usb.realtime_end_index	= 0;
	MIDI_In_Realtime_From_Usb.input_status			= 0;

	Serial_In_From_Z8.merger_state 					= WAIT_SOF;
	Serial_In_From_Z8.current_index					= 0;
	Serial_In_From_Z8.start_index					= 0;
	Serial_In_From_Z8.end_index						= 0;
	Serial_In_From_Z8.input_status					= 0;
	Realtime_From_Z8.realtime_start_index			= 0;
	Realtime_From_Z8.realtime_end_index				= 0;
	Realtime_From_Z8.input_status					= 0;
	
	// to ensure proper Z8 to EZ synchro, will have to wait for last reg from Z8
	CTRL_REGS_7BITS_FROM_Z8[LAST_CTRL_REG_7BITS_TO_EZ_INDEX] = 0;

	// fw bootload source is midi or usb, unknown yet
	fw_bl_src = FW_BL_SRC_UNKNOWN;

	timeout_cpt = 0;

}

/* ******************************************************************** */
/* Routine: Init_IOs													*/
/* Process: Init IO default states										*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void Init_IOs(void)
{

	// Inititalize IOs shared functions
//	PORTACFG = 0;		// Port A is a standard GPIO
//	PORTBCFG |= 0x0C;	// Port B uses RX2 & TX2
	PORTBCFG  = 0;		// turn off tx1 and rx1 (serial to/from z8)
	PORTCCFG |= 0x03;	// Port C uses RX0 & TX0 ( & INT0 & INT1 ) 

	// Initialize directions for each ports
	OEA = 0x20; // Port A4 input at startup, PA5 is output (INPUT=0)
	OEB = 0xC9; // everything's output exept footswitch, edit, ok (INPUT=0)
	OEC = 0xE2; // everything's input exept TX0, SYNC24_CONT, SYC24_RUN, OCD_EN

	// Initialize output Ports default values
	OUTA = 0;
//	OUTB = 0; // be sure the z8 reset is cleared, tx1 (to dbg pin) must be 1
	OUTB = 0x08; // be sure the z8 reset is cleared, tx1 (to dbg pin) must be 1
	OUTC = 0; // be sure ocd_en is cleared
}


/* ******************************************************************** */
/* Routine: SyncToZ8Board												*/
/* Process: 															*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void SyncToZ8Board(void)
{
	// Release Z8 board reset line
	Z8_RESET_PORT_OUT |=  Z8_RESET_PIN;

	//DEBUGJDS turn on USB led
	USB_LED_PORT_OUT |= USB_LED_PIN;

#ifndef SIMUL
	timeout_cpt = 255;
	// Loop until all config registers from Z8 have been received OR timout (510ms)
	do 
	{ 
		Check_Serial_In_From_Z8_Buffer();
		Check_Usb();
	}
	while( (CTRL_REGS_7BITS_FROM_Z8[LAST_CTRL_REG_7BITS_TO_EZ_INDEX] != EZ_FULLY_UPDATED) &&
		   //(!usb_bootloader_requested));
		   (timeout_cpt));		
#endif

#ifdef SIMUL
	CTRL_REGS_7BITS_FROM_Z8[0] = 0x2D;
	CTRL_REGS_7BITS_FROM_Z8[1] = 0x10;
	CTRL_REGS_7BITS_FROM_Z8[LAST_CTRL_REG_7BITS_TO_EZ_INDEX] = EZ_FULLY_UPDATED;
#endif

	// V1.5 : be sure the EZ chip is configured to accept bootloader even if Z8 chip is not answering since not programmed
	if(!timeout_cpt)
	{
		CTRL_REGS_7BITS_FROM_Z8[0] = 0x0F;
		CTRL_REGS_7BITS_FROM_Z8[1] = 0x00;
		CTRL_REGS_7BITS_FROM_Z8[LAST_CTRL_REG_7BITS_TO_EZ_INDEX] = EZ_FULLY_UPDATED;
	}

	// Send registers dedicated to Z8 (CTRL_REGS_7BITS_TO_Z8)
	Send_Single_Framed_byte_To_Z8(/*v1.1 FRAME_IS_COMMON_PARAM,*/ CTRL_REG_0_7B_FROM_EZ_INDEX, (CTRL_REGS_7BITS_TO_Z8[0] | (1<<z8_board_fully_updated_from_ez0_bit_pos)));

	USB_LED_PORT_OUT &= ~USB_LED_PIN;

 	// synchronize z8 board 
	z8_board_fully_updated_from_ez0_bit = 1;
}

/* ******************************************************************** */
/* Routine: Check_InitialSwitchesState												*/
/* Process: 															*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
/*
void Check_InitialSwitchesState(void)
{
	if(EDIT_PORT_IN & EDIT_PIN)	
		if(OK_PORT_IN & OK_PIN)
			if(CANCEL_PORT_IN & CANCEL_PIN)
				Check_Bootloader();	
}
*/