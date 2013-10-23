//#pragma NOIV	// Do not generate interrupt vectors
 
#include <stdio.h>
#include <ezusb.h>
#include <ezregs.h>
#include "pins_def.h"
#include "i2c.h"

/*
I2CPCKT volatile	I2CPckt;

void EZUSB_InitI2C(void)
{
	I2CPckt.status = I2C_IDLE;

	EI2C = 1;	// Enable I2C interrupt				
//	EA = 1;		// Enable 8051 interrupts
#ifdef TNG
   I2CMODE |= 0x02;  // enable I2C Stop interrupt
#endif

}

// seems to be not used ?

void EZUSB_WaitForEEPROMWrite(BYTE addr)
{
#ifndef TNG
   // if in progress, wait for STOP to complete
   while (I2CS & bmSTOP);
#endif

   // disable i2c interrupts
   EI2C = 0;

   do
   {
      I2CS |= bmSTART;
	   I2DAT = addr << 1;
      while (!(I2CS & 1));
      I2CS |= bmSTOP;
      while (I2CS & bmSTOP);
   } while (!(I2CS & bmACK));

   // enable i2c interrupts
   EI2C = 1;

}


BOOL EZUSB_WriteI2C_(BYTE addr, BYTE length, BYTE xdata *dat)
{
#ifndef TNG
   // if in progress, wait for STOP to complete
   while (I2CS & bmSTOP);
#endif

	if(I2CPckt.status == I2C_IDLE)
	{	
		I2CS |= bmSTART;
		I2DAT = addr << 1;

		I2CPckt.length = length;
		I2CPckt.dat = dat;
		I2CPckt.count = 0;
		I2CPckt.status = I2C_SENDING;

		return(TRUE);
	}
	
	return(FALSE);
}

BOOL EZUSB_ReadI2C_(BYTE addr, BYTE length, BYTE xdata *dat)
{
#ifndef TNG
   // if in progress, wait for STOP to complete
   while (I2CS & bmSTOP);
#endif

   if(I2CPckt.status == I2C_IDLE)
	{
		I2CS |= bmSTART;
		I2DAT = (addr << 1) | 0x01;

		I2CPckt.length = length;
		I2CPckt.dat = dat;
		I2CPckt.count = 0;
		I2CPckt.status = I2C_PRIME;

		return(TRUE);
	}

	return(FALSE);
}

*/

