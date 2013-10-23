#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "main.h"
#include "hardware.h"
#include "analog.h"
#include "serial.h" 
#include "i2c_low_level.h"
#include "i2c.h" 
#include "itrs.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "digital.h"
#include "scene.h"
#include "automation.h"
#include "autotest.h"
#include "lfo.h" 
#include "menus.h" 
#include "assign.h" 
#include "realtime.h" 
#include "expansion.h"
#include "smpte.h" // V1.8

// set the option bits
FLASH_OPTION1 = 0xFF;
FLASH_OPTION2 = 0xFF;

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
extern rom uchar bmp_home[];
extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_7;
//extern uchar STATUS_Z8_EXP; // V1.5
extern uint  current_random;
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];		// NN=1 registers can be accessed from ez board 
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 

///////////////////////////////////////////////////////////////////////////////////////////
// Main program beings here 
///////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{	
	// Disable Interrupts
	DI();		
	// Initialize IOs directions, default values and alternate functions
	InitIos();
	// Initialize LCD display
	InitLcd();
	// Initialize I2C Controller periferal
	InitI2C(); 
	// Initialize Serial periferals I2C and UARTS 0 & 1
	InitSerial(); 
	// Initialize Interrupt controller + vectors
	InitItrs();
	// Initialize default values
	InitInitialValues();
	// V1.8 Init SMPTE frame
	InitSmpteTime();
	// Init all timers
	InitTimers();
	// Enable Interrupts	
	EI();	
	// Sync to ez board
	SyncToEzBoard();
	// Check if factory default, or autotest request
	CheckBootStatus();
	// V1.8 Load individual hook config for all analog controls (load from eeprom)
//	LoadHookModesFromEEprom();
	// Initialize Analog periferals
	InitAnalog(); 
	// Enable Interrupts	
//V1.6	EI();	

	// Display home screen
	DisplayBmp(0, 0, LCD_INVERT, bmp_home);
	Z8_STATUS_4 |= lcd_locked_by_function_z4;
	// Display rom version
	DisplayRomVersion(LCD_ZONE_FW_VERSION_HOME_X, LCD_ZONE_FW_VERSION_HOME_Y);
	
	// Sync to ez board
//V1.6	SyncToEzBoard();

	delay_msec(1700);

	// Check if expansion board is connected
	CheckExpBoardPresence();	

	// Lock LCD on home screen for 2.55 seconds	
//	lcd_lock_all_cpt = CPT_2_5_SEC;
	delay_msec(1700);

	Z8_STATUS_4 &= ~lcd_locked_by_function_z4;
	// Now reinit full lcd
	//Z8_STATUS_7 |= must_reinit_all_lcd_z7;

	// Main loop start from there
	while(1) {
				// Check if some params have to be updated from ITRs
				CheckParamChangedFromItr();

				// Check potentiometers
				CheckPots();

				// Check scene send / save
				CheckScene();

				// Check if a control reg for EZ board must be saved and sent
				CheckCtrlRegToSendToEz();

				// Check menu switches
				CheckMenuSwitches();

				// Check Infrared
				CheckInfrared();

				// V1.5 check if expansions generated a pulse ?
				if(EXPANSION_PORT_IN & EXPANSION_PIN)
				{
					// Expansions Modules are connected
					CheckExpansionChanges();
				}

				// Check which function is active
				CheckCurrentActiveFunction();

				// Check joystick analog X, Y
				CheckJoystick();

				//check if must send sysex tst mode for non assignable switches
				if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
					CheckSendSwSysexTstMode();

				// Check realtime to arpeggio, lfo, sampler, smpte
				CheckRealtimeMidi();

				// Check for Serial input buffer
				CheckCrossFader();

				// Check if we are waiting a control to assign
				AssignWaitControl();

				// check LFO & arpp
				CheckAutomationAnalog();

				// Check realtime to arpeggio, lfo, sampler, smpte
				CheckRealtimeMidi();

				// Check automation switches
				CheckAutomationSw();

				// Check for Serial input buffer
				CheckSerialInBuffer();

				// Check transport functions
				CheckTransport();

				// Check if a LFO data has to be sent
				CheckLfoToSend();

				// Check Joystick SW, encoder SW, Footswitch
				CheckOtherSwitches();

				// V1.9 Check realtime to arpeggio, lfo, sampler, smpte
				CheckRealtimeMidi();

				// Each 5 seconds check ctrl regs to save to eeprom
				CheckCtrlRegToSave5Sec();
				
				// Check switches
				CheckSwitches();

				// Generate next pseudo random number
				current_random = rand();

				// no more first pass flag
				Z8_STATUS_4 &= ~first_pass_z4;
	}

} // END PROGRAM


