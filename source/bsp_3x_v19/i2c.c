#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "eeprom.h"
#include "i2c_low_level.h"
#include "i2c.h"
#include "realtime.h" // V1.9

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uint  EE_Address;
uchar I2cCommand;

//extern uchar Z8_STATUS_10;
extern volatile uchar Z8_STATUS_4;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ReadByteI2c()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
uchar ReadByteI2c(void) 
{
	uchar data;
	
//	AckPolling();
	// wait for EEProm ready 
	while(Z8_STATUS_4 & eeprom_locked_10ms_z4);
		// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
		//CheckRealtimeMidi();

	MAC_I2cStart 								// I2C Start
	MAC_I2cSendNack								// Set the No Acknowledge bit
	MAC_I2cWriteByte((I2C_WRITE_COMMAND + I2cCommand))	// EEProm Address + Write command					
  	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
	MAC_I2cWriteByte((uchar)(EE_Address>>8))	// Load MSB EEProm Address					
	WaitI2cAcknoledge();						// wait for ACK from EEProm (EE_WRITE_CTRL)
	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
	MAC_I2cWriteByte((uchar)(EE_Address & 0xFF))// Load LSB EEProm Address
  	WaitI2cAcknoledge();						// wait for ACK from EEProm (Address high)
	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
  
   	MAC_I2cStart								// I2C Start
  	WaitI2cAcknoledge();						// wait for ACK from EEProm (Address low)
	MAC_I2cWriteByte((I2C_READ_COMMAND + I2cCommand))	// EEProm Address; Read	
	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
 	WaitI2cAcknoledge();						// wait for ACK from EEProm (EE_READ_CTRL)
 	WaitI2cFullDataReceived();					// wait for receive data
	MAC_I2cReadByte(data)						// EEProm Read data
	I2cStop();                               	// I2C Stop
	return data;		

} 

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WriteByteI2c()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void WriteByteI2c(uchar data)
{
//	uchar result = I2C_NO_TIMEOUT;

	// Initiate EEProm write sequence
	InitiateI2cWrite();

	MAC_I2cWriteByte(data)						// EEProm Write data
	WaitI2cAcknoledge();				// wait for ACK from EEProm (Address low)
	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
	WaitI2cAcknoledge();						// wait for ACK from EEProm (data written)
	I2cStop();									// I2C Stop
	LockEEprom10Ms();

//	return result;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WriteMultiByteI2c()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void WriteMultiByteI2c(far uchar *data_ptr, uchar data_lenght)
{
	uchar i;

	// Initiate EEProm write sequence
	InitiateI2cWrite();

   	WaitI2cAcknoledge();					// wait for ACK from EEProm (Address low)

	for (i=0; i < data_lenght; i++)
	{		
	 	MAC_I2cWriteByte(data_ptr[i]);   	// EEProm Write data	
		WaitI2cTransmitEmpty();				// wait for transmit buffer empty
		WaitI2cAcknoledge();			    // wait for ACK from EEPROM	(data written)
    }
	I2cStop();								// I2C Stop
	LockEEprom10Ms();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitiateI2cWrite()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void  InitiateI2cWrite(void)
{
//	AckPolling();
	// wait for EEProm ready 
	while(Z8_STATUS_4 & eeprom_locked_10ms_z4);
		// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
		//CheckRealtimeMidi();


  	MAC_I2cStart								// I2C Start
  	MAC_I2cWriteByte((I2C_WRITE_COMMAND + I2cCommand))	// I2C EEProm Address; Write
  	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
   	MAC_I2cWriteByte((uchar)(EE_Address>>8))			// Load MSB EEProm Address					
	WaitI2cAcknoledge();						// wait for ACK from EEProm (EE_WRITE_CTRL)
   	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
  	MAC_I2cWriteByte((uchar)(EE_Address & 0xFF))		// Load LSB EEProm Address
	WaitI2cAcknoledge();						// wait for ACK from EEProm (Address High)
  	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ReadMultiByteI2c()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
//void ReadMultiByteEEprom(uchar *data_ptr, uchar data_lenght)
void ReadMultiByteI2c(far uchar *data_ptr, uchar data_lenght)
{
	uchar index, data_read;
	uchar last_byte = data_lenght-1;

	// Initiate EEProm write sequence
	InitiateI2cWrite();

   	WaitI2cAcknoledge();						// wait for ACK from EEProm (Address low)
   
	MAC_I2cStart	
  	MAC_I2cWriteByte((I2C_READ_COMMAND + I2cCommand))	// EEProm Address; Read	
	WaitI2cTransmitEmpty();						// wait for transmit buffer empty   
	WaitI2cAcknoledge();						// wait for ACK from EEProm (EE_READ_CTRL)
		
	for(index=0;index < data_lenght ;index++)
	{
		if(index == last_byte)
		{
			MAC_I2cSendNack						// Send No ack on last byte to read
     	}
		WaitI2cFullDataReceived();
		MAC_I2cReadByte(data_read)				// EEProm Read data		
		//V1.69 data_ptr[index] = (far uchar)(data_read);			// assign data read to pointer
		data_ptr[index] = data_read;			// assign data read to pointer
    }
    I2cStop(); 
}
/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AckPolling()
// Inputs	: None
// Outputs	: None
// Process  : Wait for EEProm ready
///////////////////////////////////////////////////////////////////////////////////////
void AckPolling(void)
{
	do {
  	MAC_I2cStart								// I2C Start
  	MAC_I2cWriteByte(EE_WRITE_CTRL + I2cCommand)	// I2C EEProm Address; Write
  	WaitI2cTransmitEmpty();						// wait for transmit buffer empty
	// Now must wait few microsec before reading SDA
	delay_usec(3);
	}
	//while(I2C_SDA_PORT_IN & I2C_SDA_PIN);		// SDA is still 1 : wait again
   	while (I2CDST & I2C_SDA_PIN_STATE);
}
*/
