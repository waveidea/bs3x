#ifndef LFO
#define LFO

// EXTERNAL MIDI CLOCK SYNCRONISATION TO LFO
// frequency is converted from 0 - 127
// Rigth part of pot is for internal syncro with freq from 0 to 63
// Left  part of pot is for external syncro with the following features
// 56-63 : external sync EX7: 2     period/mesure (1 point each 3   MIDI clock) + DECIM by 2
// 48-55 : external sync EX6: 1     period/mesure (1 point each 3   MIDI clock)
// 40-47 : external sync EX5: 1/2   period/mesure (1 point each 6   MIDI clock)
// 32-39 : external sync EX4: 1/4   period/mesure (1 point each 12  MIDI clock)
// 24-31 : external sync EX3: 1/8   period/mesure (1 point each 24  MIDI clock)
// 16-23 : external sync EX2: 1/16  period/mesure (1 point each 48  MIDI clock)
// 8-15  : external sync EX1: 1/32  period/mesure (1 point each 96  MIDI clock)
// 0-7   : external sync EX0: 1/64  period/mesure (1 point each 192 MIDI clock)

///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////

#define LFO_MIN_OFFSET        32U
#define LFO_NB_PT_PERIOD      24U // V1.7 32
#define LFO_Q0_END            (LFO_NB_PT_PERIOD >> 2U)
#define LFO_Q1_END            (LFO_NB_PT_PERIOD >> 1U)
#define LFO_Q2_END            ((3U * LFO_NB_PT_PERIOD) >> 2U)
#define LFO_Q3_END            (LFO_NB_PT_PERIOD)
#define LFO_WAV_TAB_INDEX_MAX ((LFO_NB_PT_PERIOD >> 2U) - 1U)
#define LFO_WAVE_NB			  4U


enum {INT_LFO_SYNC, EXT_LFO_SYNC};  					// Used by LFO synchro
enum {LFO_WAV_POS, LFO_WAV_NEG};    					// Used to get wave index and value
enum {LFO_WAV_SLOPE_POS, LFO_WAV_SLOPE_NEG};    		// Used to get wave index and value
enum {LFO_SINE, LFO_TRIANGLE, LFO_SQUARE, LFO_RANDOM};	// all lfo waves

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckRealtimeLfo(uchar realtime_data);
void CheckLfoToSend(void);	
void GetLfoWaveValue(void);
void ReloadLfoFreqCpt(void);
uint rand(void);

#endif