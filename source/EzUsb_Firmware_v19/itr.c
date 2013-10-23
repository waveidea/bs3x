//#pragma NOIV	// Do not generate interrupt vectors 

#include <ezusb.h>
#include <ezregs.h>
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "serial.h"
#include "itr.h"
#include "usb.h"
#include "bootloader.h"
#include "macros.inc"
#include "main.h" // V1.9

// GLOBAL VARS
static unsigned char MIDI_In_From_Con_Current_Status;
static unsigned char MIDI_In_From_Con_Msg_Size;

unsigned char timeout_cpt;

// From SERIAL.C
extern idata unsigned char TX_To_Midi_Out_Con_Buff[SERIAL_TX_BUFF_LENGTH];
extern xdata unsigned char TX_To_Serial_Z8_Buff[SERIAL_TX_BUFF_LENGTH];			// V1.8 idata
extern unsigned char tx_to_midi_out_con_start_index;
extern unsigned char tx_to_midi_out_con_end_index;
extern bit tx_to_midi_out_con_busy;
extern bit tx_to_midi_out_con_full;
extern unsigned char tx_to_serial_z8_start_index;
extern unsigned char tx_to_serial_z8_end_index;
extern bit tx_to_serial_z8_busy;
extern bit tx_to_serial_z8_full;

// From MAIN.C
extern unsigned char bdata CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_NB_FROM_Z8_TO_EZ];
extern volatile BYTE sync24_clear_clock_cpt;
extern volatile BYTE sync24_clear_cont_cpt;
extern xdata Midi_In_Struct  	MIDI_In_From_Con, MIDI_In_From_Usb, Serial_In_From_Z8;
extern idata Midi_In_Realtime_Struct  MIDI_In_Realtime_From_Con, MIDI_In_Realtime_From_Usb, Realtime_From_Z8;
extern bit z8_board_fully_updated_from_ez0_bit;
extern bit realtime_source_int_z_ez1_bit;
extern bit realtime_source_ext_z_ez1_bit;
extern bit autotest_mode_z_ez1_bit;
extern bit sync24_state_z_ez0_bit; // V1.9
extern bit sync24_source_int_z_z_ez1_bit; // V1.9

// FROM BOOTLOADER.C
extern volatile BYTE bootloader_state;
extern BYTE fw_bl_src; 	// shared reg

#pragma NOAREGS         // do not use absolute register symbols (ARx)
						// for functions called from interrupt routines.

//#pragma REGISTERBANK(2); 

/* ******************************************************************** */
/* Routine: Check_Realtime_MIDI_Data_Received 							*/
/* Process: a realtime MIDI event has been received from MIDI In Con or	*/
/*			USB In, check if this data must be retransimted to Z8 		*/
/*			Board (MIDI clock if LFO is ON and EXT sync), or must be 	*/
/*			merged with no latency										*/
/*			Also treats incoming realtime events from Z8 board			*/
/* Inputs:	data_received												*/
/* Outputs:	None														*/
/* ******************************************************************** */
//static 
void Check_Realtime_MIDI_Data_Received( unsigned char data_received, Midi_In_Realtime_Struct *midi_struct) reentrant
{
	// ignore realtime events while bootloading 
	if(bootloader_state) return;

	// treat sync24 immediately
	if(sync24_state_z_ez0_bit) 
		generateSync24(data_received, midi_struct);

	// Store realtime data received to its buffer and update pointer index
	midi_struct->realtime[midi_struct->realtime_end_index] = data_received;
	midi_struct->realtime_end_index = (midi_struct->realtime_end_index + 1) & (MIDI_IN_REALTIME_BUFF_LENGHT-1);
	if(midi_struct->realtime_start_index == midi_struct->realtime_end_index)
	{
		// overflow on realtime buffer
		midi_struct->input_status |= realtime_overflow;
	}
}

