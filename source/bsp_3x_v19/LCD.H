#ifndef LCD
#define LCD

///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define CS1_MAX_X 		63U 		// chip 1 x range 0 - 63 
#define CS2_MAX_X 		127U 	// chip 2 x range 64 - 127 
#define LCD_X_PIXEL_MAX	128U
#define LCD_Y_PAGE_MAX	8U


#define DISPLAY_ON 		0x3FU 	// LCD Display ON 
#define DISPLAY_OFF 	0x3EU 	// LCD Display OFF 
#define SET_Y 			0x40U 	// LCD SET Y command, add y - 0 to 63 (line) 
#define SET_X 			0xB8U 	// LCD SET X command, add x - 0 to 7  (page) 
#define SET_RAM_0 		0xC0U 	// LCD RAM, used for scrolling add y - 0 to 63 

// Bits Mask to access LCD commands

#define DIGITS_2			0x00U
#define DIGITS_3			(1<<0U)
#define LCD_COMMAND			0x00U
#define LCD_DATA			(1<<1U)
#define LCD_NORMAL			0x00U
#define LCD_INVERT			(1<<2U)
#define FONT_5x8_FLG		0x00U		// V1.66
#define FONT_NUM_8x16_FLG	(1<<3U)		// V1.66
#define FONT_7SEG_5x8_FLG	(1<<4U)		// V1.66

#define LCD_LEFT			0U
#define LCD_RIGHT			64U

#define BUSY_FLAG			0x80U

#define INIT_LCD_CHANNEL		(1<<0U)
#define INIT_LCD_EXPANSION		(1<<1U)
#define INIT_LCD_FUNCTION		(1<<2U)
#define INIT_LCD_SCENE			(1<<3U)
#define INIT_LCD_GROUP			(1<<4U)
#define INIT_LCD_AUTOMATION_TXT	(1<<5U)
#define INIT_LCD_LFO_WAVE_STATE	(1<<6U)
#define INIT_LCD_ARPP_STATE		(1<<7U)
#define INIT_LCD_SYNC24			(1<<8U)
#define INIT_LCD_MOTION_STATE	(1<<9U)
#define INIT_LCD_SW_STATE		(1<<10U)
#define INIT_LCD_INFRARED		(1<<11U)
#define INIT_LCD_USB			(1<<12U)
#define INIT_LCD_TRANSPORT		(1<<13U)
#define INIT_LCD_IR_GRAPH		(1<<14U)
#define INIT_LCD_MODE			(1<<15U)

