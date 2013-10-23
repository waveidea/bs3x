#ifndef IIC_LOW_LEVEL
#define IIC_LOW_LEVEL

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////

#define BAUD_I2C					400000ul
#define BRG_I2C						Z8_FREQ/(4 * BAUD_I2C)

#define SEND_START					0x40U
#define SEND_STOP					0x20U
#define SEND_NACK					0x04U
#define TRANSMIT_DATA_REG_EMPTY		0x80U
#define	RECEIVE_DATA_REG_FULL		0x40U
#define	ACKNOWLEDGE					0x20U
#define	NACKNOWLEDGE				0x01U
#define	I2C_ENABLE					0x80U
#define	I2C_FILTER					0x01U	// V1.6

#define I2C_SDA_PIN_STATE			0x40U

//V1.6 #define I2C_READ_BIT				0x01U
//V1.6 #define I2C_WRITE_BIT				0x00U

#define I2C_READ_COMMAND			0x01U	// Read input port
#define I2C_WRITE_COMMAND			0x00U	// Read/Write output port

//enum {I2C_NO_TIMEOUT, I2C_TIMEOUT};

#define I2C_TIMEOUT_CPT				0xFFU

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////

void  InitI2C(void);
void  I2cStop(void);
void  WaitI2cTransmitEmpty(void);
void  WaitI2cAcknoledge(void);
void  WaitI2cFullDataReceived(void);

// all other low level functions are Macros defined below

///////////////////////////////////////////////////////////////////////////////////////
// Macro 	: MAC_I2cStart()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
#define MAC_I2cStart		\
{							\
	I2CCTL |= SEND_START; 	\
}

///////////////////////////////////////////////////////////////////////////////////////
// Macro 	: MAC_I2cWriteByte()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
#define MAC_I2cWriteByte(byte_to_write)		\
{											\
	I2CDATA = (uchar)(byte_to_write);		\
}

///////////////////////////////////////////////////////////////////////////////////////
// Macro 	: MAC_I2cSendNack()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
#define MAC_I2cSendNack						\
{											\
	I2CCTL |= SEND_NACK; 					\
}

///////////////////////////////////////////////////////////////////////////////////////
// Macro 	: MAC_I2cReadByte()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
#define MAC_I2cReadByte(byte_read)			\
{											\
	byte_read = (uchar)I2CDATA;				\
}

#endif