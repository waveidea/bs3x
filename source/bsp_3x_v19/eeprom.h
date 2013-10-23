#ifndef EEPROM
#define EEPROM

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
#define EEPROM_I2C_ADR_POS				1U
#define EEPROM_ZERO_I2C_ADR				(0x00U)		// EEprom 0 I2C adress
#define EEPROM_ONE_I2C_ADR				(0x01U<<1U)	// EEprom 1 I2C adress
#define EEPROM_TWO_I2C_ADR				(0x02U<<1U)	// EEprom 2 I2C adress 

#define EEPROM_ZERO_FULL_SIZE			32768	// Contains control reg + scene + motion + misc
#define EEPROM_ONE_FULL_SIZE			32768	// Contains both MIDI and LCD strings + structures
#define EEPROM_TWO_FULL_SIZE			32768	// Contains both MIDI and LCD strings + structures 
#define EEPROM_ONE_TWO_FULL_SIZE		(EEPROM_ONE_FULL_SIZE + EEPROM_TWO_FULL_SIZE)	

#define EEPROM_PAGE_SIZE				64U		// Page size is 64 bytes

#define MIDI_STRUCT_SIZE				32U		// Status + Parameters + MIDI str structure
#define MIDI_STRUCT_HEADER_SIZE			 2U		// Status only structure (std mode)
#define MIDI_STRUCT_CONTROL_SIZE		11U		// whole control structure without midi str (std mode)
#define LCD_STR_SIZE					16U		// LCD str structure
//#define GROUP_NAME_LENGHT				14U		// group name are 14 bytes strings
#define GROUP_NAME_EE_LENGHT			16U		// group name are 16 bytes strings in EEPROM

//V1.6 #define BS3X_EE_READ_CTRL  				0xA1U	// I2C EEPROM control Read Address
//V1.6 define BS3X_EE_WRITE_CTRL 				0xA0U	// I2C EEPROM control Write Address
#define BS3X_I2C_SLAVE_ADR_EE			0xA0U	// I2C EEPROM control Write Address

///////////////////////////////////// EEPROM CONTROL MAPPING //////////////////////////////////////////////////////// 

// ******************************************************************************************************************
// **************************************** EEPROM 1 + EEPROM 2******************************************************
// ******************************************************************************************************************

// All MIDI and LCD strings are located in 2 last EEPROMs of 32 KB each, 
// >> Use a linear 16b address, when address > 32768, auto select second EEPROM
// Organized as follow : 
// ********************************************************
// transports functions located at the begining of EEPROM
// ********************************************************
// TRANSPORT_REC 	CONTROL STRUCT (32 bytes) Address 0
// TRANSPORT_PLAY  	CONTROL STRUCT (32 bytes) Address 32
// TRANSPORT_BW  	CONTROL STRUCT (32 bytes) Address 64
// TRANSPORT_STOP  	CONTROL STRUCT (32 bytes) Address 96
// TRANSPORT_FW	  	CONTROL STRUCT (32 bytes) Address 128
// ********************************************************
// Then All controls on group 0
// ********************************************************
// INFRARED				CONTROL STRUCT (32 bytes)
// JOYSTICK X			CONTROL STRUCT (32 bytes)
// JOYSTICK Y			CONTROL STRUCT (32 bytes)
// POT 0 				CONTROL STRUCT (32 bytes)
// POT 1 				CONTROL STRUCT (32 bytes)
// ...	 			
// POT 39 				CONTROL STRUCT (32 bytes)
// CROSSFADER L			CONTROL STRUCT (32 bytes)
// CROSSFADER R			CONTROL STRUCT (32 bytes)
// BW_ASSIGN_SW			CONTROL STRUCT (32 bytes)
// FW_ASSIGN_SW			CONTROL STRUCT (32 bytes)
// STOP_ASSIGN_SW		CONTROL STRUCT (32 bytes)
// PLAY_ASSIGN_SW		CONTROL STRUCT (32 bytes)
// REC_ASSIGN_SW		CONTROL STRUCT (32 bytes)
// JOYSTICK SW			CONTROL STRUCT (32 bytes)
// FOOTSWITCH			CONTROL STRUCT (32 bytes)
// ENCODER_SW			CONTROL STRUCT (32 bytes)
// SWITCH 0				CONTROL STRUCT (32 bytes)
// SWITCH 1				CONTROL STRUCT (32 bytes)
// ...
// SWITCH 7				CONTROL STRUCT (32 bytes)
// LFO					CONTROL STRUCT (32 bytes)
// ********************************************************
// Then All controls on group 1
// ********************************************************
// INFRARED				CONTROL STRUCT (32 bytes)
// JOYSTICK X			CONTROL STRUCT (32 bytes)
// JOYSTICK Y			CONTROL STRUCT (32 bytes)
// POT 0 				CONTROL STRUCT (32 bytes)
// POT 1 				CONTROL STRUCT (32 bytes)
// ...	 			
// POT 39 				CONTROL STRUCT (32 bytes)
// CROSSFADER L			CONTROL STRUCT (32 bytes)
// CROSSFADER R			CONTROL STRUCT (32 bytes)
// BW_ASSIGN_SW			CONTROL STRUCT (32 bytes)
// FW_ASSIGN_SW			CONTROL STRUCT (32 bytes)
// STOP_ASSIGN_SW		CONTROL STRUCT (32 bytes)
// PLAY_ASSIGN_SW		CONTROL STRUCT (32 bytes)
// REC_ASSIGN_SW		CONTROL STRUCT (32 bytes)
// JOYSTICK SW			CONTROL STRUCT (32 bytes)
// FOOTSWITCH			CONTROL STRUCT (32 bytes)
// ENCODER_SW			CONTROL STRUCT (32 bytes)
// SWITCH 0				CONTROL STRUCT (32 bytes)
// SWITCH 1				CONTROL STRUCT (32 bytes)
// ...
// SWITCH 7				CONTROL STRUCT (32 bytes)
// LFO					CONTROL STRUCT (32 bytes)

