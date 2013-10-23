#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "serial.h"
#include "realtime.h" // V1.9
#include "curve_tables.h"
#include "eeprom.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "hardware.h"
#include "digital.h"
#include "arpp.h"
#include "motion.h"
#include "autotest.h"
#include "automation.h"
#include "mackie.h" // V1.5
#include "i2c.h" // V1.6
#include "itrs.h" // V1.62
//#include "assign.h" // V1.61

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar current_to_midi;
far   uchar serial_in_buffer[SERIAL_IN_BUFF_LENGHT];	// serial in buffer
far   uchar CURVE_USER_0_TABLE[CURVE_TABLES_LENGHT];	// log table
far   uchar CURVE_USER_1_TABLE[CURVE_TABLES_LENGHT];	// log table

far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];	// NN=1 registers can be accessed from ez board
uchar serial_in_start_index;						// a near pointer on FAR ram
uchar serial_in_end_index;							// a near pointer on FAR ram
uchar serial_in_current_index;
uchar midi_delay_cpt;

volatile uchar TX_STATUS; // V1.8
uchar tx0_start_index;
uchar tx0_end_index;
uchar tx1_start_index;
uchar tx1_end_index;
far uchar TX0_Buff[TX_BUFF_LENGTH];
far uchar TX1_Buff[TX_BUFF_LENGTH];

far	uint must_reinit_lcd_from_sysex;

far uchar control_index_structs_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61
uchar last_struct_index;
extern far All_Params	 Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61 structure of parameters in EEPROM V1.61
uchar last_lcd_struct_index;
far uchar control_index_lcd_structs_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61
extern far uchar lcd_strings_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB][LCD_STR_SIZE];	// all lcd strings preloaded in RAM V1.61

extern rom char identity[];

extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_6;
extern uchar Z8_STATUS_7; // V1.3
extern uchar Z8_STATUS_8; 
extern uchar Z8_STATUS_9;
extern uchar Z8_STATUS_0; // V1.8
extern uchar Z8_STATUS_11; // V1.8
//extern far All_Params	 Param;			// structure of parameters in EEPROM
extern far All_Params *ParamCurrent;		// V1.61 current structure of parameters in EEPROM

extern uchar current_value;				// current control value
extern uchar control_index;				// current control nb
extern uchar last_control_index;		// last control nb
extern uchar next_transport_state;
extern uint  EE_Address;
extern uchar I2cCommand;
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern uchar lcd_lock_incrust_cpt;

extern rom uchar prog_txt[];
extern rom uchar upload_txt[];
extern rom uchar bmp_prg_upld_res[];
extern uchar lcd_current_incrust;
// V1.66 extern rom uchar font_5x8[];
extern rom uchar bmp_success_txt[];
extern rom uchar bmp_error_txt[];
extern rom uchar bmp_midi_txt[];
extern rom uchar bmp_lcd_txt[];
extern rom uchar info_txt[];
extern rom uchar corrupted_sysex_txt[];
extern rom uchar received_txt[];
extern rom uchar common_param_txt[];
extern rom uchar programmed_txt[];
extern rom uchar uploaded_txt[];
//extern uchar current_hook_bmp;
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes
extern uint  current_random;
extern uchar whole_sw_modes[NB_BYTES_FOR_SW];
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion	

extern uchar Motion_Status_Rec; 		// each bit indicates if part's recording
extern uchar led_midi_out1_cpt;
extern uchar led_midi_out2_cpt;
extern uchar menu_state; // V1.2

//extern uchar assign_state; // V1.61
extern far uchar last_displayed_channel;	// V1.62
// V1.66 extern rom uchar font_num_8x16[]; 		// V1.62
extern far uchar override_channel_cpt;		// V1.62

