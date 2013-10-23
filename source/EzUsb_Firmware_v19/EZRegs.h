//-----------------------------------------------------------------------------
//	File:		ezregs.h
//	Contents:	EZ-USB register declarations and bit mask definitions.
//
//	Copyright (c) 1997 AnchorChips, Inc. All rights reserved
//-----------------------------------------------------------------------------
#ifndef EZREGS_H	/* Header Sentry */
#define EZREGS_H

/*-----------------------------------------------------------------------------
	Global Variables
-----------------------------------------------------------------------------*/
// The Ez-USB registers are defined here. There used to be 3 files containing
// this information: ezregs.h (external refs), ezregs.c (bound reg names to an 
// explicit address in ezusb.lib), and ezregs.inc (included in assembly files).
// We now use ezregs.h for register address allocation instead of ezregs.c by  
// using "#define ALLOCATE_EXTERN". The file ezregs.c now just includes ezregs.h.
// When using "#define ALLOCATE_EXTERN", you get: 
// xdata volatile BYTE OUT7BUF[64]	_at_	0x7B40;
// Such lines used to be in ezregs.c, but now are created from ezregs.h by using
// the preprocessor. The definitions in ezregs.c were redundant.
// Incidently, these lines will not generate any space in the resulting hex 
// file; they just bind the symbols to the addresses for compilation. 
// Since the file ezregs.c is still used in the library build, you normally
// just need to use #include "ezregs.h" in your files (i.e. fw.c).
// If you want to generate your own (non-frameworks based) C example, then you 
// just need to put "#define ALLOCATE_EXTERN" in your main program file; 
// i.e. fw.c or a stand-alone C source file. Any time you link with ezusb.lib,
// it will not be necessary to "#define ALLOCATE_EXTERN".
// Without "#define ALLOCATE_EXTERN", you just get the external reference: 
// extern xdata volatile BYTE OUT7BUF[64]	;//	0x7B40;
// This uses the concatenation operator "##" to insert a comment "//" 
// to cut off the end of the line, "_at_	0x7B40;", which is not wanted.
// If you modify the register definitions below, please regenerate the file 
// "ezregs.inc" which uses the same basic information, but which could not be 
// derived automatically from this one source file using the preprocessor.

#ifdef ALLOCATE_EXTERN
#define EXTERN
#define _AT_ _at_
#else
#define EXTERN extern
#define _AT_ ;/ ## /
#endif

