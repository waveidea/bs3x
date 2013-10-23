#pragma NOIV					// Do not generate interrupt vectors

//-----------------------------------------------------------------------------
//	File:		periph.c
//	Contents:	Hooks required to implement USB peripheral function.
//
//	Copyright (c) 1997 AnchorChips, Inc. All rights reserved
//-----------------------------------------------------------------------------
#include <ezusb.h>
#include <ezregs.h>
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "periph.h"
#include "hardware.h"
#include "itr.h"
#include "serial.h"
#include "usb.h"
#include "usb_defs.h"

extern BOOL	GotSUD;			// Received setup data flag
extern BOOL	Sleep;
extern BOOL	Rwuen;
extern BOOL	Selfpwr;

BYTE	Configuration;		// Current configuration
BYTE	AlternateSetting;	// Alternate settings

extern bit usb_enumrated_from_ez0_bit;
//bit usb_bootloader_requested;

/* ******************************************************************** */
/* Routine: TD_Init														*/
/* Process: Init Variables default states								*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
//-----------------------------------------------------------------------------
// Task Dispatcher hooks
// The following hooks are called by the task dispatcher.
//-----------------------------------------------------------------------------
void TD_Init(void) 				// Called once at startup
{
	// Init i2c bus for eeprom ez upgrade
	EZUSB_InitI2C();
	//Rwuen = TRUE;				// Enable remote-wakeup

	// Initializations
	Init_IOs();

	// Init Serial Interface and MIDI I/O buffers
//	TD_Resume();

	Init_Variables();
	Init_Serial_Ports();

	// Enable endpoint 2 in, and endpoint 2 out
	//IN07VAL = bmEP2;			// Validate all EP's
	// IN07VAL = bmEP1;			// Validate all EP's
	IN07VAL  = USB_ITRS_EP_IN;
	OUT07VAL = bmEP2;

	// Enable double buffering on endpoint 2 in, and endpoint 2 out
//	USBPAIR = 0x09;
//	USBPAIR = 0x08;   // pair out endpoint 2&3
	USBPAIR = USB_PAIRED_EP;   // pair out endpoint 2&3

	// Arm Endpoint 2 out to receive data
	EPIO[OUT2BUF_ID].bytes = 0;

}

/* ******************************************************************** */
/* Routine: TD_Suspend													*/
/* Process: 															*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
BOOL TD_Suspend(void) 			// Called before the device goes into suspend mode
{
	// Turn off breakpoint light before entering suspend
	USBBAV |= bmBREAK;		// Clear the breakpoint
	return(TRUE);
}

BOOL TD_Resume(void) 			// Called after the device resumes
{
//	Init_Variables();
//	Init_Serial_Ports();
	//Check_InitialSwitchesState();
//	if(EDIT_PORT_IN & EDIT_PIN)	
//		if(OK_PORT_IN & OK_PIN)
//			if(CANCEL_PORT_IN & CANCEL_PIN)
//				usb_bootloader_requested = TRUE;
	return(TRUE);
}

//-----------------------------------------------------------------------------
// Device Request hooks
//	The following hooks are called by the end point 0 device request parser.
//-----------------------------------------------------------------------------

BOOL DR_GetDescriptor(void)
{
	return(TRUE);
}

BOOL DR_SetConfiguration(void)	// Called when a Set Configuration command is received
{
	Configuration = SETUPDAT[2];
	usb_enumrated_from_ez0_bit = TRUE;
	return(TRUE);				// Handled by user code
}

BOOL DR_GetConfiguration(void)	// Called when a Get Configuration command is received
{
	IN0BUF[0] = Configuration;
	EZUSB_SET_EP_BYTES(IN0BUF_ID,1);
	return(TRUE);				// Handled by user code
}

BOOL DR_SetInterface(void) 		// Called when a Set Interface command is received
{
	AlternateSetting = SETUPDAT[2];
	return(TRUE);				// Handled by user code
}

BOOL DR_GetInterface(void) 		// Called when a Set Interface command is received
{
	IN0BUF[0] = AlternateSetting;
	EZUSB_SET_EP_BYTES(IN0BUF_ID,1);
	return(TRUE);				// Handled by user code
}

BOOL DR_GetStatus(void)
{
	return(TRUE);
}

BOOL DR_ClearFeature(void)
{
	return(TRUE);
}

BOOL DR_SetFeature(void)
{
	return(TRUE);
}

BOOL DR_VendorCmnd(void)
{
	return(TRUE);
}

//-----------------------------------------------------------------------------
// USB Interrupt Handlers
//	The following functions are called by the USB interrupt jump table.
//-----------------------------------------------------------------------------

// Setup Data Available Interrupt Handler
void ISR_Sudav(void) interrupt 0
{
	GotSUD = TRUE;				// Set flag
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSUDAV;			// Clear SUDAV IRQ
}

// Setup Token Interrupt Handler
void ISR_Sutok(void) interrupt 0
{
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSUTOK;			// Clear SUTOK IRQ
}

void ISR_Sof(void) interrupt 0
{
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSOF;				// Clear SOF IRQ
	// V1.5 workaround for Rev D silicon chip : clear this bit two times
//	USBIRQ = bmSOF;				// Clear SOF IRQ
}

void ISR_Ures(void) interrupt 0
{
	EPIO[OUT2BUF_ID].bytes = 0;

	EZUSB_IRQ_CLEAR();
	USBIRQ = bmURES;			// Clear URES IRQ
}

void ISR_IBN(void) interrupt 0
{
   // ISR for the IN Bulk NAK (IBN) interrupt.
}

void ISR_Susp(void) interrupt 0
{
	Sleep = TRUE;
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSUSP;
}

void ISR_Ep0in(void) interrupt 0
{
}

void ISR_Ep0out(void) interrupt 0
{
}

void ISR_Ep1in(void) interrupt 0
{
}

void ISR_Ep1out(void) interrupt 0
{
}

void ISR_Ep2in(void) interrupt 0
{
}

void ISR_Ep2out(void) interrupt 0
{
/*
	// call the function which treats the received usb packet(s)
	Ep2out_Function();

	// Arm the OUT so it can receive the next packet
   	EPIO[OUT2BUF_ID].bytes = 0;

   	// clear the IRQ
	EZUSB_IRQ_CLEAR();
	OUT07IRQ = bmEP2;
*/
}


void ISR_Ep3in(void) interrupt 0
{
}

void ISR_Ep3out(void) interrupt 0
{
}

void ISR_Ep4in(void) interrupt 0
{
}

void ISR_Ep4out(void) interrupt 0
{
}

void ISR_Ep5in(void) interrupt 0
{
}

void ISR_Ep5out(void) interrupt 0
{
}

void ISR_Ep6in(void) interrupt 0
{
}

void ISR_Ep6out(void) interrupt 0
{
}

void ISR_Ep7in(void) interrupt 0
{
}

void ISR_Ep7out(void) interrupt 0
{
}