///////////////////////////////// MIDI STRUCTS AND STRINGS MAPPING ////////////////////////////////////////////////////////////////////
#define MIDI_STR_EE_BASE_ADR				 0
// transports
#define TRANSPORT_MIDI_STR_EE_BASE_ADR		(MIDI_STR_EE_BASE_ADR)
#define REC_MIDI_STR_EE_BASE_ADR			(TRANSPORT_MIDI_STR_EE_BASE_ADR)
#define PLAY_MIDI_STR_EE_BASE_ADR			(REC_MIDI_STR_EE_BASE_ADR			+ MIDI_STRUCT_SIZE)
#define BACKWARD_MIDI_STR_EE_BASE_ADR		(PLAY_MIDI_STR_EE_BASE_ADR			+ MIDI_STRUCT_SIZE)
#define STOP_MIDI_STR_EE_BASE_ADR			(BACKWARD_MIDI_STR_EE_BASE_ADR		+ MIDI_STRUCT_SIZE)
#define FORWARD_MIDI_STR_EE_BASE_ADR		(STOP_MIDI_STR_EE_BASE_ADR			+ MIDI_STRUCT_SIZE)
#define TRANSPORT_MIDI_STR_EE_END_ADR		(FORWARD_MIDI_STR_EE_BASE_ADR 		+ MIDI_STRUCT_SIZE)
// analog
#define INFRARED_MIDI_STR_EE_BASE_ADR		(TRANSPORT_MIDI_STR_EE_END_ADR)
#define JOYSTICK_X_MIDI_STR_EE_BASE_ADR		(INFRARED_MIDI_STR_EE_BASE_ADR  	+ (INFRARED_NB 		* MIDI_STRUCT_SIZE))
#define JOYSTICK_Y_MIDI_STR_EE_BASE_ADR		(JOYSTICK_X_MIDI_STR_EE_BASE_ADR  	+ (JOYSTICK_NB 		* MIDI_STRUCT_SIZE))
#define POTS_MIDI_STR_EE_BASE_ADR			(JOYSTICK_Y_MIDI_STR_EE_BASE_ADR	+ (JOYSTICK_NB		* MIDI_STRUCT_SIZE))
#define CROSSFADER_L_MIDI_STR_EE_BASE_ADR	(POTS_MIDI_STR_EE_BASE_ADR  		+ (POT_NB 	   		* MIDI_STRUCT_SIZE))
#define CROSSFADER_R_MIDI_STR_EE_BASE_ADR	(CROSSFADER_L_MIDI_STR_EE_BASE_ADR	+ (CROSSFADER_NB	* MIDI_STRUCT_SIZE))
// switches
#define BW_ASSIGN_SW_MIDI_STR_EE_BASE_ADR	(CROSSFADER_R_MIDI_STR_EE_BASE_ADR 	+ (CROSSFADER_NB  	* MIDI_STRUCT_SIZE))
#define FW_ASSIGN_SW_MIDI_STR_EE_BASE_ADR	(BW_ASSIGN_SW_MIDI_STR_EE_BASE_ADR 	+ (BW_ASSIGN_SW_NB 	* MIDI_STRUCT_SIZE))
#define STOP_ASSIGN_SW_MIDI_STR_EE_BASE_ADR	(FW_ASSIGN_SW_MIDI_STR_EE_BASE_ADR 	+ (FW_ASSIGN_SW_NB 	* MIDI_STRUCT_SIZE))
#define PLAY_ASSIGN_SW_MIDI_STR_EE_BASE_ADR	(STOP_ASSIGN_SW_MIDI_STR_EE_BASE_ADR+ (STOP_ASSIGN_SW_NB* MIDI_STRUCT_SIZE))
#define REC_ASSIGN_SW_MIDI_STR_EE_BASE_ADR	(PLAY_ASSIGN_SW_MIDI_STR_EE_BASE_ADR+ (PLAY_ASSIGN_SW_NB* MIDI_STRUCT_SIZE))
#define JOYSTICK_SW_MIDI_STR_EE_BASE_ADR	(REC_ASSIGN_SW_MIDI_STR_EE_BASE_ADR	+ (REC_ASSIGN_SW_NB	* MIDI_STRUCT_SIZE))
#define FOOTSWITCH_MIDI_STR_EE_BASE_ADR		(JOYSTICK_SW_MIDI_STR_EE_BASE_ADR  	+ (JOYSTICK_SW_NB 	* MIDI_STRUCT_SIZE))
#define ENCODER_SW_MIDI_STR_EE_BASE_ADR		(FOOTSWITCH_MIDI_STR_EE_BASE_ADR  	+ (FOOTSWITCH_NB 	* MIDI_STRUCT_SIZE))
#define SW_MIDI_STR_EE_BASE_ADR				(ENCODER_SW_MIDI_STR_EE_BASE_ADR	+ (ENCODER_SW_NB	* MIDI_STRUCT_SIZE))
//lfo
#define LFO_MIDI_STR_EE_BASE_ADR			(SW_MIDI_STR_EE_BASE_ADR  			+ (SW_NB			* MIDI_STRUCT_SIZE))
// total size for group 0
#define MIDI_STR_GROUP_0_EE_END_ADR			(LFO_MIDI_STR_EE_BASE_ADR 			+ (LFO_NB	    	* MIDI_STRUCT_SIZE))

