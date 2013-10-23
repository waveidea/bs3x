#ifndef MACKIE
#define MACKIE


///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////

// Sysex definition for Mackie control
#define TX_HOST_CONNECT_QUERRY			0x01 // + SS SS SS SS SS SS SS LL LL LL LL F7 (SS=serial number LL=chalenge code)
#define TX_CONFIRM_CONNECT				0x03 // + SS SS SS SS SS SS SS F7 (SS=serial number)
#define TX_ERROR_CONNECT				0x04 // + SS SS SS SS SS SS SS F7 (SS=serial number)
#define TX_FW_VERSION_REPLY				0x14 // + VV VV VV VV VV F7 (VV=firmware version eg "1.0")
#define TX_SERIAL_NB_REPLY_MACKIE		0x1B // + SS SS SS SS SS SS SS F7 (SS=serial number) (Mackie only)

#define RX_DEVICE_QUERY					0x00 // + F7
#define RX_OFFLINE_STATUS_LOGIC			0x0F // + 7F F7 (Logic only, not Mackie) V1.69 
#define RX_HOST_CONNECT_REPLY			0x02 // + SS SS SS SS SS SS SS RR RR RR RR F7 (SS=serial number RR=response to chalenge code)
#define RX_FW_VERSION_REQUEST			0x13 // + 00 F7
#define RX_SERIAL_NB_REQUEST_MACKIE		0x1A // + 00 F7 (Mackie only, not logic) V1.69 
#define RX_RESET_FADERS					0x61 // + F7 
#define RX_RESET_LEDS					0x62 // + F7 
#define RX_RESET_UNIT					0x63 // + F7 

#define MACKIE_SERIAL_NB_LENGHT			7 // SS SS SS SS SS SS SS
#define MACKIE_FW_VERSION_LENGHT		5 // VV VV VV VV VV
#define MACKIE_CHALLENGE_LENGHT			4 // LL LL LL LL

#define MACKIE_LCD_LINE_LENGHT			56 // V1.7

// algorithm (l1 to l4 = challenge code bytes 1 to 4, r1 to r4 = response code bytes 1 to 4):
// r1 = 0x7F & (l1 + (l2 ^ 0xa) - l4);
// r2 = 0x7F & ((l3>>4) ^ (l1+l4));
// r3 = 0x7F & ( l4-(l3<<2) ^ (l1|l2));
// r4 = 0x7F & (l2-l3+(0xF0^(l4<<4)));
#define RX_WRITE_LCD_TXT				0x12	// + OO YY ... F7 (OO=offset 00 thru 37 for upper line, 38 thru 6F for lower line, YY=ASCII data)
												// 6 bytes per channel are used to display texts over each channel
#define RX_WRITE_MULTIPLE_MTC			0x10	// + YY ... F7 (YY=ASCII data from ASCII table, including dots)
												// eg F0 00 00 66 10 10 36 32 31 71 30 72 30 79 30 31 F7 writes “109.02.01.126”
#define RX_WRITE_SINGLE_MTC				0x40	// B0 4i YY ... F7 (YY=ASCII data from ASCII table, i=digit id)
#define RX_WRITE_MULTIPLE_7SEG			0x11	// + YY YY F7 (YY=ASCII data from ASCII table, including dots)												

#define MACKIE_MTC_7SEG_DIGIT_DOTS_NB	16		// 7S 000.00.00.000 (includes space and dots)
#define MACKIE_MTC_DIGIT_DOTS_NB		13		// 7S 000.00.00.000 (includes space and dots)
#define MACKIE_MTC_DIGIT_NB				10		// 000.00.00.000
#define MACKIE_7SEG_NB					2		// 00

#define MACKIE_CHANNEL_NB				8
#define MACKIE_BARGRAPH_DECAY			25		// in multiple of 10ms (time to decrease a step of bargraph decay)
#define MACKIE_BARGRAPH_DECAY_STEP		2		// value max is 13, value displayed max = 6
#define MACKIE_MASTER_BARGRAPH			0xFF

// Messages sent to the host which are not available in the library

#define MACKIE_SYX_ID0				0x00
#define MACKIE_SYX_ID1				0x00
#define MACKIE_SYX_ID2				0x66
#define MACKIE_SYX_ID3				0x14 // V1.68 0x10 (mackie control is 14 while logic control is 10)
#define MACKIE_SYX_ID3_XT			0x15 // V1.68 0x11 (mackie control xt is 15 while logic control xt is 11) 
#define MACKIE_SYX_ID3_LOGIC		0x10 // V1.68 
#define MACKIE_SYX_ID3_LOGIC_XT		0x11 // V1.68

#define MACKIE_SYX_HEADER_SIZE		4	// F0 00 00 66  V1.69

//enum {MACKIE_MTC_HOUR, MACKIE_MTC_MIN, MACKIE_MTC_SEC, MACKIE_MTC_MILSEC, MACKIE_MTC_NB};
#define MTC_UPDATE_ALL_DIGITS 		0xFF

///////////////////////////////////////////////////////////
// MACKIE CONTROL IDs
//////////////////////////////////////////////////////////

#define MACKIE_FADER_BNK_LEFT_ID	0x2E
#define MACKIE_FADER_BNK_RIGHT_ID	0x2F
#define MACKIE_FADER_CHANNEL_LEFT_ID	0x30	// V1.7
#define MACKIE_FADER_CHANNEL_RIGHT_ID	0x31	// V1.7

#define MACKIE_SHIFT_ID				0x46
#define MACKIE_JOG_WHEEL_ID			0x3C

#define JOG_WHEEL_LEFT				0x41
#define JOG_WHEEL_RIGHT				0x01
 


///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////

void EnterMackieMode(void);
void LeaveMackieMode(void);
void SendMackieSysexCommand(uchar command);
void DisplayMackieTimeCodeDigit(uchar DigitNb);
void DisplayMackieTimeCodeFull(void);
void CheckIfRxMackieCommand(uchar status_received);
void DisplayMackieMeteringFull(void);
void DisplayMackieMetering(uchar channel);
void UpdateMackieBargraphDecay(void);
void SendMackieShortEvent(uchar event, uchar note_nb, uchar note_value);

#endif