// ***************************************************************************
// Routine	: Check_Midi_In_From_Con_Data_Received()
// Input	:  
// Output	: 
// Process	: 
// ***************************************************************************
//static
void Check_Midi_In_From_Con_Data_Received(unsigned char data_received)
{
	// Check if incoming data is a REALTIME event
	if(data_received > 0xF7)
	{
		// check incoming realtime data received if realtime source is MIDI IN connector
		// if(!(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_1_7BITS_TO_EZ_INDEX] & realtime_source_z_ez1))
		//if(!realtime_source_int_z_ez1_bit)
		if(!realtime_source_ext_z_ez1_bit)
		{
			Check_Realtime_MIDI_Data_Received(data_received, &MIDI_In_Realtime_From_Con);
		}
	}
	else
	{
		// Non realtime received, add it to MIDI_In_From_Con_Buffer
		// Detect if status
		if (data_received & MIDI_STATUS_MASK)
		{
			MIDI_In_From_Con_Msg_Size = Get_MIDI_Msg_Size(data_received);
			
			if (data_received == MIDI_SOX)
		 	 	MIDI_In_From_Con.merger_state =  IN_SYSEX;
      		else
			if (data_received == MIDI_EOX)
		    	MIDI_In_From_Con.merger_state =  NORMAL; // Back to NORMAL after a SYSEX or FIFO_FULL
      		else
			if (data_received <  0xF0)
			{
  				MIDI_In_From_Con.merger_state   = NORMAL;
				MIDI_In_From_Con_Current_Status = data_received ;	// Saves the current status
			}
		}
		// Non-Status byte received
		else
		{
			// wait to receive a MIDI status (only for init.)
			// Exit and ignore received data until a status is received after boot
			if (MIDI_In_From_Con.merger_state == WAIT_FOR_CORRECT_DATA)	return;

			if (MIDI_In_From_Con.merger_state == NORMAL)
			{
				if ( MIDI_In_From_Con.current_index == MIDI_In_From_Con.end_index)
				{
					// Get message size with the inserted current status
					MIDI_In_From_Con_Msg_Size  = Get_MIDI_Msg_Size(MIDI_In_From_Con_Current_Status); // -1; ???

					// Restores running status
	           		// Itr_function_param already contains Itr_Current_Status
    	       		MIDI_In_Add_Buffer(MIDI_In_From_Con_Current_Status, &MIDI_In_From_Con);
				}
				MIDI_In_From_Con_Msg_Size--;
			}
		} // end non status byte received

   		// Write byte received from MIDI In to MIDI IN Buffer
  	   	MIDI_In_Add_Buffer(data_received, &MIDI_In_From_Con);

 		if ( MIDI_In_From_Con_Msg_Size == 0 )
   		{
	 		if (MIDI_In_From_Con.merger_state != FIFO_FULL)
	    		// Set the MIDI_in_end_index pointer to enable transmission
	    		MIDI_In_From_Con.end_index = MIDI_In_From_Con.current_index;
   			else
	    		// Return to normal state after FIFO_FULL at the end of the message
	    		MIDI_In_From_Con.merger_state	  = NORMAL;
 		}
	} // end non realtime byte received
}

// ***************************************************************************
// Routine	: Check_Serial_From_Z8_Data_Received()
// Input	:  
// Output	: 
// Process	: Treat incoming serial from Z8 board
// ***************************************************************************
//static 
void Check_Serial_From_Z8_Data_Received(unsigned char data_received)
{
	// V1.4 if not bootloading then treat a normal z8 incoming data
	if(!bootloader_state) 
	{
		// if we received a clock (internal sync) then must dispatch it to midi/usb out 1
		if((data_received >= MIDI_CLOCK)&&(data_received <= MIDI_STOP))
		{
			Check_Realtime_MIDI_Data_Received(data_received, &Realtime_From_Z8);
			//v1.1
			return;
		}
	
		// wait for the first valid frame
		if(Serial_In_From_Z8.merger_state == WAIT_SOF)
		{
			// Still waiting for first valid frame
			if(data_received != SOF_TO_EZ) return;
			
			// Start of frame received
			Serial_In_From_Z8.merger_state = WAIT_EOF;
		}
		
		// Do we have a full frame now ?
		if (data_received == EOF_TO_EZ)
		{
			Serial_In_From_Z8.end_index = (Serial_In_From_Z8.current_index + 1) & (SERIAL_IN_FROM_Z8_BUFF_LENGHT-1);		
			Serial_In_From_Z8.merger_state = WAIT_SOF;
		}
	}
	
	// Add received data to Z8 serial buffer
	MIDI_In_Add_Buffer(data_received, &Serial_In_From_Z8);
	
}

