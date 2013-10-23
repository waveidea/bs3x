#ifndef HARDWARE_H		//Header sentry
#define HARDWARE_H

// DEFINES
#define USB_LED_PERSISTENCY		10	// in multiple of 2ms

// PROTOTYPING
void Init_Variables(void);
void Init_IOs(void);
void SyncToZ8Board(void);
//void Check_InitialSwitchesState(void);

#endif
