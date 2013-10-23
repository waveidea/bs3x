//#pragma NOIV	// Do not generate interrupt vectors
 
#include <ezusb.h>
#include <ezregs.h>
#include "common_def.h"
#include <common_z8_ez.h>
#include "pins_def.h"
#include "periph.h"
#include "bootloader.h"
#include "serial.h"
#include "hardware.h"
#include "usb.h"

volatile BYTE bootloader_state;
bit must_program_z8;

// BYTE I2C_Addr;

// FROM MAIN.C
extern bit usb_state_from_ez0_bit;
extern bit z8_board_fully_updated_from_ez0_bit;
extern xdata Midi_In_Struct  MIDI_In_From_Con, MIDI_In_From_Usb, Serial_In_From_Z8;
//BYTE fw_checksum;
BYTE fw_bl_src;

// FROM IDENTITY.C
extern unsigned char code identity[];

// FROM SERIAL.C
//extern unsigned char code wid_sysex_header[];

// V1.4 FROM SERIAL.C
extern bit tx_to_serial_z8_busy;


/* ******************************************************************** */
/* Routine: SoftReset()													*/
/* Process: 															*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
//void SoftReset(void)
//{
//	((void (code *) (void)) 0x0000) ();
//}

/* ******************************************************************** */
/* Routine: Check_Bootloader											*/
/* Process: 															*/
/* Inputs:	None														*/
/* Outputs:	None														*/
/* ******************************************************************** */
void Check_Bootloader(void)
{
		// reinit all chip
		TD_Init();
		// just to allow send/receive from midi and usb inputs
		z8_board_fully_updated_from_ez0_bit = 1;
		// Init i2c bus for eeprom ez upgrade
		//EZUSB_InitI2C();
		// update bootloading flags and state
		bootloader_state = SEND_IDENTITY_SYSEX; 		

		// First send the Bootloader identity sysex acknoledge to midi & usb out
		SendIdentityBootLoader();

		must_program_z8 = FALSE;

		// The data firmware upgrade starts here
		bootloader_state = WAIT_EZ_FW_DATA;				
		// Now Wait for the ez firmware data sysexs
		WaitNextBootLoaderState(bootloader_state);

		if(must_program_z8)
		{
			// Release Z8 board reset line
			Z8_RESET_PORT_OUT |=  Z8_RESET_PIN;
			// wait por
			EZUSB_Delay(200);

			// disable rx1
			SCON1 = 0x40;		/* SCON1: mode 1, 8-bit UART, disable rcvr  */

			// tx1 is now dedicated to z8 dbg pin
			OCD_EN_PORT_OUT |= OCD_EN_PIN;

			// set z8 dbg pin low
			TX1_TO_Z8_DBG_PORT_OUT &= ~TX1_TO_Z8_DBG_PIN;

			// wait 2sec
			EZUSB_Delay(2000);

			// set z8 dbg pin high
			TX1_TO_Z8_DBG_PORT_OUT |= TX1_TO_Z8_DBG_PIN;
			
			// turn on tx1 and rx1 (serial to/from z8)
			PORTBCFG |= 0x0C;

			// send ocd first byte so the z8 autobaudrate the serial line
			Send_To_Serial_Z8(OCD_FIRST_BYTE);
			EZUSB_Delay1ms();

			// place the z8 chip in ocd mode
			Send_To_Serial_Z8(OCD_WRITE_CTR_REG_CMD);
			Send_To_Serial_Z8(OCD_CTR_REG_DBGMODE);
			EZUSB_Delay(100);

			InitFlashFrqAndUnlock();

			// selet flash page 0
			WriteSingleRegOCD(OCD_FLASH_SELECT_PAGE, 0x00);

			// launch a mass erase
			WriteSingleRegOCD(OCD_FLASH_CTR_REG_ADD, OCD_FLASH_MASS_ERASE);
			
			EZUSB_Delay(Z8_ERASING_TIME);

			InitFlashFrqAndUnlock();
		
			// Now Wait for the z8 firmware data sysexs
			WaitNextBootLoaderState(bootloader_state);

			EZUSB_Delay(50);

			// relock the flash controller
			WriteSingleRegOCD(OCD_FLASH_CTR_REG_ADD, OCD_FLASH_CTR_REG_RESET);
			
			/*
			///////////////////// Z8 CRC CHECKS /////////////////////////////////
			// V1.4 it is now time to verify Z8 programmed flash 
			bootloader_state = VERIFY_Z8_CRC;	
			// V1.4 Read the Z8 memory CRC and compare to calculated CRC
			Send_To_Serial_Z8(OCD_READ_FLASH_CRC);
			// V1.4 required time is 65536 * 1/(20MHz)
			EZUSB_Delay(50);
			// V1.4 the serial port should now have received the z8 internal CRC in buffer 0 to 3
			// Send this CRC to host
			Serial_In_From_Z8.buffer[7]   = Serial_In_From_Z8.buffer[0];
			Serial_In_From_Z8.buffer[8]   = Serial_In_From_Z8.buffer[1];  			
			Serial_In_From_Z8.end_index   = WID_SYX_HEADER_SIZE + 3 + 1;
			AssignWidSysexHeader();
			// send ack firmware 
  			Serial_In_From_Z8.buffer[6]   = FRAME_IS_FIRMWARE_OR_PURE_MIDI | sysex_direction_sysxid0 | prog_upload_sysxid0;
  			// send eox
 			Serial_In_From_Z8.buffer[9]  = MIDI_EOX;
			SendDispatchFwSysex();
			///////////////////// END Z8 CRC CHECKS /////////////////////////////////
			*/
						
			// exit z8 ocd mode
			OCD_EN_PORT_OUT   &=  ~OCD_EN_PIN;

			// exit z8 ocd mode
			Z8_RESET_PORT_OUT &=  ~Z8_RESET_PIN;
		}

	// Now finish with a soft reset
	// SoftReset();
	while(1)
	{ 
		Check_Usb();
		Ep2out_Function();
	}
}

