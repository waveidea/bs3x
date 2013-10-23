#ifndef GLOBALVAR_H		//Header sentry
#define GLOBALVAR_H

#include "serial.h"
#include "pins_def.h"

/* *********************** GLOBAL VARIABLES DECLARATIONS *************************** */
unsigned char xdata midi_in_from_con_buff[MIDI_IN_FROM_CON_BUFF_LENGHT];
unsigned char xdata midi_in_from_usb_buff[MIDI_IN_FROM_USB_BUFF_LENGHT];
unsigned char xdata serial_in_from_z8_buff[SERIAL_IN_FROM_Z8_BUFF_LENGHT];

// Bits addressable Registers from 0x20 to 0x2F
unsigned char bdata config_reg_0; //  _at_ 0x0020;
	sbit bsp_group_0 	= config_reg_0 ^ 0;
	sbit bsp_group_1 	= config_reg_0 ^ 1;
	sbit bsp_group_2 	= config_reg_0 ^ 2;
	sbit bsp_group_3 	= config_reg_0 ^ 3;
	sbit merger_state 	= config_reg_0 ^ 4;
	sbit filter_state 	= config_reg_0 ^ 5;
	sbit sync24_state 	= config_reg_0 ^ 6;
	sbit lfo_on_ext 	= config_reg_0 ^ 7;

// Bits addressable Registers from 0x20 to 0x2F
unsigned char bdata config_reg_1; //  _at_ 0x0020;
	sbit sync24_from_usb = config_reg_1 ^ 0;

unsigned char bdata status_reg_0;
	sbit midi_in_usb_in_sysex 		= status_reg_0 ^ 0;
	sbit midi_overflow 				= status_reg_0 ^ 1;
 	sbit bootloading_z8 			= status_reg_0 ^ 2;
	sbit usb_present				= status_reg_0 ^ 3;
	sbit rx_from_z8_is_midi			= status_reg_0 ^ 4;
	sbit must_clear_sync24_cont		= status_reg_0 ^ 5;
	sbit must_clear_sync24_clock	= status_reg_0 ^ 6;
 	sbit bootloading_ezusb 			= status_reg_0 ^ 7;

unsigned char bdata status_reg_1;
	sbit bootloader_source					= status_reg_1 ^ 0;
	sbit bootloader_must_finish				= status_reg_1 ^ 1;


unsigned char 	RX2_index;
unsigned char 	RX2_data_buff_lenght; 
unsigned char 	RX2_data_index;
unsigned char 	RX2_channel_sel;
//BOOL 			usb_present;
//BOOL			must_send_clock_to_tx2;

// Registers for MIDI_In_From_Con merger
unsigned char MIDI_In_From_Con_Merger_State;
unsigned char MIDI_In_From_Con_Cur_Index;
unsigned char MIDI_In_From_Con_Start_Index;
unsigned char MIDI_In_From_Con_End_Index;
unsigned char MIDI_In_From_Con_Current_Status;

// Registers for MIDI_In_From_USB merger
unsigned char MIDI_In_From_USB_Merger_State;
unsigned char MIDI_In_From_USB_Cur_Index;
unsigned char MIDI_In_From_USB_Start_Index;
unsigned char MIDI_In_From_USB_End_Index;
unsigned char MIDI_In_From_USB_Current_Status;

// Registers for Serial_In_From_Z8_Buffer merger
unsigned char Serial_In_From_Z8_Merger_State;
unsigned char Serial_In_From_Z8_Cur_Index;
unsigned char Serial_In_From_Z8_Start_Index;
unsigned char Serial_In_From_Z8_End_Index;
unsigned char Serial_In_From_Z8_Current_Status;

unsigned char TX_To_Midi_Out_1_Buff[SERIAL_TX_BUFF_LENGTH];
unsigned char TX_To_Z8_Serial_Buff[SERIAL_TX_BUFF_LENGTH];
unsigned char tx_to_midi_out_1_in_index;
unsigned char tx_to_midi_out_1_out_index;
unsigned char tx_to_z8_serial_in_index;
unsigned char tx_to_z8_serial_out_index;
/*static*/   bit  tx_to_midi_out_1_busy;
/*static*/   bit  tx_to_z8_serial_busy;

int 		  bootloader_address;
int 		  bootloader_word_write;
int 		  bootloader_word_read;

#endif // GLOBALVAR_H