#define MIDI_STR_EE_LENGHT_PER_GROUP		(MIDI_STR_GROUP_0_EE_END_ADR - INFRARED_MIDI_STR_EE_BASE_ADR)
#define MIDI_STR_EE_END_ADR					(MIDI_STR_EE_BASE_ADR + INFRARED_MIDI_STR_EE_BASE_ADR + (GROUP_NB * MIDI_STR_EE_LENGHT_PER_GROUP))

////////////////////////////////////// LCD STRINGS MAPPING ////////////////////////////////////////////////////////////////////

//#define LCD_STR_EE_BASE_ADR				 	(MIDI_STR_EE_END_ADR - EEPROM_ONE_FULL_SIZE)
#define LCD_STR_EE_BASE_ADR				 	(MIDI_STR_EE_END_ADR)

// check if coherent :
#if (LCD_STR_EE_BASE_ADR % MIDI_STRUCT_SIZE)
	#error _LCD_STR_EE_BASE_ADR_ is not aligned on a eeprom page size
#endif

// no LCD str on transport functions
// analog
#define INFRARED_LCD_STR_EE_BASE_ADR		(LCD_STR_EE_BASE_ADR)
#define JOYSTICK_X_LCD_STR_EE_BASE_ADR		(INFRARED_LCD_STR_EE_BASE_ADR  		+ (INFRARED_NB 	 	* LCD_STR_SIZE))
#define JOYSTICK_Y_LCD_STR_EE_BASE_ADR		(JOYSTICK_X_LCD_STR_EE_BASE_ADR  	+ (JOYSTICK_NB 	 	* LCD_STR_SIZE))
#define POTS_LCD_STR_EE_BASE_ADR			(JOYSTICK_Y_LCD_STR_EE_BASE_ADR  	+ (JOYSTICK_NB		* LCD_STR_SIZE))
#define CROSSFADER_L_LCD_STR_EE_BASE_ADR	(POTS_LCD_STR_EE_BASE_ADR   		+ (POT_NB 	   		* LCD_STR_SIZE))
#define CROSSFADER_R_LCD_STR_EE_BASE_ADR	(CROSSFADER_L_LCD_STR_EE_BASE_ADR	+ (CROSSFADER_NB	* LCD_STR_SIZE))
//switches
#define BW_ASSIGN_SW_LCD_STR_EE_BASE_ADR	(CROSSFADER_R_LCD_STR_EE_BASE_ADR 	+ (CROSSFADER_NB  	* LCD_STR_SIZE))
#define FW_ASSIGN_SW_LCD_STR_EE_BASE_ADR	(BW_ASSIGN_SW_LCD_STR_EE_BASE_ADR 	+ (BW_ASSIGN_SW_NB 	* LCD_STR_SIZE))
#define STOP_ASSIGN_SW_LCD_STR_EE_BASE_ADR	(FW_ASSIGN_SW_LCD_STR_EE_BASE_ADR 	+ (FW_ASSIGN_SW_NB 	* LCD_STR_SIZE))
#define PLAY_ASSIGN_SW_LCD_STR_EE_BASE_ADR	(STOP_ASSIGN_SW_LCD_STR_EE_BASE_ADR	+ (STOP_ASSIGN_SW_NB* LCD_STR_SIZE))
#define REC_ASSIGN_SW_LCD_STR_EE_BASE_ADR	(PLAY_ASSIGN_SW_LCD_STR_EE_BASE_ADR	+ (PLAY_ASSIGN_SW_NB* LCD_STR_SIZE))
#define JOYSTICK_SW_LCD_STR_EE_BASE_ADR		(REC_ASSIGN_SW_LCD_STR_EE_BASE_ADR 	+ (REC_ASSIGN_SW_NB	* LCD_STR_SIZE))
#define FOOTSWITCH_LCD_STR_EE_BASE_ADR		(JOYSTICK_SW_LCD_STR_EE_BASE_ADR  	+ (JOYSTICK_SW_NB 	* LCD_STR_SIZE))
#define ENCODER_SW_LCD_STR_EE_BASE_ADR		(FOOTSWITCH_LCD_STR_EE_BASE_ADR		+ (FOOTSWITCH_NB 	* LCD_STR_SIZE))
#define SW_LCD_STR_EE_BASE_ADR				(ENCODER_SW_LCD_STR_EE_BASE_ADR		+ (ENCODER_SW_NB 	* LCD_STR_SIZE))
// lfo
#define LFO_LCD_STR_EE_BASE_ADR				(SW_LCD_STR_EE_BASE_ADR  			+ (SW_NB 			* LCD_STR_SIZE))

