#ifndef PINS_DEFS
#define PINS_DEFS

// PINS DEFINITIONS, PORTS MAPPING
#include "main.h"

// #define PE2_CONTROL_BL_EN			1
#define PE2_CONTROL_LCD_RESET 			1
#define PD6_JOYSTICK_BUTTON 			1
// #define SIMUL						1
#define MICROTIPS_MTGS12864KFGHSGW01 	1	// 
//#define SUNLIKE_SG12864HBWBGBK		1


// LCD RELATED PINS
#define LCD_DATA_PORT_OUT		PFOUT	// 8 bits data port io to/from LCD		GP OUT
#define LCD_DATA_PORT_IN		PFIN	// 8 bits data port io to/from LCD		GP OUT
#define LCD_E_PORT_OUT			PAOUT	// lcd_e pin, PAOUT0					GP OUT
#define LCD_E_PIN				(1<<0U)	// lcd_e pin, PAOUT0					GP OUT
#define LCD_RS_PORT_OUT			PAOUT	// lcd_rs pin, PAOUT0					GP OUT
#define LCD_RS_PIN				(1<<1U)	// lcd_rs pin, PAOUT1					GP OUT
#define LCD_RW_PORT_OUT			PAOUT	// lcd_rw pin, PAOUT2					GP OUT
#define LCD_RW_PIN				(1<<2U)	// lcd_rw pin, PAOUT2					GP OUT
#ifdef SUNLIKE_SG12864HBWBGBK
#define LCD_CS1_PORT_OUT		PAOUT	// lcd_rs pin, PAOUT3					GP OUT
#define LCD_CS1_PIN				(1<<3U)	// lcd_rs pin, PAOUT3					GP OUT
#define LCD_CS2_PORT_OUT		PCOUT	// lcd_rs pin, PCOUT5					GP OUT
#define LCD_CS2_PIN				(1<<5U)	// lcd_rs pin, PCOUT5					GP OUT
#endif
#ifdef MICROTIPS_MTGS12864KFGHSGW01
#define LCD_CS2_PORT_OUT		PAOUT	// lcd_rs pin, PAOUT3					GP OUT
#define LCD_CS2_PIN				(1<<3U)	// lcd_rs pin, PAOUT3					GP OUT
#define LCD_CS1_PORT_OUT		PCOUT	// lcd_rs pin, PCOUT5					GP OUT
#define LCD_CS1_PIN				(1<<5U)	// lcd_rs pin, PCOUT5					GP OUT
#endif

#ifdef PE2_CONTROL_BL_EN
#define LCD_BL_EN_PORT_OUT		PEOUT	// backlight en pin, PEOUT2				GP OUT
#define LCD_BL_EN_PIN			(1<<2U)	// backlight en pin, PEOUT2				GP OUT
#endif
#ifdef PE2_CONTROL_LCD_RESET
#define LCD_RESET_PORT_OUT		PEOUT	// reset lcd pin, PEOUT2				GP OUT
#define LCD_RESET_PIN			(1<<2U)	// reset lcd pin, PEOUT2				GP OUT
#endif