///////////////////////////////////////////////////////////
// LCD ZONES SPECIFICATION
///////////////////////////////////////////////////////////
#define LCD_ZONE_CHANNEL_X			2U			// Must be 0-127
#define LCD_ZONE_CHANNEL_Y			0U			// Must be 0-7
#define LCD_ZONE_SYNC24_X			117U			// Must be 0-127
#define LCD_ZONE_SYNC24_Y			2U			// Must be 0-7
#define LCD_ZONE_VALUE_X			32U			// Must be 0-127
#define LCD_ZONE_VALUE_Y			0U			// Must be 0-7
#define LCD_ZONE_SCENE_X			73U			// Must be 0-127
#define LCD_ZONE_SCENE_Y			0U			// Must be 0-7
#define LCD_ZONE_GROUP_X			2U			// Must be 0-127
#define LCD_ZONE_GROUP_Y			2U			// Must be 0-7
#define LCD_ZONE_AUTOMATION_TXT_X	28U			// Must be 0-127
#define LCD_ZONE_AUTOMATION_TXT_Y	2U			// Must be 0-7
#define LCD_ZONE_LFO_WAVE_STATE_X	27U			// Must be 0-127
#define LCD_ZONE_LFO_WAVE_STATE_Y	3U			// Must be 0-7
#define LCD_ZONE_ARPP_TXT_X			48U			// Must be 0-127
#define LCD_ZONE_ARPP_TXT_Y			2U			// Must be 0-7
#define LCD_ZONE_ARPP_STATE_X		49U			// Must be 0-127
#define LCD_ZONE_ARPP_STATE_Y		3U			// Must be 0-7
#define LCD_ZONE_MOTION_STATE_X		71U			// Must be 0-127
#define LCD_ZONE_MOTION_STATE_Y		3U			// Must be 0-7
#define LCD_ZONE_INFRARED_X			106U			// Must be 0-127
#define LCD_ZONE_INFRARED_Y			2U			// Must be 0-7
#define LCD_ZONE_EXPANSION_X		106U			// Must be 0-127
#define LCD_ZONE_EXPANSION_Y		0U			// Must be 0-7
#define LCD_ZONE_USB_X				117U			// Must be 0-127
#define LCD_ZONE_USB_Y				0U			// Must be 0-7
#define LCD_ZONE_GROUP_NAME_X		0U			// Must be 0-127
#define LCD_ZONE_GROUP_NAME_Y		4U			// Must be 0-7
#define LCD_ZONE_SW_STATE_X			2U			// Must be 0-127
#define LCD_ZONE_SW_STATE_Y			6U			// Must be 0-7
#define LCD_ZONE_OTHER_SW_STATE_X	2U			// Must be 0-127
#define LCD_ZONE_OTHER_SW_STATE_Y	5U			// Must be 0-7
#define LCD_ZONE_TRANSPORT_X		106U			// Must be 0-127
#define LCD_ZONE_TRANSPORT_Y		4U			// Must be 0-7
#define LCD_ZONE_STRING_X			0U			// Must be 0-127
#define LCD_ZONE_STRING_Y			7U			// Must be 0-7
#define LCD_ZONE_MODE_X				108U			// Must be 0-127
#define LCD_ZONE_MODE_Y				7U			// Must be 0-7
#define LCD_ZONE_IR_GRAPH_X			99U			// Must be 0-127
#define LCD_ZONE_IR_GRAPH_Y			0U			// Must be 0-7
#define LCD_ZONE_HOOK_DIRECTION_X	61U
#define LCD_ZONE_HOOK_DIRECTION_Y	0U
#define LCD_ZONE_CURVE_X			26U
#define LCD_ZONE_CURVE_Y			0U

#define LCD_ZONE_IR_GRAPH_Y_PAGE	6U	// Must be 0-127
#define LCD_IR_BARGRAPH_MAX			54U //46

#define LCD_ZONE_INCRUST_X			1U
#define LCD_ZONE_INCRUST_Y			2U
#define LCD_ZONE_INCRUST_X_PIX		96U
#define LCD_ZONE_INCRUST_Y_PAGE		5U

#define LCD_ZONE_FW_VERSION_HOME_X	105U
#define LCD_ZONE_FW_VERSION_HOME_Y	5U
//#define LCD_ZONE_NB_EXP_HOME_X		108U	// V1.6
//#define LCD_ZONE_NB_EXP_HOME_Y		5U		// V1.6


#define LCD_JOY_POS_PIX_SQUARE			3U
#define LCD_JOY_VERT_PIX				((1U<<LCD_JOY_POS_PIX_SQUARE)-1U)
#define LCD_JOY_INITIAL_VERTICAL_VALUE	(unsigned long)((LCD_JOY_VERT_PIX) << (32U-LCD_JOY_POS_PIX_SQUARE))
#define LCD_ZONE_JOY_BORDER_LEFT_X		4U
#define LCD_ZONE_JOY_BORDER_LEFT_Y		2U
#define LCD_ZONE_JOY_MIN_X				LCD_ZONE_JOY_BORDER_LEFT_X + 4U
#define LCD_ZONE_JOY_BORDER_RIGHT_X		(LCD_ZONE_JOY_MIN_X + 48U + LCD_JOY_POS_PIX_SQUARE - 1U)
#define LCD_ZONE_JOY_BORDER_RIGHT_Y		2U

#define LCD_ZONE_FADER_START			13U
#define LCD_ZONE_FADER_LENGHT			64U
#define LCD_ZONE_FADER_KNOB_SIZE		3U
#define LCD_ZONE_FADER_END				(LCD_ZONE_FADER_START + LCD_ZONE_FADER_LENGHT + LCD_ZONE_FADER_KNOB_SIZE + 1U)
#define LCD_ZONE_FADER_PAGE				4U