extern rom uchar reorg_other_sw[TRANSPORT_NB]; // V1.8

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitSerial()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 Serials periferals I2C and UARTS
///////////////////////////////////////////////////////////////////////////////////////
void InitSerial(void)
{	
	// Init UART 0
	U0CTL1 = 0x02; 					// NO itr on rx FOR NOW, brg normal, 8 bits, no infrared
	U0CTL0 = 0xC0; 					// rx & tx on, no parity, no loop, no break, no cts
  	U0BRH = (uchar)(BRG0 >> 8);		// set uart baud rate generator freq
  	U0BRL = (uchar)(BRG0 & 0xFF);	// set uart baud rate generator freq

	// Init UART 1
	U1CTL1 = 0x02; 					// no rx itr, brg normal, 8 bits, no infrared	
	U1CTL0 = 0x80; 					// tx only, rx off, no parity, no loop, no break, no cts
  	U1BRH = (uchar)(BRG1 >> 8);		// set uart baud rate generator freq
  	U1BRL = (uchar)(BRG1 & 0xFF);	// set uart baud rate generator freq
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendToTx0()
// Inputs	: data to send
// Outputs	: None
// Process  : Send a byte to uart 0 (ezboard)
///////////////////////////////////////////////////////////////////////////////////////
void SendToTx0(uchar value)
{		
	while( TX_STATUS & tx0_full_txstat );

	DI();	// Disable Interrupts

	if(!(TX_STATUS & tx0_busy_txstat))
	{
		TX_STATUS |= tx0_busy_txstat;
		U0TXD = value;
    }
    else 
	{
		TX0_Buff[tx0_end_index++ & (TX_BUFF_LENGTH-1)] = value;
		if( ((tx0_end_index ^ tx0_start_index) & (TX_BUFF_LENGTH-1)) == 0 )
			TX_STATUS |= tx0_full_txstat;
    }	

	EI();	// Re-Enable interrupts

	// wait for the uart transmit reg becomes empty
//	while (!(U0STAT0 & TDRE));
	// now send the data
//	U0TXD = value;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendToTx1()
// Inputs	: data to send
// Outputs	: None
// Process  : Send a byte to uart 1 (midi out 2)
///////////////////////////////////////////////////////////////////////////////////////
void SendToTx1(uchar value)
{		
	while( TX_STATUS & tx1_full_txstat );

	DI();	// Disable Interrupts

	if(!(TX_STATUS & tx1_busy_txstat))
	{
		TX_STATUS |= tx1_busy_txstat;
		U1TXD = value;
    }
    else 
	{
		TX1_Buff[tx1_end_index++ & (TX_BUFF_LENGTH-1)] = value;
		if( ((tx1_end_index ^ tx1_start_index) & (TX_BUFF_LENGTH-1)) == 0 )
			TX_STATUS |= tx1_full_txstat;
    }	

	EI();	// Re-Enable interrupts

	// wait for the uart transmit reg becomes empty
//	while (!(U1STAT0 & TDRE));
	// now send the data
//	U1TXD = value;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckSerialInBuffer()
// Inputs	: None
// Outputs	: None
// Process  : Check for serial Input buffer, program rom if required
///////////////////////////////////////////////////////////////////////////////////////
void CheckSerialInBuffer(void)
{
	uchar local_end_index_minus_one = (serial_in_end_index - 1)&(SERIAL_IN_BUFF_LENGHT-1);
	uchar SYSEX_ID0_byte, start_index_offset;//, SYSEX_ID2_byte;
	
//DBG
//	uchar i;

	// Detect if there is a valid frame into the buffer
	while(serial_in_start_index != local_end_index_minus_one)
	{ 
		// Check valid frame is present (if SOF and EOF are presents)
		if( (serial_in_buffer[serial_in_start_index] 	     == SOF_TO_EZ) &&
		    (serial_in_buffer[(local_end_index_minus_one-1)&(SERIAL_IN_BUFF_LENGHT-1)] == EOF_TO_EZ) )
		{
			// Detect here which frame type has been received
			SYSEX_ID0_byte  = serial_in_buffer[(serial_in_start_index + FRAME_TYPE_POS)&(SERIAL_IN_BUFF_LENGHT-1)];

			if( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == /*v1.1 FRAME_IS_PURE_MIDI*/ FRAME_IS_FIRMWARE_OR_PURE_MIDI)
			{
				////////////////////////////// PURE MIDI RECEIVED //////////////////////////////////
				// Here we received a pure midi data buffer 
				// It may be a Note ON/OFF event dedicated to arpeggio and Mackie control mode
				// It may be a Sysex dedicated Mackie control mode
				// Bit 5 of SYSEX_ID0 contains the MIDI In source (usb or midi in connector)
				////////////////////////////////////////////////////////////////////////////////////
				ParsePureMidiInBuffer(local_end_index_minus_one, SYSEX_ID0_byte);
				serial_in_start_index = local_end_index_minus_one;
			}	
			else // frame is not pure midi
			{
				// be sure to reload struct after a sysex has been received
				last_control_index = CTRL_UNDEF;

				// Detect here which frame type has been received
				if( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_COMMON_PARAM)
				{
					//////////////////////////////////////////////////////////////////////////////////////
					////////////////////// COMMON PARAM REG RECEIVED / REQUESTED /////////////////////////				
					//////////////////////////////////////////////////////////////////////////////////////
					start_index_offset = Prog_Upload_Common_Param(SYSEX_ID0_byte, serial_in_start_index);
					if(start_index_offset == INVALID_REQUEST)
					{
						Display_Invalid_Sysex_Incrust();
						serial_in_start_index = local_end_index_minus_one;
					}
					else
						serial_in_start_index = (serial_in_start_index + start_index_offset)&(SERIAL_IN_BUFF_LENGHT-1);
				}
				else
				if( ( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_LCD_CTRL_ASSIGN) ||
					( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN) )
				{
					//////////////////////////////////////////////////////////////////////////////////////
					////////////////////// CONTROL ASSIGN RECEIVED / REQUESTED ///////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////
					start_index_offset = Prog_Upload_Ctrl_Assign(SYSEX_ID0_byte, serial_in_start_index);
					if(start_index_offset == INVALID_REQUEST)
					{
						Display_Invalid_Sysex_Incrust();
						serial_in_start_index = local_end_index_minus_one;
					}
					else
						serial_in_start_index = (serial_in_start_index + start_index_offset)&(SERIAL_IN_BUFF_LENGHT-1);
				}
			} // end frame was not pure midi
		} // end valid frame 
		else
		{
			// here buffer is not empty, but not a valid frame !		
			Display_Invalid_Sysex_Incrust();
			serial_in_start_index = local_end_index_minus_one;
		}
	} // end while buffer not fully parsed

	// the buffer has now been treated
	serial_in_start_index = local_end_index_minus_one;
}



///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendControlToMidi()
// Inputs	: None
// Outputs	: None
// Process  : Send to midi current_value, control_index
///////////////////////////////////////////////////////////////////////////////////////
void SendControlToMidi(void)
{
	uchar tmp, sending_control_chain, current_value_saved, control_index_saved;
	uchar next_curve_model; 
	uchar must_insert_channel, fixed_channel_saved; // V1.4
	uchar tmpCheckRealtime; // V1.9

	if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
	{
		SendControlInAutotestMode(midi_out_2_en);
		return;
	}

	// Check if we are currently recording a motion and not playing a motion
	if( (Motion_Status_Rec) && (!(Z8_STATUS_9 & sending_motion_to_midi_z9)))
		CheckMotionRecording();

	// Need to fill structure to send : already filed ?
//  V1.8 structure is filled before a call to SendControlToMidi()
//	V1.8 so we can check individual hook modes before calling SendControlToMidi()
//	V1.8 FillParamCurrentStruct(); // V1.61

	//V 1.4 : option shift can lock or not the midi outputs
	if(Z8_STATUS_2_NVM & shift_lock_midi_z2nvm)
	// if shift active and not sending scene and not sending assigned transports then exit
	if(Z8_STATUS_9 & shift_state_z9)
	{
		// here shift is active, exit if not in scene sending
		if(!(Z8_STATUS_4 & sending_scene_z4))
			// exit if not sending transport assigned switches
			if((control_index < BW_ASSIGN_SW_CTRL_INDEX)||(control_index>REC_ASSIGN_SW_CTRL_INDEX))	
				return;
	}

	// check if scene exclude & scene sending then exit
	if(Z8_STATUS_4 & sending_scene_z4)
		if(ParamCurrent->ctrl_status_1 & scene_exclude)
			return;

	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	// CheckRealtimeMidi();

	// SEND MIDI IN USER DEFINED OR MACKIE MODE OR LOGIC MODE
	//V1.69 if(Z8_STATUS_2_NVM & operating_mode1_z2nvm)
	if(OPERATING_MODE_NVM >= MACKIE_MODE)
	{
		sending_control_chain 	= FALSE;
		do 
		{
			
			// V1.2 Check MIDI structure is coherent with no corrupted bytes
			if(last_control_index != control_index)
			if(CheckMidiStruct() == MIDI_STRUCT_NOT_VALID)
		    {
				// Here must display struct error on LCD incrust;
				 Display_Invalid_Midi_Struct_Incrust();
				return;
			}

			// if midi delay still locks midi then exit (decimate) or wait (scene or control chain)
		 	if(midi_delay_cpt)
			{
			 	if( (!(Z8_STATUS_4 & sending_scene_z4)) && (!sending_control_chain))
					return;
				// here midi delay still active and sending scenes or control chained : wait
				do { asm("nop");} while (midi_delay_cpt);
			}

			// Here we need to modify ParamCurrent->midi_string[] according to struct information
			// insert midi channel if required
			// V1.4 : SHIFT can lock bs3x midi channel insertion 
			must_insert_channel = FALSE;
			if(ParamCurrent->midi_channel_position)
			{
				// if SHIFT is NOT used to lock channel
				if(!(Z8_STATUS_0_NVM & shift_lock_channel_z0nvm))
					must_insert_channel = TRUE;
				else
				// if SHIFT is used to lock channel
				{
					// if SHIFT NOT active then insert channel
					if(!(Z8_STATUS_9 & shift_state_z9))
						must_insert_channel = TRUE;
				}
			}

			// do we have to insert MIDI channel in the string ?
			if(must_insert_channel)
			{
				// save fixed MIDI channel present in the MIDI string
				fixed_channel_saved = ParamCurrent->midi_string[ParamCurrent->midi_channel_position-1] & 0x0F;
				ParamCurrent->midi_string[ParamCurrent->midi_channel_position-1] &= 0xF0;
				ParamCurrent->midi_string[ParamCurrent->midi_channel_position-1] |= ( current_channel & 0x0F);  // V1.2 added & 0x0F
			}
			else
			{
				// V1.62 Here the MIDI channel is fixed or locked by the SHIFT button configured for that
				tmp = ParamCurrent->midi_string[0];
				// check if we can retriewe the MIDI channel in the MIDI string and display it while sending
				if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
				if(tmp >= MIDI_NOTE_OFF)
				if((tmp & 0xF0) <= MIDI_PITCH_WHEEL)
				{
					tmp &= 0x0F;
					// here the midi string status is between 0x80 and 0xE0
					// display the MIDI channel from this string
					if(last_displayed_channel != tmp)
					{
						DisplayLcdInt(LCD_ZONE_CHANNEL_X, LCD_ZONE_CHANNEL_Y, DIGITS_2|LCD_INVERT|FONT_NUM_8x16_FLG/* V1.66 , font_num_8x16 */, tmp+1 );
						last_displayed_channel = tmp;
						override_channel_cpt = MIDI_CHANNEL_OVERRIDE_PERSIST;
					}
				}
			}

 			// Get the curve model associated to this control
			next_curve_model = ParamCurrent->ctrl_status_2 & CURVE_MODEL_MSK;
			// Update LCD curve model
			UpdateLcdCurveModel(next_curve_model);
			// rescale current value according to curve model
			if(next_curve_model == CURVE_INV_LIN)
				current_value = 127 - current_value;
			else
			if(next_curve_model == CURVE_LOG)
				current_value = CURVE_LOG_TABLE[current_value];				
			else
			if(next_curve_model == CURVE_INV_LOG)
				current_value = 127 - CURVE_LOG_TABLE[current_value];				
			else
			if(next_curve_model == CURVE_RANDOM)
			{
				// select high/low byte of 16 bits random number
				if(Z8_STATUS_6 & random_byte_select)
					current_value = (uchar)(current_random) & 0x7F;	
				else
					current_value = (uchar)(current_random >> 2) & 0x7F;	
				// toggle high/low byte random select
				Z8_STATUS_6 ^= random_byte_select;
			}
			else
			if(next_curve_model == CURVE_USER_0)
				current_value = CURVE_USER_0_TABLE[current_value];			
			else
			if(next_curve_model == CURVE_USER_1)
				current_value = CURVE_USER_1_TABLE[current_value];			

			// Check if Min & Max values are different from 0-127
			Truncate_Current_Value();
			
			// V1.9 update transport state if the control is a realtime control
			if(current_value < 16)
			{
				tmpCheckRealtime = ParamCurrent->midi_string[0] | current_value;

				if(tmpCheckRealtime == MIDI_START)
				{
					Z8_STATUS_11 |= force_transport_play_z11;
					return;
				}

				if(tmpCheckRealtime == MIDI_STOP)
				{
					Z8_STATUS_11 |= force_transport_stop_z11;
					return;
				}
			}

			// V1.4 check if the control value has to be splitted into 2 nibbles
			if(ParamCurrent->ctrl_status_2 & split_ctrl_value)
			{
				// assign control value 1 as the MSB nibble
				ParamCurrent->midi_string[ParamCurrent->ctrl_value_position_0-1]  = (current_value & 0x70) >> 4;
				ParamCurrent->midi_string[ParamCurrent->ctrl_value_position_1-1]  =  current_value & 0x0F;
			}
			else
			{
				// insert control value 0 if required
				if(ParamCurrent->ctrl_value_position_0)
					ParamCurrent->midi_string[ParamCurrent->ctrl_value_position_0-1]  = current_value & 0x7F; // V1.2 added & 0x7F
				// insert control value 1 if required
				if(ParamCurrent->ctrl_value_position_1)
					ParamCurrent->midi_string[ParamCurrent->ctrl_value_position_1-1]  = current_value & 0x7F; // V1.2 added & 0x7F
			}


			// v1.3 : generate and insert checksum if required
			if(ParamCurrent->checksum_start_position)
				InsertChecksumInMidiString();
				
			// Now send the full midi string in user defined mode
			SendDispatchToMidi(&ParamCurrent->midi_string[0], ParamCurrent->midi_str_lenght, ParamCurrent->ctrl_status_1);

			//if(!(Z8_STATUS_11 & sending_realtime_to_midi_z11))
			//	CheckRealtimeMidi();
			
			// V1.4 restore the original fixed MIDI channel in the MIDI string
			if(must_insert_channel)
			{
				ParamCurrent->midi_string[ParamCurrent->midi_channel_position-1] &= 0xF0;
				ParamCurrent->midi_string[ParamCurrent->midi_channel_position-1] |= fixed_channel_saved;
			}

			// Now create a MIDI delay if required
			if(ParamCurrent->ctrl_status_1 & midi_delay_msk)
				LoadMidiDelay();
  			// IS THERE A CONTROL CHAINED ?
			// control_chained = (uchar)(ParamCurrent->ctrl_chain & 0x7FU); 
			// if(control_chained)
			if(ParamCurrent->ctrl_chain & 0x7F)
			{
				// Update LCD control chain if first control chain
				if(!sending_control_chain)
				{
					// first control chain to send : save original control index and value
					current_value_saved 	= current_value;
					control_index_saved 	= control_index;

					Z8_STATUS_5 |= must_show_ctrl_chain_z5;
					sending_control_chain = TRUE;
				}
				// get index of control chained
				control_index = ParamCurrent->ctrl_chain & 0x7F; //control_chained;
				// need to crossfade from preceding control ?
				if(ParamCurrent->ctrl_chain & 0x80)
					current_value = 127 - current_value_saved;

				FillParamCurrentStruct(); // V1.61
/*
// BLOCK REMOVED IN V1.61 
				// get struct address of chained control
				GetCtrlStructEEaddFromIndexGroup();
				// read control chained whole struct
				ReadMultiByteI2c((far uchar *)&ParamCurrent->ctrl_status_0, MIDI_STRUCT_SIZE);
				//last_control_index = control_index;
*/
			}
			else 
			{
				// if a control chain has been sent, we need to restore original structure and indexes
				if(sending_control_chain)
				{
					//last_control_index = CTRL_UNDEF;
					control_index = control_index_saved;
					current_value = current_value_saved;	

					FillParamCurrentStruct(); // V1.61
/*
// BLOCK REMOVED IN V1.61 
					GetCtrlStructEEaddFromIndexGroup();
					// read control chained whole struct
					ReadMultiByteI2c((far uchar *)&ParamCurrent->ctrl_status_0, MIDI_STRUCT_SIZE);
*/
				}
				return;
			}
		}
		while(sending_control_chain);

	}
	// SEND MIDI IN STANDARD MODE MODE OR PROPELLERHEADS REASON MODE
	// The MIDI string to send is assigned to ParamCurrent->midi_str
	else
	{
		// v1.1 v1.5
		// V1.69 if(Z8_STATUS_2_NVM & operating_mode0_z2nvm) 
		if(OPERATING_MODE_NVM == REASON_MODE)
		{
			// Reason mode
			ParamCurrent->midi_string[0] = MIDI_CONTROL_CHANGE | current_channel;
			ParamCurrent->midi_string[1] = control_index & 0x7F; 
		}
		else
		{
			// Standard mode
			// Note on or control change ?
			if(ParamCurrent->ctrl_status_0 & NOTE_CTRL_CHANGE)
					tmp = MIDI_NOTE_ON;
			else	tmp = MIDI_CONTROL_CHANGE;

			ParamCurrent->midi_string[0] = tmp | current_channel;
			// Now send number
			ParamCurrent->midi_string[1] = ParamCurrent->ctrl_status_0 & (~NOTE_CTRL_CHANGE); 
		}

		// Now send value
		ParamCurrent->midi_string[2] = current_value & 0x7F;

		SendDispatchToMidi(&ParamCurrent->midi_string[0], STD_MODE_MIDI_STR_LENGHT, ParamCurrent->ctrl_status_1);

		//if(!(Z8_STATUS_11 & sending_realtime_to_midi_z11))
		//	CheckRealtimeMidi();
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendDispatchToMidi()
// Inputs	: 	size of midi str to send, far ram pointer on buffer to send, destination outs 
// Outputs	: 	None
// Process  : 	send  ParamCurrent->midi_str global buffer to different midi outputs
//				Also update MIDI led outputs
///////////////////////////////////////////////////////////////////////////////////////
void SendDispatchToMidi(far uchar *buff_ptr, uchar midi_str_size, uchar destination)
{
	uchar index;
	uchar must_send_to_ez_board = FALSE;

	// Check if must also send to EZ_BOARD (if midi_out_1 en or (usb on & midi_usb en))
	if(destination & midi_out_1_en)
	{
		// turn on midi led out 2
		LED_MIDI_1_PORT_OUT  |= LED_MIDI_1_PIN;
		led_midi_out1_cpt = LED_MIDI_OUT_PERSISTENCY;
		must_send_to_ez_board = TRUE;
	}
	else
	if((destination & usb_out_en)&&(CTRL_REG_7BITS_FROM_EZ[0] & usb_state_from_ez0))
	 	must_send_to_ez_board = TRUE;

	// Check if to send TX1, send midi str as is
	if(destination & midi_out_2_en)
	{
		// turn on midi led out 2
		LED_MIDI_2_PORT_OUT |= LED_MIDI_2_PIN;
		led_midi_out2_cpt = LED_MIDI_OUT_PERSISTENCY;
	}

	// Check if to send TX0 or USB, send start of frame
	if( must_send_to_ez_board)
	{
		// Here midi data must also be sent on TX0 or USB : send begining of frame to TX0
		SendToTx0(SOF_TO_EZ);
		// Send also frame type indicator + outputs to select 
		SendToTx0(FRAME_TO_EZ_IS_MIDI | (destination & MIDI_OUT_SEL_MSK));
		// send also midi data lenght
		SendToTx0(midi_str_size);
	}
	
	// Now send all MIDI useful bytes
	for(index = 0; index < midi_str_size; index++)
	{
		//current_to_midi = ParamCurrent->midi_string[index];
		current_to_midi = buff_ptr[index];

		// Is the midi str also destinated to EZ board ?
		if(must_send_to_ez_board)
			// Here send to ez board
			SendToTx0(current_to_midi);		
			
		// Check if to send TX1, send midi str as is
		if(destination & midi_out_2_en)
			// Here send to physical midi connector TX1
			SendToTx1(current_to_midi);


	} // End of Midi str bytes to send

	if(must_send_to_ez_board)
		// Here midi data must was sent on TX0 or USB : send end of frame to TX0
		SendToTx0(EOF_TO_EZ);

	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	//if(!(Z8_STATUS_9  & sending_motion_to_midi_z9))
	//if(!(Z8_STATUS_11 & sending_realtime_to_midi_z11))
	//	CheckRealtimeMidi();

	// always turn off midi led out 1 and 2
	//LED_MIDI_1_PORT_OUT &= ~LED_MIDI_1_PIN;
	//LED_MIDI_2_PORT_OUT &= ~LED_MIDI_2_PIN;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	LoadMidiDelay()
// Inputs	: 	delay on 2 bits (0-30-60-90 ms) is in global var
// Outputs	: 	None
// Process  : 	
///////////////////////////////////////////////////////////////////////////////////////
void LoadMidiDelay(void)
{
	uchar ctrl_status_1_local = ParamCurrent->ctrl_status_1 & midi_delay_msk;
	midi_delay_cpt = 0;

	if(ctrl_status_1_local == 1)
		 midi_delay_cpt = 3;	// x10ms (decremented by timer 0)
	else
	if(ctrl_status_1_local == 2)
		 midi_delay_cpt = 6;	// x10ms (decremented by timer 0)
	else
	if(ctrl_status_1_local == 3)
		 midi_delay_cpt = 9;	// x10ms (decremented by timer 0)
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendUploadCtrlToMidi()
// Inputs	: 	buffer_size : all nibbles size to upload + headers
// Outputs	: 	None
// Process  : 	generic_buffer contains all nibbles to send from index WID_SYX_UPLOAD_HEADER_SIZE (9)
///////////////////////////////////////////////////////////////////////////////////////
void SendUploadGenericBufferToMidi(uchar SYSEX_ID0_byte, uchar SYSEX_ID1_byte, uchar SYSEX_ID2_byte, uchar SYSEX_ID3_byte)
{
	uchar tmp, checksum_send;
//	uchar tmp2 = 0;

	generic_buffer[0] = MIDI_SOX;
	generic_buffer[1] = WID_SYX_ID0;
	generic_buffer[2] = WID_SYX_ID1;
	generic_buffer[3] = WID_SYX_ID2;
	generic_buffer[4] = WID_SYX_ID3;
	generic_buffer[5] = WID_SYX_ID4;
	// frame type
	generic_buffer[6] = SYSEX_ID0_byte | sysex_direction_sysxid0; //| prog_upload_sysxid0;
	// frame lenght
	generic_buffer[7] = SYSEX_ID1_byte; 
	// frame address or index
	generic_buffer[8] = SYSEX_ID2_byte;
	// frame group or dont care
	generic_buffer[9] = SYSEX_ID3_byte;

	// A chekcsum result on all datas has to be sent ?
//	if(SYSEX_ID0_byte & checksum_return_sysxid0)
//	{
		// all other generic_buffer bytes contain nibbles to send to midi
		checksum_send = 0;
		// calculate checksum to send on all nibbles into the buffer
		for(tmp=0; tmp < SYSEX_ID1_byte; tmp++)
			checksum_send = (checksum_send + generic_buffer[10 + tmp]) & 127;

//		tmp2 = 1;
		generic_buffer[10 + SYSEX_ID1_byte] = checksum_send;
//	}

//	generic_buffer[10 + SYSEX_ID1_byte + tmp2] = MIDI_EOX;
	generic_buffer[10 + SYSEX_ID1_byte + 1] = MIDI_EOX;

	// force acknoledge to be sent on all midi outputs
	//ParamCurrent->ctrl_status_1 |= (midi_out_1_en | midi_out_2_en | usb_out_en);

	// total lenght is upload header + all datas uploaded + checksum + F7
	// SendDispatchToMidi(&generic_buffer[0], (11 + SYSEX_ID1_byte + tmp2), (midi_out_1_en | midi_out_2_en | usb_out_en));
	SendDispatchToMidi(&generic_buffer[0], (11 + SYSEX_ID1_byte + 1), (midi_out_1_en | midi_out_2_en | usb_out_en));
	
	//if(!(Z8_STATUS_11 & sending_realtime_to_midi_z11))
	//	CheckRealtimeMidi();	
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendCtrlRegToEzBoard()
// Inputs	: 	ctrl_reg_index_to_ez (address of register to send)
// Outputs	: 	None
// Process  : 	Send a single control register to EZ Board
///////////////////////////////////////////////////////////////////////////////////////
void SendCtrlRegToEzBoard(uchar ctrl_reg_index_to_ez)
{
	// Send Start of frame
	SendToTx0(SOF_TO_EZ);
	// Send also frame type indicator + register select value 
	SendToTx0(FRAME_TO_EZ_IS_CTRL_REG | ctrl_reg_index_to_ez);
	// Send the register value
	SendToTx0(CtrlReg[ctrl_reg_index_to_ez] & 0x7F);
	// Send End of frame
	SendToTx0(EOF_TO_EZ);
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Prog_Upload_Common_Param()
// Inputs	: 	Frame status, frame start index
// Outputs	: 	return frame lenght treated
// Process  : 	program or upload a common parameter
///////////////////////////////////////////////////////////////////////////////////////
uchar Prog_Upload_Common_Param(uchar SYSEX_ID0_byte, uchar serial_in_start_index)
{
	uchar frame_index		   	= 0;
	uchar common_param_lenght  	= serial_in_buffer[(serial_in_start_index + FRAME_DATA_LENGHT_POS)&(SERIAL_IN_BUFF_LENGHT-1)];
	uchar common_param_address 	= serial_in_buffer[(serial_in_start_index + FRAME_ADDR_INDEX_POS )&(SERIAL_IN_BUFF_LENGHT-1)];
	uchar SYSEX_ID3_byte		= serial_in_buffer[(serial_in_start_index + FRAME_GROUP_POS 	 )&(SERIAL_IN_BUFF_LENGHT-1)];
	uchar tmp					= 0;
	far uchar *local_ptr;
	uchar must_program_eeprom  	= FALSE;
	uchar must_read_eeprom  	= FALSE;
	uchar must_nibble_to_byte  	= FALSE;
	uchar must_display_lcd		= FALSE;
//v1.1	uint must_reinit_lcd  		= 0;
	uchar tmp2;

	// Check the received sysex validity
	if(Check_Sysex_Validity(SYSEX_ID0_byte, common_param_address, SYSEX_ID3_byte) == INVALID_REQUEST)
		return(INVALID_REQUEST);

	// nothing to re-init on LCD for now
	must_reinit_lcd_from_sysex = 0;

	///// COMMON PARAMETERS ALSO CONTAINS LCD MESSAGES TO DISPLAY FROM EZ TO Z8 BOARD /////
	if(common_param_address == LCD_MESSAGE_TO_Z8_INDEX)
	{
				////////////////////////////// LCD MESSAGE REQUESTED /////////////////////////////////
				// Decode message to display on lcd z8 in SYSEX_ID3_byte
				// copy overflow flags from EZ to Z8_STATUS_8 buffer
				Z8_STATUS_8 &= ~OVERFLOW_FROM_EZ_MSK;
 				// Z8_STATUS_8 |= SYSEX_ID3_byte;
				Z8_STATUS_8 |= serial_in_buffer[(serial_in_start_index + FRAME_DATA_START)&(SERIAL_IN_BUFF_LENGHT-1)];
				// Display overflow errors
				DisplayInputOverflowIncrust();
				return (FRAME_DATA_START + common_param_lenght + 1);
	}

	///// COMMON PARAMETERS ALSO CONTAINS THE 3X IDENTITY REQUEST (UPLOAD ONLY) /////
	if(common_param_address == IDENTITY_REQUEST_INDEX)
	{
				// indicates we are programming/uploading : no scenes allowed
				// Z8_STATUS_9 |= programming_z9;

				////////////////////////////// BS3X IDENTITY REQUESTED /////////////////////////////////
				generic_buffer[0] = MIDI_SOX;
				generic_buffer[1] = WID_SYX_ID0;
				generic_buffer[2] = WID_SYX_ID1;
				generic_buffer[3] = WID_SYX_ID2;
				generic_buffer[4] = WID_SYX_ID3;
				generic_buffer[5] = WID_SYX_ID4;
				// frame type
				generic_buffer[6] = SYSEX_ID0_byte | sysex_direction_sysxid0; //| prog_upload_sysxid0;
				// frame lenght
				generic_buffer[7] = common_param_lenght; 
				// frame address or index
				generic_buffer[8] = common_param_address;
				// frame group or dont care
				generic_buffer[9] = SYSEX_ID3_byte;
				// Now copy all identity bytes to generic buffer
				for(tmp=0; tmp<common_param_lenght; tmp++)
					generic_buffer[10+tmp] = identity[tmp] & 0x7F;
				// Send EOX
				generic_buffer[10 + common_param_lenght] = MIDI_EOX;
				// force identity to be sent on all midi outputs
				//ParamCurrent->ctrl_status_1 |= (midi_out_1_en | midi_out_2_en | usb_out_en);
				// total lenght is upload header + all datas uploaded + F7
				SendDispatchToMidi(&generic_buffer[0], (11 + common_param_lenght), (midi_out_1_en | midi_out_2_en | usb_out_en));

				// indicates we are have finished programming/uploading
				// Z8_STATUS_9 &= ~programming_z9;

				return (UPLOAD_Z8_FRAME_LENGHT);
	}

	// V1.2 : from there, there are read/write to ext eeprom 0 since common parameters 
	I2cCommand = BS3X_I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;	

	//////////////////////////////// NEW COMMON PARAM REG RECEIVED /////////////////////////
	///// 0xFF 0b00pxxx00 0b0lllllll 0b0000aaaa 00 vv vv vv 0xFE 
	///// (p=prog/upload) (l=control reg lenght) (a=reg address 0-31) (vv=reg value 0-127)
	////////////////////////////////////////////////////////////////////////////////////////
//	if(common_param_address <= LFO_FREQ_INDEX)
//	if(common_param_address <= ARP_MODE_INDEX)
	if(common_param_address <= LAST_SINGLE_REGISTER_INDEX)
	{
			/////////////////// PROGRAM/UPLOAD SINGLE CONTROL REGISTER /////////////////////
			////////////////////////////////////////////////////////////////////////////////
			local_ptr  = &CtrlReg[common_param_address];
			must_display_lcd 		= TRUE;
			// the following zones may have been updated, then reinit LCD
			must_reinit_lcd_from_sysex	= (uint)(	INIT_LCD_CHANNEL 		|	INIT_LCD_SCENE		|	
									  				INIT_LCD_SYNC24     	|	INIT_LCD_INFRARED	|
									  				INIT_LCD_IR_GRAPH		|	INIT_LCD_MODE ) ;

			//V1.3 tempo is sent as 2 nibbles			
			if(common_param_address == INT_TEMPO_INDEX)
				must_nibble_to_byte = TRUE;

			// V1.3 also reinit current selected automation
			UpdateAutomationLeds();

	}
	else
	if(common_param_address <= USER_CURVE_1_HIGH_INDEX)
	{
			///////////////////////////// PROGRAM/UPLOAD NEW USER CURVE ////////////////////
			////////////////////////////////////////////////////////////////////////////////
			must_program_eeprom = TRUE;
			must_display_lcd = TRUE;
			// V1.2 I2cCommand = EEPROM_ZERO_I2C_ADR;

			// select if 64 low or high bytes have to be programmed/uploaded
			tmp = 0; // V1.62

			// select if curve to be programmed/uploaded is USER 0 or 1, get eeprom address
			if(common_param_address <= USER_CURVE_0_HIGH_INDEX)
			{
				if(common_param_address == USER_CURVE_0_HIGH_INDEX)
					tmp = EEPROM_PAGE_SIZE;

				EE_Address = (uint)CURVE_USER_0_LOW_START_EE_ADR;
				local_ptr  = &CURVE_USER_0_TABLE[tmp];
			}
			else
			{
				if(common_param_address == USER_CURVE_1_HIGH_INDEX)
					tmp = EEPROM_PAGE_SIZE;

				EE_Address = (uint)CURVE_USER_1_LOW_START_EE_ADR;
				local_ptr  = &CURVE_USER_1_TABLE[tmp];
			}

			EE_Address += (uint)tmp;
	}
	else
	if(common_param_address == CTRL_REG_0_7B_FROM_EZ_INDEX)
	{
			/////////////////// PROGRAM/UPLOAD SINGLE COMMON PARAMETER /////////////////////
			////////////////////////////////////////////////////////////////////////////////
			local_ptr  = &CTRL_REG_7BITS_FROM_EZ[0];
			// only the USB bmp may have been affected by this common parameter
			must_reinit_lcd_from_sysex   = (uint) INIT_LCD_USB;
	}
	else
	if(common_param_address == GROUP_NAME_INDEX)
	{
			///////////////////////////// PROGRAM/UPLOAD NEW GROUP NAME ////////////////////
			////////////////////////////////////////////////////////////////////////////////
			// we use lcd_text_value as the buffer for getting/programming group name 
			local_ptr  = &lcd_text_value[0];
			must_program_eeprom 	= TRUE;
			must_read_eeprom 		= TRUE;
			must_nibble_to_byte 	= TRUE;
			must_display_lcd 		= TRUE;
			must_reinit_lcd_from_sysex 	= (uint)(	INIT_LCD_CHANNEL 		|	INIT_LCD_SCENE		|	
									    			INIT_LCD_SYNC24     	|	INIT_LCD_INFRARED	|
									  	    		INIT_LCD_IR_GRAPH		|	INIT_LCD_MODE) ;
									  
			// nibbles are in this buffer : double buffer size
			//common_param_lenght = common_param_lenght << 1;
			// get group from frame
			EE_Address  = (uint)(GROUP_NAME_BASE_EE_ADR + 
						  (uint)( (uint)GROUP_NAME_EE_LENGHT * 
						          (uint)serial_in_buffer[(serial_in_start_index + FRAME_GROUP_POS)&(SERIAL_IN_BUFF_LENGHT-1)]));
			
			// V1.2 I2cCommand	= EEPROM_ZERO_I2C_ADR;
	}

	////////////////////////////////////////////////////////////////////////////////
	/////////////////// NEED TO PROGRAM OR UPLOAD COMMON PARAMETERS ? //////////////
	////////////////////////////////////////////////////////////////////////////////
	if(!(SYSEX_ID0_byte & prog_upload_sysxid0))
	{
		////////////////////////////////////////////////////////////////////////////////
		//////////////////////// PROGRAM a new common parameters ///////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// V1.3 NEW BLOCK
		// Copy all bytes to program to generic buffer, nibbles to byte if required
		if(must_nibble_to_byte)
		{
			Serial_Nibb_To_Generic_Bytes((uchar)(serial_in_start_index + FRAME_DATA_START), common_param_lenght);
			// tmp2 will be used as lenght for programming eeprom : double since un-nibbeled data writen to eeprom
			tmp2 = common_param_lenght >> 1;
		}
		else
			// tmp2 will be used as lenght for programming eeprom
			tmp2 = common_param_lenght;

		//Now copy generic buffer to real registers
		for(tmp=0; tmp < tmp2; tmp++)
		{
			// here the received data were NOT nibbleled
			if(!must_nibble_to_byte) 
				// standard byte to byte copy of serial in buffer to generic buffer
				generic_buffer[tmp]  = serial_in_buffer[(serial_in_start_index + FRAME_DATA_START + tmp)&(SERIAL_IN_BUFF_LENGHT-1)];
			
			// now recopy genereic buffer to real register
			local_ptr[tmp] = generic_buffer[tmp];

		}
		// END OF NEW BLOCK V1.3
		
		// V1.3 special action required here
		if(common_param_address == INT_TEMPO_INDEX)
		{
			// reinit timer 2 for smpte sync
			//ReinitTimer2();
			// V1.8 reinit timer 3 for midi clock sync
			ReinitTimer3();
		}

		// V1.4 Autotest mode accesible via sysex
		if(common_param_address == CTRL_REG_1_7BITS_TO_EZ_INDEX)
			if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
				AutoTest();

		// Write new common parameter to eeprom if required
		if(must_program_eeprom)
			// Write all new common parameters bytes to external eeprom
			WriteMultiByteI2c(&generic_buffer[0], tmp2);

		// Need to update LCD after a common param programming, not all zone reinitialized since
 	    // lcd zone incrust is reinitialized for common param result upload/download
//v1.1		if(must_reinit_lcd_from_sysex)
//v1.1			UpdateLcdZones( (uint)(must_reinit_lcd_from_sysex) );

		// size of the treated buffer from start index (+1 is for eof)
		tmp2 = FRAME_DATA_START + common_param_lenght + 1;

	} // end program new common parameter
	else
	{
		////////////////////////////////////////////////////////////////////////////////
		//////////////////////// UPLOAD a common parameters ///////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// depending on nibbles or not, calculate eeprom buffer lenght to read
		if(must_nibble_to_byte)
			tmp2 = common_param_lenght >> 1;
		else
			tmp2 = common_param_lenght;

		// common param to upload is located in eeprom ?
		if(must_read_eeprom)	
			ReadMultiByteI2c(&local_ptr[0], tmp2);
		
		// byte to byte or byte to nibbles ?
		if(must_nibble_to_byte)
			// Copy all nibbles to upload to generic buffer, send buffer to midi outs
			Byte_To_Generic_Nibbles(WID_SYX_UPLOAD_HEADER_SIZE, common_param_lenght, &local_ptr[0]);
		else
		{
			// Copy all bytes to upload to generic buffer, send buffer to midi outs
			for(tmp=0; tmp < common_param_lenght; tmp++)
				generic_buffer[tmp+WID_SYX_UPLOAD_HEADER_SIZE] = local_ptr[tmp] & 0x7F;
		}

		// Send to MIDI the uploaded common paramerer
		SendUploadGenericBufferToMidi(SYSEX_ID0_byte, common_param_lenght, common_param_address, SYSEX_ID3_byte);

		// size of the treated buffer from start index
		tmp2 = UPLOAD_Z8_FRAME_LENGHT;

	} // end upload
	
	// A common parameter has been programmed/uploaded : must update LCD ?
	// V1.2 only show programmed/uploaded message only if not in a menu
	if(menu_state == NO_MENU)
	if(must_display_lcd)
	{
		// will be incrust title
		rom_to_lcd_text_copy(info_txt);

		if(lcd_current_incrust != LCD_INCRUST_COMMON_PARAM)
		{
			DisplayEmptyIncrust();
			// display error on received sysex
			rom_to_lcd_text_copy(common_param_txt);
			DisplayLcdText(LCD_ZONE_INCRUST_X+10, LCD_ZONE_INCRUST_Y+2, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */);
			if(!(SYSEX_ID0_byte & prog_upload_sysxid0))
				rom_to_lcd_text_copy(programmed_txt);
			else
				rom_to_lcd_text_copy(uploaded_txt);
			DisplayLcdText(LCD_ZONE_INCRUST_X+15, LCD_ZONE_INCRUST_Y+3, LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */);

			// Lock LCD incrust zone for 1 second
			lcd_lock_incrust_cpt = CPT_1_SEC;
		}

		lcd_current_incrust = LCD_INCRUST_COMMON_PARAM;			
	}

	// Update serial in index for treated frame
	return (tmp2);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Prog_Upload_Ctrl_Assign()
// Inputs	: 	Frame status, frame start index
// Outputs	: 	return frame lenght treated
// Process  : 	program or upload a control assign (midi or lcd)
///////////////////////////////////////////////////////////////////////////////////////
uchar Prog_Upload_Ctrl_Assign(uchar SYSEX_ID0_byte, uchar serial_in_start_index)
{

	uchar group_saved, control_index_saved, tmp, tmp2, tmp3/* V1.8, tmp4*/, tmp5, upload_size, prog_result ;
//	uchar ack_type;
	far uchar *local_ptr;
	rom uchar *local_bmp_ptr;
	rom uchar *local_bmp_ptr2;

	////////////////////////////////////////////////////////////////////////////////////////
	//////////////// PROG OR UPLOAD RECEIVED ///////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	// be sure to reload next struct since it will be used there
	group_saved			= current_group;
    control_index_saved	= control_index;
	// common to prog & upload : get group and control numbers
	control_index = serial_in_buffer[(serial_in_start_index + FRAME_ADDR_INDEX_POS)&(SERIAL_IN_BUFF_LENGHT-1)];
	current_group = serial_in_buffer[(serial_in_start_index + FRAME_GROUP_POS     )&(SERIAL_IN_BUFF_LENGHT-1)];


	// Check the received sysex validity
	if(Check_Sysex_Validity(SYSEX_ID0_byte, control_index, current_group) == INVALID_REQUEST)
	{
		// restore modified global vars
		current_group		= group_saved;
    	control_index		= control_index_saved;
		return(INVALID_REQUEST);
	}

	// is it a midi or lcd string to program/upload ?
	if((SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN)
	{
		// get eeprom address for the MIDI control to prog or upload and I2cCommand
		GetCtrlStructEEaddFromIndexGroup();
		// be sure to skip the first byte (ctrl_status_0 which is used only in standard mode)
		//EE_Address++;
		local_bmp_ptr2 = bmp_midi_txt;
		//ack_type = MIDI_STR_ACK;
	}
	else
	{
		// need to program/upload a LCD str : get LCD str address from group & index and I2cCommand
		GetLcdStrEEaddFromIndexGroup();
		local_bmp_ptr2 = bmp_lcd_txt;
		//ack_type = LCD_STR_ACK;
	}
	// upload result is always ok
	local_bmp_ptr = bmp_success_txt;

	///////////////////////////////////// NEW PROG CONTROL RECEIVED /////////////////////////
	///// 0xFF SYSEX_ID0 0b0lllllll 0b0sssssss 0b0ggggggg VV VV ... CS 0xFE ////////////////
	///// (s=ctrl g=group l=lenght VV=struct content x=dont care CS=checksum) 
	////////////////////////////////////////////////////////////////////////////////////////
	// Need to program or upload ?
	if(!(SYSEX_ID0_byte & prog_upload_sysxid0))
	{
		// here we received a new sysex control prog from EZ board
		// will be incrust title
		rom_to_lcd_text_copy(prog_txt);

		// get struct lenght to program in number of nibbles (contains midi header lenght + midi or lcd string lenght)
		tmp3 = serial_in_buffer[(serial_in_start_index + FRAME_DATA_LENGHT_POS)&(SERIAL_IN_BUFF_LENGHT-1)];

		// calculate checksum on all struct or lcd str bytes & Unsplit all nibbles into bytes to write to ext eeprom
		tmp5 = Serial_Nibb_To_Generic_Bytes((uchar)(serial_in_start_index + FRAME_DATA_START), tmp3);

		// compare received and calculated checksum
		if(tmp5 == serial_in_buffer[(serial_in_start_index + FRAME_DATA_START + tmp3)&(SERIAL_IN_BUFF_LENGHT-1)])
		{
			// checksum is ok
			// Write to ext eeprom reconstitued midi struct or lcd str (un-nibbleled)
			// V1.2 WriteMultiByteEEprom(&generic_buffer[0], tmp3);
			WriteMultiByteI2c(&generic_buffer[0], tmp3 >> 1); // send lenght / 2 since un nibbeled
			//prog_result = MIDI_NOERROR;
			//V1.2 prog_result = 0;
			prog_result = acknoledge_sysxid0;
			// MAYBE REMOVED ONCE DEBUGGED if it was a LCD string programmed then display it
			// V1.2 if((SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_LCD_CTRL_ASSIGN)
			// V1.2 {
			// V1.2 	UpdateLcdString();
			// V1.2 }

		}
		else
		{
			// bad checksum
			// prog_result = MIDI_ERROR;
			//V1.2 prog_result = acknoledge_sysxid0;
			prog_result = 0;
			local_bmp_ptr = bmp_error_txt;
		}

		// Acknoledge prog if required, lenght is 0
		if(SYSEX_ID0_byte & acknoledge_sysxid0)
		{
			// clear ack bit and insert prog result
			tmp  = (SYSEX_ID0_byte & ~acknoledge_sysxid0) | prog_result;
			// clear checksum return request since no checksum insertion here
//			tmp &= ~checksum_return_sysxid0;
			// Send prog ack result, lenght is 0 (ack result in SYSEX_ID0)
			SendUploadGenericBufferToMidi( tmp, 0, control_index, current_group);
		}

		// Reload switch toggle info if a switch midi was programmed
		// extract control_index from frame FF PROG_BYTE GG NN
		if((SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN)
		{
			// get control index from sysex
			tmp = serial_in_buffer[(serial_in_start_index + FRAME_ADDR_INDEX_POS)&(SERIAL_IN_BUFF_LENGHT-1)] & 0x7F;
		    // A switch has been programmed
			//V1.2 if(tmp >= BW_ASSIGN_SW_CTRL_INDEX)
			if( (tmp >= BW_ASSIGN_SW_CTRL_INDEX) && (tmp < LFO_CTRL_INDEX))
			{
				// V1.8 : reload toggle/push modes from EE
				GetCurrentGroupSwitchModesFromEE();

/* BLOCK REMOVED IN V1.8
				tmp5 = serial_in_buffer[(serial_in_start_index + FRAME_DATA_START + CTR_STATUS_1_SW_MODE_NIBBLE_INDEX) & (SERIAL_IN_BUFF_LENGHT-1)];

				// one of the 8 all_switch has been programmed
				if(tmp >= SWITCH_BASE_CTRL_INDEX)
				{
					// the ID is all_sw
					// convert 0-7 bit pos into a mask bit					
					tmp4 = 1 << (tmp - SWITCH_BASE_CTRL_INDEX);
					whole_sw_modes[ALL_SW_ID] &= ~(tmp4);
					if(tmp5 & CTR_STATUS_1_SW_MODE_MSK)
						whole_sw_modes[ALL_SW_ID] |= tmp4;
				}
				else
				// one of the 8 other_switch has been programmed
				{
					// the ID is other_sw
					// convert 0-7 bit pos into a mask bit					
					// V1.8 : need to reorg the other switches 0-4 (transport + shift)
					if( (tmp - BW_ASSIGN_SW_CTRL_INDEX) < TRANSPORT_NB)
						tmp4 = 1 << reorg_other_sw[(tmp - BW_ASSIGN_SW_CTRL_INDEX)];
					else
						tmp4 = 1 << (tmp - BW_ASSIGN_SW_CTRL_INDEX);
					whole_sw_modes[OTHER_SW_ID] &= ~(tmp4);
					if(tmp5 & CTR_STATUS_1_SW_MODE_MSK)
						whole_sw_modes[OTHER_SW_ID] |= tmp4;
				}
*/
				
			} // end a switch midi was programmed
		} // end a program midi was requested

		// V1.61 reinit all param and lcd structs already preloaded in RAM
		InitParamStructsInRam();

		// Update serial in index for treated frame (FF SS LL NN GG vv vv .... vv CS FE = 7 bytes + all vv)
		// +1 since end_index is at EOF+2 index
		tmp2 = FRAME_HEADER_LENGHT + tmp3;
	
	} // end frame was control programming request
	else
	/////////////////////////////////// NEW UPLOAD CONTROL RECEIVED /////////////////////////			
	///// 0xFF SYSEX_ID0 0b0lllllll 0b0sssssss 0b0ggggggg 0xFE 
	///// (p=midi/lcd l=lenght=0 s=ctrl g=group x=dont care) 
	//if(SYSEX_ID0_byte & prog_upload_sysxid0)
	{
		// here we received a new sysex control upload request from EZ board
		// get upload buffer size and pointer
		if((SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN)
		{
			// midi struct to upload
			upload_size = MIDI_STRUCT_SIZE;
			local_ptr   = &ParamCurrent->ctrl_status_0;
		}
		else
		{
			// LCD str to upload is placed into midi_string ptr
			upload_size = LCD_STR_SIZE;
			local_ptr   = &ParamCurrent->midi_string[0];
		}

		// will be incrust title
		rom_to_lcd_text_copy(upload_txt);
		
		// Read full structure from ext eeprom
		ReadMultiByteI2c((far uchar *)local_ptr, upload_size);
		
		// if midi struct to upload, reduce buffer size if less than max size
		if((SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN)
			upload_size = MIDI_STRUCT_CONTROL_SIZE + ParamCurrent->midi_str_lenght;

		// Now split all struct bytes into nibbles
		Byte_To_Generic_Nibbles(WID_SYX_UPLOAD_HEADER_SIZE, (uchar)(upload_size<<1), &local_ptr[0]);

		// Now send the full control struc to upload to MIDI outs
		SendUploadGenericBufferToMidi(SYSEX_ID0_byte, upload_size << 1, control_index, current_group);

		// Update serial in index for treated frame (FF SS NN GG LL vv vv .... vv CS FE = 6 bytes + all vv)
		tmp2 = UPLOAD_Z8_FRAME_LENGHT;

	} // end frame was control upload request

	// V1.2 display programmed/uploaded message only if not in a menu
	if(menu_state == NO_MENU)
	{
		// Display prog / upload incrust zone & title
		if(lcd_current_incrust != LCD_INCRUST_PROG_UPLOAD_RESULT)
		{
			DisplayEmptyIncrust();
			// display common parts of prog/upload result
			DisplayBmp(LCD_ZONE_INCRUST_X+10, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, bmp_prg_upld_res);
		}
		// Display current group and index to be programmed/uploaded
		DisplayLcdInt(LCD_ZONE_INCRUST_X+43, LCD_ZONE_INCRUST_Y+2, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, control_index );
		DisplayLcdInt(LCD_ZONE_INCRUST_X+43, LCD_ZONE_INCRUST_Y+3, DIGITS_2|LCD_INVERT|FONT_5x8_FLG/* V1.66 , font_5x8 */, current_group );
		// Display prog type (midi/lcd)
		DisplayBmp(LCD_ZONE_INCRUST_X+43, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, local_bmp_ptr2);
		// Display prog result
		DisplayBmp(LCD_ZONE_INCRUST_X+65, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, local_bmp_ptr);
	
		// Lock LCD incrust zone for 1 second
		lcd_lock_incrust_cpt = CPT_1_SEC;
	}

	// restore modified global vars
	current_group		= group_saved;
    control_index		= control_index_saved;
	last_control_index 	= CTRL_UNDEF;
	lcd_current_incrust = LCD_INCRUST_PROG_UPLOAD_RESULT;

	// Update serial in index for treated frame
	return (tmp2);

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Truncate_Current_Value()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Truncate current_value according to min and max
///////////////////////////////////////////////////////////////////////////////////////
void Truncate_Current_Value(void)
{
	uchar  range;

	range = ParamCurrent->ctrl_value_max - ParamCurrent->ctrl_value_min + 1;

	if(range == 128) return; 

	if(current_value == 0)   current_value = ParamCurrent->ctrl_value_min;
   else
 	if(current_value == 127) current_value = ParamCurrent->ctrl_value_max;
   else
   {
      current_value = (uchar)((((uint)current_value * (uint)(range)) >> (int)7) & 0x7F);
      current_value += ParamCurrent->ctrl_value_min;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Serial_Nibb_To_Generic_Bytes()
// Inputs	: 	None
// Outputs	: 	checksum on all nibbles
// Process  : 	convert nibbles to bytes, place in geneic buffer
///////////////////////////////////////////////////////////////////////////////////////
uchar Serial_Nibb_To_Generic_Bytes(uchar start_index, uchar lenght)
{
	uchar tmp2, tmp4;
	uchar tmp, tmp5;

	tmp  = 0; // used as index
	tmp5 = 0; // used as checksum

	for(tmp4=0; tmp4 < lenght; tmp4++)
	{
		// Unsplit nibble to bytes, treat only even bytes, update checksum
		if(!(tmp4 & 1))
		{
			// get MSB nibble
			tmp2 = serial_in_buffer[(start_index + tmp4) & (SERIAL_IN_BUFF_LENGHT-1)];
			generic_buffer[tmp]  = tmp2 << 4;
			tmp5 = (tmp5 + tmp2) & 127;
			// get LSB nibble
			generic_buffer[tmp] |=  serial_in_buffer[(start_index + tmp4 + 1) & (SERIAL_IN_BUFF_LENGHT-1)];
			tmp5 = (tmp5 + (generic_buffer[tmp] & 0x0F) ) & 127;
			tmp++;
		}
	}

	return(tmp5);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Byte_To_Generic_Nibbles()
// Inputs	: 	
// Outputs	: 	None
// Process  : 	convert nibbles to bytes, place in geneic buffer
///////////////////////////////////////////////////////////////////////////////////////
void Byte_To_Generic_Nibbles(uchar start_index, uchar lenght, far uchar *buff_src)
{
	uchar tmp2, tmp4;

	tmp2=0;
	for(tmp4=0; tmp4 < lenght; tmp4=tmp4+2)
	{
		// create MSB nibble, offset index from header size (f0 20 4f... ) (int cast is a workaround for zdsII v4.8.0)
		generic_buffer[start_index+tmp4]    = buff_src[tmp2] >> (int)4;
		//generic_buffer[start_index+tmp4]    = buff_src[tmp2] >> 4;
		// create LSB nibble
		generic_buffer[start_index+tmp4+1]  = buff_src[tmp2] & 0x0F;
		tmp2++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Check_Sysex_Validity()
// Inputs	: 	
// Outputs	: 	None
// Process  : 	check the received sysex is valid
///////////////////////////////////////////////////////////////////////////////////////
uchar Check_Sysex_Validity(uchar SYSEX_ID0, uchar SYSEX_ID2, uchar SYSEX_ID3)
{
    uchar result = 0;

    ////////////////////////////////////////////////////////////////
    // Invalid request list for MIDI AND LCD CONTROL ASSIGN UPL/DOWN
    ////////////////////////////////////////////////////////////////
    if(  ((SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_LCD_CTRL_ASSIGN  )    ||
         ((SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN ) )
    {
        // 1) Invalid request if group is diffferent than 0 for transport indexes
        if( (SYSEX_ID2 <= FORWARD_CTRL_INDEX) && (SYSEX_ID3) )
             result = INVALID_REQUEST;

        // 2) Invalid request if index number is out of range
        if(SYSEX_ID2 >= CONTROLS_NB)
           	result = INVALID_REQUEST;

        // 3) Invalid request if group number is out of range
        if( SYSEX_ID3 >= GROUP_NB)
             result = INVALID_REQUEST;
    }
    else
    ////////////////////////////////////////////////////////////////
    // Invalid request list for COMMON PARAMETERS UPL/DOWN
    ////////////////////////////////////////////////////////////////
    if( (SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_COMMON_PARAM)
    {
        // 1) Invalid request if program request on the following common params
        if( (!(SYSEX_ID0 & prog_upload_sysxid0))&&(SYSEX_ID2 == IDENTITY_REQUEST_INDEX))
             result = INVALID_REQUEST;

        // 2) Invalid request if common params address is out of range
        if( SYSEX_ID2 > LAST_COMMON_PARAM_INDEX)
			// only identity address 0x7F can be out of range
	  		if(SYSEX_ID2 != IDENTITY_REQUEST_INDEX)
	            result = INVALID_REQUEST;

        // 3) Invalid request if group number is out of range
        if( SYSEX_ID3 >= GROUP_NB)
             result = INVALID_REQUEST;

    }

    return(result);
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendControlInAutotestMode()
// Inputs	: 	
// Outputs	: 	None
// Process  : 	send the current control in autotest mode : small sysex is sent
// 				F0 CONTROL_INDEX VALUE F7 to midi out 2 ONLY
///////////////////////////////////////////////////////////////////////////////////////
void SendControlInAutotestMode(uchar outputs)
{
	// encapsulate into a sysex
	ParamCurrent->midi_string[0] = MIDI_SOX;
	// Now send number
	ParamCurrent->midi_string[1] = control_index; 
	// Now send value
	ParamCurrent->midi_string[2] = current_value & 0x7F;
	// encapsulate into a sysex
	ParamCurrent->midi_string[3] = MIDI_EOX;
	// the whole midi str in autotest mode is a 4 byte sysex 
	ParamCurrent->midi_str_lenght = MIDI_STR_LENGHT_AUTOTEST_MODE;
	// send only to midi out 2
	ParamCurrent->ctrl_status_1	  = outputs; //V1.5 midi_out_2_en;

	SendDispatchToMidi(&ParamCurrent->midi_string[0], MIDI_STR_LENGHT_AUTOTEST_MODE, ParamCurrent->ctrl_status_1);
}

// V1.2
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckMidiStruct()
// Inputs	: 	
// Outputs	: 	MIDI_STRUCT_VALID - MIDI_STRUCT_NOT_VALID
// Process  : 	check and correct the midi struct before sending to MIDI
///////////////////////////////////////////////////////////////////////////////////////
uchar CheckMidiStruct(void)
{
	uchar local_lenght = ParamCurrent->midi_str_lenght;
 
	// V1.4 the value must be splitted, then both values positions must be different than 0
	if(ParamCurrent->ctrl_status_2 & split_ctrl_value)
		if( (ParamCurrent->ctrl_value_position_0 == 0) || (ParamCurrent->ctrl_value_position_1 == 0))
			return (MIDI_STRUCT_NOT_VALID);

	if(local_lenght <= MIDI_STR_MAX_LENGHT)
  	 if(ParamCurrent->midi_channel_position <= local_lenght)
	  if(ParamCurrent->ctrl_value_position_0 <= local_lenght)
	   if(ParamCurrent->ctrl_value_position_1 <= local_lenght)
	    if(ParamCurrent->ctrl_value_min < ParamCurrent->ctrl_value_max)
		   if(ParamCurrent->checksum_start_position < local_lenght) // V1.3
			 return(MIDI_STRUCT_VALID);

	return(MIDI_STRUCT_NOT_VALID);
}


// V1.3
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	InsertChecksumInMidiString()
// Inputs	: 	
// Outputs	: 	
// Process  : 	Calculate and insert checksum in the current MIDI string to send
///////////////////////////////////////////////////////////////////////////////////////
void InsertChecksumInMidiString(void)
{
	uchar calculated_checksum;
	uchar index;

	// calculate checksum from checksum start position
	index = ParamCurrent->checksum_start_position - 1; 
	// to the end of the MIDI string -2 (just before CS F7)
	calculated_checksum = 0;
	while(index < (ParamCurrent->midi_str_lenght - 2))
	{
		calculated_checksum = (calculated_checksum + ParamCurrent->midi_string[index]) & 0x7F;
		index ++;
	}

	// V1.7 : the roland method is used to calculate checksum
	calculated_checksum = (128 - calculated_checksum) & 0x7F;

	// now insert the checksum in the MIDI string (just before F7)
	ParamCurrent->midi_string[ParamCurrent->midi_str_lenght-2] = calculated_checksum;
}


// V1.5
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	ParsePureMidiInBuffer()
// Inputs	: 	
// Outputs	: 	
// Process  : 	Pure MIDI is present in the RX buffer : check arpeggio and mackie mode
///////////////////////////////////////////////////////////////////////////////////////
void ParsePureMidiInBuffer(uchar end_index_local, uchar SYSEX_ID0_byte)
{
	uchar status_received;
	uchar must_check_arpp;
	uchar must_check_mackie;
	uchar must_send_arpp;
	uchar must_check_scene;	// V1.8

/* block removed in v1.8 since scenes can be launched from prog change event
	// If both arpeggio is OFF and Mackie control not selected then ignore MIDI In buffer
	if(!(Z8_STATUS_0_NVM & arpp_state_z0nvm))
		// Arpp is OFF, is the mackie mode selected ?
		//V1.69 if(!(Z8_STATUS_2_NVM & operating_mode1_z2nvm) || (Z8_STATUS_2_NVM & operating_mode0_z2nvm))
		if( (OPERATING_MODE_NVM != MACKIE_MODE) && (OPERATING_MODE_NVM != LOGIC_MODE))
			// both arpp and mackie mode are OFF : exit
		  	return;
*/

// V1.69	must_send_arpp = FALSE;

	// parse all midi buffer
	while(serial_in_start_index != end_index_local)
	{
		// skip frame header FF 02
		if(serial_in_buffer[serial_in_start_index] == SOF_TO_EZ)
			serial_in_start_index = (serial_in_start_index + 2)&(SERIAL_IN_BUFF_LENGHT-1);	

		// Next byte here should be a status MIDI byte : check this
		status_received = serial_in_buffer[serial_in_start_index]; 	
		// exit if not a status (corrupted frame)
		if(!(status_received & MIDI_STATUS_MASK))
			return;

		must_check_arpp   = FALSE;
		must_check_mackie = FALSE;
		must_check_scene  = FALSE;	// V1.8
 
		//////////////////////////////////////////////////////////////////
	 	// Here we have a non corrupted frame, a status is present
		//////////////////////////////////////////////////////////////////
		// Check for status byte : 	
		//		SYSEX 			: check if mackie command
		//		PITCHWHEEL		: check if mackie command
		//		NOTE ON			: check if mackie command and Arpeggio
		//		NOTE OFF		: check Arpeggio
		// 		CC				: Check if mackie command
		//		CHAN PRESSURE 	: Check if mackie command
		// 		PROG CHANGE		: Check if must send scene V1.8
		switch(status_received & 0xF0)
		{
			case MIDI_SOX 			 : 	must_check_mackie = TRUE;
										break;
			case MIDI_PITCH_WHEEL 	 : 	must_check_mackie = TRUE;
										break;
			case MIDI_NOTE_ON 		 : 	must_check_mackie = TRUE;
										must_check_arpp   = TRUE;
										break;
			case MIDI_NOTE_OFF 		 : 	must_check_mackie = TRUE;
										must_check_arpp   = TRUE;
										break;
			case MIDI_CONTROL_CHANGE : 	must_check_mackie = TRUE;
										break;
			case MIDI_CHANN_PRESSURE : 	must_check_mackie = TRUE;
										break;
			case MIDI_PROGRAM_CHANGE : 	must_check_scene  = TRUE;
										break;

		} // end switch status received

		// V1.5 : Check arpeggiator only if it is enabled and MIDI has been received on its selected source
		if(must_check_arpp)
		if(Z8_STATUS_0_NVM & arpp_state_z0nvm)
		{
			must_send_arpp = FALSE;

			// Arpp is On and a midi Note has been received : was it from correponding arpp midi source ?
			if(SYSEX_ID0_byte & pure_midi_from_usb_sysxid0)
			{
				// we are treating the usb in buffer here
				if(CTRL_REG_1_7BITS_TO_EZ & arpp_midi_src_z_ez1)
				{
					//v1.1 must_send_midi_to_z8 = TRUE;
					must_send_arpp = TRUE;
				}
			}
			else
			{
				// we are treating the midi in buffer here
				if(!(CTRL_REG_1_7BITS_TO_EZ & arpp_midi_src_z_ez1))
				{
					//v1.1 must_send_midi_to_z8 = TRUE;
					must_send_arpp = TRUE;
				}
			}

			if(must_send_arpp)
				CheckArpeggiator(status_received);
		}


		// V1.5 : Check if midi contains mackie control actions
		// Are we in mackie control operating mode ?
		if(must_check_mackie)
		// V1.69 if(Z8_STATUS_2_NVM & operating_mode1_z2nvm)
		// V1.69 if(!(Z8_STATUS_2_NVM & operating_mode0_z2nvm))
		if( (OPERATING_MODE_NVM == MACKIE_MODE) ||
			(OPERATING_MODE_NVM == LOGIC_MODE))
		{
			// Mackie control : check if the sysex or cc or note a mackie control command/action
			CheckIfRxMackieCommand(status_received);
		}

		// V1.8 : scenes can now be launched from program change events
		if(must_check_scene)
		// are the scenes cpnfigured to be sent from a program change ?
		if(Z8_STATUS_3_NVM & send_scene_from_prg_chg_z3nvm)
		{
			// the new scene is now indicated by the prog change nb
			current_scene = serial_in_buffer[(serial_in_start_index + 1)&(SERIAL_IN_BUFF_LENGHT-1)]; 
			// flag the scene send since received a program change
			Z8_STATUS_0 |=  snapshot_changed_z0;
			// flag the scene must be sent (as opposed to saved)
			Z8_STATUS_0 &= ~save_zsend_snapshot_z0;
			// force scene snapshot to send (maybe scenes arpp are selected)
			Z8_STATUS_11 |= send_scene_from_prog_ch_z11;
			// display the new scene sent
			// UpdateLcdZones(INIT_LCD_SCENE | INIT_LCD_FUNCTION);
			// DisplayLcdInt(LCD_ZONE_SCENE_X+3, LCD_ZONE_SCENE_Y+1, DIGITS_2|LCD_INVERT|FONT_5x8_FLG, current_scene);
		}


		// pass to next message
		serial_in_start_index = (serial_in_start_index + Get_MIDI_Msg_Size(status_received) + 1) & (SERIAL_IN_BUFF_LENGHT-1);
		// skip EOF if present
		if(serial_in_buffer[serial_in_start_index] == EOF_TO_EZ)
			serial_in_start_index = (serial_in_start_index + 1)&(SERIAL_IN_BUFF_LENGHT-1);	

	} // end while
}

// ***************************************************************************
// Routine	: Get_MIDI_Msg_Size()
// Input	: current MIDI status 
// Output	: number of expected bytes following the status 
// Process	: From V1.5: this function also returns Sysex lenght present in the buffer
// ***************************************************************************
//static 
unsigned char Get_MIDI_Msg_Size(unsigned char status)
{
	uchar start_index_local; // V1.5
	uchar sysex_lenght;

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
	// V1.5	return(0xFF);	
	{
		// V1.5 : return real sysex lenght (search for EOX in the buffer)
		sysex_lenght = 0;
		start_index_local = serial_in_start_index;
		while(serial_in_buffer[start_index_local] != MIDI_EOX)
		{
			sysex_lenght++;
			// if EOX is not in the buffer then the buffer is corrupted
			if(sysex_lenght == SERIAL_IN_BUFF_LENGHT) return(0xFF);
			start_index_local = (start_index_local + 1)&(SERIAL_IN_BUFF_LENGHT-1);	
		}
		return(sysex_lenght);
	}
   else
		return(0); // eox
}

// V1.5
// ***************************************************************************
// Routine	: CheckIfEzMustForwardMidiToZ8()
// Input	: None
// Output	: 
// Process	: Z8 may indicates to EZ that EZ must formward all MIDI events
//			  Only update flag, regsiter will be sent later to EZ
// ***************************************************************************
void CheckIfEzMustForwardMidiToZ8(void)
{
	// if arppegio is ON or mackie mode is active or Scenes launchable from rx prog change
	// then Z8 must receive all incoming MIDI events
	if( (Z8_STATUS_3_NVM & send_scene_from_prg_chg_z3nvm) || // V1.8
		(Z8_STATUS_0_NVM & arpp_state_z0nvm) || 
	//V1.69 	((Z8_STATUS_2_NVM & operating_mode1_z2nvm) && (!(Z8_STATUS_2_NVM & operating_mode0_z2nvm)) ))
		((OPERATING_MODE_NVM /* & OPERATING_MODE_MSK_Z3_NVM */) == MACKIE_MODE) ||
		((OPERATING_MODE_NVM /* & OPERATING_MODE_MSK_Z3_NVM */) == LOGIC_MODE))
			CTRL_REG_1_7BITS_TO_EZ |= ez_fw_midi_to_z8_z_ez1;
	else
			CTRL_REG_1_7BITS_TO_EZ &= ~ez_fw_midi_to_z8_z_ez1;
}


// V1.61
// ***************************************************************************
// Routine	: FillParamCurrentStruct()
// Input	: None
// Output	: 
// Process	: Check if current param struct to send is already present in the 
// param struct located in RAM or if we need to load it from external eeprom
// ***************************************************************************
void FillParamCurrentStruct(void)
{
	uchar i;
	uchar tmp;

	// check if current control index param struct is already in ram ?
	if(control_index != CTRL_UNDEF)
	{
		for(i=0; i<PARAM_STRUCT_IN_RAM_MAX_NB; i++)
		{
			if(control_index  == control_index_structs_in_ram[i])
			{
				// set current struct pointer to current control index
				ParamCurrent = &Param[i];
				// return since control index found in table
				return;
			}
		}
	}

	// HERE NO CONTROL INDEX FOUND IN RAM (otherwise the above retrun exits from this function

	// increase index on control indexes located in table	
	last_struct_index++;
	if(last_struct_index >= PARAM_STRUCT_IN_RAM_MAX_NB) 
		last_struct_index = 0;

	// add current struct for current control index into the table
	control_index_structs_in_ram[last_struct_index] = control_index;

	// get control EEprom address and I2cCommand
	GetCtrlStructEEaddFromIndexGroup();
	
	// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
	// CheckRealtimeMidi();

	// depending on mode load only header of full struct
	//V1.69 if( (Z8_STATUS_2_NVM & operating_mode1_z2nvm)/* || (assign_state == ASSIGN_ACTIVE)*/) // If mackie or user mode or assign active
	if(OPERATING_MODE_NVM >= MACKIE_MODE)
		 tmp = MIDI_STRUCT_SIZE; 			// usr mode : fill all struct (32 bytes)
	else tmp = MIDI_STRUCT_HEADER_SIZE; 	// std mode : fill only status 0 & 1 (2 bytes)
	// Read struct full or status only from eeprom 
	ReadMultiByteI2c((far uchar *)&Param[last_struct_index].ctrl_status_0, tmp);
	// set current struct pointer to current control index
	ParamCurrent = &Param[last_struct_index];

}


// V1.61
// ***************************************************************************
// Routine	: FillParamCurrentStruct()
// Input	: None
// Output	: 
// Process	: Check if current param struct to send is already present in the 
// param struct located in RAM or if we need to load it from external eeprom
// ***************************************************************************
void FillLcdCurrentStruct(void)
{
	uchar i, j;

	// check if current control index param struct is already in ram ?
	if(control_index != CTRL_UNDEF)
	{
		for(i=0; i<PARAM_STRUCT_IN_RAM_MAX_NB; i++)
		{
			if(control_index  == control_index_lcd_structs_in_ram[i])
			{
				// copy current lcd struct to buffer to display
				for(j=0; j<LCD_STR_SIZE; j++)
					lcd_text_value[j]=lcd_strings_in_ram[i][j];
				lcd_text_value[LCD_STR_SIZE]='\0';
				// return since control index found in table
				return;
			}
		}
	}

	// HERE NO CONTROL INDEX FOUND IN RAM (otherwise the above retrun exits from this function

	// increase index on control indexes located in table	
	last_lcd_struct_index++;
	if(last_lcd_struct_index >= PARAM_STRUCT_IN_RAM_MAX_NB) 
		last_lcd_struct_index = 0;

	// add current struct for current control index into the table
	control_index_lcd_structs_in_ram[last_lcd_struct_index] = control_index;

	// get control EEprom address and I2cCommand
	GetLcdStrEEaddFromIndexGroup();

	// Read lcd string from eeprom 
	ReadMultiByteI2c(&lcd_strings_in_ram[last_lcd_struct_index][0], LCD_STR_SIZE);
	
	for(j=0; j<LCD_STR_SIZE; j++)
		lcd_text_value[j]=lcd_strings_in_ram[last_lcd_struct_index][j];
	lcd_text_value[LCD_STR_SIZE]='\0';
}