/* Register Assignments 3/18/99 TPM */
EXTERN xdata volatile BYTE OUT7BUF[64]	_AT_	0x7B40;
EXTERN xdata volatile BYTE IN7BUF[64]	_AT_	0x7B80;
EXTERN xdata volatile BYTE OUT6BUF[64]	_AT_	0x7BC0;
EXTERN xdata volatile BYTE IN6BUF[64]	_AT_	0x7C00;
EXTERN xdata volatile BYTE OUT5BUF[64]	_AT_	0x7C40;
EXTERN xdata volatile BYTE IN5BUF[64]	_AT_	0x7C80;
EXTERN xdata volatile BYTE OUT4BUF[64]	_AT_	0x7CC0;
EXTERN xdata volatile BYTE IN4BUF[64]	_AT_	0x7D00;
EXTERN xdata volatile BYTE OUT3BUF[64]	_AT_	0x7D40;
EXTERN xdata volatile BYTE IN3BUF[64]	_AT_	0x7D80;
EXTERN xdata volatile BYTE OUT2BUF[64]	_AT_	0x7DC0;
EXTERN xdata volatile BYTE IN2BUF[64]	_AT_	0x7E00;
EXTERN xdata volatile BYTE OUT1BUF[64]	_AT_	0x7E40;
EXTERN xdata volatile BYTE IN1BUF[64]	_AT_	0x7E80;
EXTERN xdata volatile BYTE OUT0BUF[64]	_AT_	0x7EC0;
EXTERN xdata volatile BYTE IN0BUF[64]	_AT_	0x7F00;
EXTERN xdata volatile BYTE OUT8DATA	_AT_	0x7F60;
EXTERN xdata volatile BYTE OUT9DATA	_AT_	0x7F61;
EXTERN xdata volatile BYTE OUT10DATA	_AT_	0x7F62;
EXTERN xdata volatile BYTE OUT11DATA	_AT_	0x7F63;
EXTERN xdata volatile BYTE OUT12DATA	_AT_	0x7F64;
EXTERN xdata volatile BYTE OUT13DATA	_AT_	0x7F65;
EXTERN xdata volatile BYTE OUT14DATA	_AT_	0x7F66;
EXTERN xdata volatile BYTE OUT15DATA	_AT_	0x7F67;
EXTERN xdata volatile BYTE IN8DATA	_AT_	0x7F68;
EXTERN xdata volatile BYTE IN9DATA	_AT_	0x7F69;
EXTERN xdata volatile BYTE IN10DATA	_AT_	0x7F6A;
EXTERN xdata volatile BYTE IN11DATA	_AT_	0x7F6B;
EXTERN xdata volatile BYTE IN12DATA	_AT_	0x7F6C;
EXTERN xdata volatile BYTE IN13DATA	_AT_	0x7F6D;
EXTERN xdata volatile BYTE IN14DATA	_AT_	0x7F6E;
EXTERN xdata volatile BYTE IN15DATA	_AT_	0x7F6F;
EXTERN xdata volatile BYTE OUT8BCH	_AT_	0x7F70;
EXTERN xdata volatile BYTE OUT8BCL	_AT_	0x7F71;
EXTERN xdata volatile BYTE OUT9BCH	_AT_	0x7F72;
EXTERN xdata volatile BYTE OUT9BCL	_AT_	0x7F73;
EXTERN xdata volatile BYTE OUT10BCH	_AT_	0x7F74;
EXTERN xdata volatile BYTE OUT10BCL	_AT_	0x7F75;
EXTERN xdata volatile BYTE OUT11BCH	_AT_	0x7F76;
EXTERN xdata volatile BYTE OUT11BCL	_AT_	0x7F77;
EXTERN xdata volatile BYTE OUT12BCH	_AT_	0x7F78;
EXTERN xdata volatile BYTE OUT12BCL	_AT_	0x7F79;
EXTERN xdata volatile BYTE OUT13BCH	_AT_	0x7F7A;
EXTERN xdata volatile BYTE OUT13BCL	_AT_	0x7F7B;
EXTERN xdata volatile BYTE OUT14BCH	_AT_	0x7F7C;
EXTERN xdata volatile BYTE OUT14BCL	_AT_	0x7F7D;
EXTERN xdata volatile BYTE OUT15BCH	_AT_	0x7F7E;
EXTERN xdata volatile BYTE OUT15BCL	_AT_	0x7F7F;
EXTERN xdata volatile BYTE CPUCS	_AT_	0x7F92;
EXTERN xdata volatile BYTE PORTACFG	_AT_	0x7F93;
EXTERN xdata volatile BYTE PORTBCFG	_AT_	0x7F94;
EXTERN xdata volatile BYTE PORTCCFG	_AT_	0x7F95;
EXTERN xdata volatile BYTE OUTA		_AT_	0x7F96;
EXTERN xdata volatile BYTE OUTB		_AT_	0x7F97;
EXTERN xdata volatile BYTE OUTC		_AT_	0x7F98;
EXTERN xdata volatile BYTE PINSA	_AT_	0x7F99;
EXTERN xdata volatile BYTE PINSB	_AT_	0x7F9A;
EXTERN xdata volatile BYTE PINSC	_AT_	0x7F9B;
EXTERN xdata volatile BYTE OEA		_AT_	0x7F9C;
EXTERN xdata volatile BYTE OEB		_AT_	0x7F9D;
EXTERN xdata volatile BYTE OEC		_AT_	0x7F9E;
EXTERN xdata volatile BYTE UART230	_AT_	0x7F9F;
EXTERN xdata volatile BYTE ISOERR	_AT_	0x7FA0;
EXTERN xdata volatile BYTE ISOCTL	_AT_	0x7FA1;
EXTERN xdata volatile BYTE ZBCOUT	_AT_	0x7FA2;
EXTERN xdata volatile BYTE ZBCIN	_AT_	0x7FA3;
EXTERN xdata volatile BYTE I2CS		_AT_	0x7FA5;
EXTERN xdata volatile BYTE I2DAT	_AT_	0x7FA6;
EXTERN xdata volatile BYTE IVEC		_AT_	0x7FA8;
EXTERN xdata volatile BYTE IN07IRQ	_AT_	0x7FA9;
EXTERN xdata volatile BYTE OUT07IRQ	_AT_	0x7FAA;
EXTERN xdata volatile BYTE USBIRQ	_AT_	0x7FAB;
EXTERN xdata volatile BYTE IN07IEN	_AT_	0x7FAC;
EXTERN xdata volatile BYTE OUT07IEN	_AT_	0x7FAD;
EXTERN xdata volatile BYTE USBIEN	_AT_	0x7FAE;
EXTERN xdata volatile BYTE USBBAV	_AT_	0x7FAF;
EXTERN xdata volatile WORD BPADDR	_AT_	0x7FB2;
//volatile EXTERN xdata BYTE BPADDRL	_AT_	0x7FB3;
EXTERN xdata volatile EPIOC EPIO[16]	_AT_	0x7FB4;
EXTERN xdata volatile BYTE SUDPTRH	_AT_	0x7FD4;
EXTERN xdata volatile BYTE SUDPTRL	_AT_	0x7FD5;
EXTERN xdata volatile BYTE USBCS	_AT_	0x7FD6;
EXTERN xdata volatile BYTE TOGCTL	_AT_	0x7FD7;
EXTERN xdata volatile BYTE USBFRAMEL	_AT_	0x7FD8;
EXTERN xdata volatile BYTE USBFRAMEH	_AT_	0x7FD9;
EXTERN xdata volatile BYTE FNADDR	_AT_	0x7FDB;
EXTERN xdata volatile BYTE USBPAIR	_AT_	0x7FDD;
EXTERN xdata volatile BYTE IN07VAL	_AT_	0x7FDE;
EXTERN xdata volatile BYTE OUT07VAL	_AT_	0x7FDF;
EXTERN xdata volatile BYTE INISOVAL	_AT_	0x7FE0;
EXTERN xdata volatile BYTE OUTISOVAL	_AT_	0x7FE1;
EXTERN xdata volatile BYTE FASTXFR	_AT_	0x7FE2;
EXTERN xdata volatile BYTE AUTOPTRH	_AT_	0x7FE3;
EXTERN xdata volatile BYTE AUTOPTRL	_AT_	0x7FE4;
EXTERN xdata volatile BYTE AUTODATA	_AT_	0x7FE5;
EXTERN xdata volatile BYTE SETUPDAT[8]	_AT_	0x7FE8;
EXTERN xdata volatile BYTE OUT8ADDR	_AT_	0x7FF0;
EXTERN xdata volatile BYTE OUT9ADDR	_AT_	0x7FF1;
EXTERN xdata volatile BYTE OUT10ADDR	_AT_	0x7FF2;
EXTERN xdata volatile BYTE OUT11ADDR	_AT_	0x7FF3;
EXTERN xdata volatile BYTE OUT12ADDR	_AT_	0x7FF4;
EXTERN xdata volatile BYTE OUT13ADDR	_AT_	0x7FF5;
EXTERN xdata volatile BYTE OUT14ADDR	_AT_	0x7FF6;
EXTERN xdata volatile BYTE OUT15ADDR	_AT_	0x7FF7;
EXTERN xdata volatile BYTE IN8ADDR	_AT_	0x7FF8;
EXTERN xdata volatile BYTE IN9ADDR	_AT_	0x7FF9;
EXTERN xdata volatile BYTE IN10ADDR	_AT_	0x7FFA;
EXTERN xdata volatile BYTE IN11ADDR	_AT_	0x7FFB;
EXTERN xdata volatile BYTE IN12ADDR	_AT_	0x7FFC;
EXTERN xdata volatile BYTE IN13ADDR	_AT_	0x7FFD;
EXTERN xdata volatile BYTE IN14ADDR	_AT_	0x7FFE;
EXTERN xdata volatile BYTE IN15ADDR	_AT_	0x7FFF;

