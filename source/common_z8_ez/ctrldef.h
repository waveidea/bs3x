#ifndef CONTROLDEF
#define CONTROLDEF

///////////////////////////////////////////////////////////
// Control structure definition
///////////////////////////////////////////////////////////

// CONTROLS AND FEATURES DEFINITION
#define ROTARY_NB				32U						// rotary pots
#define SLIDER_NB				8U						// slider pots
#define POT_NB					(ROTARY_NB + SLIDER_NB)	// total potentiometers
#define SW_NB					8U						// on/off switches
#define LFO_NB					1U
#define INFRARED_NB				1U	
#define JOYSTICK_NB				1U
#define JOYSTICK_SW_NB			1U
#define FOOTSWITCH_NB			1U
#define ENCODER_SW_NB			1U
#define CROSSFADER_NB			1U
#define BACKWARD_NB				1U
#define FORWARD_NB				1U
#define PLAY_NB					1U
#define STOP_NB					1U
#define REC_NB					1U
#define TRANSPORT_NB			(BACKWARD_NB+FORWARD_NB+PLAY_NB+STOP_NB+REC_NB)
#define BW_ASSIGN_SW_NB			BACKWARD_NB	
#define FW_ASSIGN_SW_NB			FORWARD_NB	
#define PLAY_ASSIGN_SW_NB		PLAY_NB		
#define STOP_ASSIGN_SW_NB		STOP_NB		
#define REC_ASSIGN_SW_NB		REC_NB		
#define OTHER_SW_NB				(TRANSPORT_NB+JOYSTICK_SW_NB+FOOTSWITCH_NB+ENCODER_SW_NB)
//#define EXP_ANA_NB				16U
//#define EXP_DIG_NB				8U
#define CONTROLS_WITH_GROUP_NB	(POT_NB+SW_NB+LFO_NB+INFRARED_NB+(JOYSTICK_NB*2)+OTHER_SW_NB+(CROSSFADER_NB*2U)/* V1.5 +EXP_ANA_NB+EXP_DIG_NB*/)
#define CONTROLS_WITH_SCENE_NB	(POT_NB+SW_NB+INFRARED_NB+(JOYSTICK_NB*2U)+OTHER_SW_NB+(CROSSFADER_NB*2U)/* V1.5 +EXP_ANA_NB+EXP_DIG_NB*/)
#define CONTROLS_NB				(POT_NB+SW_NB+LFO_NB+INFRARED_NB+(JOYSTICK_NB*2U)+OTHER_SW_NB+(CROSSFADER_NB*2U)+TRANSPORT_NB/* V1.5 +EXP_ANA_NB+EXP_DIG_NB */)
#define ANALOG_SOURCES_NB		(POT_NB+(LFO_NB*3)+INFRARED_NB+(JOYSTICK_NB*2)+CROSSFADER_NB)
#define SCENE_NB				100U
#define SCENE_ARP_NB			64U // V1.8
#define GROUP_NB				21U
#define CHANNEL_NB				16U
#define NB_BYTES_FOR_SW			((SW_NB+OTHER_SW_NB)/8U)
#define NON_ASSIGN_SW_NB		16U // shift, hold, group, channel, scene sel, snapshot, up, down, encoder, autom_on, autom_type, autom_sel, edit, ok, cancel, mode
//////////////////////////////////////////////////////////////////////////////
#define MIDI_STR_MAX_LENGHT			21U
#define LCD_STR_MAX_LENGHT			16U
#define CTRL_PER_ROW				8U
#define GROUP_NAME_LENGHT           14

#define GROUP_INDEX_OFFSET	8U	// The CC offset from one group to another

//////////////////////////////////////////////////////////////////////////////
// The following declarartion must be a bloc of data that cannot be rearanged
//////////////////////////////////////////////////////////////////////////////
// This is the structure of all controls that respond to scenes snapshots
typedef struct {
uchar infrared_hold;							// infrared holded value     DO NOT CHANGE ORDER
uchar joystick_x;								// current joystick x value
uchar joystick_y;								// current joystick y value
uchar pots[POT_NB];								// all 40 pots
uchar crossfader_l;								// The current crossfader left value 
uchar crossfader_r;								// The current crossfader right value 
uchar other_sw_value;							// 5 assignable transports + encoder sw + joystick sw + footswitch
uchar all_sw_value;								// all 8 switches values
} Controls_Scene;

