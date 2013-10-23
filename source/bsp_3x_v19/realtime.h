#ifndef REALTIME
#define REALTIME

#define REALTIME_BUFFER_LENGHT	32	// V1.8
#define MAX_TEMPO_MINUS_30		220

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckRealtimeMidi(void);
void UpdateSmpteState(uchar next_transport_state); // V1.8

#endif