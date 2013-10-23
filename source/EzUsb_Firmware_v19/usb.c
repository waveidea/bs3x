//#pragma NOIV	// Do not generate interrupt vectors
 
#include <ezusb.h>
#include <ezregs.h>
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "usb.h"
#include "usb_defs.h"
#include "serial.h"
#include "itr.h"
#include "bootloader.h"
#include "hardware.h"
#include "fw.h"
#include "main.h" // V1.9
#include "macros.inc"


// Number of bytes which should be sent
BYTE code usb_midi_bytes_to_send[] = {
	0, // 0: invalid/reserved event
	0, // 1: invalid/reserved event
	2, // 2: two-byte system common messages like MTC, Song Select, etc.
	3, // 3: three-byte system common messages like SPP, etc.
	3, // 4: SysEx starts or continues
	1, // 5: Single-byte system common message or sysex sends with following single byte
	2, // 6: SysEx sends with following two bytes
	3, // 7: SysEx sends with following three bytes
	3, // 8: Note Off
	3, // 9: Note On
	3, // a: Poly-Key Press
	3, // b: Control Change
	2, // c: Program Change
	2, // d: Channel Pressure
	3, // e: PitchBend Change
	1, // f: single byte 
};

// FROM FW.C
extern volatile BOOL	GotSUD;
extern volatile BOOL	Sleep;						// Sleep mode enable flag
extern BOOL				Rwuen;

// FROM MAIN.C
extern xdata Midi_In_Struct  MIDI_In_From_Usb;
extern unsigned char bdata CTRL_REGS_7BITS_TO_Z8[CTRL_REG_MAX_FROM_EZ_TO_Z8];
extern bit usb_state_from_ez0_bit;
extern bit usb_enumrated_from_ez0_bit;
extern bit z8_board_fully_updated_from_ez0_bit;
extern bit realtime_source_int_z_ez1_bit;
extern bit realtime_source_ext_z_ez1_bit;
extern idata Midi_In_Realtime_Struct  MIDI_In_Realtime_From_Usb;
extern xdata Midi_In_Struct Serial_In_From_Z8;
//extern WORD first_pass; 	// V1.5


// FROM BOOTLOADER.C
extern volatile BYTE bootloader_state;
extern BYTE fw_bl_src; 	// shared reg

// FROM ITR.C
extern unsigned char timeout_cpt;

// ***************************************************************************
// Routine	: Check_If_Usb_Attached()
// Input	: 
// Output	: yes/no
// Process	: also update usb led
// ***************************************************************************
void Check_If_Usb_Attached(void)
{
	bit must_change_usb_state = FALSE;

	// V1.5
//	if(first_pass) return;

	// usb was NOT present
	if(!usb_state_from_ez0_bit)
	{
		// is the vusb present ?
		if(VUSB_PORT_IN & VUSB_PIN)
		{
			// reconnect the device
			USBCS &= ~bmDISCON;
			//V1.5 USBCS = 0x06; // reconnect
			// is the 3x enumerated ?
			if(usb_enumrated_from_ez0_bit)
				must_change_usb_state = TRUE;	
		}
	}
	else
	// the usb was present
	{
		// is the vusb NOT present ?
		if(!(VUSB_PORT_IN & VUSB_PIN))
		{
			// disconnect the device
			USBCS |= bmDISCON;
			//V1.5USBCS = 0x0A; // disconnect
			// the device is no more enumerated
			usb_enumrated_from_ez0_bit = FALSE;
			must_change_usb_state = TRUE;	
		}
	}

	if(must_change_usb_state)
	{
		// V1.5
//		USB_LED_PORT_OUT |= USB_LED_PIN;

		// send only if z8 ready to receive
		if(z8_board_fully_updated_from_ez0_bit)
		{
			usb_state_from_ez0_bit = !usb_state_from_ez0_bit;
			// send only if not in bootloader state
			if(!bootloader_state)
				Send_Single_Framed_byte_To_Z8(/*v1.1 FRAME_IS_COMMON_PARAM,*/ CTRL_REG_0_7B_FROM_EZ_INDEX, CTRL_REGS_7BITS_TO_Z8[0]);
		}

		//V1.5 
//		USB_LED_PORT_OUT &= ~USB_LED_PIN;
	}


}