#define CTRL_STRUCT_START_FIELD		infrared_hold
#define CTRL_STRUCT_SIZE			47U
#define CTRL_STRUCT_BASE_SW_INDEX	45U

//////////////////////////////////////////////////////////////////////////////////////
// This is the structure of all controls that respond to scenes or not
typedef struct {
Controls_Scene 	Ctrl_sc;		// All controls that are used with scenes
uchar 		lfo;				// the LFO does not respond to scenes
} All_Controls;

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// CONTROL STRUCTURE DEFINITION /////////////////////////////////
// This is the structure of control definition mapped in EEProm as well
typedef struct {
uchar ctrl_status_0;			// 1 byte, used in std mode only (prog/ctrl & number)
uchar ctrl_status_1;			// 1 byte, control status      DO NOT CHANGE ORDER
uchar ctrl_status_2;			// 1 byte, control status
uchar midi_str_lenght;			// 1 byte, midi string lenght
uchar midi_channel_position;		// 1 byte, midi channel position
uchar ctrl_value_position_0;		// 1 byte, value position 0
uchar ctrl_value_position_1;		// 1 byte, value position 1
uchar ctrl_value_min;			// 1 byte, control min value
uchar ctrl_value_max;			// 1 byte, control max value
//uchar checksum_position;		// 1 byte, checksum position
uchar checksum_start_position;		// 1 byte, check first byte
uchar ctrl_chain;			// 1 byte, control chain definition
uchar midi_string[MIDI_STR_MAX_LENGHT];	// 21 bytes, the midi string
} All_Params ;

#define CTRL_STRUCT_HEADER_SIZE 11
#define CTRL_STRUCT_TOTAL_SIZE (CTRL_STRUCT_HEADER_SIZE + MIDI_STR_MAX_LENGHT)

enum {ctrl_status_0_index, ctrl_status_1_index, ctrl_status_2_index, 
midi_str_lenght_index, midi_channel_position_index, ctrl_value_position_0_index, 
ctrl_value_position_1_index, ctrl_value_min_index, ctrl_value_max_index, 
checksum_start_position_index, ctrl_chain_index, midi_string_index};

// ctrl_status_1 is at position 1
#define CTRL_STATUS_1_STRUCT_OFFSET	1L		// DO NOT CHANGE ORDER

// ctrl_status_0 bits definition
#define NOTE_CTRL_CHANGE		(1<<7)		// bit 7 = Note on/program change 0=ctrl, 1=Note on
#define CTRL_NUMERO_MSK			0x7F
// bit 6:0 	= number 0-127

// ctrl_status_1 bits definition
#define switch_ctrl_hook_mode	(1<<7)		// bit 7 = switch mode (0=push/1=toggle), pot : jump/hook=1 V1.8 DO NOT CHANGE ORDER
#define scene_exclude			(1<<6)		// bit 6 = 1=scene exclude
#define autosend				(1<<5)		// bit 5 = autosend at startup
#define usb_out_en				(1<<4)		// bit 4 = send over USB   		DO NOT CHANGE ORDER
#define midi_out_1_en			(1<<3)		// bit 3 = midi out 1 enable	DO NOT CHANGE ORDER
#define midi_out_2_en			(1<<2)		// bit 2 = midi out 2 enable    DO NOT CHANGE ORDER
#define midi_delay_msk		 	 0x03U		// bit 1:0 = midi delay			DO NOT CHANGE ORDER

#define MIDI_OUT_SEL_MSK		 0x1CU		// bits 2, 3, 4

// ctrl_status_2 bits definition
//#define sysex_checksum		(1<<7)		// bit 7 = 1: a checksum sysex must be included in the MIDI str
//#define ad_7bitsz_8bits		(1<<6)		// V1.4 bit 6 = 1 = analog is 8 bits 0 = analog is 7 bits
//#define split_msb_lsb			(1<<5)		// V1.4 bit 5 = 1 = send msb first 0 = send lsb first
#define split_ctrl_value		(1<<4)		// V1.4 bit 4 = 1 = split control value on 2 nibbles 0= 1 byte per value
//#define split_msb_lsb			(1<<3)		// V1.4 bit 5 = 1 = send msb first 0 = send lsb first
// bit 2:0 	= curve model used				// DO NOT CHANGE ORDER
#define CURVE_MODEL_MSK			7U			// curves are in control_status_2 bits 0-2
#define CURVE_TABLES_LENGHT		128U		// 7 bits for curves models