#undef EXTERN
#undef _AT_

/*-----------------------------------------------------------------------------
	Special Function Registers (SFRs)
	The byte registers and bits defined in the following list are based
	on the Synopsis definition of the 8051 Special Function Registers for EZ-USB. 
    If you modify the register definitions below, please regenerate the file 
    "ezregs.inc" which includes the same basic information for assembly inclusion.
-----------------------------------------------------------------------------*/

sfr SP      = 0x81;
sfr DPL     = 0x82;
sfr DPH     = 0x83;
sfr DPL1    = 0x84;
sfr DPH1    = 0x85;
sfr DPS     = 0x86;
			/*  DPS  */
			sbit SEL   = 0x86+0;
sfr PCON    = 0x87;	/*  PCON  */
			//sbit IDLE   = 0x87+0;
			//sbit STOP   = 0x87+1;
			//sbit GF0    = 0x87+2;
			//sbit GF1    = 0x87+3;
			//sbit SMOD0  = 0x87+7;
sfr TCON    = 0x88;
			/*  TCON  */
			sbit IT0    = 0x88+0;
			sbit IE0    = 0x88+1;
			sbit IT1    = 0x88+2;
			sbit IE1    = 0x88+3;
			sbit TR0    = 0x88+4;
			sbit TF0    = 0x88+5;
			sbit TR1    = 0x88+6;
			sbit TF1    = 0x88+7;