// total size of lcd strings for group 0
#define LCD_STR_GROUP_0_EE_END_ADR			(LFO_LCD_STR_EE_BASE_ADR			+ (LFO_NB	 		* LCD_STR_SIZE))

#define LCD_STR_EE_LENGHT_PER_GROUP			(LCD_STR_GROUP_0_EE_END_ADR - INFRARED_LCD_STR_EE_BASE_ADR)
#define LCD_STR_EE_END_ADR					(LCD_STR_EE_BASE_ADR  + (GROUP_NB * LCD_STR_EE_LENGHT_PER_GROUP))

// check if coherent :
#ifndef LCD_STR_EE_END_ADR
	#error _LCD_STR_EE_END_ADR_ is not defined in eeprom.h
#endif

#if LCD_STR_EE_END_ADR > EEPROM_ONE_TWO_FULL_SIZE
	#error EEPROM 1 PLUS 2 SIZES ARE TOO SHORT FOR MIDI AND LCD STRINGS SIZE 
#endif

#if (LCD_STR_EE_END_ADR % LCD_STR_SIZE)
	#error _LCD_STR_EE_BASE_ADR_ is not aligned on a eeprom page size
#endif

// ******************************************************************************************************************
// ************************************************** EEPROM 0 ******************************************************
// ******************************************************************************************************************

//////////////////////////////////////// SCENES MAPPING ////////////////////////////////////////////////////////////////////
// Scenes location are located in first EEPROM, starting from address 0
// Organized as follow : scene 0 (all ctrls), scene 1 (all controls), ...
#define SCENE_PAGE_SIZE						 1	// scenes are re-aligned on EEPROM pages size
												// there are 61 controls used with scenes, align on 64 bytes
#define SCENE_OFFSET						(SCENE_PAGE_SIZE * EEPROM_PAGE_SIZE)
#define SCENE_BLOC_SIZE						(POT_NB+INFRARED_NB+JOYSTICK_NB+JOYSTICK_NB+CROSSFADER_NB+CROSSFADER_NB+(OTHER_SW_NB/8)+(SW_NB/8))

// check if coherent :
#if (SCENE_BLOC_SIZE > EEPROM_PAGE_SIZE)
	#error SCENE BLOCK SIZE IS HIGHER THAN 1 EEPROM PAGE (64 bytes) 
#endif

#define SCENE_BASE_EE_ADR					 0	// begining of first eeprom
// check if coherent :
#if (SCENE_BASE_EE_ADR % 64)
	#error _SCENE_BASE_EE_ADR_ is not aligned on a eeprom page size
#endif

// no scene on transports
// analog
#define INFRARED_SCENE_EE_BASE_ADR			(SCENE_BASE_EE_ADR)
#define JOYSTICK_X_SCENE_EE_BASE_ADR		(INFRARED_SCENE_EE_BASE_ADR  		+ (INFRARED_NB 		))
#define JOYSTICK_Y_SCENE_EE_BASE_ADR		(JOYSTICK_X_SCENE_EE_BASE_ADR  		+ (JOYSTICK_NB 		))
#define POTS_SCENE_EE_BASE_ADR			  	(JOYSTICK_Y_SCENE_EE_BASE_ADR		+ (JOYSTICK_NB 		))
#define CROSSFADER_L_SCENE_EE_BASE_ADR		(POTS_SCENE_EE_BASE_ADR		  		+ (POT_NB			))	
#define CROSSFADER_R_SCENE_EE_BASE_ADR		(CROSSFADER_L_SCENE_EE_BASE_ADR		+ (CROSSFADER_NB	))	
// switches
#define OTHER_SW_SCENE_EE_BASE_ADR			(CROSSFADER_R_SCENE_EE_BASE_ADR		+ (CROSSFADER_NB	))
#define SW_SCENE_EE_BASE_ADR				(OTHER_SW_SCENE_EE_BASE_ADR			+ (OTHER_SW_NB / 8U	))
#define SCENE_0_EE_END_ADR				    (SW_SCENE_EE_BASE_ADR				+ (SW_NB / 8U		)) // sw scene is 1 bit
// LFO has no scene
#define END_SCENE_EE_ADR					(SCENE_OFFSET * SCENE_NB)	// 6400	

#ifndef END_SCENE_EE_ADR
	#error _END_SCENE_EE_ADR_ is not defined in eeprom.h
#endif

/////////////////////////////////  GROUP NAMES MAPPING ///////////////////////////////////////////////////////

// Group names are located in first eeprom after scenes
#define GROUP_NAME_BASE_EE_ADR				 END_SCENE_EE_ADR												// 6400
// check if coherent :
#if (GROUP_NAME_BASE_EE_ADR % 64)
	#error _GROUP_NAME_BASE_EE_ADR_ is not aligned on a eeprom page size
