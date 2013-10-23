;$include "..\..\common_z8_ez\identity_inc.asm"
;xdef FW_VERSION,FW_SUB_VERSION,HW_VERSION,HW_SUB_VERSION,MANUF_YEAR,MANUF_MONTH,SERIAL_4,SERIAL_3,SERIAL_2,SERIAL_1,SERIAL_0
$include (..\..\common_z8_ez\identity_inc.asm)
public identity
//ORG 0080H
//ORG 0080H

; V1.5 : In reality, serial number will start at 0x87 since the hex2bix tool from cypress will add 0x7 bytes at the begining of the eeprom

CSEG	AT 80H
;extrn FW_VERSION,FW_SUB_VERSION,HW_VERSION,HW_SUB_VERSION,MANUF_YEAR,MANUF_MONTH,SERIAL_4,SERIAL_3,SERIAL_2,SERIAL_1,SERIAL_0
identity: DB FW_VERSION,FW_SUB_VERSION,HW_VERSION,HW_SUB_VERSION,MANUF_YEAR,MANUF_MONTH,SERIAL_4,SERIAL_3,SERIAL_2,SERIAL_1,SERIAL_0
END