#ifndef MOTION
#define MOTION

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

#define MOTION_PART_NB			8U		// 8 superposables motions of 1 pot/switch
#define MOTION_PPQN				24U 	// 24 ticks per quarter of note
#define MOTION_PART_LENGHT		64U		// memory size (byte) for 1 motion part
#define MOTION_LENGHT_NB		8U		// possibles lenghts are 0 to 7
#define MOTION_CLOCK_STEP		6U		// motion sampled each 6 ticks
#define MOTION_PTS_PER_LENGHT	(MOTION_PART_LENGHT / MOTION_LENGHT_NB)			

// motion_length		0		1		2		3		4		5		6		7
// LENGHT (measure)		1/2		1		1 1/2	2		2 1/2	3		3 1/2	4
// NB ticks received	48		96		144		192		240		288		336		384
// Clock step 			3 fixed
// Freq @ 120 BPM (ms)  62.4 = 16 Hz fixed
// Nb points			16		32		48		64		80		96		112		128
// Clock step 			6 fixed
// Freq @ 120 BPM (ms)  124.8 = 8 Hz fixed
// Nb points			8		16		24		32		40		48		56		64


///////////////////////////// MOTION STRUCTURE DEFINITION /////////////////////////////////
// DO NOT CHANGE ORDER : THESE 2 FIELDS MUST BE CONITGUOUS SINCE SAVE INTO EEPROM
typedef struct {
uchar motion_clock_counter;			// count from 0 to clock step on each tick and send/record midi when equal
uchar motion_last_data_sent;		// last data sent when playing a motion
uchar motion_data_index;			// index on data to be played/recorded
uchar motion_ctrl_index;			// Numero of the control index associated to the montion sampler
uchar motion_length;				// lenght of the motion part is from 0 to 7
uchar motion_data_table[MOTION_PART_LENGHT];
} Motion_Struct ;

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckRealtimeMotion(uchar realtime_data);
void CheckMotionRecording(void);
void SaveMotionToEEprom(void);

#endif