#endif

#define GROUP_NAME_END_EE_ADR				(GROUP_NAME_BASE_EE_ADR + (GROUP_NB * GROUP_NAME_EE_LENGHT))	// 6400 + (21*16) = 6736

#ifndef GROUP_NAME_END_EE_ADR
	#error _GROUP_NAME_END_EE_ADR_ is not defined in eeprom.h
#endif

/////////////////////////////////  CONTROL REGISTERS MAPPING /////////////////////////////////////////////////
// Control and status registers are located in first eeprom after group names realigned on eeprom page size
// Group names are 16 byte lenght : the next mapping is control regs which MUST be alligned on a eeprom page
// Depending on group nb : the first address of control regs must be offseted
#if (GROUP_NAME_END_EE_ADR % 64)
	#define CTRL_REG_OFFSET_TO_ADD			(EEPROM_PAGE_SIZE-(GROUP_NAME_END_EE_ADR % 64))
#else
	#define CTRL_REG_OFFSET_TO_ADD			0
#endif

#define CTRL_REG_AND_SW_START_EE_ADR		 (GROUP_NAME_END_EE_ADR + CTRL_REG_OFFSET_TO_ADD)
// check if coherent :
#if (CTRL_REG_AND_SW_START_EE_ADR % 64)
	#error _CTRL_REG_AND_SW_START_EE_ADR_ is not aligned on a eeprom page size
#endif
// start with 2 bytes for all 16 switches
#define OTHER_SWITCHES_EE_ADR				 CTRL_REG_AND_SW_START_EE_ADR
#define ALL_SWITCHES_EE_ADR					(OTHER_SWITCHES_EE_ADR + 1)
// then all control registers
#define CTRL_REG_START_EE_ADR				(ALL_SWITCHES_EE_ADR   + 1)
// the first ones must be sent to EZ board at startup and on change
#define CTRL_REG_0_7BITS_TO_EZ_EE_ADR		(CTRL_REG_START_EE_ADR)
#define CTRL_REG_1_7BITS_TO_EZ_EE_ADR		(CTRL_REG_START_EE_ADR + 1)
#define LAST_CTRL_REG_7BITS_TO_EZ_EE_ADR	(CTRL_REG_START_EE_ADR + 2)
// the followings are only used by Z8 board
#define Z8_STATUS_0_EE_ADR					(CTRL_REG_START_EE_ADR + 3)
#define Z8_STATUS_1_EE_ADR					(CTRL_REG_START_EE_ADR + 4)
#define Z8_STATUS_2_EE_ADR					(CTRL_REG_START_EE_ADR + 5)
#define MIDI_CHANNEL_EE_ADR					(CTRL_REG_START_EE_ADR + 6)
#define GROUP_EE_ADR						(CTRL_REG_START_EE_ADR + 7)
#define SCENE_SELECT_EE_ADR					(CTRL_REG_START_EE_ADR + 8)
#define INFRARED_HOLD_EE_ADR				(CTRL_REG_START_EE_ADR + 9)
#define NEXT_FUNCTION_EE_ADR				(CTRL_REG_START_EE_ADR + 10)
#define CURRENT_AUTOMATION_EE_ADR			(CTRL_REG_START_EE_ADR + 11)
#define MOTION_PART_EE_ADR					(CTRL_REG_START_EE_ADR + 12)
#define LFO_AMPL_EE_ADR						(CTRL_REG_START_EE_ADR + 13)
#define LFO_FREQ_EE_ADR						(CTRL_REG_START_EE_ADR + 14)
#define LFO_OFFSET_EE_ADR					(CTRL_REG_START_EE_ADR + 15)
#define ARP_GATE_EE_ADR						(CTRL_REG_START_EE_ADR + 16)
#define ARP_ACCENT_EE_ADR					(CTRL_REG_START_EE_ADR + 17)
#define ARP_RHYTHM_EE_ADR					(CTRL_REG_START_EE_ADR + 18)
#define ARP_RANGE_EE_ADR					(CTRL_REG_START_EE_ADR + 19)
#define ARP_TRANSPOSE_EE_ADR				(CTRL_REG_START_EE_ADR + 20)
#define ARP_MELODY_EE_ADR					(CTRL_REG_START_EE_ADR + 21)
#define ARP_DIR_EE_ADR						(CTRL_REG_START_EE_ADR + 22)
#define ARP_CLOCK_RESOLUTION_EE_ADR			(CTRL_REG_START_EE_ADR + 23)
#define ARP_CHANNEL_EE_ADR					(CTRL_REG_START_EE_ADR + 24)
#define ARP_VELOCITY_EE_ADR					(CTRL_REG_START_EE_ADR + 25)
#define ARP_SWING_EE_ADR					(CTRL_REG_START_EE_ADR + 26)
#define ARP_MELODY_BNK_EE_ADR				(CTRL_REG_START_EE_ADR + 27)
#define ARP_RHYTHM_BNK_EE_ADR				(CTRL_REG_START_EE_ADR + 28)
#define ARP_ACCENT_BNK_EE_ADR				(CTRL_REG_START_EE_ADR + 29)
#define ARP_MODE_EE_ADR						(CTRL_REG_START_EE_ADR + 30)
#define ARP_OFFSET_EE_ADR					(CTRL_REG_START_EE_ADR + 31)
#define INTERNAL_TEMPO_EE_ADR				(CTRL_REG_START_EE_ADR + 32)
#define OPERATING_MODE_NVM_EE_ADR			(CTRL_REG_START_EE_ADR + 33) // V1.69
#define Z8_STATUS_3_NVM_EE_ADR				(CTRL_REG_START_EE_ADR + 34) // V1.8
#define SCENE_SELECT_ARP_EE_ADR				(CTRL_REG_START_EE_ADR + 35) // V1.8