// ANALOG INPUTS PINS
#define ANA_INFRARED_PORT_IN	PHIN	// infrared pin, PHIN0					ALT FCT 
#define ANA_INFRARED_PIN		(1<<0U)	// infrared pin, PHIN0					ALT FCT
#define ANA_CROSSFADER_PORT_IN	PHIN	// crossfader pin, PHIN1				ALT FCT
#define ANA_CROSSFADER_PIN		(1<<1U)	// crossfader pin, PHIN1				ALT FCT
#define ANA_ROW0_PORT_IN		PBIN	// row 0 pin, PBIN0						ALT FCT
#define ANA_ROW0_PIN			(1<<0U)	// row 0 pin, PBIN0						ALT FCT
#define ANA_ROW1_PORT_IN		PBIN	// row 1 pin, PBIN1						ALT FCT
#define ANA_ROW1_PIN			(1<<1U)	// row 1 pin, PBIN1						ALT FCT
#define ANA_ROW4_PORT_IN		PBIN	// row 4 pin, PBIN4						ALT FCT
#define ANA_ROW4_PIN			(1<<4U)	// row 4 pin, PBIN4						ALT FCT
#define ANA_JOY_X_PORT_IN		PBIN	// joystick X pin, PBIN5				ALT FCT
#define ANA_JOY_X_PIN			(1<<5U)	// joystick X pin, PBIN5				ALT FCT
#define ANA_JOY_Y_PORT_IN		PBIN	// joystick Y pin, PBIN6				ALT FCT
#define ANA_JOY_Y_PIN			(1<<6U)	// joystick Y pin, PBIN6				ALT FCT
#define ANA_LFO_AMPL_PORT_IN	PBIN	// lfo amplitude pin, PBIN7				ALT FCT
#define ANA_LFO_AMPL_PIN		(1<<7U)	// lfo amplitude pin, PBIN7				ALT FCT
#define ANA_ROW3_PORT_IN		PBIN	// row 3 pin, PBIN3						ALT FCT
#define ANA_ROW3_PIN			(1<<3U)	// row 3 pin, PBIN3						ALT FCT
#define ANA_ROW2_PORT_IN		PBIN	// row 2 pin, PBIN2						ALT FCT
#define ANA_ROW2_PIN			(1<<2U)	// row 2 pin, PBIN2						ALT FCT
#define ANA_LFO_FREQ_PORT_IN	PHIN	// lfo frequency pin, PHIN2				ALT FCT
#define ANA_LFO_FREQ_PIN		(1<<2U)	// lfo frequency pin, PHIN2				ALT FCT
#define ANA_LFO_OFFS_PORT_IN	PHIN	// lfo amplitude pin, PHIN3				ALT FCT
#define ANA_LFO_OFFS_PIN		(1<<3U)	// lfo amplitude pin, PHIN3				ALT FCT

// FRONT PANEL BUTTONS ON INTERRUPT PINS
#define MODE_PORT_IN			PDIN	// mode button, PD0IN					GP IN WITH ITR
#define MODE_PIN				(1<<0U)	// mode button, PD0IN					GP IN WITH ITR
//#define FOOTSWITCH_PORT_IN	PDIN	// foot sw, PC0IN						GP IN WITH ITR
//#define FOOTSWITCH_PIN		(1<<1U)	// foot sw, PC0IN						GP IN WITH ITR
#define FOOTSWITCH_PORT_IN		PEIN	// foot sw, PC0IN						GP IN WITH ITR
#define FOOTSWITCH_PIN			(1<<1U)	// foot sw, PC0IN						GP IN WITH ITR

//#define EDIT_PORT_IN			PCIN	// edit, PC2IN							GP IN WITH ITR
//#define EDIT_PIN				(1<<2U)	// edit, PC2IN							GP IN WITH ITR
#define EDIT_PORT_IN			PCIN	// edit, PC0IN							GP IN WITH ITR
#define EDIT_PIN				(1<<0U)	// edit, PC0IN							GP IN WITH ITR

//#define OK_PORT_IN			PDIN	// ok, PD2IN							GP IN WITH ITR
//#define OK_PIN				(1<<2U)	// ok, PD2IN							GP IN WITH ITR
#define OK_PORT_IN				PCIN	// ok, PC1IN							GP IN WITH ITR
#define OK_PIN					(1<<1U)	// ok, PC1IN							GP IN WITH ITR

//#define CANCEL_PORT_IN		PDIN	// cancel, PD3IN						GP IN WITH ITR
//#define CANCEL_PIN			(1<<3U)	// cancel, PD3IN						GP IN WITH ITR
#define CANCEL_PORT_IN			PDIN	// cancel, PD3IN						GP IN WITH ITR
#define CANCEL_PIN				(1<<1U)	// cancel, PD3IN						GP IN WITH ITR