// ***************************************************************************
// Routine	: Get_MIDI_Msg_Size()
// Input	: current MIDI status 
// Output	: number of expected bytes following the status (excluding SYSEX status)
// Process	: 
// ***************************************************************************
//static 
unsigned char Get_MIDI_Msg_Size(unsigned char status) reentrant
{
   	if ((status >= 0x80) && (status <= 0xEF))
	{
		if ((status >= 0xC0) && (status <= 0xDF))
			return(1);
		else
			return(2);
	}
    else
	if ((status==MIDI_TIME_CODE) || (status==MIDI_SONG_SELECTED))
		return(1);
    else
	if (status==MIDI_SONG_POSITION_POINTER)
      	return(2);
   	else
	if (status==MIDI_SOX)
		return(0xFF);
   else
		return(0); // eox
}

// ***************************************************************************
// Routine	: generateSync24()
// Input	: 
// Output	: None
// Process	: 
// ***************************************************************************
// V1.9 sync24 treated from itr (no more from main loop)
void generateSync24(unsigned char data_in, Midi_In_Realtime_Struct *midi_struct) reentrant
{
	// Realtime message comming from Internal sync ?
	if(midi_struct == &Realtime_From_Z8)
	{
		// internal sync ?
		if(!sync24_source_int_z_z_ez1_bit)	
			Update_Sync24(data_in);		
	}
	else
	// Realtime messages comming from external 
	{
		// external sync ?
		if(sync24_source_int_z_z_ez1_bit)	
			Update_Sync24(data_in);		
	}
}

// ***************************************************************************
// Routine	: MIDI_In_Add_Buffer()
// Input	: char to add to MIDI in buffer
// Output	: None
// Process	: Add data to MIDI In buffer and update circular pointer
// ***************************************************************************
//static 
void MIDI_In_Add_Buffer(unsigned char data_in, Midi_In_Struct *midi_struct) reentrant
{
	static unsigned char current_index_local_p1_one;

	//current_index_local = midi_struct->current_index;
	current_index_local_p1_one = (midi_struct->current_index+1)&(MIDI_IN_FROM_CON_BUFF_LENGHT-1);

	// Detects FIFO_IN FULL
	if (midi_struct->start_index == current_index_local_p1_one)
	{
		midi_struct->input_status |=  input_overflow;
       	midi_struct->merger_state = FIFO_FULL;
       	// Skip current MIDI Msg: restore in_index with previous value
	    midi_struct->current_index = midi_struct->end_index;
	}
	else
	{
		midi_struct->buffer[midi_struct->current_index] = data_in;
		midi_struct->current_index = current_index_local_p1_one;
	}
}

#pragma AREGS

/* ******************************************************************** */
/* Routine: serial_port_0 ISR											*/
/* Process: Send/Receive from RX0/TX0 (connectors)						*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void serial_port_0_isr (void) interrupt COM0_VECT using 2
{
	static unsigned char data_received;

	/* ********* Received data interrupt ******** */
	if (RI)
	{
		// Now treat itr from serial port 0
		data_received = SBUF0;		
		// clear interupt flag
		RI = 0;		

		// treat incoming data only if bootloader is off or src bootloader is midi in 
		// since the bl uses usb_in buffer for communicating with the host via midi
		if(z8_board_fully_updated_from_ez0_bit)
		{
			if(fw_bl_src != FW_BL_SRC_USB_IN)
				// Treat incoming data received
				Check_Midi_In_From_Con_Data_Received(data_received);
		}
	}

	/* ********* Transmited data interrupt ******** */
	if (TI)
	{
	  	TI = 0;
		if (tx_to_midi_out_con_start_index != tx_to_midi_out_con_end_index)
		{
			SBUF0 = TX_To_Midi_Out_Con_Buff[tx_to_midi_out_con_start_index++ & (SERIAL_TX_BUFF_LENGTH-1)];
			tx_to_midi_out_con_full = FALSE;
		}
		else
			tx_to_midi_out_con_busy = FALSE;
	}
}