sfr TMOD    = 0x89;
			/*  TMOD  */
			//sbit M00    = 0x89+0;
			//sbit M10    = 0x89+1;
			//sbit CT0    = 0x89+2;
			//sbit GATE0  = 0x89+3;
			//sbit M01    = 0x89+4;
			//sbit M11    = 0x89+5;
			//sbit CT1    = 0x89+6;
			//sbit GATE1  = 0x89+7;
sfr TL0     = 0x8A;
sfr TL1     = 0x8B;
sfr TH0     = 0x8C;
sfr TH1     = 0x8D;
sfr CKCON   = 0x8E;
			/*  CKCON  */
			//sbit MD0    = 0x89+0;
			//sbit MD1    = 0x89+1;
			//sbit MD2    = 0x89+2;
			//sbit T0M    = 0x89+3;
			//sbit T1M    = 0x89+4;
			//sbit T2M    = 0x89+5;
sfr SPC_FNC = 0x8F; // Was WRS in Reg320
			/*  CKCON  */
			//sbit WRS    = 0x8F+0;
sfr EXIF    = 0x91; // EXIF Bit Values differ from Reg320
			/*  EXIF  */
			//sbit USBINT = 0x91+4;
			//sbit I2CINT = 0x91+5;
			//sbit IE4    = 0x91+6;
			//sbit IE5    = 0x91+7;
sfr MPAGE  = 0x92;
sfr SCON0  = 0x98;
			/*  SCON0  */
			sbit RI    = 0x98+0;
			sbit TI    = 0x98+1;
			sbit RB8   = 0x98+2;
			sbit TB8   = 0x98+3;
			sbit REN   = 0x98+4;
			sbit SM2   = 0x98+5;
			sbit SM1   = 0x98+6;
			sbit SM0   = 0x98+7;
sfr SBUF0  = 0x99;
sfr IE     = 0xA8;
			/*  IE  */
			sbit EX0   = 0xA8+0;
			sbit ET0   = 0xA8+1;
			sbit EX1   = 0xA8+2;
			sbit ET1   = 0xA8+3;
			sbit ES0   = 0xA8+4;
			sbit ET2   = 0xA8+5;
			sbit ES1   = 0xA8+6;
			sbit EA    = 0xA8+7;
sfr IP     = 0xB8;
			/*  IP  */
			sbit PX0   = 0xB8+0;
			sbit PT0   = 0xB8+1;
			sbit PX1   = 0xB8+2;
			sbit PT1   = 0xB8+3;
			sbit PS0   = 0xB8+4;
			sbit PT2   = 0xB8+5;
			sbit PS1   = 0xB8+6;
sfr SCON1  = 0xC0;
			/*  SCON1  */
			sbit RI1   = 0xC0+0;
			sbit TI1   = 0xC0+1;
			sbit RB81  = 0xC0+2;
			sbit TB81  = 0xC0+3;
			sbit REN1  = 0xC0+4;
			sbit SM21  = 0xC0+5;
			sbit SM11  = 0xC0+6;
			sbit SM01  = 0xC0+7;
sfr SBUF1  = 0xC1;
sfr T2CON  = 0xC8;
			/*  T2CON  */
			sbit CP_RL2 = 0xC8+0;
			sbit C_T2  = 0xC8+1;
			sbit TR2   = 0xC8+2;
			sbit EXEN2 = 0xC8+3;
			sbit TCLK  = 0xC8+4;
			sbit RCLK  = 0xC8+5;
			sbit EXF2  = 0xC8+6;
			sbit TF2   = 0xC8+7;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2    = 0xCC;
