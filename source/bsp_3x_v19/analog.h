#ifndef ANALOG
#define ANALOG


// #define INFRARED
#define RIBBON

///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define INFRARED_AD_CHANNEL			8U
#define JOYSTICK_X_AD_CHANNEL		5U
#define JOYSTICK_Y_AD_CHANNEL		6U
#define CROSSFADER_AD_CHANNEL		9U
#define AUTOMATION_POT_0_AD_CHANNEL	7U
#define AUTOMATION_POT_1_AD_CHANNEL	10U
#define AUTOMATION_POT_2_AD_CHANNEL	11U
#define ADC_NB						12U

// Joystick incrust area definitions
#define JOYSTICK_AREA_X_MIN			LCD_ZONE_INCRUST_X
#define JOYSTICK_AREA_X_MAX			LCD_ZONE_INCRUST_X + 64U
#define JOYSTICK_AREA_Y_MIN			LCD_ZONE_INCRUST_Y
#define JOYSTICK_AREA_Y_MAX			LCD_ZONE_INCRUST_X + 64U

#define	JOYSTICK_SW	 5U
#define	FOOTSWITCH	 6U
#define	ENCODER_SW	 7U

enum { ROW0, ROW1, ROW2, ROW3, ROW4 }
#define POTS_ROW_NB		5U

#define HOOK_BIT			0x80U
#define POSITIVE_BIT		0x80U
#define NEGATIVE_BIT		0x00U

#define MIN_INFRARED_VALUE	10U		// infared offset to remove for noise suppression

#define SHIFT_STEP_SIZE		5U		// when shift active : inc/decr sw by step size or 1

// how analog data from itr are organized (Analog[] table base indexes)
#define POTS_ANALOG_INDEX      		0
#define JOYSTICK_X_ANALOG_INDEX     (POTS_ROW_NB*CTRL_PER_ROW) // 40
#define JOYSTICK_Y_ANALOG_INDEX		((POTS_ROW_NB*CTRL_PER_ROW)+JOYSTICK_NB) // 41
#define AUTOM_POT_0_ANALOG_INDEX	((POTS_ROW_NB*CTRL_PER_ROW)+JOYSTICK_NB+JOYSTICK_NB) // 42
#define INFRARED_ANALOG_INDEX		((POTS_ROW_NB*CTRL_PER_ROW)+JOYSTICK_NB+JOYSTICK_NB+LFO_NB) // 43
#define CROSSFADER_ANALOG_INDEX		((POTS_ROW_NB*CTRL_PER_ROW)+JOYSTICK_NB+JOYSTICK_NB+LFO_NB+INFRARED_NB) // 44
#define AUTOM_POT_1_ANALOG_INDEX	((POTS_ROW_NB*CTRL_PER_ROW)+JOYSTICK_NB+JOYSTICK_NB+LFO_NB+INFRARED_NB+CROSSFADER_NB) // 45
#define AUTOM_POT_2_ANALOG_INDEX	((POTS_ROW_NB*CTRL_PER_ROW)+JOYSTICK_NB+JOYSTICK_NB+LFO_NB+INFRARED_NB+CROSSFADER_NB+LFO_NB) // 46

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitAnalog(void);
void Get7BitsADValue(uchar next_channel, uchar next_mux);
void CheckPots(void);
uchar CheckJoystickOneAxisSendMidi(uchar joystick_axis_analog_index); // V1.61
void CheckJoystick(void);
void CheckInfrared(void);
void CheckCrossFader(void);
//void CalibrateAdc(void);
uchar CheckHookPot(uchar *param_value, uchar physical_value, uchar new_pot_value);

#endif