// ***************************************************************************
// Routine	: SendIdentityBootLoader()
// Input	: 
// Output	: None
// Process	: send all 11 identity bytes to usb & midi out 1, 
//            we dont know yet what is the bootloader midi source : use the Z8 input buffer 
// ***************************************************************************
void SendIdentityBootLoader(void)
{
  unsigned char i;
 
  //Serial_In_From_Z8.start_index = 0;
  Serial_In_From_Z8.end_index   = WID_SYX_HEADER_SIZE + 4 + IDENTITY_LENGHT + 1;
 
  AssignWidSysexHeader();

  // send identity common param id and address
  Serial_In_From_Z8.buffer[6]   = FRAME_IS_COMMON_PARAM | sysex_direction_sysxid0 | prog_upload_sysxid0;

  // Send length common param address
  Serial_In_From_Z8.buffer[7]   = IDENTITY_LENGHT;
  // Send IDENTITY common param address  
  Serial_In_From_Z8.buffer[8]   = IDENTITY_REQUEST_INDEX;
  // Send group : dummy byte here  
  Serial_In_From_Z8.buffer[9]   = 0;
  // send identity bytes
  for(i=0; i<IDENTITY_LENGHT ; i++)
	Serial_In_From_Z8.buffer[10+i] = identity[i];
  // send eox
  Serial_In_From_Z8.buffer[21]   = MIDI_EOX;
  
  SendDispatchFwSysex();
}