sfr TH2    = 0xCD;
sfr PSW    = 0xD0;
			/*  PSW  */
			sbit P     = 0xD0+0;
			sbit FL    = 0xD0+1;
			sbit OV    = 0xD0+2;
			sbit RS0   = 0xD0+3;
			sbit RS1   = 0xD0+4;
			sbit F0    = 0xD0+5;
			sbit AC    = 0xD0+6;
			sbit CY    = 0xD0+7;
sfr EICON  = 0xD8; // Was WDCON in DS80C320; Bit Values differ from Reg320
			/*  EICON  */
			sbit INT6  = 0xD8+3;
			sbit RESI  = 0xD8+4;
			sbit ERESI = 0xD8+5;
			sbit SMOD1 = 0xD8+7;
sfr ACC    = 0xE0;
sfr EIE    = 0xE8; // EIE Bit Values differ from Reg320
                        /*  EIE  */
			sbit EUSB    = 0xE8+0;
			sbit EI2C    = 0xE8+1;
			sbit EIEX4   = 0xE8+2;
			sbit EIEX5   = 0xE8+3;
			sbit EIEX6   = 0xE8+4;
sfr B      = 0xF0;
sfr EIP    = 0xF8; // EIP Bit Values differ from Reg320
                        /*  EIP  */
			sbit PUSB    = 0xF8+0;
			sbit PI2C    = 0xF8+1;
			sbit EIPX4   = 0xF8+2;
			sbit EIPX5   = 0xF8+3;
			sbit EIPX6   = 0xF8+4;

/*-----------------------------------------------------------------------------
	Bit Masks
-----------------------------------------------------------------------------*/

/* CPU Control & Status Register */
#define bmCHIPREV		(bmBIT7 | bmBIT6 | bmBIT5 | bmBIT4)
#define bmCLK24OE		bmBIT1
#define bm8052RES		bmBIT0
/* Port Configuration Registers */
/* Port A */
#define bmRXD1OUT		bmBIT7
#define bmRXD0OUT		bmBIT6
#define bmFRD			bmBIT5
#define bmFWR			bmBIT4
#define bmCS			bmBIT3
#define bmOE			bmBIT2
#define bmT1OUT			bmBIT1
#define bmT0OUT			bmBIT0
/* Port B */
#define bmT2OUT			bmBIT7
#define bmINT6			bmBIT6
#define bmINT5			bmBIT5
#define bmINT4			bmBIT4
#define bmTXD1			bmBIT3
#define bmRXD1			bmBIT2
#define bmT2EX			bmBIT1
#define bmT2			bmBIT0
/* Port C */
#define bmRD			bmBIT7
#define bmWR			bmBIT6
#define bmT1			bmBIT5
#define bmT0			bmBIT4
#define bmINT1			bmBIT3
#define bmINT0			bmBIT2
#define bmTXD0			bmBIT1
#define bmRXD0			bmBIT0
/* Isochronous Status & End Point Valid Registers */
#define bmEP15			bmBIT7
#define bmEP14			bmBIT6
#define bmEP13			bmBIT5
#define bmEP12			bmBIT4
#define bmEP11			bmBIT3
#define bmEP10			bmBIT2
#define bmEP9			bmBIT1
#define bmEP8			bmBIT0
/* I2C Control & Status Register */
#define bmSTART			bmBIT7
#define bmSTOP			bmBIT6
#define bmLASTRD		bmBIT5
#define bmID			(bmBIT4 | bmBIT3)
#define bmBERR			bmBIT2
#define bmACK			bmBIT1
#define bmDONE			bmBIT0
/* Interrupt Vector Register */
#define bmIV4			bmBIT6
#define bmIV3			bmBIT5
#define bmIV2			bmBIT4
#define bmIV1			bmBIT3
#define bmIV0			bmBIT2
/* End point Interrupt Request, End Point Interrupt Enable */
/* And End Point Valid Registers */
#define bmEP7			bmBIT7
#define bmEP6			bmBIT6
#define bmEP5			bmBIT5
#define bmEP4			bmBIT4
#define bmEP3			bmBIT3
#define bmEP2			bmBIT2
#define bmEP1			bmBIT1
#define bmEP0			bmBIT0
/* Global Interrupt Request & Enable Registers */
#define bmIBN        bmBIT5
#define bmURES			bmBIT4
#define bmSUSP			bmBIT3
#define bmSUTOK			bmBIT2
#define bmSOF			bmBIT1
#define bmSUDAV			bmBIT0
/* Global Control */
#define bmBREAK			bmBIT3
#define bmBPPULSE		bmBIT2
#define bmBPEN			bmBIT1
#define bmAVEN			bmBIT0
/* USB Control & Status Register */
#define bmRWAKEUP		bmBIT7
#define bmDISCON		bmBIT3
#define bmDISCOE		bmBIT2
#define bmRENUM			bmBIT1
#define bmSIGRESUME		bmBIT0
/* End Point 0 Control & Status Register */
#define bmOUT			bmBIT3
#define bmIN			bmBIT2
#define bmHS			bmBIT1
#define bmHSSTALL		bmBIT0
/* End Point Control & Status Registers */
#define bmEPSTALL		bmBIT0
#define bmEPBUSY		bmBIT1
/* Fast Transfer Register */
#define bmFISO			bmBIT7
#define bmFBLK			bmBIT6
#define bmRPOL			bmBIT5
#define bmRMOD1			bmBIT4
#define bmRMOD0			bmBIT3
#define bmWPOL			bmBIT2
#define bmWMOD1			bmBIT1
#define bmWMOD0			bmBIT0
/* Endpoint Pairing Register */
#define bmISOSEND0		bmBIT7
#define bmPR6OUT		bmBIT5
#define bmPR4OUT		bmBIT4
#define bmPR2OUT		bmBIT3
#define bmPR6IN			bmBIT2
#define bmPR4IN			bmBIT1
#define bmPR2IN			bmBIT0
/* End point control offsets */
enum
{
	IN0BUF_ID = 0,
	IN1BUF_ID,
	IN2BUF_ID,
	IN3BUF_ID,
	IN4BUF_ID,
	IN5BUF_ID,
	IN6BUF_ID,
	IN7BUF_ID,
	OUT0BUF_ID,
	OUT1BUF_ID,
	OUT2BUF_ID,
	OUT3BUF_ID,
	OUT4BUF_ID,
	OUT5BUF_ID,
	OUT6BUF_ID,
	OUT7BUF_ID
};

