#ifndef LCDLOWLEVEL
#define LCDLOWLEVEL

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Functions prototypes
///////////////////////////////////////////////////////////

void InitLcd(void);
void WriteToLcd(uchar current_x_pixel, uchar data_type, uchar value);
void WaitLcdBusyBit(void);
void SetCurrentXY(uchar x_pixel, uchar y_page);
void SetLcdEPulse(void);
void IntToLcdText(uchar value, uchar lcd_format);
void DisplayLcdText(uchar x_pixel, uchar y_page, uchar lcd_format/* V1.66 , rom uchar * font_ptr*/);
void DisplayLcdInt (uchar x_pixel, uchar y_page, uchar lcd_format,/* V1.66 rom uchar * font_ptr,*/ uchar value);
void FillLcdZone(uchar start_x, uchar start_y, uchar size_x, uchar size_y, uchar fill_value);
void DisplayBmp(uchar start_x_pixel, uchar start_y_page, uchar lcd_format, rom uchar * bmp_ptr);


#endif