// end of control regs
#define CTRL_REG_REAL_END_EE_ADR			(SCENE_SELECT_ARP_EE_ADR + 1) // V1.8
#define CTRL_REG_REAL_NB					(CTRL_REG_REAL_END_EE_ADR - CTRL_REG_START_EE_ADR) // 14U
#define CTRL_REG_PAGE_NB					1U	// Control registers use 1 page total
#define CTRL_REG_END_EE_ADR					(CTRL_REG_AND_SW_START_EE_ADR + (CTRL_REG_PAGE_NB * EEPROM_PAGE_SIZE))	// 6736 + 64 = 6800

// check if coherent :
#if (CTRL_REG_REAL_NB >= 64)
	#error _CTRL_REG_REAL_NB_ is higher than 64
#endif

#if (CTRL_REG_END_EE_ADR % 64)
	#error _CTRL_REG_END_EE_ADR_ is not aligned on a eeprom page size
#endif

#ifndef CTRL_REG_END_EE_ADR
	#error _CTRL_REG_END_EE_ADR_ is not defined in eeprom.h
#endif

///////////////////////////////// USER CURVE MODELS DATA MAPPING ///////////////////////////////////////////////////////
// user defined curves models are located after the 64 control registers
#define CURVE_USER_0_LOW_START_EE_ADR		 CTRL_REG_END_EE_ADR
#define CURVE_USER_0_HIGH_START_EE_ADR		 (CURVE_USER_0_LOW_START_EE_ADR  + (CURVE_TABLES_LENGHT/2))		// 6864
#define CURVE_USER_1_LOW_START_EE_ADR		 (CURVE_USER_0_HIGH_START_EE_ADR + (CURVE_TABLES_LENGHT/2))		// 6928
#define CURVE_USER_1_HIGH_START_EE_ADR		 (CURVE_USER_1_LOW_START_EE_ADR  + (CURVE_TABLES_LENGHT/2))		// 6992
#define CURVE_USER_END_EE_ADR			 	 (CURVE_USER_1_HIGH_START_EE_ADR + (CURVE_TABLES_LENGHT/2))		// 7056

// check if coherent :
#if (CURVE_USER_END_EE_ADR % 64)
	#error _CURVE_USER_END_EE_ADR_ is not aligned on a eeprom page size
#endif

#ifndef CURVE_USER_END_EE_ADR
	#error _CURVE_USER_END_EE_ADR_ is not defined in eeprom.h
#endif

///////////////////////////////// HOOK AND TOGGLE DATA MAPPING ///////////////////////////////////////////////////////
// Hook and toggles values are located after the 64 control registers
//#define HOOK_VALUES_START_EE_ADR			 CTRL_REG_END_EE_ADR
//#define HOOK_PAGE_SIZE					 1	// Hook and toggles values are aligned on pages size
//#define HOOK_VALUES_END_EE_ADR			 (HOOK_VALUES_START_EE_ADR + (HOOK_PAGE_SIZE * GROUP_NB))

///////////////////////////////////// AUTOMATIONS DATA MAPPING FOR MOTION ////////////////////////////////////////////
// Motion recorded data are located after hook/toggle data

#define MOTION_START_EE_ADR			 		CURVE_USER_END_EE_ADR // HOOK_VALUES_END_EE_ADR
#define MOTION_DATA_PART_0_EE_ADR			MOTION_START_EE_ADR									// 7056 + 64 = 7120
#define MOTION_DATA_PART_1_EE_ADR			(MOTION_DATA_PART_0_EE_ADR + MOTION_PART_LENGHT)	// 7184
#define MOTION_DATA_PART_2_EE_ADR			(MOTION_DATA_PART_1_EE_ADR + MOTION_PART_LENGHT)	// 7248
#define MOTION_DATA_PART_3_EE_ADR			(MOTION_DATA_PART_2_EE_ADR + MOTION_PART_LENGHT)	// 7312
#define MOTION_DATA_PART_4_EE_ADR			(MOTION_DATA_PART_3_EE_ADR + MOTION_PART_LENGHT)	// 7376
#define MOTION_DATA_PART_5_EE_ADR			(MOTION_DATA_PART_4_EE_ADR + MOTION_PART_LENGHT)	// 7440
#define MOTION_DATA_PART_6_EE_ADR			(MOTION_DATA_PART_5_EE_ADR + MOTION_PART_LENGHT)	// 7504
#define MOTION_DATA_PART_7_EE_ADR			(MOTION_DATA_PART_6_EE_ADR + MOTION_PART_LENGHT)	// 7568