// ctrl_chain bits definition
#define CROSSFADE_CHAIN_EN_POS	7		// bit 7 = 1=crossfade
#define CROSSFADE_CHAIN_EN		(1<<CROSSFADE_CHAIN_EN_POS)		// bit 7 = 1=crossfade
#define CTRL_CHAIN_NUMERO_MSK	 0x7F
// bit 6:0 	= cahined index number 0-62

enum {	TRANSPORT_REC, TRANSPORT_PLAY, TRANSPORT_BW, TRANSPORT_STOP, TRANSPORT_FW, TRANSPORT_PAUSE };

///////////////////////////////////////////////////////////////////////////////
// Control number mapping 
///////////////////////////////////////////////////////////////////////////////
// Digital without group
#define REC_CTRL_INDEX				0U	// +1
#define PLAY_CTRL_INDEX				1U	// +1
#define BACKWARD_CTRL_INDEX			2U	// +1
#define STOP_CTRL_INDEX				3U	// +1
#define FORWARD_CTRL_INDEX			4U	// +1
// Analog with groups
#define INFRARED_CTRL_INDEX			5U	// +1
#define JOYSTICK_X_CTRL_INDEX		6U	// +1
#define JOYSTICK_Y_CTRL_INDEX		7U	// +1
#define POTS_BASE_CTRL_INDEX		8U	// +32
#define SLIDER_BASE_CTRL_INDEX		40U	// +8
#define CROSSFADER_L_CTRL_INDEX		48U	// +1
#define CROSSFADER_R_CTRL_INDEX		49U	// +1
// Digital with groups
#define BW_ASSIGN_SW_CTRL_INDEX		50U	// +1
#define FW_ASSIGN_SW_CTRL_INDEX		51U	// +1
#define STOP_ASSIGN_SW_CTRL_INDEX	52U	// +1
#define PLAY_ASSIGN_SW_CTRL_INDEX	53U	// +1
#define REC_ASSIGN_SW_CTRL_INDEX	54U	// +1
#define JOYSTICK_SW_CTRL_INDEX		55U	// +1
#define FOOTSWITCH_CTRL_INDEX		56U	// +1
#define ENCODER_SW_CTRL_INDEX		57U	// +1
#define SWITCH_BASE_CTRL_INDEX		58U	// +8
// not a physical control
#define LFO_CTRL_INDEX				66U	// +1
// TOTAL 67

// V1.5 : add expansions indexes
#define EXP_ANA_CTRL_INDEX			67U	// +16
#define EXP_DIG_CTRL_INDEX			83U	// +8
// TOTAL 91