// ***************************************************************************
// Routine	: SendFwAcknoledge()
// Input	: 
// Output	: None
// Process	: send single byte ack to usb & midi out 1, use the Z8 input buffer 
// ***************************************************************************
void SendFwAcknoledge(unsigned char result)
{

  //Serial_In_From_Z8.start_index = 0;
  Serial_In_From_Z8.end_index   = WID_SYX_HEADER_SIZE + 1 + 1;
  
  AssignWidSysexHeader();

  // send ack firmware 
  Serial_In_From_Z8.buffer[6]   = /*v1.1 FRAME_IS_FIRMWARE*/FRAME_IS_FIRMWARE_OR_PURE_MIDI | sysex_direction_sysxid0 | prog_upload_sysxid0 | result;
  
  // send eox
  Serial_In_From_Z8.buffer[7]   = MIDI_EOX;

  SendDispatchFwSysex();
  
}

// ***************************************************************************
// Routine	: AssignWidSysexHeader()
// Input	: 
// Output	: None
// Process	: 
// ***************************************************************************
void AssignWidSysexHeader(void)
{
//	BYTE i;

	// send wid sysex header  
	Serial_In_From_Z8.buffer[0]   = MIDI_SOX;
	Serial_In_From_Z8.buffer[1]   = WID_SYX_ID0;
	Serial_In_From_Z8.buffer[2]   = WID_SYX_ID1;
	Serial_In_From_Z8.buffer[3]   = WID_SYX_ID2;
	Serial_In_From_Z8.buffer[4]   = WID_SYX_ID3;
	Serial_In_From_Z8.buffer[5]   = WID_SYX_ID4;

//  	for(i=0; i<WID_SYX_HEADER_SIZE ; i++)
//		Serial_In_From_Z8.buffer[i] = wid_sysex_header[i];

}

// ***************************************************************************
// Routine	: SendDispatchFwSysex()
// Input	: 
// Output	: None
// Process	: optimize by grouping with same fct from main.c
// ***************************************************************************
void SendDispatchFwSysex(void)
{
	// BYTE start_index_local = Serial_In_From_Z8.start_index;
	BYTE start_index_local = 0;

  	// Send all bytes to MIDI Out Connector
  	if(fw_bl_src != FW_BL_SRC_USB_IN)
	{
  		while (start_index_local != Serial_In_From_Z8.end_index)
	  	{
			Send_To_Midi_Out_Con(Serial_In_From_Z8.buffer[start_index_local]);
			start_index_local++; // no need to modulo since always start from 0
	  	}
  	}

	// Also send to usb if present
	if(fw_bl_src  != FW_BL_SRC_MIDI_IN)
	if(usb_state_from_ez0_bit)
	{
		Serial_In_From_Z8.start_index 	= 0;
		// Send all bytes to USB Out 
		Create_Send_USB_Frame_From_MIDI_Buffer(CN0, &Serial_In_From_Z8, Serial_In_From_Z8.end_index);
  	}

	// V1.4
 	Serial_In_From_Z8.start_index 		= 0;
	Serial_In_From_Z8.end_index   		= 0;
	Serial_In_From_Z8.current_index		= 0;
}

// ***************************************************************************
// Routine	: WaitNextBootLoaderState()
// Input	: 
// Output	: None
// Process	: loop until received sysex is the one awaited
// ***************************************************************************
void WaitNextBootLoaderState(unsigned char current_state)
{
	// Loop until receiving expected firmware sysex 
	do
 	{
		Check_If_Usb_Attached();		
		Check_Usb();

		// bootloader source not yet known or midi in ?
		if(fw_bl_src != FW_BL_SRC_USB_IN)
			// check if awaited sysex received on midi in
			Check_MIDI_Buff_Send_To_Z8_Board(&MIDI_In_From_Con, MIDI_In_From_Con.end_index);
		
		// bootloader source not yet known or usb in ?
		if(fw_bl_src != FW_BL_SRC_MIDI_IN)	
		{
			// check if awaited sysex received on usb in
			if(usb_state_from_ez0_bit)
			{
				Check_MIDI_Buff_Send_To_Z8_Board(&MIDI_In_From_Usb, MIDI_In_From_Usb.end_index);
				Ep2out_Function();
			}
		}
	}
	while(bootloader_state == current_state);
}

