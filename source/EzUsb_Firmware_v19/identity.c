#include <common_z8_ez.h>
//#pragma SRC
//#pragma ASM 
//$include (..\..\common_z8_ez\identity_inc.asm)
//#pragma ENDASM 
//#include <identity.h>
//extern FW_VERSION, FW_SUB_VERSION,HW_VERSION,HW_SUB_VERSION,MANUF_YEAR,
//MANUF_MONTH,SERIAL_4,SERIAL_3,SERIAL_2,SERIAL_1,SERIAL_0;
//extern FW_VERSION, FW_SUB_VERSION,HW_VERSION,HW_SUB_VERSION,MANUF_YEAR,
//MANUF_MONTH,SERIAL_4,SERIAL_3,SERIAL_2,SERIAL_1,SERIAL_0;
			  
/////////////////////////////////////////////////////////////////////////
////////////////////// EZ IDENTITY //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

unsigned char code identity[] = {
FW_VERSION,FW_SUB_VERSION,HW_VERSION,HW_SUB_VERSION,MANUF_YEAR,
MANUF_MONTH,SERIAL_4,SERIAL_3,SERIAL_2,SERIAL_1,SERIAL_0 };

// ADD ?CO?IDENTITY(00080h) TO project -> OPTION -> BL51 LOCATE -> CODE 
// to compile identity at fixed code address 0x0080
