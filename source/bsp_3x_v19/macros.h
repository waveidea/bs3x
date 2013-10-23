#ifndef MACROS
#define MACROS

///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////
/*
#define MAC_SET_LCD_PARAM(start_x, start_y, format, ptr) \\
{	start_x_pixel = start_x; 	\\
	start_y_page  = start_y; 	\\
	lcd_format	  = format;		\\
	font_bmp_ptr  = ptr;		}
*/

#define MAC_ENABLE_ITR_ADC		IRQ0ENH	|= 0x01;
#define MAC_DISABLE_ITR_ADC		IRQ0ENH	&= 0xFE;
//#define MAC_ENABLE_ITR			EI();
//#define MAC_DISABLE_ITR			DI();

//#define MAC_ENABLE_ITR_ADC		EI();
//#define MAC_DISABLE_ITR_ADC		DI();
//#define MAC_ENABLE_ITR			
//#define MAC_DISABLE_ITR			

#endif
