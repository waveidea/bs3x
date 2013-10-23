#ifndef SMPTE
#define SMPTE

///////////////////////////////////////////////////////////
// GLOBAL definitions
///////////////////////////////////////////////////////////

#define SMPTE_QUARTER_FRAME			0xF1
#define QUARTER_FRAME				0x04

enum {	SMPTE_FRAME_LOW_NIBBLE	, SMPTE_FRAME_HIGH_NIBBLE, 
		SMPTE_SECONDS_LOW_NIBBLE, SMPTE_SECONDS_HIGH_NIBBLE,
		SMPTE_MINUTES_LOW_NIBBLE, SMPTE_MINUTES_HIGH_NIBBLE,
		SMPTE_HOURS_LOW_NIBBLE	, SMPTE_HOURS_HIGH_NIBBLE_PLUS_TYPE };

enum {	SMPTE_HOUR_DIGIT, SMPTE_MINUTE_DIGIT, SMPTE_SECOND_DIGIT, SMPTE_FRAME_DIGIT};

#define update_frame_digit	(1<<SMPTE_FRAME_DIGIT)
#define update_second_digit	(1<<SMPTE_SECOND_DIGIT)
#define update_minute_digit	(1<<SMPTE_MINUTE_DIGIT)
#define update_hour_digit	(1<<SMPTE_HOUR_DIGIT)

#define SMPTE_ALL_DIGIT		( update_frame_digit | update_second_digit | update_minute_digit | update_hour_digit)

#define SMPTE_BYTE_NB					8U

enum { FPS24, FPS25, FPSDP30, FPS30 };
#define SMPTE_FRAME_RATE_NB	  			4U

#define SMPTE_UPDT_SEC_FROM_SW			4
//#define SMPTE_UPDT_SEC_FROM_SW_SHIFT	8

// Timer reload values depending on Frame rate :
//
// Reload_value = period x 20000000 / prescaler (8)
//
// Frame rate = 24 FPS
// 			24 FRAMES PER SECOND
// 24 * 4 = 96 QUARTER FRAMES PER SECOND
// >> QUARTER FRAME SENT EACH 10.416666 milliseconds
// >> Timer reload value = 26041.666666
// >> Timer reload value = 26041 >> QUARTER FRAME SENT EACH 10.4164 milliseconds
// >> Error = -0.0002666666 ms per quarter frame
// >> Timer reload value = 26042 >> QUARTER FRAME SENT EACH 10.4168 milliseconds
// >> Error = +0.0001333333 ms per quarter frame
// >> Must load 26042 then 26041 each 2 quarter frame

// Frame rate = 25 FPS
// 			25 FRAMES PER SECOND
// 25 * 4 = 100 QUARTER FRAMES PER SECOND
// >> QUARTER FRAME SENT EACH 10 milliseconds
// >> Timer reload value = 25000
// >> No error

// Frame rate = 30 FPS
// 			30 FRAMES PER SECOND
// 30 * 4 = 120 QUARTER FRAMES PER SECOND
// >> QUARTER FRAME SENT EACH 8.3333 milliseconds
// >> Timer reload value = 20833.3333333
// >> Timer reload value = 20833 >> QUARTER FRAME SENT EACH 8.3332 milliseconds
// >> Error = -0.0001333333 ms per quarter frame
// >> Timer reload value = 20834 >> QUARTER FRAME SENT EACH 8.3336 milliseconds
// >> Error = +0.0002666666 ms per quarter frame
// >> Must load 20834 then 20833 each 2 quarter frame

enum { FULL_SMPTE, UPDATE_QUARTER };

#define SMPTE_SECONDS_NB_MAX	60U
#define SMPTE_MINUTES_NB_MAX	60U
#define SMPTE_HOURS_NB_MAX		24U

#define SMPTE_FULL_FRAME_SYSEX_HEADER_SIZE	5U	// MIDI_SOX, 0x7F, 0x7F, 0x01, 0x01
#define SMPTE_FULL_FRAME_SYSEX_SIZE			10U	// MIDI_SOX, 0x7F, 0x7F, 0x01, 0x01, HR, MN, SC, FR, MIDI_EOX

#define SMPTE_LOCK_TIME			70U				// in multiple of 10ms

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// SMPTE STRUCTURE DEFINITION /////////////////////////////////
typedef struct {
uchar frame;			// 1 byte, frame		DO NOT CHANGE ORDER
uchar second;			// 1 byte, seconds		DO NOT CHANGE ORDER
uchar minute;			// 1 byte, minutes		DO NOT CHANGE ORDER
uchar hour;				// 1 byte, hours		DO NOT CHANGE ORDER
uchar type;				// 1 byte, fps	
} SMPTE_STRUCT ;


///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////

void InitSmpteTime(void);
void SendSmpteToMidi(uchar command);
void IncreaseDisplaySmpteTime(uchar frame_nb);
void DecreaseDisplaySmpteTime(uchar frame_nb);
void DisplaySmpteTime(uchar digit);

#endif
