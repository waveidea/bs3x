#ifndef HARDWARE
#define HARDWARE


///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define PORT_ALT_FCT			2U
#define PORT_DIR				1U
#define TEN						0x80U	// timer enable bit mask

#define CTRL_UNDEF				0xFFU

#define GENERIC_BUFF_SIZE		128U	// V1.5 128U

#define LED_MIDI_OUT_PERSISTENCY	4U	// Led persictency in multiple of 10ms

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void rom_to_lcd_text_copy(rom uchar *src);
void InitIos(void);
void InitTimers(void);
void ReinitTimer1(void);
void ReinitTimer2(void);
void ReinitTimer3(void); // V1.8
void InitInitialValues(void);
void delay_usec(uchar delay_time);
void delay_msec(unsigned int delay_time);
void CheckParamChangedFromItr(void);
void CheckCurrentActiveFunction(void);
void CheckCtrlRegToSave5Sec(void);
void CheckCtrlRegToSendToEz(void);
void SyncToEzBoard(void);
void InitParamStructsInRam(void); // V1.61
void UpdateOperatingMode(void);

#endif