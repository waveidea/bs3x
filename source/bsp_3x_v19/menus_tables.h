#ifndef MENUS_TABLES
#define MENUS_TABLES

// Strings values for menus  {offset of first substr, offset of second str, ..., str1, str2, ...}
#define ON_OFF_NB         2U
rom uchar OffOnTxtPtr[]  		= { ON_OFF_NB,6,"OFF\0ON\0"}; //"OFF","ON"};
#define LFO_WAVES_NB      4U
rom uchar LfoWaveTypePtr[]		= { LFO_WAVES_NB,9,18,25,"Sine\0Triangle\0Square\0Random\0" }; //"Sine", "Triangle", "Square", "Random" };
//#define MODES_NB          2U
//rom uchar ModesTxtPtr[]      	= { MODES_NB,11,"Standard\0User-defined\0" }; //"Standard", "User-defined"};
//V1.69 #define MODES_NB          5U	// v1.69
//V1.7 rom uchar ModesTxtPtr[]      	= { OPERATING_MODE_NB,14,21,27,34,"Standard\0Reason\0Logic\0Mackie\0User-defined\0" }; //"Standard", "User-defined"}; // v1.5
rom uchar ModesTxtPtr[]      	= { OPERATING_MODE_NB,14,21,28,41,"Standard\0Reason\0Mackie\0User-defined\0Logic\0" }; //"Standard", "User-defined"}; // v1.5
#define FILTER_PARAM_NB   3U
rom uchar FilterTxtPtr[]		= { FILTER_PARAM_NB,15,31,"Filter All\0Filter Realtime\0Disable Filter\0"}; //"Filter All", "Filter Realtime", "Disable Filter"};


#define VIDEO_MODE_NB     2U
rom uchar VideoModeTxtPtr[] 	= { VIDEO_MODE_NB,9,"Normal\0Inverted\0" };
#define FILTER_TYPE_NB    2U
rom uchar FilterTypeTxtPtr[]	= { FILTER_TYPE_NB,18,"Realtime Events\0All Events\0"};
//rom uchar MidiInsTxtPtr[]		= { MIDI_INS_NB,10,"Midi In\0Usb In\0"};
#define TRANSPORT_TRIG_NB 	3U  // transport submenus : trig LFO, trig Motion, trig sync-24	
#define SYNC24_NB		  	2U  // V1.3 2U  // sync24 sub menus : state, source, sync // sync-24 source removed in V1.5
#define REALTIME_PARAM_NB	4U  // realtime sub menus : internal state, external source, tempo, tempo mode // V1.69 3U
//#define REALTIME_SRC_NB	  3U // midi in, usb in, internal
//rom uchar RealtimeSrcTxtPtr[]	= { REALTIME_SRC_NB,11,18,"Midi In\0Usb In\0Internal\0"};
#define REALTIME_SRC_NB	  2U // midi in, usb in
rom uchar RealtimeSrcTxtPtr[]	= { REALTIME_SRC_NB,10,"Midi In\0Usb In\0"};
#define SYNC_TYPE_NB	  2U // internal / external
rom uchar SyncTypeTxtPtr[]	= { SYNC_TYPE_NB,11,"Internal\0External\0"};

// Strings values for menus  {offset of first substr, offset of second str, ..., str1, str2, ...}
#define	ASSIGN_STD_PARAM_NB  	2U // assign std sub menus : event type, event numero
#define	ASSIGN_STD_EVT_TYPE_NB 	2U
rom uchar StdEventTxtPtr[] = { ASSIGN_STD_EVT_TYPE_NB,17,"Control Change\0Note On\0" };

#define	ASSIGN_USR_PARAM_NB  	6U 	// assign user sub menus : midi delay, curves, min, max, 
									// chain crossfader en, chained ctrl

#define ASSIGN_USR_MIDI_DELAY_NB      4U
rom uchar MididelayTxtPtr[]		= { ASSIGN_USR_MIDI_DELAY_NB,9,15,21,"None\0 30ms\0 60ms\0 90ms\0" };
#define ASSIGN_USR_CURVE_NB      7U
rom uchar CurveModelTxtPtr[]	= { ASSIGN_USR_CURVE_NB,14,22,26,34,41,48,"Linear\0Inv lin\0Log\0Inv log\0Random\0User 1\0User 2\0" };

#define	ASSIGN_COM_PARAM_NB   6U // assign common sub menus : switch_ctrl_mode, scene_exclude 
								 // autosend, midi_out_1_en, midi_out_2_en, usb_out_en

#define	ASSIGN_COM_SW_MODE_NB 	2U
rom uchar SwModeTxtPtr[] = { ASSIGN_COM_SW_MODE_NB,7,"Push\0Toggle\0" };
#define ASSIGN_ANALOG_MODE_NB    2U
rom uchar AnalModeTxtPtr[] 		= { ASSIGN_ANALOG_MODE_NB,7,"Jump\0Hook\0"};

rom uchar ArpBanksTxtPtr[]	= { ARP_RHYTHM_BNK_NB,11,24,31,"Master\0Techno/House\0Breaks\0Latin\0" };
rom uchar ArpModesTxtPtr[]	= { ARPP_MODE_NB,11,16,26,"Normal\0Hold\0Auto trig\0Keyboard\0" };
rom uchar ArpClkResoTxtPtr[]= { ARP_CLK_RESO_NB,12,17,23,29,35,43," 4th\0 8th\0 16th\0 32th\0 64th\0 8 trip\0 16 trip\0" };

