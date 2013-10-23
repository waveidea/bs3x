#ifndef MENUS
#define MENUS

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////

// Nb of menus lines to display on LCD
#define MENUS_MAX_DEPTH         4U
#define UNDEFINED_VALUE         255U
#define NO_BIT                  255U
#define MUST_TOGGLE_SINGLE_BIT  255U

#define MIDI_OUTS_NB    		3U // out1,2, usb out
#define MERGER_OUTS_NB  		2U // out1, usb out
#define MIDI_INS_NB     		2U // in, usb

// Display value type
enum {DISP_NUMERIC, DISP_NUMERIC_PLUS_1, DISP_STRING, DISP_NUMERIC_PLUS_30};
enum {READ_REG_VALUE, WRITE_REG_VALUE};

// different menus depht
enum {TOP_LEVEL_MENU, MENU_DEPTH_1, MENU_DEPTH_2, MENU_DEPTH_3 };
// Param of key pressed
enum {UP_PRESSED, DOWN_PRESSED};
// Action associated to Up & Down keys
enum {CHANGE_MENU_INDEX, CHANGE_MENU_VALUE};
// The different tables IDs affected by menus
enum {SUB_MENU, TABLE_GLOBAL_PARAMS, TABLE_CTRL_ASSIGN};

#define MIDI_CFG_PARAM_NB 7U // channel, merger, filter, realtime, shift-lock-channel, shift-lock_midi,mackie fader feedback,  V1.4 V1.69 5U
#define MERGE_PARAM_NB    2U // sources, destinations
#define LFO_PARAM_NB      3U // state, waveform, sync
#define ARPP_PARAM_NB     3U // state, hold, sync
#define MOTION_PARAM_NB   3U // state, loop, sync

/////////////////////// THE GENERIC MENU STRUCTURE //////////////////////////////////////
typedef struct {
	rom uchar MenuStr[16U];    	// Menu name to display on LCD
//	uchar MenuStr[16];    	// Menu name to display on LCD
    uchar menu_depth;               // the menu depth level
	uchar value_max;				// max value for current variable to modify (nb of submenus if submenu)
	uchar value_index;      		// index into table of registers to modify (all var to modify are into a table)
    uchar table_id;                 // id on the table to modify
    uchar bit_pos_msk;              // Bit(s) to modify on byte value_index of table_id (mask) 
	uchar display_type;				// DISP_STR, DISP_VAL, DIPS_VAL_1 (how to display the value)
	rom uchar *MenuValueStr;		// String to be displayed instead of the numerical value, if display_type = DISP_STR
	rom uchar *GlobalMenuBmp;     // Bmp associated to the global menu selected
//	uchar *MenuValueStr;		// String to be displayed instead of the numerical value, if display_type = DISP_STR
//	uchar *GlobalMenuBmp;     // Bmp associated to the global menu selected
	uchar SubMenuBaseIndex;			// index of the first submenu for this menu
    uchar NextMenuExist;            // TRUE if a next menu exist on same depth
} MENU;

// LCD MENUS POSITIONNING
#define LCD_MENUS_BASE_X			10U			// Must be 0-127
#define LCD_MENUS_MAX_X_PIXEL		100U		// Must be 0-127
#define LCD_MENUS_BASE_Y			2U			// Must be 0-7
#define LCD_MENUS_VALUE_X			(LCD_MENUS_BASE_X+5U)			// Must be 0-127
#define LCD_MENUS_VALUE_Y			7U			// Must be 0-7
#define LCD_MENUS_TOP_LEVEL_X		10U			// Must be 0-127
#define LCD_MENUS_TOP_LEVEL_BMP_X	50U			// Must be 0-127
#define LCD_MENUS_TOP_LEVEL_BMP_Y	3U			// Must be 0-127
#define LCD_MENUS_TITLE_BMP_X		64U			// Must be 0-127
#define LCD_MENUS_TITLE_BMP_Y		0U			// Must be 0-127
#define LCD_SELECT_LINE_ARROW_X_MAX 6U
#define LCD_MENUS_BMP_X_MAX			24U
#define LCD_MENUS_BMP_Y_MAX			3U

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void 	CheckMenuSwitches(void);
void 	MenuEnter(void);
void 	DisplayMenus(void);
uchar 	GetBitPositionFromMask(uchar bits_mask, uchar action);
uchar 	MenuGetValue(void);
void 	MenuSetValue(uchar next_value);
void 	DisplayMenuValue(void);
void 	RemoveLcdMenuLine(void);
void 	AddLcdMenuLine(uchar current_lcd_line);
void 	DisplaySelectedLine(void);
void 	ButtonCancelClick(void);
void 	ButtonOkClick(void);
void 	ChangeCurrentMenu(uchar direction);
void 	ExitMenusMode(void);
#endif