// V1.4
///////////////////////////////////////////////////////////////////////////////
// Control number mapping in AUTO-TEST mode
///////////////////////////////////////////////////////////////////////////////
// Digital without group
#define AUTOTEST_REC_CTRL_INDEX				0U	// +1
#define AUTOTEST_PLAY_CTRL_INDEX			1U	// +1
#define AUTOTEST_BACKWARD_CTRL_INDEX		2U	// +1
#define AUTOTEST_STOP_CTRL_INDEX			3U	// +1
#define AUTOTEST_FORWARD_CTRL_INDEX			4U	// +1
// Analog with groups
#define AUTOTEST_INFRARED_CTRL_INDEX		5U	// +1
#define AUTOTEST_JOYSTICK_X_CTRL_INDEX		6U	// +1
#define AUTOTEST_JOYSTICK_Y_CTRL_INDEX		7U	// +1
#define AUTOTEST_POTS_BASE_CTRL_INDEX		8U	// +32
#define AUTOTEST_SLIDER_BASE_CTRL_INDEX		40U	// +8
#define AUTOTEST_CROSSFADER_L_CTRL_INDEX	48U	// +1
#define AUTOTEST_CROSSFADER_R_CTRL_INDEX	49U	// +1
// Digital with groups
#define AUTOTEST_BW_ASSIGN_SW_CTRL_INDEX	50U	// +1
#define AUTOTEST_FW_ASSIGN_SW_CTRL_INDEX	51U	// +1
#define AUTOTEST_STOP_ASSIGN_SW_CTRL_INDEX	52U	// +1
#define AUTOTEST_PLAY_ASSIGN_SW_CTRL_INDEX	53U	// +1
#define AUTOTEST_REC_ASSIGN_SW_CTRL_INDEX	54U	// +1
#define AUTOTEST_JOYSTICK_SW_CTRL_INDEX		55U	// +1
#define AUTOTEST_FOOTSWITCH_CTRL_INDEX		56U	// +1
#define AUTOTEST_ENCODER_SW_CTRL_INDEX		57U	// +1
#define AUTOTEST_SWITCH_BASE_CTRL_INDEX		58U	// +8
// not a physical control
#define AUTOTEST_LFO_CTRL_INDEX_POT1		66U	// +1
#define AUTOTEST_LFO_CTRL_INDEX_POT2		67U	// +1
#define AUTOTEST_LFO_CTRL_INDEX_POT3		68U	// +1
// Non assignable switches indexes for autotest mode
#define AUTOTEST_CHANNEL_SW_INDEX			69U	// +1
#define AUTOTEST_SCENE_SEL_SW_INDEX			70U	// +1
#define AUTOTEST_GROUP_SW_INDEX				71U	// +1
#define AUTOTEST_SHIFT_SW_INDEX				72U	// +1
#define AUTOTEST_HOLD_SW_INDEX				73U	// +1
#define AUTOTEST_EDIT_SW_INDEX				74U	// +1
#define AUTOTEST_OK_SW_INDEX				75U	// +1
#define AUTOTEST_CANCEL_SW_INDEX			76U	// +1
#define AUTOTEST_MODE_SW_INDEX				77U	// +1
#define AUTOTEST_AUTOM_SW_TYPE_INDEX		78U	// +1
#define AUTOTEST_AUTOM_SW_SEL_INDEX			79U	// +1
#define AUTOTEST_AUTOM_SW_ON_INDEX			80U	// +1
#define AUTOTEST_UP_SW_INDEX				81U	// +1
#define AUTOTEST_DOWN_SW_INDEX				82U	// +1
#define AUTOTEST_SNAPSHOT_SW_INDEX			83U	// +1
#define AUTOTEST_ENCODER_UP_INDEX			84U	// +1
#define AUTOTEST_CONTROLS_NB				85U
#define AUTOTEST_EXPANSION_INDEX			127U // expansion board test result is sent in a sysex with this index 
// TOTAL 85


// control indexes for non assignable switches in test mode
// only used when the 3X must generate sysex in test mode
// beacause LFO features 3 pots with indexes 66,67,68 in test mode
// thus switches starts at 69

#define TEST_MODE_NON_ASSIGN_SW_BASE_INDEX	69U	// V1.4 67U

#define TEST_MODE_CHANNEL_BIT_MASK			(1<<0U) // do not change order (must be equal to FCT_CHANNEL) 
#define TEST_MODE_SCENE_SEL_BIT_MASK		(1<<1U)	// do not change order (must be equal to FCT_SCENE)
#define TEST_MODE_GROUP_BIT_MASK			(1<<2U)	// do not change order (must be equal to FCT_GROUP)
#define TEST_MODE_SHIFT_BIT_MASK			(1<<3U)
#define TEST_MODE_HOLD_BIT_MASK				(1<<4U)
#define TEST_MODE_EDIT_BIT_MASK				(1<<5U)
#define TEST_MODE_OK_BIT_MASK				(1<<6U)
#define TEST_MODE_CANCEL_BIT_MASK			(1<<7U)
#define TEST_MODE_MODE_BIT_MASK				(1<<8U)
#define TEST_MODE_AUTOM_SW_TYPE_BIT_MASK	(1<<9U)
#define TEST_MODE_AUTOM_SW_SEL_BIT_MASK		(1<<10U)
#define TEST_MODE_AUTOM_SW_ON_BIT_MASK		(1<<11U)
#define TEST_MODE_UP_SW_BIT_MASK			(1<<12U)
#define TEST_MODE_DOWN_SW_BIT_MASK			(1<<13U)
#define TEST_MODE_SNAPSHOT_BIT_MASK			(1<<14U)
#define TEST_MODE_ENCODER_UP_BIT_MASK		(1<<15U)

#endif