// V1.62
#define	MISC_PARAM_NB   2U // video mode, shift mode

// V1.69
#define	TEMPO_UPDT_MODE_NB 	2U
rom uchar TempoUpdtModeTxtPtr[] = { TEMPO_UPDT_MODE_NB,11,"One shot\0Continuous\0" };

// V1.8
#define INT_SYNC_MODE_NB	  2U
rom uchar IntSyncModesTxtPtr[]  = {INT_SYNC_MODE_NB,13,"MIDI Clock","SMPTE"};

// V1.8
rom uchar SmpteFrameRateTxtPtr[]  = { SMPTE_FRAME_RATE_NB,10,16,23,"24FPS","25FPS","30FPSD","30FPS" };

/////////////////////// MENUS BITMAPS //////////////////////////////////////

rom uchar midi_setup_bmp[] = { 24,3,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x40,0xC0,0x40,0x40,0xC0,0x40,0x80,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x0C,0xC2,0xC1,0x00,
0x00,0x00,0x03,0x02,0x02,0x03,0x00,0x00,0x00,0xC1,0xC2,0x0C,0xF0,0x00,0x00,0x00,
0x00,0x00,0x00,0x03,0x0C,0x10,0x20,0x4C,0x4C,0x80,0x80,0xB0,0xB0,0x80,0x80,0x4C,
0x4C,0x20,0x10,0x0C,0x03,0x00,0x00,0x00
};

rom uchar operating_mode_bmp[] = {24,3,
0x00,0x00,0xC0,0xE0,0xD0,0xB0,0x70,0xF0,0xE0,0xC0,0x00,0x00,0x00,0x00,0xC0,0xE0,
0xD0,0xB0,0x70,0xF0,0xE0,0xC0,0x00,0x00,0x00,0x00,0xE3,0xA7,0xAF,0x0F,0x2F,0xEF,
0x27,0x03,0xE0,0x20,0xC0,0x00,0x83,0xC7,0xEF,0x8F,0x8F,0x8F,0x87,0x03,0x00,0x00,
0x00,0x00,0x02,0x3A,0x23,0x20,0xF8,0x73,0x20,0x00,0x03,0xFA,0x81,0xF8,0x00,0xB9,
0xAB,0xE8,0x00,0xF8,0x2B,0xD8,0x00,0x00
};

rom uchar video_mode_bmp[] = {24,3,
0x00,0x00,0xF0,0x10,0x10,0x90,0x50,0xD0,0xD0,0xD0,0x90,0x10,0x10,0x10,0x10,0x90,
0x10,0x10,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0x00,0xFF,0x80,0x8F,0x9F,0xBF,0x3E,
0xBD,0xBF,0x9F,0x8F,0xC0,0xE2,0x67,0x2F,0x22,0x3E,0x20,0x60,0xE0,0xFF,0x00,0x00,
0x00,0x00,0x7F,0x7F,0x7F,0x7F,0x7F,0x78,0x7B,0x60,0x71,0x7B,0x7F,0x70,0x60,0x41,
0x42,0x44,0x40,0x60,0x70,0x7F,0x00,0x00
};

rom uchar group_bmp[] = { 24,3,
0x00,0x00,0x80,0xC0,0xA0,0x60,0xE0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x0F,0x1F,0xDF,0x1E,0x1F,
0x0F,0x07,0x00,0x01,0x01,0x01,0x01,0x89,0x99,0xBF,0x98,0x08,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x0F,0x08,0x08,0x08,0x3E,0x1C,0x08,0x00,0x1E,0x3F,0x7F,
0x7F,0x77,0x7B,0x3D,0x1E,0x00,0x00,0x00
};

rom uchar scene_sel_bmp[] = {24,3,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x80,0xC0,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0xFC,0xF6,0xEE,0xDE,0xBE,
0xFE,0xFC,0xF8,0xF0,0x00,0x40,0xFF,0x41,0x41,0x00,0xD4,0x57,0x74,0x00,0x00,0x00,
0x00,0x01,0x03,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x07,0x03,0x01,0x00,0x00,0x1F,0x10,
0x10,0x00,0x11,0x01,0x11,0x00,0x00,0x00
};

rom uchar infrared_bmp[] = {24,3,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x20,0x60,0x60,0x60,0x20,0xA0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x60,0xF0,0x64,0x42,
0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x42,0xC4,0xF0,0xC0,0x40,0x00,0x00,
0x00,0x00,0x00,0x40,0x50,0x55,0x50,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x40,0x50,0x55,0x50,0x40,0x00,0x00
};

rom uchar sync24_bmp[] = {24,3,
0x00,0x00,0x00,0x80,0x40,0x20,0xE0,0xA0,0xE0,0x20,0x40,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x44,0x90,0x80,0xA0,
0x80,0x90,0x44,0x20,0x1F,0x00,0x04,0x04,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0xF0,0x10,0x10,0xF0,0x81,0x83,
0xF7,0x13,0x11,0xF0,0x80,0x80,0x00,0x00
};