// ***************************************************************************
// Routine	: DecodeFirmwareSysex()
// Input	: 
// Output	: must send ack bool
// Process	: decode the sysex dedicated to firmware
// ***************************************************************************
void DecodeFirmwareSysex(Midi_In_Struct *midi_struct)
{
	BYTE SYSXID0;
	WORD Address;
	BYTE i, j, tmp1, tmp2, tmp3, lenght_div_2;
	BYTE result = 0;
	bit must_send_ack = FALSE;
	BYTE fw_checksum = 0;

	SYSXID0 = midi_struct->buffer[6];

	// check the received sysex is dedicated to firmware otherwise return
	// also check it is a programming sysex : ack request must be set for firmware programming
	if((SYSXID0 & (FRAME_TO_Z8_TYPE_MSK | acknoledge_sysxid0)) == (/*v1.1 FRAME_IS_FIRMWARE*/FRAME_IS_FIRMWARE_OR_PURE_MIDI | acknoledge_sysxid0))
	{
		must_send_ack = TRUE;

	  	// must finish firmware upgrade ?
		if(SYSXID0 & fw_znext_finish_sysxid0)
		{
			// bootloader_state = TERMINATE_FW_UPGRADE;
			bootloader_state++;
			result = acknoledge_sysxid0;
			// pass to z8 programming ?
			if(SYSXID0 & prog_ez_z8_sysxid0)
			{
				must_program_z8 = TRUE;
				// reset crc z8
				// z8_flash_crc = 0;
			}
		}
		else
		{
			// here we have to program EZ or Z8 firmware data
			// extract address from sysex
			Address  =  midi_struct->buffer[8]  << 14;
			Address += (midi_struct->buffer[9]  << 7);
			Address += (midi_struct->buffer[10]);

			// un nibble data, decode it, calculate checksum
			fw_checksum = 0;
			j= FW_SYSX_DATA_START;
			for(i=0; i < midi_struct->buffer[7]; i=i+2)
			{
				tmp1 = midi_struct->buffer[FW_SYSX_DATA_START+i];
				tmp2 = midi_struct->buffer[FW_SYSX_DATA_START+i+1];
				fw_checksum = (fw_checksum + tmp1) & 0x7F;
				fw_checksum = (fw_checksum + tmp2) & 0x7F;
				midi_struct->buffer[j] = ( (tmp1 << 4) + (tmp2 & 0x0F)) ^ 0x55;
				j++;
			}

			// compare checksum
			if(fw_checksum == midi_struct->buffer[FW_SYSX_DATA_START+i])
			{
				lenght_div_2 = midi_struct->buffer[7] >> 1;
				
				// NEED TO PROGRAM EZ USB EEPROM ?
				if(!(SYSXID0 & prog_ez_z8_sysxid0))
				{
					if(Address < EE24C64_SIZE)
						//if(!(LSB(Address) % 32))
						{
							EZUSB_IRQ_DISABLE();
							CKCON |= 0x07; // Set stretch to Non zero when accessing I2C (insert 7 wait states)
							// write and verify eeprom result=0 if error
		 					result = EEPROMWriteVerify(Address, lenght_div_2, &midi_struct->buffer[FW_SYSX_DATA_START]);
							CKCON &= 0xF8; // Set stretch to 0 after i2c access (required if using paired endpoints : see chip errata)
							EZUSB_IRQ_ENABLE();
						}
				}
				// OR NEED TO PROGRAM Z8 ?
				else
				{
					// calculated checksum is ok to start
					result = acknoledge_sysxid0;		

					// ///////////////// PROGRAM Z8 /////////////////
					// Send write command
					Send_To_Serial_Z8(OCD_WRITE_PRG_MEM_CMD);
					// Send address msb to write
					Send_To_Serial_Z8(Address >> 8);
					// Send address msb to write
					Send_To_Serial_Z8(Address & 0xFF);
					// Send data_lenght msb to write
					Send_To_Serial_Z8(0x00);
					// Send data_lenght lsb to write
					Send_To_Serial_Z8(lenght_div_2);
					// Now send datas
					for(i=0; i < lenght_div_2; i++)
					{
						Send_To_Serial_Z8(midi_struct->buffer[FW_SYSX_DATA_START+i]);
					}		

					// V1.4 Wait for the transmit buffer being empty (everything sent to Z8)
					while(tx_to_serial_z8_busy);

					// wait raw time write
					EZUSB_Delay(50); // V1.4   10

					// V1.4 ///////////////// READ BACK Z8 AND CHECK IF SAME /////////////////
					// Send write command
					Send_To_Serial_Z8(OCD_READ_PRG_MEM_CMD);
					// Send address msb to write
					Send_To_Serial_Z8(Address >> 8);
					// Send address msb to write
					Send_To_Serial_Z8(Address & 0xFF);
					// Send data_lenght msb to write
					Send_To_Serial_Z8(0x00);
					// Send data_lenght lsb to write
					Send_To_Serial_Z8(lenght_div_2);

					// V1.4 Wait for the transmit buffer being empty (everything sent to Z8)
					while(tx_to_serial_z8_busy);

					// V1.4 enable rx1 for reading back datas from z8
					SCON1 |= 0x50;		/* SCON1: mode 1, 8-bit UART, enable rcvr  */
					
					// V1.4 wait for all data being received from Z8
					while(Serial_In_From_Z8.current_index < lenght_div_2);

//		EZUSB_Delay(50);
//		Send_To_Midi_Out_Con(0xF0);
//		Send_To_Midi_Out_Con(lenght_div_2);
//		Send_To_Midi_Out_Con(Serial_In_From_Z8.start_index);
//		Send_To_Midi_Out_Con(Serial_In_From_Z8.current_index);
//		Send_To_Midi_Out_Con(Serial_In_From_Z8.end_index);
//		Send_To_Midi_Out_Con(00);
//		Send_To_Midi_Out_Con(00);		
//		Send_To_Midi_Out_Con(0xF7);

					// V1.4 Now read datas from serial_in_from_z8 input buffer
					for(i=0; i < lenght_div_2; i++)
					{
						// read data received from serial port buffer
						tmp3 = Serial_In_From_Z8.buffer[i];

//		Send_To_Midi_Out_Con(0xF0);
//		Send_To_Midi_Out_Con(midi_struct->buffer[FW_SYSX_DATA_START+i] >> 4);
//		Send_To_Midi_Out_Con(midi_struct->buffer[FW_SYSX_DATA_START+i] & 0x0F);
//		Send_To_Midi_Out_Con(tmp3 >> 4);
//		Send_To_Midi_Out_Con(tmp3 & 0x0F);	
//		Send_To_Midi_Out_Con(0xF7);
					
						// compare data read back with data written
						if(tmp3 != midi_struct->buffer[FW_SYSX_DATA_START+i])
						{
							// a byte doesnt correspond to what was programmed : send ack error
							result = 0;
							break;
						}
					}	
	
					// V1.4 disable rx1
					SCON1 &= 0xEF;		/* SCON1: mode 1, 8-bit UART, disable rcvr  */

				} // end prog z8

				// 10ms delay after ez or z8 programming
				//EZUSB_Delay(10);			

		   	} // end checksum ok
		} // end program instead of terminate
	} // end it was a good fw sysex

	// the whole sysex has now been treated
	// no circular buffer while upgrading firmware
//	midi_struct->start_index 	= 0;
//	midi_struct->end_index	 	= 0;
//	midi_struct->current_index	= 0;
	
	// Send acknoledge to host
	if(must_send_ack)
		SendFwAcknoledge(result);	

}