#define LCD_ZONE_LFO_INCRUST_OFFS_X		(LCD_ZONE_INCRUST_X+8U)
#define LCD_ZONE_LFO_INCRUST_FREQ_X		(LCD_ZONE_INCRUST_X+34U)
#define LCD_ZONE_LFO_INCRUST_AMPL_X 	(LCD_ZONE_INCRUST_X+56U)
#define LCD_ZONE_LFO_INCRUST_WAV_X		(LCD_ZONE_INCRUST_X+77U)
enum {FORCE_AMPL_DISPLAY, FORCE_FREQ_DISPLAY, FORCE_OFFSET_DISPLAY, FORCE_WAV_DISPLAY};
enum {FORCE_RYTHM_MELODY_DISPLAY, FORCE_ACC_TRANSP_DISPLAY, FORCE_GATE_RANGE_DISPLAY, 
	  FORCE_ARP_DIR_DISPLAY, FORCE_ARP_MODE_DISPLAY};
enum {FORCE_MOTION_PART_DISPLAY, FORCE_MOTION_LENGTH_DISPLAY, FORCE_MOTION_STATES_DISPLAY, FORCE_MOTION_REC_DISPLAY};
#define LCD_ZONE_ASSIGN_TEXT_X			60U
#define LCD_ZONE_ASSIGN_TEXT_Y			5U
#define LCD_ZONE_ASSIGN_TEXT_X_LENGHT	63U

// V1.4 small bmp for shift button
#define LCD_ZONE_SHIFT_BMP_X			94U		// Must be 0-127
#define LCD_ZONE_SHIFT_BMP_Y			0U		// Must be 0-7

#define SW_STATE_WIDTH					12U

#define FONT_6x8_BDR_LEFT_INV			('~'+1U)
#define FONT_6x8_BDR_RIGHT_INV			('~'+2U)

#define LCD_7SEG_FONT_LENGHT 			(' '+64U)

#define FILL_BLANK						0xFFU

#define FONT_START_OFFSET				4U // bytes used to format the font
#define BMP_START_OFFSET				2U // bytes used to format the bmp

enum {NO_HOOK_BMP, HOOK_DOWN_BMP, HOOK_UP_BMP, CTRL_CHAIN_BMP}
#define HOOK_FADER_BMP					fader_hook_bmp_state_z4

#define	must_display_part 				(1<<0)
#define	must_display_length				(1<<1)
#define	must_display_all_states 		(1<<2)
#define	must_display_single_state		(1<<3)
#define must_display_rec_text			(1<<4)
#define must_display_part_bar			(1<<5)

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
//void Display_Upgrade(void);
void UpdateSwState(uchar sw_index);
void UpdateOtherSwState(uchar sw_index);
void UpdateLcdTransport(void);
void UpdateLcdFunction(void);
// void DisplayBargraphMotion(void);
void UpdateLcdZones(int init_zones);
void DisplayRomVersion(uchar x, uchar y);
void UpdateLCDJoystickArea(void);
uchar RescaleIrBargraphValue(uchar value, uchar *y_page_full);
void UpdateLCDInfraredBargraph(void);
void UpdateLcdString(void);
void UpdateLCDCrossfaderArea(void);
void CreateStdLcdStrFromCtrlIndex(void);
void UpdateLcdHookState(uchar next_hook_state);
void UpdateLcdFaderHookState(uchar next_hook_state);
void DisplayFullLfoLcdIncrust(uchar force_display);
void DisplayInputOverflowIncrust(void);
void DisplayEmptyIncrust(void);
void DisplayCurrentValue(void);
void UpdateLcdCurveModel(uchar next_curve_model);
void Display_Invalid_Sysex_Incrust(void);
void Display_Invalid_Midi_Struct_Incrust(void);
void DisplayFullArppLcdIncrust(uchar force_display);
void DisplaySmallBargraphValue(uchar x_pos, uchar value);
void DisplayFullMotionLcdIncrust(uchar force_display);
void DisplayMotionSmallState(uchar motion_part_display);
void DisplayMotionRecGraph(uchar current_motion_part, uchar data_index);
void DisplayBs3xInformations(void); // V1.4
void CheckDisplayCurrentValueLocked(void); // V1.62
void DisplayTempo(void);	// V1.69
void DisplaySceneTypeBitmap(void); // V1.8

#endif