rom uchar automation_bmp[] = {24,3,
0x00,0x00,0x00,0x80,0xC0,0xA0,0xC0,0x80,0x80,0x80,0x80,0x00,0x80,0xE0,0xA0,0xE0,
0x80,0xE0,0xA0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x88,0x88,0x88,0xC9,
0xEA,0xD9,0x98,0x98,0xF8,0xE8,0x88,0xAB,0x8A,0x8B,0x88,0x88,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0x88,0x88,0xC8,0xC9,0xA9,0xB9,0x98,0x8C,0x8F,0x8B,0x08,0x8A,
0x08,0x88,0x08,0x88,0x00,0x00,0x00,0x00
};
/* removed in V1.8
rom uchar analog_mode_bmp[] = {24,3,
0x00,0x00,0x80,0xC0,0xA0,0x60,0xE0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x80,0xC0,
0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x0F,0x5F,0x5F,0x5E,0x1F,
0x0F,0x07,0x20,0x40,0x80,0x00,0x00,0xAA,0xAA,0xAA,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x10,0x35,0x75,0x35,0x10,0x00,0x00,0x00,0x00,0x00,0x1E,0x3F,0x7F,
0x7F,0x77,0x7B,0x3D,0x1E,0x00,0x00,0x00
};
*/

rom uchar transport_bmp[] = {24,3,
0x00,0x00,0x00,0xF0,0xE0,0xC0,0x80,0x00,0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0x00,
0xE0,0xE0,0xE0,0xE0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x13,0x11,0x10,0x10,
0x10,0x93,0x93,0xF0,0x93,0x93,0x10,0x10,0x13,0x13,0x13,0x13,0x13,0x00,0x00,0x00,
0x00,0x00,0x00,0x20,0x30,0x28,0x30,0x60,0xA0,0x60,0x21,0x03,0x21,0x38,0x28,0xF8,
0xA0,0xF8,0x28,0x20,0x00,0x00,0x00,0x00
};

////////////////////////// all menus and sub menus indexes ///////////////////////////////
enum {
// TOP LEVEL MENUS
MIDI_CONFIG_INDEX, OPERATING_MODE_INDEX, GROUP_SELECT_INDEX, SCENE_SELECT_INDEX, 
INFRARED_INDEX, SYNC24_INDEX, AUTOMATIONS_INDEX, /* V1.8 ANALOG_MODE_INDEX,*/ /* TRANSPORT_TRIGG_INDEX, */
MISC_INDEX,
// SUB LEVEL MENUS
MIDI_CHANNEL_INDEX, MIDI_MERGE_INDEX, MIDI_FILTER_INDEX, REALTIME_INDEX, /*V1.4*/ SHIFT_LOCKS_MIDI,
SHIFT_LOCKS_CHANNEL, MACKIE_FAD_FBK_INDEX, // V1.69
SCENE_FROM_PRG_CH_INDEX, // V1.8
SYNC24_STATE_INDEX, /* V1.5 SYNC24_SOURCE_INDEX, */ /*V1.3*/ SYNC24_SYNC_INDEX,
LFO_INDEX, ARPPEGGIATOR_INDEX, MOTION_SAMPLER_INDEX, 
// V1.62
VIDEO_MODE_INDEX, SHIFT_MODE_INDEX,
/* TRIGG_LFO_INDEX, TRIGG_MOTION_INDEX, TRIGG_SYNC24_INDEX, */
// SUB SUB LEVEL MENUS
EXT_SYNC_SOURCE_INDEX, INTERNAL_TEMPO_INDEX, TEMPO_UPDT_MODE_INDEX, // V1.69
INT_SYNC_STATE_INDEX,
INT_SYNC_MODE_INDEX, SMPTE_FRAME_RATE_INDEX, // V1.8
MERGER_SOURCE_INDEX, MERGER_OUTPUT_INDEX, 
FILTER_STATE_INDEX, FILTER_TYPE_INDEX, 
LFO_STATE_INDEX, LFO_WAVEFORM_INDEX, LFO_SYNC_INDEX,
ARPP_STATE_INDEX, ARPP_MIDI_SRC_INDEX, ARPP_SYNC_INDEX, ARPP_DEST_INDEX ,ARPP_CHANNEL_INDEX, 
ARPP_MODE_INDEX, ARPP_VELOCITY_INDEX, ARPP_SWING_INDEX, ARPP_CLK_RESO_INDEX, 
ARPP_MELODY_BNK_INDEX, ARPP_RHYTHM_BNK_INDEX, ARPP_ACCENT_BNK_INDEX, ARPP_OFFSET_INDEX,
MOTION_STATE_INDEX, MOTION_LOOP_INDEX, MOTION_SYNC_INDEX,
// SUB SUB SUB LEVEL MENUS
MIDI_IN_INDEX, USB_IN_INDEX,
MIDI_OUT1_INDEX, USB_OUT_INDEX,
ARP_DST_MIDI_OUT1_INDEX, ARP_DST_MIDI_OUT2_INDEX, ARP_DST_USB_OUT_INDEX,
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// MENUS ASSIGN TABLES INDEX ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// TOP LEVEL MENUS
ASSIGN_STD_MODE_INDEX, ASSIGN_USR_MODE_INDEX, ASSIGN_COMMON_MODE_INDEX,
// SUB LEVEL MENUS : standard assign mode
ASSIGN_STD_EVENT_TYPE_INDEX, ASSIGN_STD_EVENT_NUM_INDEX,
// SUB LEVEL MENUS : user assign mode
ASSIGN_USR_MIDI_DELAY_INDEX, ASSIGN_USR_CURVE_INDEX, 
ASSIGN_USR_MIN_VAL_INDEX, ASSIGN_USR_MAX_VAL_INDEX,
ASSIGN_USR_CROSS_CHAIN_EN_INDEX, ASSIGN_USR_CROSS_CHAIN_INDEX,
// SUB LEVEL MENUS : common assign mode
ASSIGN_COM_AUTOSEND_INDEX, ASSIGN_COM_SCN_EXCLUDE_INDEX,
ASSIGN_COM_USB_OUT_EN_INDEX, ASSIGN_COM_MIDI_OUT1_EN_INDEX,
ASSIGN_COM_MIDI_OUT2_EN_INDEX, ASSIGN_COM_SW_MODE_INDEX,
ASSIGN_COM_ANAL_MODE_INDEX // V1.8
};