#define MOTION_LENGHT_AND_INDEX_SIZE		2U	// the couple motion lenght & motion index is also saved: 2 bytes

// here we reserve a whole eeprom page 64 bytes (even data below are only 16 bytes 2 * 8)

#define MOTION_LENGHT_AND_INDEX_0_EE_ADR	(MOTION_DATA_PART_7_EE_ADR + MOTION_PART_LENGHT)					// 7568 + 2 = 7570 
#define MOTION_LENGHT_AND_INDEX_1_EE_ADR	(MOTION_LENGHT_AND_INDEX_0_EE_ADR + MOTION_LENGHT_AND_INDEX_SIZE)	// 7572
#define MOTION_LENGHT_AND_INDEX_2_EE_ADR	(MOTION_LENGHT_AND_INDEX_1_EE_ADR + MOTION_LENGHT_AND_INDEX_SIZE)	// 7574
#define MOTION_LENGHT_AND_INDEX_3_EE_ADR	(MOTION_LENGHT_AND_INDEX_2_EE_ADR + MOTION_LENGHT_AND_INDEX_SIZE)	// 7576
#define MOTION_LENGHT_AND_INDEX_4_EE_ADR	(MOTION_LENGHT_AND_INDEX_3_EE_ADR + MOTION_LENGHT_AND_INDEX_SIZE)	// 7578
#define MOTION_LENGHT_AND_INDEX_5_EE_ADR	(MOTION_LENGHT_AND_INDEX_4_EE_ADR + MOTION_LENGHT_AND_INDEX_SIZE)	// 7580
#define MOTION_LENGHT_AND_INDEX_6_EE_ADR	(MOTION_LENGHT_AND_INDEX_5_EE_ADR + MOTION_LENGHT_AND_INDEX_SIZE)	// 7582
#define MOTION_LENGHT_AND_INDEX_7_EE_ADR	(MOTION_LENGHT_AND_INDEX_6_EE_ADR + MOTION_LENGHT_AND_INDEX_SIZE)	// 7584

#if ((MOTION_LENGHT_AND_INDEX_7_EE_ADR-MOTION_LENGHT_AND_INDEX_0_EE_ADR) >= 64)
	#error _MOTION_REAL_END_EE_ADR_ is higher than 64
#endif

#define MOTION_REAL_END_EE_ADR		 		(MOTION_START_EE_ADR + MOTION_LENGHT_AND_INDEX_7_EE_ADR)
#define MOTION_END_EE_ADR			 		(MOTION_LENGHT_AND_INDEX_0_EE_ADR + EEPROM_PAGE_SIZE)				// 7568 + 64 = 7632


//	#comment EEPROM_0 TOTAL SIZE: MOTION_END_EE_ADR

//#define MOTION_TOTAL_DATA_LENGHT			(MOTION_PART_NB * MOTION_PART_LENGHT)

/*
#define MOTION_SAMPLE_SIZE					6U // int time; uchar pass; uchar ctrl0; uchar value0; uchar ctrl1; uchar value1;
#define MOTION_SAMPLING_RATE				28U
#define MOTION_PASS_NB						8U
#define MOTION_CTRL_PER_PASS				2U
#define MOTION_TIME_LENGHT					9U
#define MOTION_TOTAL_LENGHT					(MOTION_SAMPLE_SIZE*MOTION_SAMPLING_RATE*MOTION_PASS_NB*MOTION_CTRL_PER_PASS*MOTION_TIME_LENGHT)
*/

// check if coherent :
#if (MOTION_START_EE_ADR % 64)
	#error Motion Start address MUST be aligned on EEprom page
#endif
#if (MOTION_PART_LENGHT % 64)
	#error Motion part lenght MUST be modulo 64
#endif
#if (MOTION_PART_LENGHT > 64)
	#error Motion part lenght MUST be lower than 64
#endif

// check if coherent :
#if (MOTION_END_EE_ADR % MIDI_STRUCT_SIZE)
	#error _MOTION_END_EE_ADR_ is not aligned on a eeprom page size
#endif

#ifndef MOTION_END_EE_ADR
	#error _MOTION_END_EE_ADR_ is not defined in eeprom.h
#endif

///////////////////////////////// V1.8 ARPPEGGIO SCENES MAPPING IN EEEPROM 0 ///////////////////////////////////////////////////////
#define ARP_SCENES_START_EE_ADR			 MOTION_END_EE_ADR
#define ARP_SCENES_TOTAL_BYTES			(ARP_PARAM_NB * SCENE_ARP_NB)
#define ARP_SCENES_END_EE_ADR			(ARP_SCENES_START_EE_ADR + ARP_SCENES_TOTAL_BYTES)

// check if coherent :
#if (ARP_SCENES_START_EE_ADR % 64)
	#error _ARP_SCENES_START_EE_ADR_ is not aligned on a eeprom page size
#endif

//#if (ARP_PARAM_NB % 16)
//	#error _ARP_PARAM_NB_ is not 16
//#endif

#if (ARP_SCENES_END_EE_ADR > EEPROM_ZERO_FULL_SIZE)
	#error _ARP_SCENES_END_EE_ADR_ is higher than eeprom0 full size !
