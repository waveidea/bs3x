#ifndef I2C_H		//Header sentry
#define I2C_H

// FUNCTIONS PROTOTYPES
void EZUSB_InitI2C(void);
//void EZUSB_WaitForEEPROMWrite(BYTE addr);
BOOL EZUSB_WriteI2C_(BYTE addr, BYTE length, BYTE xdata *dat);
BOOL EZUSB_ReadI2C_(BYTE addr, BYTE length, BYTE xdata *dat);

#endif