#define MENUS_ASSIGN_FIRST_INDEX 	ASSIGN_STD_MODE_INDEX	

/////////////////////// THE WHOLE MENUS STRUCTURES DEFINITION //////////////////////////////////////
rom MENU menutable[] = {
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// TOP LEVEL MENUS
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      { // index 0 [CALL TO A SUBMENU]
        "MIDI CONFIG",          // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        MIDI_CFG_PARAM_NB,      // value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        // value_index          : No table index for a global menu
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : Dynamically filled
        midi_setup_bmp,         // GlobalMenuBmp        : Associated BMP ptr if global menu
        MIDI_CHANNEL_INDEX,     // SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 1
        "OPERATING MODE",       // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        OPERATING_MODE_NB,          // value_max            : This global menu value is 0 - 3
        OPERATING_MODE_NVM_INDEX,      // value_index          : Z8_STATUS_2 // V1.69
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        // v1.5 operating_mode_z2nvm,      
		// bit_pos_msk          : position of the bit to modify
		// OPERATING_MODE_MSK_Z3_NVM,	// V1.69
		NO_BIT,
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        ModesTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        operating_mode_bmp,     // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      }
	  ,
      { // index 2
        "GROUP SELECT",         // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        GROUP_NB,               // value_max            : This global menu value is 0 - 3
        GROUP_INDEX,            // value_index          : current_group is CtrlReg[7]
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : No value since numeric
        group_bmp,              // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 3
        "SCENE SELECT",         // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        SCENE_NB,               // value_max            : This global menu value is 0 - 3
        SCENE_INDEX,            // value_index          : current_scene is CtrlReg[4]
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : No value since numeric
        scene_sel_bmp,          // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 4
        "Z-AXIS",             // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        infrared_state_z1nvm,      // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        infrared_bmp,           // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 5 [CALL TO A SUBMENU]
        "SYNC-24",              // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        SYNC24_NB,          // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        sync24_bmp,         	// GlobalMenuBmp        : Associated BMP ptr if global menu
        SYNC24_STATE_INDEX,     // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 6 [CALL TO A SUBMENU]
        "AUTOMATIONS",          // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        AUTOMATION_NB,          // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        automation_bmp,         // GlobalMenuBmp        : Associated BMP ptr if global menu
        LFO_INDEX,              // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
/* removed in fw 1.8
		{ // index 7
        "ANALOG MODE",          // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        ANALOG_MODE_NB,         // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        hook_zjump_mode_z1nvm,     // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        AnalModeTxtPtr,         // MenuValueStr         : string values are sine/square/random/triangle
        analog_mode_bmp,        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
*/
/*      { // index 8
        "TRANSPORT TRIGG",      // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        TRANSPORT_TRIG_NB,      // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : Z8_STATUS_1
        SUB_MENU,    			// table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,         			// MenuValueStr         : string values are sine/square/random/triangle
        transport_bmp,        	// GlobalMenuBmp        : Associated BMP ptr if global menu
        TRIGG_LFO_INDEX,		// SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
*/      { // index 8
        "SETUP",     			// Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        MISC_PARAM_NB,      	// value_max            : This global menu value is 0 - 1
        UNDEFINED_VALUE,   	  	// value_index          : Z8_STATUS_1
        SUB_MENU,    			// table_id             : ID of the table to modify
        NO_BIT, 				// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,        			// MenuValueStr         : string values are sine/square/random/triangle
        video_mode_bmp,         // GlobalMenuBmp        : Associated BMP ptr if global menu
        VIDEO_MODE_INDEX,       // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// SUB LEVEL MENUS
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      { // index 9
        // MIDI SETUP SUB MENUS  0
        "Midi Channel",         // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        CHANNEL_NB,             // value_max            : This global menu value is 0 - 3
        CHANNEL_INDEX,          // value_index          : current_channel is CtrlReg[2]
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC_PLUS_1,    // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // MIDI SETUP SUB MENUS  1 [CALL TO A SUBSUBMENU]
        "Midi Merge",           // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        MERGE_PARAM_NB,         // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        MERGER_SOURCE_INDEX,    // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 11 : MIDI FILTER
        "Midi Filter",          // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        FILTER_PARAM_NB,        // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        FILTER_STATE_INDEX,     // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
	  {
        // index 11 : REALTIME
        "Realtime config",      // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        REALTIME_PARAM_NB,      // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        INT_SYNC_STATE_INDEX,   // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth // V1.4
      },
      {	// V1.4
        // index 11 : SHIFT LOCKS MIDI ?
        "Shift lck Midi",       // Menu associated string 
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,        		// value_max            : This global menu value is 0 - 3
        Z8_STATUS_2_NVM_INDEX,	// value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        shift_lock_midi_z2nvm,	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,     		 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {	// V1.4
        // index 11 : SHIFT LOCKS MIDI CHANNEL ?
        "Shift lck Chan.",       // Menu associated string 
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,        		// value_max            : This global menu value is 0 - 3
        Z8_STATUS_0_NVM_INDEX,	// value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        shift_lock_channel_z0nvm,	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,     		 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {	// V1.69
        // index 11 : ENABLE/DISABLE Mackie fader feedback ?
        "Mackie fadr fbk",       // Menu associated string 
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,        		// value_max            : This global menu value is 0 - 3
        Z8_STATUS_2_NVM_INDEX,	// value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        enable_mackie_fader_fbk_z2nvm,	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,     		 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
	  {	// V1.8
        // index 17 : SCENE_FROM_PRG_CH_INDEX
        "Prg Ch > scene",       	// Menu associated string 
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,        		// value_max            : This global menu value is 0 - 3
        Z8_STATUS_3_NVM_INDEX,	// value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        send_scene_from_prg_chg_z3nvm,	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,     		 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 12
        "SYNC-24 State",        // Menu associated string
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        CTRL_REG_0_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        sync24_state_z_ez0,     // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
/* V1.5 { // index 13
        "SYNC-24 Source",       // Menu associated string
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        REALTIME_SRC_NB,        // value_max            : This global menu value is 0 - 3
        CTRL_REG_1_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        // V1.3 SYNC24_SRC_MSK,  		// bit_pos_msk          : position of the bit to modify
		sync24_source_ext_z_z_ez1,
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        RealtimeSrcTxtPtr,          // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
*/
      { // V1.3 index 13
        "SYNC-24 Sync",       // Menu associated string
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        SYNC_TYPE_NB,        // value_max            : This global menu value is 0 - 3
        CTRL_REG_1_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        sync24_source_int_z_z_ez1,  		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        SyncTypeTxtPtr,          // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
	  },
      { // index 14 : AUTOMATIONS 0 [CALL TO A SUBSUBMENU]
        "Low Freq Osc",         // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        LFO_PARAM_NB,           // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        LFO_STATE_INDEX,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 15 : AUTOMATIONS 1 [CALL TO A SUBSUBMENU]
        "Arpeggiator",          // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ARPP_PARAM_NB,          // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        ARPP_STATE_INDEX,       // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 16 : AUTOMATIONS 2 [CALL TO A SUBSUBMENU]
        "Motion Sampler",       // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        MOTION_PARAM_NB,        // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        MOTION_STATE_INDEX,     // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth  <<<<<<< ??????????? FALSE ??????????
      }, 
	  {	// V1.62
        // index 17 : Video mode
        "Video mode",       	// Menu associated string 
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        VIDEO_MODE_NB,        		// value_max            : This global menu value is 0 - 3
        Z8_STATUS_2_NVM_INDEX,	// value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        new_video_mode_z2nvm,	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        VideoModeTxtPtr,     		 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {	// V1.62
        // index 18 : SHIFT mode ?
        "Shift mode",       	// Menu associated string 
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ASSIGN_COM_SW_MODE_NB,  // value_max            : This global menu value is 0 - 3
        Z8_STATUS_0_NVM_INDEX,	// value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        shift_toggle_mode_z0nvm,// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        SwModeTxtPtr,     		 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },

/*      { // index 17 : TRANSPORT TRIGGER
        "Trigg Lfo",            // Menu associated string
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        transport_trig_lfo_z1nvm,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 18 : TRANSPORT TRIGGER
        "Trigg Motion",         // Menu associated string
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        transport_trig_motion_z1nvm,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 19 : TRANSPORT TRIGGER
        "Trigg Sync-24",        // Menu associated string
        MENU_DEPTH_1,   	    // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_0_NVM_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        transport_trig_sync24_z0nvm,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
*/
	  /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// SUB-SUB LEVEL MENUS
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      {
        // index 11 : REALTIME MIDI INPUT
        "External Source",      // Menu associated string
        MENU_DEPTH_2,   	    // menu_depth           : the menu depth
        REALTIME_SRC_NB,        // value_max            : This global menu value is 0 - 3
        CTRL_REG_1_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        realtime_source_ext_z_ez1,  		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        RealtimeSrcTxtPtr,      // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : MIDI INTERNAL TEMPO
        "Internal tempo",       // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        MAX_TEMPO_MINUS_30,		            // value_max            : This global menu value is 0 - 3
        INT_TEMPO_INDEX,  		// value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC_PLUS_30,   // display_type         : The value of this menu is a string type (sub menus names)
        NULL,         			// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : MIDI INTERNAL TEMPO UPDATE MODE V1.69
        "tempo mode",      		// Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        TEMPO_UPDT_MODE_NB,		// value_max            : This global menu value is 0 - 3
        Z8_STATUS_2_NVM_INDEX,  // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        tempo_change_mode_z2nvm, // bit_pos_msk          : position of the bit to modify
        DISP_STRING,   			// display_type         : The value of this menu is a string type (sub menus names)
        TempoUpdtModeTxtPtr,   	// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 11 : REALTIME MIDI INPUT
        "Internal state",       // Menu associated string 
        MENU_DEPTH_2,   	    // menu_depth           : the menu depth
        ON_OFF_NB,        		// value_max            : This global menu value is 0 - 3
        CTRL_REG_1_7BITS_TO_EZ_INDEX,// value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        realtime_source_int_z_ez1,	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,     		 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // internal mode sync : clock or smpte // V1.8
        "Internal mode",         		// Menu associated string
        MENU_DEPTH_2,         		// menu_depth           : the menu depth
        INT_SYNC_MODE_NB,    			// value_max            : 12 sub menus from this global menu
        Z8_STATUS_3_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        int_sync_mode_smpte_zclock_z3nvm, 		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        IntSyncModesTxtPtr,         	// MenuValueStr         : Dynamically filled
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
       { // index 0 [CALL TO A SUBMENU]
        "SMPTE rate",      		// Menu associated string // V1.8
        MENU_DEPTH_2,         		// menu_depth           : the menu depth
        SMPTE_FRAME_RATE_NB,    	// value_max            : 12 sub menus from this global menu
        Z8_STATUS_3_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        SMPTE_FRAME_RATE_MASK_BIT,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        SmpteFrameRateTxtPtr,      	// MenuValueStr         : Dynamically filled
        NULL,      		        // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 20 : MIDI MERGER SOURCE [CALL TO A SUBSUBSUBMENU]
        "Merger Sources",        // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        MIDI_INS_NB,            // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        MIDI_IN_INDEX,          // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 21 : MIDI MERGER DESTINATION [CALL TO A SUBSUBSUBMENU]
        "Merger Outputs",       // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        MERGER_OUTS_NB,         // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        MIDI_OUT1_INDEX,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 22 : MIDI FILTER STATE
        "Filter State",         // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        CTRL_REG_0_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        filter_state_z_ez0,     // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 23 : MIDI FILTER TYPE
        "Filter Type",          // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        FILTER_TYPE_NB,         // value_max            : This global menu value is 0 - 3
        CTRL_REG_0_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        filter_type_z_ez0,      // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        FilterTypeTxtPtr,       // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 24 : AUTOMATIONS LFO STATE
        "Lfo State",            // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,      // value_index      : table index to modify is 0
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        lfo_state_z1nvm,           // bit_pos_msk       : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 25 : AUTOMATIONS LFO WAVEFORM
        "Lfo Waveform",         // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        LFO_WAVES_NB,           // value_max            : This global menu value is 0 - 3
        Z8_STATUS_2_NVM_INDEX,      // value_index          : Z8_STATUS_2
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        LFO_WAVE_MSK_Z2_NVM,        // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        LfoWaveTypePtr,         // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 25 : AUTOMATIONS LFO WAVEFORM
        "Lfo Sync",        // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        SYNC_TYPE_NB,           // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,  // value_index          : Z8_STATUS_2
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        lfo_sync_ext_z1nvm,   	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        SyncTypeTxtPtr,         // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 26 : AUTOMATIONS ARPEGGIATOR STATE
        "Arpp State",           // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        // V1.5 CTRL_REG_1_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_1
		Z8_STATUS_0_NVM_INDEX,
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        // V1.5 arpp_state_z_ez1,       // bit_pos_msk          : position of the bit to modify
		arpp_state_z0nvm,
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
	  {
        // index 26 : AUTOMATIONS ARPEGGIATOR SOURCE FOR MIDI NOTES
        "Arpp Midi Src",          // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        MIDI_INS_NB,            // value_max            : This global menu value is 0 - 3
//		REALTIME_SRC_NB-1,		// value_max            : This global menu value is 0 - 3
		CTRL_REG_1_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        arpp_midi_src_z_ez1,     // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
//      MidiInsTxtPtr,          // MenuValueStr         : string values are sine/square/random/triangle
	    RealtimeSrcTxtPtr,      // MenuValueStr         : string values are sine/square/random/triangle
		NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
	  {
        // index 26 : AUTOMATIONS ARPEGGIATOR SOURCE FOR MIDI NOTES
        "Arpp Sync",       		// Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        SYNC_TYPE_NB,           // value_max            : This global menu value is 0 - 3
		Z8_STATUS_0_NVM_INDEX, 	// value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        arpp_sync_ext_z0nvm,    // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
	    SyncTypeTxtPtr,		    // MenuValueStr         : string values are sine/square/random/triangle
		NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 21 : ARPPEGIO DESTINATION [CALL TO A SUBSUBSUBMENU]
        "Arpp Outputs",         // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        MERGER_OUTS_NB+1,       // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,        // value_index          : table index to modify is 0
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        ARP_DST_MIDI_OUT1_INDEX, // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 9
        // MIDI SETUP SUB MENUS  0
        "Arpp channel",         // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        CHANNEL_NB,             // value_max            : This global menu value is 0 - 3
        ARP_CHANNEL_INDEX,          // value_index          : current_channel is CtrlReg[2]
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC_PLUS_1,    // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp mode",            // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ARPP_MODE_NB,           // value_max            : This global menu value is 0 - 3
        ARP_MODE_INDEX,      	// value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        ArpModesTxtPtr,         // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp velocity",        // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        128,              		// value_max            : This global menu value is 0 - 3
        ARP_VELOCITY_INDEX,     // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,            		// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp swing",            // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        128,              		// value_max            : This global menu value is 0 - 3
        ARP_SWING_INDEX,     // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,            		// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp clock reso",      // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ARP_CLK_RESO_NB,        // value_max            : This global menu value is 0 - 3
        ARP_CLOCK_RESOLUTION_INDEX,   // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        ArpClkResoTxtPtr,     	// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp melody bnk",      // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ARP_MELODY_BNK_NB,      // value_max            : This global menu value is 0 - 3
        ARP_MELODY_BNK_INDEX,   // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        ArpBanksTxtPtr,     	// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp rhythm bnk",      // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ARP_RHYTHM_BNK_NB,      // value_max            : This global menu value is 0 - 3
        ARP_RHYTHM_BNK_INDEX,   // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        ArpBanksTxtPtr,         // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp accent bnk",      // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ARP_ACCENT_BNK_NB,      // value_max            : This global menu value is 0 - 3
        ARP_ACCENT_BNK_INDEX,   // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        ArpBanksTxtPtr,    		// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 27 : AUTOMATIONS ARPEGGIATOR HOLD
        "Arpp offset",      	// Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        48,      				// value_max            : This global menu value is 0 - 3
        ARP_OFFSET_INDEX,   	// value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        NO_BIT,     			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,    				// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
       {
        // index 28 : AUTOMATIONS MOTION SAMPLER STATE
        "Motion State",         // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        motion_state_z1nvm,        // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 29 : AUTOMATIONS MOTION SAMPLER LOOP
        "Motion Loop",          // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,      // value_index          : Z8_STATUS_1
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        motion_loop_state_z1nvm,   // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE	                   // NextMenuExist       : TRUE if a next menu exist on same depth
      },
      {
        // index 25 : AUTOMATIONS LFO WAVEFORM
        "Motion Sync",        // Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        SYNC_TYPE_NB,           // value_max            : This global menu value is 0 - 3
        Z8_STATUS_1_NVM_INDEX,  // value_index          : Z8_STATUS_2
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        motion_sync_ext_z1nvm,   	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        SyncTypeTxtPtr,         // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// SUB-SUB-SUB LEVEL MENUS
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      {
        // index 30 : MIDI MERGER SOURCES MIDI IN ENABLE
        "Midi In",              // Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        CTRL_REG_0_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        merger_src_midi_in_state_z_ez0,// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 31 : MIDI MERGER SOURCES USB IN ENABLE
        "Usb In",               // Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        CTRL_REG_0_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        merger_src_usb_in_state_z_ez0,// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 32 : MIDI MERGER DESTINATIONS MIDI OUT 1 ENABLE
        "Midi Out 1",            // Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        CTRL_REG_0_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        merger_dst_out1_state_z_ez0,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 33 : MIDI MERGER DESTINATIONS USB OUT ENABLE
        "Usb Out 1",            // Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        CTRL_REG_0_7BITS_TO_EZ_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        merger_dst_usb_out_state_z_ez0,// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 32 : ARPPEGIO DESTINATIONS MIDI OUT 1 ENABLE
        "Midi Out 1",           // Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_0_NVM_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        arpp_dst_midi_out1_z0nvm,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 32 : ARPPEGIO DESTINATIONS MIDI OUT 1 ENABLE
        "Midi Out 2",           // Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_0_NVM_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        arpp_dst_midi_out2_z0nvm,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      {
        // index 33 : ARPPEGIO DESTINATIONS USB OUT ENABLE
        "Usb Out",            // Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        ON_OFF_NB,              // value_max            : This global menu value is 0 - 3
        Z8_STATUS_0_NVM_INDEX,      // value_index          : Z8_STATUS_7
        TABLE_GLOBAL_PARAMS,    // table_id             : ID of the table to modify
        arpp_dst_usb_out_z0nvm, // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,            // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	  ///////////////////////////// MENUS ASSIGN TABLES INDEX ///////////////////////////////////////////////////
	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
      { // index 0 [CALL TO A SUBMENU]
        "ASSIGN STD MODE",          // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        ASSIGN_STD_PARAM_NB,    // value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        // value_index          : No table index for a global menu
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : Dynamically filled
        midi_setup_bmp,         // GlobalMenuBmp        : Associated BMP ptr if global menu
        ASSIGN_STD_EVENT_TYPE_INDEX,     // SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 1 [CALL TO A SUBMENU]
        "ASSIGN USR MODE",      // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        ASSIGN_USR_PARAM_NB,   // value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,      	// value_index          : Z8_STATUS_2
        SUB_MENU,    			// table_id             : ID of the table to modify
        NO_BIT,      			// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,            		// MenuValueStr         : string values are sine/square/random/triangle
        operating_mode_bmp,     // GlobalMenuBmp        : Associated BMP ptr if global menu
        ASSIGN_USR_MIDI_DELAY_INDEX,  // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "ASSIGN COMMON",          // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        ASSIGN_COM_PARAM_NB,    // value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        // value_index          : No table index for a global menu
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : Dynamically filled
        midi_setup_bmp,         // GlobalMenuBmp        : Associated BMP ptr if global menu
        ASSIGN_COM_AUTOSEND_INDEX,  // SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// SUB LEVEL MENUS
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      { // index 9
        // ASSIGN STD MODE SUB MENUS  0
        "Event type",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ASSIGN_STD_EVT_TYPE_NB, // value_max            : This global menu value is 0 - 3
        ctrl_status_0_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        NOTE_CTRL_CHANGE,       // bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        StdEventTxtPtr,         // MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN STD MODE SUB MENUS  1
        "Event number",         // Menu associated string // V1.5 numero
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        128,			        // value_max            : This global menu value is 0 - 3
        ctrl_status_0_index,    // value_index          : table index to modify is 0
        TABLE_CTRL_ASSIGN,      // table_id             : ID of the table to modify
        CTRL_NUMERO_MSK,        // bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,    	// SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS 1
        "Midi delay",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ASSIGN_USR_MIDI_DELAY_NB, // value_max            : This global menu value is 0 - 3
        ctrl_status_1_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        midi_delay_msk,       	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        MididelayTxtPtr,         // MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS 2
        "Curve model",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ASSIGN_USR_CURVE_NB, 	// value_max            : This global menu value is 0 - 3
        ctrl_status_2_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        CURVE_MODEL_MSK,       	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        CurveModelTxtPtr,       // MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS 3
        "Min value",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        128, 					// value_max            : This global menu value is 0 - 3
        ctrl_value_min_index,   // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        NO_BIT,       			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,    		// display_type         : The value of this menu is a string type (sub menus names)
        NULL, 			        // MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS 4
        "Max value",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        128, 					// value_max            : This global menu value is 0 - 3
        ctrl_value_max_index,   // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        NO_BIT,       			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,    		// display_type         : The value of this menu is a string type (sub menus names)
        NULL, 			        // MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 9
        // ASSIGN USER MODE SUB MENUS  0
        "Crossfade chain",     	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ON_OFF_NB, 				// value_max            : This global menu value is 0 - 3
        ctrl_chain_index,    	// value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        CROSSFADE_CHAIN_EN,		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,      	   	// MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS  1
        "Chained control",      // Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        CONTROLS_NB,	        // value_max            : This global menu value is 0 - 3
        ctrl_chain_index, 		// value_index          : table index to modify is 0
        TABLE_CTRL_ASSIGN,      // table_id             : ID of the table to modify
        CTRL_CHAIN_NUMERO_MSK,  // bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,    	// SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 9
        // ASSIGN COMMON MODE SUB MENUS 0
        "Autosend",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ON_OFF_NB, 				// value_max            : This global menu value is 0 - 3
        ctrl_status_1_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        autosend,       		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,         	// MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { 
        // ASSIGN COMMON MODE SUB MENUS  1
        "Scene exclude",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ON_OFF_NB, 				// value_max            : This global menu value is 0 - 3
        ctrl_status_1_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        scene_exclude,     		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,         	// MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { 
        // ASSIGN COMMON MODE SUB MENUS  2
        "Send to USB",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ON_OFF_NB, 				// value_max            : This global menu value is 0 - 3
        ctrl_status_1_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        usb_out_en,     		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,         	// MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { 
        // ASSIGN COMMON MODE SUB MENUS  2
        "Send to Out 1",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ON_OFF_NB, 				// value_max            : This global menu value is 0 - 3
        ctrl_status_1_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        midi_out_1_en,     		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,         	// MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { 
        // ASSIGN COMMON MODE SUB MENUS  2
        "Send to Out 2",         	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ON_OFF_NB, 				// value_max            : This global menu value is 0 - 3
        ctrl_status_1_index,    // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        midi_out_2_en,     		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,         	// MenuValueStr         : No value since numeric
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS  1
        "Button mode",      	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ASSIGN_COM_SW_MODE_NB,  // value_max            : This global menu value is 0 - 3
        ctrl_status_1_index, 	// value_index          : table index to modify is 0
        TABLE_CTRL_ASSIGN,      // table_id             : ID of the table to modify
        switch_ctrl_hook_mode,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        SwModeTxtPtr,       	 // MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,    	// SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // V1.8
        // ASSIGN_COM_ANAL_MODE_INDEX SUB MENUS  1
        "Analog mode",      	// Menu associated string
        MENU_DEPTH_1,           // menu_depth           : the menu depth
        ASSIGN_ANALOG_MODE_NB,  // value_max            : This global menu value is 0 - 3
        ctrl_status_1_index, 	// value_index          : table index to modify is 0
        TABLE_CTRL_ASSIGN,      // table_id             : ID of the table to modify
        switch_ctrl_hook_mode,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        AnalModeTxtPtr,        	// MenuValueStr         : string values are sine/square/random/triangle
        NULL,                   // GlobalMenuBmp        : Associated BMP ptr if global menu
        UNDEFINED_VALUE,    	// SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      }

};

#endif