#endif

#if !defined(ARP_PARAM_NB)
	#error _ARP_PARAM_NB_ is not defined
#endif

#if !defined(SCENE_ARP_NB) 
	#error _SCENE_ARP_NB_ is not defined
#endif

#if !defined(ARP_SCENES_START_EE_ADR)  
	#error _ARP_SCENES_START_EE_ADR_ is not defined
#endif

//#if (ARP_SCENES_START_EE_ADR > 7168)
//	#error _ARP_SCENES_START_EE_ADR_ is not defined
//#endif
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// EXPANSION BOARD MIDI STRINGS MAPPING ////////////////////////////////////////////
// Expansion board controls midi and lcd strings are located at the end of eeprom 0
// These Midi strings must be aligned with a page size or half page size since MIDI struct size is 32


// expansion analog midi strings
#define EXP_ANA_MIDI_STR_EE_BASE_ADR		(MOTION_END_EE_ADR)		// 7632
// expansion digital midi strings
#define EXP_DIG_MIDI_STR_EE_BASE_ADR		(EXP_ANA_MIDI_STR_EE_BASE_ADR  		+ (EXP_ANA_NB		* MIDI_STRUCT_SIZE))
// end of midi strings for group 0
#define EXP_MIDI_STR_GROUP_0_EE_END_ADR		(EXP_DIG_MIDI_STR_EE_BASE_ADR  		+ (EXP_DIG_NB		* MIDI_STRUCT_SIZE))

#define EXP_MIDI_STR_EE_LENGHT_PER_GROUP	(EXP_MIDI_STR_GROUP_0_EE_END_ADR - EXP_ANA_MIDI_STR_EE_BASE_ADR)
#define EXP_MIDI_STR_EE_END_ADR				(EXP_ANA_MIDI_STR_EE_BASE_ADR + (GROUP_NB * EXP_MIDI_STR_EE_LENGHT_PER_GROUP))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// EXPANSION BOARD LCD STRINGS MAPPING ////////////////////////////////////////////

#define EXP_LCD_STR_EE_BASE_ADR			 	(EXP_MIDI_STR_EE_END_ADR)

// check if coherent :
#if (EXP_LCD_STR_EE_BASE_ADR % MIDI_STRUCT_SIZE)
	#error _EXP_LCD_STR_EE_BASE_ADR_ is not aligned on a eeprom page size
#endif

// expansion analog lcd strings
#define EXP_ANA_LCD_STR_EE_BASE_ADR			(EXP_LCD_STR_EE_BASE_ADR)

// expansion digital lcd strings
#define EXP_DIG_LCD_STR_EE_BASE_ADR			(EXP_ANA_LCD_STR_EE_BASE_ADR  		+ (EXP_ANA_NB		* LCD_STR_SIZE))

// total size of expansion lcd strings for group 0
#define EXP_LCD_STR_GROUP_0_EE_END_ADR		(EXP_DIG_LCD_STR_EE_BASE_ADR		+ (EXP_DIG_NB 		* LCD_STR_SIZE))

#define EXP_LCD_STR_EE_LENGHT_PER_GROUP		(EXP_LCD_STR_GROUP_0_EE_END_ADR - EXP_LCD_STR_EE_BASE_ADR)
#define EXP_LCD_STR_EE_END_ADR				(EXP_LCD_STR_EE_BASE_ADR  + (GROUP_NB * EXP_LCD_STR_EE_LENGHT_PER_GROUP))

// check if coherent :
#if (EXP_LCD_STR_EE_END_ADR % LCD_STR_SIZE)
	#error _EXP_LCD_STR_EE_END_ADR_ is not aligned on a eeprom page size
#endif

#if EXP_LCD_STR_EE_END_ADR >= EEPROM_ZERO_FULL_SIZE
	#error EEPROM 0 SIZE IS TOO SHORT FOR SCENES, GRP NAMES, CONTROLS REGS, MOTION DATA, EXPANSION BOARD (MIDI + LCD) STRINGS
#endif
*/

// PERFORM FINAL CHECKS ON DEFINES
#define EE_BYTES_NB_PER_CTRL			(MIDI_STRUCT_SIZE + LCD_STR_SIZE)
#define TOTAL_EE_BYTES_ASSIGNED			(TRANSPORT_NB * MIDI_STRUCT_SIZE)+(CONTROLS_WITH_GROUP_NB * EE_BYTES_NB_PER_CTRL * GROUP_NB)

#if (TOTAL_EE_BYTES_ASSIGNED > EEPROM_ONE_TWO_FULL_SIZE)
	#error EEPROM 1 PLUS 2 SIZES ARE TOO SHORT FOR MIDI AND LCD STRINGS SIZE
#endif
#if (TOTAL_EE_BYTES_ASSIGNED != LCD_STR_EE_END_ADR)
	#error TOTAL EEPROM BYTES MISMACTH 
#endif

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////


void  GetCtrlStructEEaddFromIndexGroup(void);
void  GetLcdStrEEaddFromIndexGroup(void);
//void  AckPolling(void);
void LockEEprom10Ms(void);
void GetCurrentGroupSwitchModesFromEE(void);

#endif