/* ******************************************************************** */
/* Routine: Create_Send_Realtime_USB_Frame_From_MIDI_Buffer				*/
/* Process: Send a single realtime byte MIDI to USB bus					*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void Create_Send_Realtime_USB_Frame_From_MIDI_Buffer( unsigned char CN, unsigned char realtime_data)
{
	// V1.5
	// if(first_pass) return;

	if((EPIO[USB_IN_ID].cntrl & bmEPBUSY))
		return;

	// turn on USB led
//	USB_LED_PORT_OUT |= USB_LED_PIN;
//	timeout_cpt = USB_LED_PERSISTENCY;

	// Prepare header byte 0
	USB_IN_BUFFER[0] = ((CN << 4) & 0xF0) | (USB_SINGLE_BYTE & 0x0F);		
	USB_IN_BUFFER[1] = realtime_data;
	USB_IN_BUFFER[2] = 0;
	USB_IN_BUFFER[3] = 0;

	// Now send out to USB
	EPIO[USB_IN_ID].bytes = MIDI_USB_FRAME_LENGHT;

	// turn off USB led
//	USB_LED_PORT_OUT &= ~USB_LED_PIN;
}

#pragma NOAREGS; 

/* ******************************************************************** */
/* Routine: Check_Usb													*/
/* Process: 															*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void Check_Usb(void)
{
		// V1.5
		// if(first_pass) return;

		if(GotSUD)					// Wait for SUDAV
		{
			SetupCommand();	 		// Implement setup command
  			GotSUD = FALSE;		   	// Clear SUDAV flag
		}

		// Poll User Device
		// NOTE: Idle mode stops the processor clock.  There are only two
		// ways out of idle mode, the WAKEUP pin, and detection of the USB
		// resume state on the USB bus.  The timers will stop and the
		// processor will not wake up on any other interrupts.

		if (Sleep)
	    {
			if(TD_Suspend())
		    { 
		    	Sleep = FALSE;	   			// Clear the "go to sleep" flag.  Do it here to prevent any race condition between wakeup and the next sleep.
		    	do
		    	{
   			    	EZUSB_Susp();			// Place processor in idle mode.
		    	}
            	while(!Rwuen && EZUSB_EXTWAKEUP());
                // Must continue to go back into suspend if the host has disabled remote wakeup
                // *and* the wakeup was caused by the external wakeup pin.
                
    			// 8051 activity will resume here due to USB bus or Wakeup# pin activity.
    			EZUSB_Resume();	// If source is the Wakeup# pin, signal the host to Resume.		
    			TD_Resume();
		    }   
	    } // end if sleep
}

/* ******************************************************************** */
/* Routine: Create_Send_USB_Frame_From_MIDI_Buffer						*/
/* Process: Treat each MIDI messages from MIDI buffer, encapsulate MIDI */
/*			into USB frames 											*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void Create_Send_USB_Frame_From_MIDI_Buffer( BYTE CN, Midi_In_Struct *midi_struct, BYTE end_index_local)
{
	unsigned char /* buffer_length,*/ cin, start_index_local, end_sysex_index, mess_lenght, i;
	unsigned char usb_frame[MIDI_USB_FRAME_LENGHT];
	unsigned char status; // V1.7

	bit usb_out_in_sysex; //, must_skip_header_frame;
	BYTE usb_buffer_index;
	
	if(!usb_state_from_ez0_bit)
		return;

	// V1.5
	// if(first_pass) return;

	usb_out_in_sysex 	= FALSE;
	usb_buffer_index 	= 0;
	start_index_local	= midi_struct->start_index;

	// Treat all bytes to send
	while(start_index_local != end_index_local)
	{
		// turn on USB led
		USB_LED_PORT_OUT |= USB_LED_PIN;
		timeout_cpt = USB_LED_PERSISTENCY;
	
//		must_skip_header_frame = FALSE;

		// wait for the usb buffer to become ready
		//if(!(EPIO[USB_IN_ID].cntrl & bmEPBUSY))
		{
			// Prepare header byte 0
			usb_frame[0] = (CN << 4); // & 0xF0;		
	
			// ignore frame header FF UU LL if NOT in sysex or if bootloading and sending from serial_in_from_z8
			if( (!bootloader_state) && (!usb_out_in_sysex) && (midi_struct == &Serial_In_From_Z8) )
			{
			//	must_skip_header_frame = TRUE;
				// V1.4 in the case of a single midi string contains multiple MIDI events, the different midi
				// events are not seperated by FF UU LL : then do not skip these bytes since not present
				// the idea is to skip only if these bytes are found in the string
				if(midi_struct->buffer[start_index_local] == SOF_TO_EZ)
					start_index_local = (start_index_local + 3) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);	
			}
	
			// Initialize USB frame buffer to zero to avoid eventual zero padding
			usb_frame[1] = 0;
			usb_frame[2] = 0;
			usb_frame[3] = 0;
	
			// Check if already in sysex ?
			if(usb_out_in_sysex)
			{
				// We are in sysex, check remaining sysex bytes to send
				usb_out_in_sysex = FALSE;
				// sysex_length contains remaining bytes of the long sysex to send
				// 1 byte remaining in SYSEX : it is F7
				if(mess_lenght == 1)
				{
					cin 			 = USB_SYSEX_END_SINGLE & 0x0F;
					usb_frame[1] 	 = MIDI_EOX;
					start_index_local = (start_index_local + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
				}
				else
				// sysex end with 2 or 3 bytes or into unfinished sysex
				{
					usb_frame[1] = midi_struct->buffer[start_index_local];
					
					// 2 Bytes remaining in SYSEX : it is DD F7
					if(mess_lenght == 2)
					{
						cin 			 = USB_SYSEX_END_TWO & 0x0F;
						usb_frame[2] 	 = MIDI_EOX;
						start_index_local = (start_index_local + 2) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
					}
					else 
					// sysex end with 3 bytes or into unfinished sysex
					{					
						usb_frame[2] = midi_struct->buffer[(start_index_local+1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1)];
						usb_frame[3] = midi_struct->buffer[(start_index_local+2) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1)];
						start_index_local = (start_index_local + 3) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
		
						if(mess_lenght==3)
						{
							cin = USB_SYSEX_END_TREE & 0x0F;
						}
						else // More than 3 remaining bytes were in sysex
			 		    {
							cin = USB_SYSEX_START_IN & 0x0F;
							usb_out_in_sysex = TRUE;
						}
						
						mess_lenght -= 3; // 3 bytes of the long sysex have been sent
					}		
				}
			} // end we already were in sysex
			else
			// Is it a START of SYSEX ?
			if(midi_struct->buffer[start_index_local] == MIDI_SOX)
			{
				usb_out_in_sysex = TRUE;
				usb_frame[1] 	 = MIDI_SOX;
				start_index_local = (start_index_local + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
	
				// Search for end sysex index position F7
				end_sysex_index = start_index_local;
				mess_lenght    = 1;
				while( (midi_struct->buffer[end_sysex_index] != MIDI_EOX) && (end_sysex_index != end_index_local))
				{
						if(++mess_lenght < MIDI_USB_FRAME_LENGHT)
						{
							usb_frame[mess_lenght] = midi_struct->buffer[end_sysex_index];
							start_index_local = (start_index_local + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
						}
						end_sysex_index = (end_sysex_index + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
				}
				
				mess_lenght++;

				// Now end_sysex_index should contains position of EOX F7
				// Check if special case : 2 bytes sysex ?
				if(mess_lenght == 2)
				{
					cin 			 = USB_SYSEX_END_TWO & 0x0F;
					usb_frame[2] 	 = MIDI_EOX;
					usb_out_in_sysex = FALSE;
				}
				else
				if(mess_lenght==3)
				{
					cin 			 = USB_SYSEX_END_TREE & 0x0F;
					usb_frame[3] 	 = MIDI_EOX;
					usb_out_in_sysex = FALSE;
				}
				else
				{
					cin 			 = USB_SYSEX_START_IN & 0x0F;
					//usb_out_in_sysex = TRUE;
					mess_lenght 	-= 3; // 3 bytes of the long sysex have been sent
				}
			} // end start_index is START_OF_SYSEX
			else
			// Not a start of sysex, Not in sysex, not an end of sysex
			{
				status = midi_struct->buffer[start_index_local]; // V1.7

				// get midi mess lenght from status
				EA = 0;
				mess_lenght = Get_MIDI_Msg_Size(status) + 1;
				EA = 1;
				
				// V1.7 cin = midi_struct->buffer[start_index_local] >> 4;
				if( (status >= MIDI_NOTE_OFF) && (status < MIDI_SOX ))
					cin = status >> 4;
				else
					cin = mess_lenght;
				
				for(i=0; i < mess_lenght; i++)
				{
					usb_frame[i+1] = midi_struct->buffer[start_index_local];
					start_index_local = (start_index_local + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);
				}
			}
	
			// add cin to usb first byte
			usb_frame[0] |= cin; 
	
			// assign local usb buffer to real usb buffer
			for(i=0; i < MIDI_USB_FRAME_LENGHT; i++)
				 // USB_IN_BUFFER[i] = usb_frame[i];
				USB_IN_BUFFER[i + usb_buffer_index] = usb_frame[i];				
	
			usb_buffer_index += MIDI_USB_FRAME_LENGHT;

			Check_Usb();

			// buffer full ? send and reset index
			if(usb_buffer_index >= USB_EP_BUFFER_LENGHT)
			{
				if(!(EPIO[USB_IN_ID].cntrl & bmEPBUSY))
					// send buffer to usb out
	//				EPIO[USB_IN_ID].bytes = MIDI_USB_FRAME_LENGHT;
					EPIO[USB_IN_ID].bytes = usb_buffer_index;
				usb_buffer_index = 0;
	 		}

			// ignore frame header EOF (FE)
			if( (!bootloader_state) && (!usb_out_in_sysex) && (midi_struct == &Serial_In_From_Z8))
			//if(must_skip_header_frame)
				// V1.4 in the case of a single midi string contains multiple MIDI events, the different midi
				// events are not seperated by FF UU LL : then do not skip these bytes since not present
				// the idea is to skip only if these bytes are found in the string
				if(midi_struct->buffer[start_index_local] == EOF_TO_EZ)
					start_index_local = (start_index_local + 1) & (MIDI_IN_FROM_CON_BUFF_LENGHT - 1);	

		} // en endpoint was not busy

		// turn off USB led
		//USB_LED_PORT_OUT &= ~USB_LED_PIN; 

	    // V1.9 Check both MIDI in and USB in realtime buffers 
		//Check_Realtime();
 
 	} // end while

	// remaining bytes in the buffer ?
	if(usb_buffer_index)
	{
		if(!(EPIO[USB_IN_ID].cntrl & bmEPBUSY))
			// send buffer to usb out
			EPIO[USB_IN_ID].bytes = usb_buffer_index;
	}	
}

// ******************************************************************** 
// Routine: ISR_Ep2out ISR												
// Process: 								 							
// Inputs:	None														
// Outputs:	None														
// ******************************************************************** 
void Ep2out_Function(void)
{ 
	unsigned char cin, frame, index, n, nb_bytes;

	// V1.5
	// if(first_pass) return;

	// treat incoming data only if bootloader is off or src bootloader is usb in 
	// since the bl uses midi _in buffer for communicating with the host via usb
	if(usb_state_from_ez0_bit)
	if(z8_board_fully_updated_from_ez0_bit)
	if(fw_bl_src != FW_BL_SRC_MIDI_IN)
	{
		// Is there something in the OUT2BUF buffer? Store it to usb buffer
		if(	!(EPIO[OUT2BUF_ID].cntrl & bmEPBUSY) && (n = EPIO[OUT2BUF_ID].bytes) )
	   	{
			// turn on USB led
			USB_LED_PORT_OUT |= USB_LED_PIN;
			timeout_cpt = USB_LED_PERSISTENCY;

	  		// frame is the base address of each new midi frame (0, 4, ..)
			//frame=0;
	  		for(frame=0; frame < n; frame=frame + MIDI_USB_FRAME_LENGHT)
			//while(frame < n)
	  		{
	      		// Get CIN
	      		cin = OUT2BUF[frame] & 0x0F;

				// V1.9 workaround XP : always receive 64 bytes !
				if(cin == 0) break;

				// consider only data In on cable nb 0
				if( (OUT2BUF[frame] >> 4) == CN0)
				{
		      		// Get message length V1.9
		      		nb_bytes  = usb_midi_bytes_to_send[cin];

					// invalid ? V1.9
                	if(nb_bytes == 0) continue; 
                
					// Check if realtime, update realtime register and exit
		      		// V1.9 if(cin == (USB_SINGLE_BYTE & 0x0F))
					if(cin == 0x0F) // V1.9
					{
						// check incoming realtime data received if realtime source is USB IN
						// if(!realtime_source_int_z_ez1_bit)
							if(realtime_source_ext_z_ez1_bit)
							{
								EA = 0;
								Check_Realtime_MIDI_Data_Received(OUT2BUF[frame+1], &MIDI_In_Realtime_From_Usb);
								EA = 1;
							}
					}
			        else // Non realtime message
		      		{
			      		// Get message length  V1.9
			      		//nb_bytes  = usb_midi_bytes_to_send[cin];
						// add all other message bytes
						
						//DBG
						//Send_To_Midi_Out_Con(0xF0);

		         		for(index=0; index < nb_bytes ; index++)
						{
							//DBG
							//Send_To_Midi_Out_Con(OUT2BUF[index+frame+1] >> 4);
							//Send_To_Midi_Out_Con(OUT2BUF[index+frame+1] & 0x0F);
		            		EA = 0;
							// add data to midi_in_from_usb_buff
		            		MIDI_In_Add_Buffer(OUT2BUF[index+frame+1], &MIDI_In_From_Usb);

							EA = 1;
						}

						//DBG
						//Send_To_Midi_Out_Con(0xF7);
	
		         		// Check if message is complete OR if we are in a sysex
						// V1.9 if(nb_bytes)
						if(cin != (USB_SYSEX_START_IN & 0x0F))
			    			// Set the end_index pointer to enable transmission
			    			MIDI_In_From_Usb.end_index = MIDI_In_From_Usb.current_index;
		   			
					} // end non realtime
				} // end data was on CN0
				
				//frame += MIDI_USB_FRAME_LENGHT;
			
	    	} // enfor frame

			// Arm the OUT so it can receive the next packet
		   	EPIO[OUT2BUF_ID].bytes = 0;

			// turn off USB led
			//USB_LED_PORT_OUT &= ~USB_LED_PIN;	
		
		} // end if usb buffer contains data
	} // end bootloader source buffer is not midi in connector
}

#pragma AREGS