#define INFRARED_SW_PORT_IN		PDIN	// infrared sw, PD4IN					GP IN WITH ITR
#define INFRARED_SW_PIN			(1<<4U)	// infrared sw, PD4IN					GP IN WITH ITR
#ifdef PD6_JOYSTICK_BUTTON
#define JOYSTICK_SW_PORT_IN		PDIN	// joystick sw, PD6IN					GP IN WITH ITR
#define JOYSTICK_SW_PIN			(1<<6U)	// joystick sw, PD6IN					GP IN WITH ITR
#endif
#define UP_SW_PORT_IN			PCIN	// up sw, PC3IN							GP IN WITH ITR
#define UP_SW_PIN				(1<<3U)	// up sw, PC3IN							GP IN WITH ITR
#define DOWN_SW_PORT_IN			PDIN	// down sw, PD7IN						GP IN WITH ITR
#define DOWN_SW_PIN				(1<<7U)	// down sw, PD7IN						GP IN WITH ITR

//#define ENCODER_A_PORT_IN		PCIN	// encoder A, PD1IN						GP IN WITH ITR
//#define ENCODER_A_PIN			(1<<0U)	// encoder A, PD1IN						GP IN WITH ITR
#define ENCODER_A_PORT_IN		PCIN	// encoder A, PD1IN						GP IN WITH ITR
#define ENCODER_A_PIN			(1<<2U)	// encoder A, PD1IN						GP IN WITH ITR

//#define ENCODER_SW_PORT_IN	PCIN	// encoder sw, PC1IN					GP IN WITH ITR
//#define ENCODER_SW_PIN		(1<<1U)	// encoder sw, PC1IN					GP IN WITH ITR
#define ENCODER_SW_PORT_IN		PDIN	// encoder sw, PC1IN					GP IN WITH ITR
#define ENCODER_SW_PIN			(1<<2U)	// encoder sw, PC1IN					GP IN WITH ITR


// FRONT PANEL BUTTONS OR UC INPUTS WITHOUT INTERRUPT
#define ENCODER_B_PORT_IN		PEIN	// encoder B, PE0IN						GP IN 
#define ENCODER_B_PIN			(1<<0U)	// encoder B, PE0IN						GP IN 

//#define SHIFT_PORT_IN			PEIN	// shift, PE1IN							GP IN 
//#define SHIFT_PIN				(1<<1U)	// shift, PE1IN							GP IN 
#define SHIFT_PORT_IN			PDIN	// shift, PE1IN							GP IN 
#define SHIFT_PIN				(1<<3U)	// shift, PE1IN							GP IN 

#define CHANNEL_PORT_IN			PGIN	// channel, PG3IN						GP IN 
#define CHANNEL_PIN				(1<<3U)	// channel, PG3IN						GP IN 
#define GROUP_PORT_IN			PGIN	// group, PG4IN							GP IN 
#define GROUP_PIN				(1<<4U)	// group, PG4IN							GP IN 

//#define SCENE_SEL_PORT_IN		PGIN	// scene select, PG5IN					GP IN 
//#define SCENE_SEL_PIN			(1<<5U)	// scene select, PG5IN					GP IN 
#define SCENE_SEL_PORT_IN		PGIN	// scene select, PG5IN					GP IN 
#define SCENE_SEL_PIN			(1<<6U)	// scene select, PG5IN					GP IN 

//#define SNAPSHOT_PORT_IN		PGIN	// snapshot, PG6IN						GP IN 
//#define SNAPSHOT_PIN			(1<<6U)	// snapshot, PG6IN						GP IN 
#define SNAPSHOT_PORT_IN		PGIN	// snapshot, PG6IN						GP IN 
#define SNAPSHOT_PIN			(1<<5U)	// snapshot, PG6IN						GP IN 