// ***************************************************************************
// Routine	: EEPROMWriteVerify()
// Input	: 
// Output	: None
// Process	: 
// ***************************************************************************
BYTE EEPROMWriteVerify(WORD addr, BYTE length, BYTE xdata *buf)
{
	BYTE 		j;
	BYTE xdata 	ee_str[3];
	BYTE xdata  data_to_verify;
  	
	/*
	buf[0] = MSB(addr);	// address msb
	buf[1] = LSB(addr);	// address msb
	// all other datas are already in the buffer
    EZUSB_WriteI2C(I2C_Addr, length, buf);
	// now wait write complete
	//EZUSB_Delay(12);
	EZUSB_WaitForEEPROMWrite(I2C_Addr);
	//EZUSB_Delay(2);
	// re-set eeprom adress since it has been incremented
	EZUSB_WriteI2C(I2C_Addr, 2, buf);
//	EZUSB_WaitForEEPROMWrite(I2C_Addr);
	// verify written datas 
	for(j=0;j<length;j++)
	{
		// read back data written
		EZUSB_ReadI2C(I2C_Addr, 0x01, &data_to_verify); 
		// check if equal to data written before
		if(data_to_verify != buf[j+2])
			return(0);
		//addr++;
	}
	// verif ok
	return(acknoledge_sysxid0);
*/		
	for(j=0;j<length;++j)
	{
		ee_str[0] = MSB(addr);	// address msb
		ee_str[1] = LSB(addr);	// address lsb
		ee_str[2] = buf[j];		// byte to write

		EZUSB_WriteI2C(I2C_Addr, 3, ee_str);
		// wait 1ms before pooling write finish
		//EZUSB_Delay(5);
		//Check_Usb();
		// now wait write complete
		EZUSB_WaitForEEPROMWrite(I2C_Addr);
		// wait 1ms before re-reading data		
		//EZUSB_Delay(8);
		// re-set eeprom adress since it has been incremented
		EZUSB_WriteI2C(I2C_Addr, 2, ee_str);
		// read back data written
		EZUSB_ReadI2C(I2C_Addr, 0x01, &data_to_verify); 

		// check if equal to data written before
		if(data_to_verify != buf[j])
			return(0);
		addr++;
	}
	// verif ok
	return(acknoledge_sysxid0);

}