#define EP0CS	EPIO[0].cntrl
#define IN0BC	EPIO[0].bytes
#define IN1CS	EPIO[1].cntrl
#define IN1BC	EPIO[1].bytes
#define IN2CS	EPIO[2].cntrl
#define IN2BC	EPIO[2].bytes
#define IN3CS	EPIO[3].cntrl
#define IN3BC	EPIO[3].bytes
#define IN4CS	EPIO[4].cntrl
#define IN4BC	EPIO[4].bytes
#define IN5CS	EPIO[5].cntrl
#define IN5BC	EPIO[5].bytes
#define IN6CS	EPIO[6].cntrl
#define IN6BC	EPIO[6].bytes
#define IN7CS	EPIO[7].cntrl
#define IN7BC	EPIO[7].bytes
#define OUT0CS	EPIO[8].cntrl
#define OUT0BC	EPIO[8].bytes
#define OUT1CS	EPIO[9].cntrl
#define OUT1BC	EPIO[9].bytes
#define OUT2CS	EPIO[10].cntrl
#define OUT2BC	EPIO[10].bytes
#define OUT3CS	EPIO[11].cntrl
#define OUT3BC	EPIO[11].bytes
#define OUT4CS	EPIO[12].cntrl
#define OUT4BC	EPIO[12].bytes
#define OUT5CS	EPIO[13].cntrl
#define OUT5BC	EPIO[13].bytes
#define OUT6CS	EPIO[14].cntrl
#define OUT6BC	EPIO[14].bytes
#define OUT7CS	EPIO[15].cntrl
#define OUT7BC	EPIO[15].bytes

/*-----------------------------------------------------------------------------
	Macros
-----------------------------------------------------------------------------*/
/* Convert End point ID (d0000eee) to EPIO offset */
#define EPID(id)		(((~id & 0x80) >> 4) + (id & 0x07))


#endif	/* EZREGS_H */
