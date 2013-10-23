//-----------------------------------------------------------------------------
//	File:		fw.c
//	Contents:	Firmware frameworks task dispatcher and device request parser
//				source.
//
//	Copyright (c) 2002 Cypress Semiconductor, Inc. All rights reserved
//
// $Archive: /USB/ez811/firmware/host/fw.c $
// $Date: 1/23/02 11:25a $
// $Revision: 4 $
//-----------------------------------------------------------------------------
#include "ezusb.h"
#include "ezregs.h"
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "main.h"
#include "usb.h"
#include "bootloader.h"
#include "serial.h"
#include "hardware.h"
#include "fw.h"

//-----------------------------------------------------------------------------
// Random Macros
//-----------------------------------------------------------------------------
#define	min(a,b) (((a)<(b))?(a):(b))
#define	max(a,b) (((a)>(b))?(a):(b))

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define	DELAY_COUNT				0x9248*8L		// Delay for 8 sec at 24Mhz, 4 sec at 48

#define NO_RENUM 

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
volatile BOOL	GotSUD;
BOOL			Rwuen;
BOOL			Selfpwr;
volatile BOOL	Sleep;						// Sleep mode enable flag

WORD	pDeviceDscr;	// Pointer to Device Descriptor; Descriptors may be moved
WORD	pConfigDscr;	
WORD	pStringDscr;

//WORD first_pass; // V1.5
	
// From MAIN.C
extern unsigned char bdata CTRL_REGS_7BITS_TO_Z8[CTRL_REG_MAX_FROM_EZ_TO_Z8];
extern unsigned char bdata CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_NB_FROM_Z8_TO_EZ];
extern bit usb_state_from_ez0_bit;
extern bit usb_enumrated_from_ez0_bit;
extern xdata Midi_In_Struct MIDI_In_From_Con, MIDI_In_From_Usb;
extern bit merger_src_midi_in_state_z_ez0_bit;
extern bit merger_src_usb_in_state_z_ez0_bit;
extern bit must_start_bootloader_from_ez0_bit;


//-----------------------------------------------------------------------------
// Code
//-----------------------------------------------------------------------------

