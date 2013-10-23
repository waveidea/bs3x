#ifndef AUTOTEST
#define AUTOTEST

#define TOTAL_STRUCT_TO_INIT		((GROUP_NB * CONTROLS_WITH_GROUP_NB) + (TRANSPORT_NB))
#define INIT_BARGRAPH_LENGHT		LCD_X_PIXEL_MAX
#define INIT_BARGRAPH_STEP			(TOTAL_STRUCT_TO_INIT / INIT_BARGRAPH_LENGHT)
#if INIT_BARGRAPH_STEP == 0U
	INIT_BARGRAPH_STEP = 1U
#endif

#define	NO_ERROR	0U
#define EEPROM_NB	3U

#define MIDI_STR_LENGHT_AUTOTEST_MODE	4U

#define EEPROM_AUTOTEST_EXPANSION_I2C_ADR	(0x07U<<1U)	// EEprom autotest I2C adress 

// TEST_RESULT_REG_0 bits definition
//#define test_ee_2	(1<<2)
//#define test_ee_1	(1<<1)
//#define test_ee_0	(1<<0)

#define AUTOTEST_VALUE		0x55
///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckBootStatus(void);
void FactoryDefault(void);
void FulfillControlStructure(void);
void AutoTest(void);
void CheckSendSwSysexTstMode(void);
void AutoTestExpansion(void);

#endif