/* ******************************************************************** */
/* Routine: serial_port_1 ISR											*/
/* Process: parse incoming from RX2/TX2 pin (sent from z8 board)		*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* Notes: This function uses the global var RX2_index					*/
/* ******************************************************************** */
void serial_port_1_isr (void) interrupt COM1_VECT using 2
{
	static unsigned char data_received;

	/* ********* Received data interrupt ******** */
	if (RI1)
	{
		// Now treat itr from serial port 0
		data_received = SBUF1;		
		// clear interupt flag
		RI1 = 0;		

		// waiting for the bootloaded CRC from Z8 ?
		//if(bootloader_state == VERIFY_Z8_CRC)
			
		//else
		// ignore serial in from z8 when bootloading
		// V1.4 if(!bootloader_state)
			// Treat incoming data received
			Check_Serial_From_Z8_Data_Received(data_received);
	}

	/* ********* Transmited data interrupt ******** */
	if (TI1)
	{
	  	TI1 = 0;
			
		if (tx_to_serial_z8_start_index != tx_to_serial_z8_end_index)
		{
			SBUF1 = TX_To_Serial_Z8_Buff[tx_to_serial_z8_start_index++ & (SERIAL_TX_BUFF_LENGTH-1)];
			tx_to_serial_z8_full = FALSE;
		}
		else
			tx_to_serial_z8_busy = FALSE;
	}
}

/* ******************************************************************** */
/* Routine: timer_0_isr ISR												*/
/* Process: timer 0 ISR (2 ms permanent timer) 							*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* Notes: 																*/
/* ******************************************************************** */
void timer_0_isr (void) interrupt TMR0_VECT using 2 
{
	TR0 = 0; // stop timer 0

	// V1.5 workaround for Rev D silicon chip : interupt timer bit must be cleared
//	TF0 = 0;

	// execute only if not  bootloading since bootloader uses sync24_clear_clock_cpt
	// V1.5 also clear sync24 signals only if not in test mode
	if( (!bootloader_state) && (!autotest_mode_z_ez1_bit))
	{
		// Update sync24 states if required
		if(sync24_clear_clock_cpt)
			if(!(--sync24_clear_clock_cpt))
				SYNC24_CLOCK_PORT_OUT  &=  ~SYNC24_CLOCK_PIN; // Set CLOCK = 0
	
		if(sync24_clear_cont_cpt)
			if(!(--sync24_clear_cont_cpt))
				SYNC24_CONT_PORT_OUT  &=  ~SYNC24_CONT_PIN; // Set CONT = 0
	}
	
	// decrease timeout counter if asserted
	// used for synch between ez and z8 board
	// then used for USB led persistency
	// if autotest mode : do not turn off the USB led
	// V1.4 if(!(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_1_7BITS_TO_EZ_INDEX] & autotest_mode_z_ez1_bit))
	if(!autotest_mode_z_ez1_bit)
	{
		if(timeout_cpt) 
		{	
			timeout_cpt--;	
			if(!timeout_cpt)
				USB_LED_PORT_OUT &= ~USB_LED_PIN;
		}
	}	

	// reload and restart timer
    TH0  = (TIMER0_RELOAD >> 8);  		// Setup reload value
    TL0  = (TIMER0_RELOAD & 0xFF);		// Setup overflow value
	TR0 = 1; // start Timer 0
}