// ***************************************************************************
// Routine	: WriteSingleRegOCD()
// Input	: 
// Output	: None
// Process	: 
// ***************************************************************************
void WriteSingleRegOCD(WORD address, BYTE value) 
{
	Send_To_Serial_Z8(OCD_WRITE_REG_CMD);
	Send_To_Serial_Z8(address >> 8);
	Send_To_Serial_Z8(address & 0xFF);
	Send_To_Serial_Z8(1);
	Send_To_Serial_Z8(value);
	EZUSB_Delay(50);
}

// ***************************************************************************
// Routine	: InitFlashFrqAndUnlock()
// Input	: 
// Output	: None
// Process	: 
// ***************************************************************************
void InitFlashFrqAndUnlock(void)
{
	// rst reg
	WriteSingleRegOCD(OCD_FLASH_CTR_REG_ADD, OCD_FLASH_CTR_REG_RESET);
 	// Write flash frequency control regs high & low					
	WriteSingleRegOCD(OCD_FLASH_FREQ_REG_HIGH_ADD, OCD_Z8_FLASH_FREQ_REG >> 8);
	WriteSingleRegOCD(OCD_FLASH_FREQ_REG_LOW_ADD , OCD_Z8_FLASH_FREQ_REG & 0xFF);
	EZUSB_Delay(50);
	// unlock flash reg
	WriteSingleRegOCD(OCD_FLASH_CTR_REG_ADD, OCD_FLASH_FIRST_UNLOCK);
	WriteSingleRegOCD(OCD_FLASH_CTR_REG_ADD, OCD_FLASH_SECOND_UNLOCK);
	EZUSB_Delay(50);
}