// Task dispatcher
void main(void)
{
	DWORD	i;
	WORD	offset;
	DWORD	DevDescrLen;
//	DWORD	j; //=0;
	WORD	IntDescrAddr;
	WORD	ExtDescrAddr;

	// V1.5 workaround for chip silicon Rev D
//	EZUSB_Discon(FALSE);
//	USBCS |= bmDISCON;

//	USBCS = 0x0A; // disconnect
//	USBCS = 0x06; // reconnect

	// V1.5 disable usb for long time at boot
//	first_pass = 65535;

	// Initialize Global States
	Sleep 	= FALSE;	// Disable sleep mode
	Rwuen 	= FALSE;	// Disable remote wakeup
	Selfpwr = FALSE;	// Disable self powered
	GotSUD 	= FALSE;	// Clear "Got setup data" flag

	ISOCTL |= 1;									 // disable isochronous endpoints to free memory ISODISAB=1 (bit 0)
	CPUCS &= ~bmCLK24OE;							 // turn off Clock 24 out pin

	// Initialize user device
	TD_Init();

	// clear usb flags at starttup
	// this reg must be cleared before the usb is active since it contains all usb flags
	CTRL_REGS_7BITS_TO_Z8[0]	= 0;

	// The following section of code is used to relocate the descriptor table. 
	// Since the SUDPTRH and SUDPTRL are assigned the address of the descriptor 
	// table, the descriptor table must be located in on-part memory.
	// The 4K demo tools locate all code sections in external memory.
	// The descriptor table is relocated by the frameworks ONLY if it is found 
	// to be located in external memory.
	pDeviceDscr = (WORD)&DeviceDscr;
	pConfigDscr = (WORD)&ConfigDscr;
	pStringDscr = (WORD)&StringDscr;
	if ((WORD)&DeviceDscr & 0xe000)
	{
		IntDescrAddr = INTERNAL_DSCR_ADDR;
		ExtDescrAddr = (WORD)&DeviceDscr;
		DevDescrLen = (WORD)&UserDscr - (WORD)&DeviceDscr + 2;
		for (i = 0; i < DevDescrLen; i++)
			*((BYTE xdata *)IntDescrAddr+i) = 0xCD;
		for (i = 0; i < DevDescrLen; i++)
			*((BYTE xdata *)IntDescrAddr+i) = *((BYTE xdata *)ExtDescrAddr+i);
		pDeviceDscr = IntDescrAddr;
		offset = (WORD)&DeviceDscr - INTERNAL_DSCR_ADDR;
		pConfigDscr -= offset;
		pStringDscr -= offset;
	}


	// V1.5 workaround for chip silicon Rev D number 6
	// Firmware upload from external EEPROM must complete before the USB becomes active.
	//EZUSB_Delay(1000);
	//EZUSB_Delay(800);

	EZUSB_IRQ_ENABLE();				// Enable USB interrupt (INT2)
	EZUSB_ENABLE_RSMIRQ();			// Wake-up interrupt

	// The 8051 is responsible for all USB events, even those that have happened
	// before this point.  We cannot ignore pending USB interrupts.
	// The chip will come out of reset with the flags all cleared.
	USBIRQ = 0xff;					// Clear any pending USB interrupt requests

	USBBAV = USBBAV | 1 & ~bmBREAK;					// Disable breakpoints and autovectoring
	USBIEN |= bmSUDAV | bmSUTOK | bmSUSP | bmURES;	// Enable selected interrupts
	EA = 1;											// Enable 8051 interrupts


   	#ifndef NO_RENUM
   	// Note: at full speed, high speed hosts may take 5 sec to detect device
   	EZUSB_Discon(TRUE); // Renumerate
   	#endif

	// This loop waits until we receive a setup packet from the host.
	// NOTE: The device will continue to renumerate until it receives a setup
	// packet.  This fixes a microsoft USB bug that loses disconnect/reconnect 
	// events during initial USB device driver configuration dialog box.
	// B2 Load:  This code is not needed for B2 load, only for renumeration.
/*	// V1.5
	#ifndef NO_RENUM
		while(!GotSUD)
		{
			if(!GotSUD)
				EZUSB_Discon(TRUE);	// renumerate until setup received
			for(j=0;(j<DELAY_COUNT) && (!GotSUD);++j);
		}
	#endif
*/
	CKCON = (CKCON&(~bmSTRETCH)) | FW_STRETCH_VALUE; // Set stretch to 0 (after renumeration)

	// CHIP IS NOW READY TO OPERATE, CHECK IF BOOTLOADER IS REQUESTED
	// WAIT ALSO FOR THE Z8 INCOMING CONFIGURATION BYTES

	/* Check if USB is attached to host */
	Check_If_Usb_Attached();

	// check if bootloader request
	//Check_InitialSwitchesState();
	if(EDIT_PORT_IN & EDIT_PIN)	
		if(OK_PORT_IN & OK_PIN)
			if(CANCEL_PORT_IN & CANCEL_PIN)
				Check_Bootloader();	

	// turn on tx1 and rx1 (serial to/from z8)
	PORTBCFG |= 0x0C;	// Port B uses RX2 & TX2

	// if usb present : wait for the device beeing enumerated
//	while( (VUSB_PORT_IN & VUSB_PIN) && (!usb_enumrated_from_ez0_bit))
//			Check_Usb();

	// V1.2 Change the serial port Baud rate from OCD baud rate to 31250 kb/sec
	// TIMER 1 : BAUD RATE GENERATOR FOR SERIAL PORTS 1
    TH1  = 256-SERIAL_TO_Z8_BAUD_RELOAD;  	// Setup Baudrate reload value
    TL1  = 256-SERIAL_TO_Z8_BAUD_RELOAD;  	// Setup Baudrate overflow value

	// Turn on timer 0 (2 ms permanent timer for sync24)
	TR0 = 1;

	// synchronize the EZ to Z8 board
	SyncToZ8Board();

	/* ******************************* Main Loop ********************************* */
	while(TRUE)						
	{

		Check_Usb();

 		// V1.9 Check both MIDI in and USB in realtime buffers 
		Check_Realtime();

		// Call to main user loop there
		// TD_Poll();
		// Check if USB is attached to host 
		Check_If_Usb_Attached();
	
		// Check if MIDI In buffer contains complete event 
		Check_Midi_In_Buffer(&MIDI_In_From_Con, merger_src_midi_in_state_z_ez0_bit);
	
		// Check both MIDI in and USB in realtime buffers 
		//Check_Realtime();

		// Check Overflow flags
		Check_Overflow_Flags();
			
		// Check Serial buffer from Z8 board 
		Check_Serial_In_From_Z8_Buffer();

 		// V1.9 Check both MIDI in and USB in realtime buffers 
		//Check_Realtime();

		// Check Realtime from Z8 board (internal sync)
		//Check_Realtime_From_Z8();

		// Check if bootloader must be started
		if(must_start_bootloader_from_ez0_bit)
			Check_Bootloader();		

		// Check if USB In buffer contains complete event 
		if(usb_state_from_ez0_bit)
		{
		        // Check if USB MIDI In buffer contains complete event 
				Ep2out_Function();
	
				Check_Midi_In_Buffer(&MIDI_In_From_Usb, merger_src_usb_in_state_z_ez0_bit);
		}

		// Check both MIDI in and USB in realtime buffers 
		Check_Realtime();

		// V1.5
/*		if(first_pass)
		{
			first_pass--;
			if(first_pass==0)
			{
//				EZUSB_IRQ_ENABLE();				// Enable USB interrupt (INT2)
//				USB_LED_PORT_OUT |= USB_LED_PIN;
				USBCS = 0x06; // reconnect

				EZUSB_IRQ_ENABLE();				// Enable USB interrupt (INT2)
				EZUSB_ENABLE_RSMIRQ();			// Wake-up interrupt
				// The 8051 is responsible for all USB events, even those that have happened
				// before this point.  We cannot ignore pending USB interrupts.
				// The chip will come out of reset with the flags all cleared.
				USBIRQ = 0xff;					// Clear any pending USB interrupt requests
			
				USBBAV = USBBAV | 1 & ~bmBREAK;					// Disable breakpoints and autovectoring
				USBIEN |= bmSUDAV | bmSUTOK | bmSUSP | bmURES;	// Enable selected interrupts
			}
		}
*/
	}
}

