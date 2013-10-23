#ifndef ITRS
#define ITRS

#define ENCODER_LOCK_CPT				1U  // encoder lock time in multiple of 10 ms
#define SWITCH_LOCK_CPT					15U // switch  lock time in multiple of 10 ms
#define VALID_STATE_CPT					8U	// checks to perform before declaring a valid level


#define MIDI_CHANNEL_OVERRIDE_PERSIST	140U // V1.62 multiple of 10ms

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitItrs(void);
void UpdateFunctionsToIncrease(void);
void UpdateFunctionsToDecrease(void);
void CheckSnapshotState(void);
void CheckInfraredSwitchState(void);


#endif