#define EXPANSION_PORT_IN		PCIN	// expansion en, PC7IN					GP IN 
#define EXPANSION_PIN			(1<<7U)	// expansion en, PC7IN					GP IN 
#define EXPANSION_PORT_OUT		PCOUT	// expansion en, PC7OUT					GP OUT for autotest only V1.5

// FRONT PANEL MULTIPLEXED BUTTONS
#define PARAM_SW_1_IN			PCIN	// switches multiplexed, PC4IN			GP IN 
#define PARAM_SW_1_PIN			(1<<4U)	// switches multiplexed, PC4IN			GP IN 
#define PARAM_SW_0_IN			PCIN	// switches multiplexed, PC6IN			GP IN 
#define PARAM_SW_0_PIN			(1<<6U)	// switches multiplexed, PC6IN			GP IN 

// LEDS OUTPUTS PINS
#define LED_MIDI_1_PORT_OUT			PEOUT	// led midi out 1 pin, PEOUT3			GP OUT
#define LED_MIDI_1_PIN				(1<<3U)	// led midi out 1 pin, PEOUT3			GP OUT
#define LED_MIDI_2_PORT_OUT			PEOUT	// led midi out 2 pin, PEOUT4			GP OUT
#define LED_MIDI_2_PIN				(1<<4U)	// led midi out 2 pin, PEOUT4			GP OUT
#define LED_MOTION_PORT_OUT			PEOUT	// led motion pin, PEOUT5				GP OUT
#define LED_MOTION_PIN				(1<<5U)	// led motion pin, PEOUT5				GP OUT
#define LED_ARP_PORT_OUT			PEOUT	// led arp pin, PEOUT6					GP OUT
#define LED_ARP_PIN					(1<<6U)	// led arp pin, PEOUT6					GP OUT
#define LED_LFO_PORT_OUT			PEOUT	// led lfo pin, PEOUT7					GP OUT
#define LED_LFO_PIN					(1<<7U)	// led lfo pin, PEOUT7					GP OUT
#define LED_AUTOMATION_PORT_OUT		PGOUT	// led automation pin, PGOUT7			GP OUT
#define LED_AUTOMATION_PIN			(1<<7U)	// led automation pin, PGOUT7			GP OUT
#define LED_CURRENT_AUTOM_PORT_OUT	PEOUT	// leds for current autom			

// GENERAL PURPOSE OUTPUTS PINS
#define MUX_PORT_OUT			PGOUT	// A0,A1,A2 							GP OUT
#define A0_PIN					(1<<0U)	// A0									GP OUT
#define A1_PIN					(1<<1U)	// A1									GP OUT
#define A2_PIN					(1<<2U)	// A2									GP OUT

// SERIAL UART AND I2C COMUNICATION
#define MIDI_OUT_2_PORT_OUT		PDOUT	// midi out 2 pin, PEOUT4				ALT FCT
#define MIDI_OUT_2_PIN			(1<<5U)	// midi out 2 pin, PEOUT4				ALT FCT
#define TX0_TO_EZ_PORT_OUT		PAOUT	// tx to ez pin, PAOUT5					ALT FCT
#define TX0_TO_EZ_PIN			(1<<5U)	// tx to ez pin, PAOUT5					ALT FCT
#define RX0_FROM_EZ_PORT_IN		PAIN	// rx from ez pin, PAIN4				ALT FCT
#define RX0_FROM_EZ_PIN			(1<<4U)	// rx from ez pin, PAIN4				ALT FCT
#define I2C_SCL_PORT_OUT		PAOUT	// i2c scl pin, PAOUT6					ALT FCT
#define I2C_SCL_PIN				(1<<6U)	// i2c scl pin, PAOUT6					ALT FCT
#define I2C_SDA_PORT_OUT		PAOUT	// i2c sda pin, PAOUT7					ALT FCT
#define I2C_SDA_PORT_IN			PAIN	// i2c sda pin, PAIN7					ALT FCT
#define I2C_SDA_PIN				(1<<7U)	// i2c sda pin, PAOUT7					ALT FCT

#endif