// Device request parser
void SetupCommand(void)
{
	void	*dscr_ptr;
	DWORD	i;

	// V1.5
	//if(first_pass) return;

	switch(SETUPDAT[1])
	{
		case SC_GET_DESCRIPTOR:						// *** Get Descriptor
			if(DR_GetDescriptor())
				switch(SETUPDAT[3])			
				{
					case GD_DEVICE:				// Device
						SUDPTRH = MSB(pDeviceDscr);
						SUDPTRL = LSB(pDeviceDscr);
						break;
					case GD_CONFIGURATION:			// Configuration
						if(dscr_ptr = (void *)EZUSB_GetConfigDscr(SETUPDAT[2]))
						{
							SUDPTRH = MSB(dscr_ptr);
							SUDPTRL = LSB(dscr_ptr);
						}
						else
							EZUSB_STALL_EP0(); 	// Stall End Point 0
						break;
					case GD_STRING:				// String
						if(dscr_ptr = (void *)EZUSB_GetStringDscr(SETUPDAT[2]))
						{
							// Workaround for rev D errata number 8
							// If you're certain that you will never run on rev D,
							// you can just do this:
							// SUDPTRH = MSB(dscr_ptr);
							// SUDPTRL = LSB(dscr_ptr);
							STRINGDSCR *sdp;
							BYTE len;

							sdp = dscr_ptr;

							len = sdp->length;
							if (len > SETUPDAT[6]) 
								len = SETUPDAT[6]; //limit to the requested length
							
							while (len)
							{
								for(i=0; i<min(len,64); i++)
									*(IN0BUF+i) = *((BYTE xdata *)sdp+i);

								//set length and arm Endpoint
								EZUSB_SET_EP_BYTES(IN0BUF_ID,min(len,64));	
								len -= min(len,64);
                        		(BYTE *)sdp += 64;

								// Wait for it to go out (Rev C and above)
								while(EP0CS & 0x04)
									;
							}

							// Arm a 0 length packet just in case.  There was some reflector traffic about
							// Apple hosts asking for too much data.  This will keep them happy and will
							// not hurt valid hosts because the next SETUP will clear this.
							EZUSB_SET_EP_BYTES(IN0BUF_ID,0);	
							// Clear the HS-nak bit
							EP0CS = bmHS;
						}
						else 
							EZUSB_STALL_EP0();	// Stall End Point 0
						break;
					default:				// Invalid request
						EZUSB_STALL_EP0();		// Stall End Point 0
				}
			break;
		case SC_GET_INTERFACE:						// *** Get Interface
			DR_GetInterface();
			break;
		case SC_SET_INTERFACE:						// *** Set Interface
			DR_SetInterface();
			break;
		case SC_SET_CONFIGURATION:					// *** Set Configuration
			DR_SetConfiguration();
			break;
		case SC_GET_CONFIGURATION:					// *** Get Configuration
			DR_GetConfiguration();
			break;
		case SC_GET_STATUS:						// *** Get Status
			if(DR_GetStatus())
				switch(SETUPDAT[0])
				{
					case GS_DEVICE:				// Device
						IN0BUF[0] = ((BYTE)Rwuen << 1) | (BYTE)Selfpwr;
						IN0BUF[1] = 0;
						EZUSB_SET_EP_BYTES(IN0BUF_ID,2);
						break;
					case GS_INTERFACE:			// Interface
						IN0BUF[0] = 0;
						IN0BUF[1] = 0;
						EZUSB_SET_EP_BYTES(IN0BUF_ID,2);
						break;
					case GS_ENDPOINT:			// End Point
						IN0BUF[0] = EPIO[EPID(SETUPDAT[4])].cntrl & bmEPSTALL;
						IN0BUF[1] = 0;
						EZUSB_SET_EP_BYTES(IN0BUF_ID,2);
						break;
					default:				// Invalid Command
						EZUSB_STALL_EP0();		// Stall End Point 0
				}
			break;
		case SC_CLEAR_FEATURE:						// *** Clear Feature
			if(DR_ClearFeature())
				switch(SETUPDAT[0])
				{
					case FT_DEVICE:				// Device
						if(SETUPDAT[2] == 1)
							Rwuen = FALSE; 		// Disable Remote Wakeup
						else
							EZUSB_STALL_EP0();	// Stall End Point 0
						break;
					case FT_ENDPOINT:			// End Point
						if(SETUPDAT[2] == 0)
                  		{
							EZUSB_UNSTALL_EP( EPID(SETUPDAT[4]) );
                     		EZUSB_RESET_DATA_TOGGLE( SETUPDAT[4] );
                  		}
						else
							EZUSB_STALL_EP0();	// Stall End Point 0
						break;
				}
			break;
		case SC_SET_FEATURE:						// *** Set Feature
			if(DR_SetFeature())
				switch(SETUPDAT[0])
				{
					case FT_DEVICE:				// Device
						if(SETUPDAT[2] == 1)
							Rwuen = TRUE;		// Enable Remote Wakeup
						else
							EZUSB_STALL_EP0();	// Stall End Point 0
						break;
					case FT_ENDPOINT:			// End Point
						if(SETUPDAT[2] == 0)
							EZUSB_STALL_EP( EPID(SETUPDAT[4]) );
						else
							EZUSB_STALL_EP0();	 // Stall End Point 0
						break;
				}
			break;
		default:							// *** Invalid Command
			if(DR_VendorCmnd())
				EZUSB_STALL_EP0();				// Stall End Point 0
	}

	// Acknowledge handshake phase of device request
	// Required for rev C does not effect rev B
	EP0CS |= bmBIT1;
}

// Wake-up interrupt handler
void resume_isr(void) interrupt WKUP_VECT
{
	// V1.5
	//if(first_pass) return;

	EZUSB_CLEAR_RSMIRQ();
}
