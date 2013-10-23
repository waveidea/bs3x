#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "itrs.h"
#include "eeprom.h"
#include "serial.h"
#include "analog.h"
#include "realtime.h"
#include "automation.h"
#include "smpte.h" // V1.8

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
// uchar up_sw_lock_cpt;				// debounce up button
// uchar down_sw_lock_cpt;				// debounce down button
// uchar mode_sw_lock_cpt;				// debounce mode button
far uchar snapshot_long_push_cpt;		// detect short/long snapshot push // V1.64 far
// uchar encoder_sw_lock_cpt;
uchar lcd_lock_incrust_cpt;
far uchar ir_sw_long_push_cpt;	// V1.64 far
far uchar override_channel_cpt;	// V1.62

uchar led_midi_out1_cpt;
uchar led_midi_out2_cpt;

uchar row_analog_itr;
volatile uchar index_analog_itr;
uchar current_analog_index;
far uchar Analog[ANALOG_SOURCES_NB];
//uchar pin_state_param_sw0_itr;
//uchar pin_state_param_sw1_itr;
//uchar motion_sw_type_long_push;

extern volatile uchar TX_STATUS; // V1.8
extern uchar tx0_start_index;
extern uchar tx0_end_index;
extern uchar tx1_start_index;
extern uchar tx1_end_index;
extern far uchar TX0_Buff[TX_BUFF_LENGTH];
extern far uchar TX1_Buff[TX_BUFF_LENGTH];
//extern uchar lcd_lock_all_cpt;						// allow to lock all LCD zones

uint seconds_5_timer_cpt;			// used to create a 5 seconds timer from timer 0

extern uchar Z8_STATUS_0;
extern uchar Z8_STATUS_3;
extern volatile uchar Z8_STATUS_4;
extern uchar Z8_STATUS_5;
extern uchar Z8_STATUS_6;
extern uchar Z8_STATUS_7;
extern uchar Z8_STATUS_8;
extern uchar Z8_STATUS_9;
extern uchar Z8_STATUS_10;
extern uchar Z8_STATUS_11; // V1.8
extern uchar current_function;			// current active function		
extern far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
//V1.7 extern uchar LFO_Freq_Counter;		
extern uchar serial_in_start_index;		// a near pointer on FAR ram
extern uchar serial_in_end_index;		// a near pointer on FAR ram
extern uchar serial_in_current_index;
extern far uchar serial_in_buffer[SERIAL_IN_BUFF_LENGHT];
extern uchar midi_delay_cpt;
extern uchar menu_state;
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];	// NN=1 registers can be accessed from ez board 

extern far uchar realtime_generated_buffer[REALTIME_BUFFER_LENGHT];
extern uchar realtime_generated_end_index;
extern far uchar realtime_received_buffer[REALTIME_BUFFER_LENGHT];
extern uchar realtime_received_end_index;
extern far uint non_assign_sw_test_mode;
extern volatile uchar mackie_bargraph_timeout;

extern far    uchar encoder_autotest_value;	// V1.5
extern uchar lcd_current_incrust; // V1.69

//extern volatile uint tempo_reload_cpt;	// V1.8
extern uchar current_smpte_reload_cpt;// V1.8
extern rom uint  smpte_reload_cpt[SMPTE_FRAME_RATE_NB][3];// V1.8
extern far SMPTE_STRUCT smpte_code;// V1.8
extern far uchar smpte_lock_cpt;// V1.8

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateFunctionsToIncrease()
// Inputs	: None
// Outputs	: None
// Process  : Increase group, channel, scene if switches are pressed during up/dow or encoder
///////////////////////////////////////////////////////////////////////////////////////
void UpdateFunctionsToIncrease(void)
{
	// Are we in menus mode ?
	if(menu_state != NO_MENU)
	{
		Z8_STATUS_6 |= up_down_sw_changed_z6;
		Z8_STATUS_6 |= up_down_sw_direction_z6;
		return;
	}

	// V1.69 the tempo is adjustable when OK pressed in normal display
	if(lcd_current_incrust == LCD_INCRUST_TEMPO_ADJUST)
	{
		if(internal_tempo < MAX_TEMPO_MINUS_30) 
		{	
			internal_tempo++;
			Z8_STATUS_10 |= tempo_changed_z10;
		}
		return;
	}

	// check if midi channel must be increased
	//if(CHANNEL_PORT_IN & CHANNEL_PIN)
	if(current_function == FCT_CHANNEL)	
	{
		Z8_STATUS_0 |= must_increase_channel_z0;
		Z8_STATUS_0 |= must_change_channel_z0;
	}
	else
	// check if group must be increased
	// if(GROUP_PORT_IN & GROUP_PIN)
	if(current_function == FCT_GROUP)
	{
		Z8_STATUS_3 |= must_increase_group_z3;
		Z8_STATUS_3 |= must_change_group_z3;
	}
	else
	// check if scene select must be increased
	//if(SCENE_SEL_PORT_IN & SCENE_SEL_PIN)
	if(current_function == FCT_SCENE)
	{
		Z8_STATUS_0 |= must_increase_scene_z0;
		Z8_STATUS_0 |= must_change_scene_z0;
	}
	else
	// check if scene arpp select must be increased V1.8
	if(current_function == FCT_SCENE_ARP)
	{
		Z8_STATUS_11 |= must_increase_scene_arp_z11;
		Z8_STATUS_11 |= must_change_scene_arp_z11;
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateFunctionsToDecrease()
// Inputs	: None
// Outputs	: None
// Process  : decrease group, channel, scene if switches are pressed during up/dow or encoder
///////////////////////////////////////////////////////////////////////////////////////
void UpdateFunctionsToDecrease(void)
{

	// Are we in menus mode ?
	if(menu_state != NO_MENU)
	{
		Z8_STATUS_6 |= up_down_sw_changed_z6;
		Z8_STATUS_6 &= (~up_down_sw_direction_z6);
		return;
	}

	// V1.69 the tempo is adjustable when OK pressed in normal display
	if(lcd_current_incrust == LCD_INCRUST_TEMPO_ADJUST)
	{
		if(internal_tempo) 
		{
			internal_tempo--;
			Z8_STATUS_10 |= tempo_changed_z10;
		}
		return;
	}

	// check if midi channel must be increased
	//if(CHANNEL_PORT_IN & CHANNEL_PIN)
	if(current_function == FCT_CHANNEL)
	{
		Z8_STATUS_0 &= (~must_increase_channel_z0);
		Z8_STATUS_0 |= must_change_channel_z0;
	}
	else
	// check if group must be increased
	//if(GROUP_PORT_IN & GROUP_PIN)
	if(current_function == FCT_GROUP)
	{
		Z8_STATUS_3 &= (~must_increase_group_z3);
		Z8_STATUS_3 |= must_change_group_z3;
	}
	else
	// check if scene select must be increased
	//if(SCENE_SEL_PORT_IN & SCENE_SEL_PIN)
	if(current_function == FCT_SCENE)	
	{
		Z8_STATUS_0 &= (~must_increase_scene_z0);
		Z8_STATUS_0 |= must_change_scene_z0;
	}
	else // V1.8
	if(current_function == FCT_SCENE_ARP)	
	{
		Z8_STATUS_11 &= (~must_increase_scene_arp_z11);
		Z8_STATUS_11 |= must_change_scene_arp_z11;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckSnapshotState()
// Inputs	: None
// Outputs	: None
// Process  : Check for snapshot switch state
///////////////////////////////////////////////////////////////////////////////////////
void CheckSnapshotState(void)
{
	if(!(Z8_STATUS_0 & snapshot_changed_z0))						// Already something to do with Scenes ?
	{
		if(!(Z8_STATUS_0 & snapshot_locked_z0))						// scene butt must pass through 0 to reenable scene
		{
			if(SNAPSHOT_PORT_IN & SNAPSHOT_PIN)						// Scene button is currently pressed
			{
				if(Z8_STATUS_0 & must_recheck_snapshot_z0)			// and was already pressed before
				{
					if(++snapshot_long_push_cpt == CPT_2_5_SEC)		// If long press ?
					{
						Z8_STATUS_0  |= snapshot_changed_z0;		// Must change scene
						Z8_STATUS_0  |= snapshot_locked_z0;			// Lock scene until sw is relaxed
						Z8_STATUS_0  |= save_zsend_snapshot_z0;		// and Save it
						Z8_STATUS_0  &= ~must_recheck_snapshot_z0;	// Finish with scene
					}
				}
				else	Z8_STATUS_0  |= must_recheck_snapshot_z0;
			} // end button pressed
			else													// Scene button is not pressed
			{
				if(Z8_STATUS_0 & must_recheck_snapshot_z0)			// and was already pressed before
				{
					Z8_STATUS_0  |= snapshot_changed_z0;			// scene button has been relaxed
					Z8_STATUS_0  &= ~save_zsend_snapshot_z0;		// must Send Scene
					Z8_STATUS_0  &= ~must_recheck_snapshot_z0;		// Finish with scene
				}
			}
		} // end scene was not locked
		else 
		// Scene is locked since button did not pass through 0
		if(!(SNAPSHOT_PORT_IN & SNAPSHOT_PIN))					// scene button relaxed ?
				Z8_STATUS_0 &= ~snapshot_locked_z0;				// unlock next scene since button relaxed
  	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckInfraredSwitchState()
// Inputs	: None
// Outputs	: None
// Process  : Check for infrared switch state
///////////////////////////////////////////////////////////////////////////////////////
void CheckInfraredSwitchState(void)
{
	if(!(Z8_STATUS_3 & ir_sw_changed_z3))							// Already something to do with infrared sw ?
	{
		if(!(Z8_STATUS_3 & ir_sw_locked_z3))						// infrared sw  must pass through 0 to reenable infrared sw
		{
			if(INFRARED_SW_PORT_IN & INFRARED_SW_PIN)			// infrared sw is currently pressed
			{
				if(Z8_STATUS_3 & must_recheck_ir_sw_z3)			// and was already pressed before
				{
					if(++ir_sw_long_push_cpt == CPT_2_5_SEC)	// If long press ?
					{
						Z8_STATUS_3  |= ir_sw_changed_z3;			// Must change infrared sw
						Z8_STATUS_3  |= ir_sw_locked_z3;			// Lock infrared sw until sw is relaxed
						Z8_STATUS_3  |= onoff_zhold_infrared_z3;	// must toggle state of infrared
						Z8_STATUS_3  &= ~must_recheck_ir_sw_z3;	// Finish with infrared sw
					}
				}
				else	Z8_STATUS_3  |= must_recheck_ir_sw_z3;
			} // end button pressed
			else												// infrared sw is not pressed
			{
				if(Z8_STATUS_3 & must_recheck_ir_sw_z3)			// and was already pressed before
				{
					Z8_STATUS_3  |= ir_sw_changed_z3;				// infrared sw has been relaxed
					Z8_STATUS_3  &= ~onoff_zhold_infrared_z3;		// must hold infarred value
					Z8_STATUS_3  &= ~must_recheck_ir_sw_z3;		// Finish with infrared sw
				}
			}
		} // end infrared sw was not locked
		else 
		// infrared sw is locked since button did not pass through 0
		if(!(INFRARED_SW_PORT_IN & INFRARED_SW_PIN))			// infrared sw relaxed ?
				Z8_STATUS_3 &= ~ir_sw_locked_z3;					// unlock next infrared sw since button relaxed
  	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer0()
// Inputs	: None
// Outputs	: None
// Process  : Timer 0 ISR - Should be there each 10ms
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer0(void) 
{
	// UPDATE LCD INCRUST WINDOW ONLY LOCK COUNTER
 	if(lcd_lock_incrust_cpt)
		if(lcd_lock_incrust_cpt != CPT_INFINITE)
			if(--lcd_lock_incrust_cpt == 0)
				Z8_STATUS_3 |= must_reinit_incrust_lcd_z3;
			
	// UPDATE SNAPSHOT STATE
	CheckSnapshotState();

	// UPDATE INFRARED SWITCH STATE	
	CheckInfraredSwitchState();

	// UPDATE TIMER 5 SECONDS COUNTER
    if(seconds_5_timer_cpt)
		seconds_5_timer_cpt -= 0x0001L;

	// CHECK LFO INTERNALLY SYNCRONIZED
	if(!(Z8_STATUS_1_NVM & lfo_state_z1nvm))	// Is the LFO off ?
		Z8_STATUS_5 &= ~lfo_waiting_midi_clock_z5;

	// UPDATE MIDI DELAY COUNTER
    if(midi_delay_cpt)
		midi_delay_cpt--;

	// UPDATE Mackie control bargraph decay counter
	if(mackie_bargraph_timeout)
		if(--mackie_bargraph_timeout == 0)
			Z8_STATUS_9 |= must_decrease_mackie_bargraph_z9;


	// Turn off Midi leds after persitancy, only if NOT in autotest mode
	if(!(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1))
	{
		// MANAGE LEDS MIDI OUT1 persistency
		if(led_midi_out1_cpt)
		{
			led_midi_out1_cpt--;
			if(!led_midi_out1_cpt)
				LED_MIDI_1_PORT_OUT &= ~LED_MIDI_1_PIN;
		}
	
		// MANAGE LEDS MIDI OUT2 persistency
		if(led_midi_out2_cpt)
		{
			led_midi_out2_cpt--;
			if(!led_midi_out2_cpt)
				LED_MIDI_2_PORT_OUT &= ~LED_MIDI_2_PIN;
		}
	} // end not in autotest mode

	// V1.62 UPDATE LCD CHANNEL WHEN PERSISTANCY OF OVERRIDED CHANNEL IS FINISHED
	if(override_channel_cpt)
		if(--override_channel_cpt == 0)
			Z8_STATUS_6 |= must_reinit_channel_lcd_z6;

	// V1.8 MUST UNLOCK SMPTE ?
	if(smpte_lock_cpt)
		smpte_lock_cpt--;


	// Finish by acknoledging itr bits
	IRQ0  &= 0xDF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer1()
// Inputs	: None
// Outputs	: None
// Process  : Timer 1 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer1(void) 
{
	// Unlock eeprom
	Z8_STATUS_4 &= ~eeprom_locked_10ms_z4;

	// Finish by acknoledging itr bits
	IRQ0  &= 0xBF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer2()
// Inputs	: None
// Outputs	: None
// Process  : Timer 2 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer2(void) 
{
	uint tempo_reload_cpt_local;

    // if midi sync is internal then generate clock tick or SMPTE
	
	// V1.8
	// midi sync internal ?
	if(CTRL_REG_1_7BITS_TO_EZ & realtime_source_int_z_ez1)
	{
		// if midi sync is configured as SMPTE
		if(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm)
		// SMPTE mode : update smpte time
		{
			current_smpte_reload_cpt++;
			if(current_smpte_reload_cpt == 3)
				current_smpte_reload_cpt = 0;
			tempo_reload_cpt_local = (uint)smpte_reload_cpt[smpte_code.type][current_smpte_reload_cpt]; 
			// Flag to send new smpte quarter frame
			Z8_STATUS_11 |= must_send_smpte_quarter_z11;
			T2RH   	= (uchar)(tempo_reload_cpt_local >> 8);		// Timer 2 reload value High 
			T2RL   	= (uchar)(tempo_reload_cpt_local & 0xFF);		// Timer 2 reload value Low 		
		}
	} // end internal sync on

	// Finish by acknoledging itr bits
	IRQ0  &= 0x7F;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_rx0()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 RX ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_rx0(void) 
{
	// uchar local_u0rxd = U0RXD;
	static uchar must_store;

	// REALTIME EVENT RECEIVED
	if((U0RXD >= MIDI_CLOCK) && (U0RXD <= MIDI_STOP))
	{
		// treat realtime incoming data only if z8 fully updated from ez
		if(CTRL_REG_7BITS_FROM_EZ[0] & z8_board_fully_updated_from_ez0)
		{

			realtime_received_buffer[realtime_received_end_index] = U0RXD;
			realtime_received_end_index = (realtime_received_end_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
		} // end z8 was fully updated from ez
	} // end MIDI REALTIME event received
	else
	{
		must_store = TRUE;
	
		// are we waiting SOF ?
		if(!(Z8_STATUS_5 & serial_in_state_z5))
		{
			if(U0RXD != SOF_TO_EZ)
				must_store = FALSE;
			else
				// we are now waiting EOF
				Z8_STATUS_5 ^= serial_in_state_z5;

		} // end waiting SOF
		// Here we are into a message received, or waiting EOF
		else
		{
			if(U0RXD == EOF_TO_EZ)
			{
				// Here we received a valid EOF : update pointer end index
				serial_in_end_index = (serial_in_current_index+2)&(SERIAL_IN_BUFF_LENGHT-1);
				// we are now waiting new SOF
				Z8_STATUS_5 ^= serial_in_state_z5;
			}
			else
			if(U0RXD == SOF_TO_EZ)
			{
				must_store = FALSE;
				// now rewait sof
				Z8_STATUS_5 ^= serial_in_state_z5;
			}
		} // end waiting EOF

		if(must_store)
		{
			serial_in_buffer[serial_in_current_index] = U0RXD;
			serial_in_current_index = (serial_in_current_index+1)&(SERIAL_IN_BUFF_LENGHT-1);

//DBG
//		if(U0RXD == SOF_TO_EZ)
//			SendToTx1(0xFE);

			// check that no overflow occured
			if(serial_in_current_index == serial_in_start_index)
				if(serial_in_current_index != ((serial_in_end_index-1)&(SERIAL_IN_BUFF_LENGHT-1)))
					Z8_STATUS_8 |= serial_in_overflow_z8;
		}
	}

	// Finish by acknoledging itr bits
	IRQ0  &= 0xEF;
}
/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_mode_sw()
// Inputs	: None
// Outputs	: None
// Process  : MODE BUTTON ISR - PD0 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_mode_sw(void) 
{
	// Ignore falling edge interrupts
		if(MODE_PORT_IN & MODE_PIN)
		{
			if(!(Z8_STATUS_4 & mode_changed_z4))
			if(Z8_STATUS_7 & mode_passed_0_z7)
			{
				Z8_STATUS_4 |= mode_changed_z4;
				Z8_STATUS_7 &= ~mode_passed_0_z7;	
			}
		}
//		else Z8_STATUS_7 |= mode_passed_0_z7;

	// Finish by acknoledging itr bits
	IRQ1  &= 0xFE;
}
*/
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_encoder()
// Inputs	: None
// Outputs	: None
// Process  : ENCODER CHANGE ISR : CALLED ON BOTH EDGES - PC2 ISR
//			  V1.5 : in mackie mode the encoder sends midi events if encoder sw is active
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_encoder(void) 
{
/*
	// rising edge ?
	if(ENCODER_A_PORT_IN & ENCODER_A_PIN)
	{
		if(ENCODER_B_PORT_IN & ENCODER_B_PIN)
			Z8_STATUS_6 |= encoder_rising_state_z6;
	    else
			Z8_STATUS_6 &= ~encoder_rising_state_z6;
	} // encoder A is 1
	else
	// falling edge
	{
		// is B = 1 on A falling edge
		if(!(ENCODER_B_PORT_IN & ENCODER_B_PIN))
		{
			if(Z8_STATUS_6 & encoder_rising_state_z6)
			{
				// mackie mode ?
				// V1.62 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS) == MACKIE_MODE))
				if((Z8_STATUS_2_NVM & operating_mode1_z2nvm) && (!(Z8_STATUS_2_NVM & operating_mode0_z2nvm)))
				{
					// if encoder sw is active sends midi else use normal encoder function
					if(ENCODER_SW_PORT_IN & ENCODER_SW_PIN)
					{
						Z8_STATUS_10 |=	must_send_mackie_encoder_z10;
						Z8_STATUS_10 &=	~mackie_encoder_left_zright_z10;
					} // end mackie mode and encoder sw active
					else
						UpdateFunctionsToIncrease();
				} // end mackie mode
				// V1.5 : encoder up in autotest mode
				else
				if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
				{
					encoder_autotest_value++;
					if(encoder_autotest_value == 128)
						encoder_autotest_value = 127;
					non_assign_sw_test_mode |= TEST_MODE_ENCODER_UP_BIT_MASK;	
				}
				else
					UpdateFunctionsToIncrease();
			}
		}
	    else
		{
			if(!(Z8_STATUS_6 & encoder_rising_state_z6))
			{
				// mackie mode ?
				// V1.62 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS == MACKIE_MODE)
				if((Z8_STATUS_2_NVM & operating_mode1_z2nvm) && (!(Z8_STATUS_2_NVM & operating_mode0_z2nvm)))
				{
					// if encoder sw is active sends midi else use normal encoder function
					if(ENCODER_SW_PORT_IN & ENCODER_SW_PIN)
					{
						Z8_STATUS_10 |=	must_send_mackie_encoder_z10;
						Z8_STATUS_10 |=	mackie_encoder_left_zright_z10;
					} // end mackie mode and encoder sw active
					else
						UpdateFunctionsToDecrease();
				} // end mackie mode
				// V1.5 : encoder in autotest mode
				else
				if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
				{
					encoder_autotest_value--;
					if(encoder_autotest_value == 255)
						encoder_autotest_value = 0;
					non_assign_sw_test_mode |= TEST_MODE_ENCODER_UP_BIT_MASK;	
				}

				else
					UpdateFunctionsToDecrease();
			}
		}
	}
*/

	uchar valid_state_cpt = VALID_STATE_CPT >> 1;
	
	// rising edge ?
	do { 
		if(ENCODER_A_PORT_IN & ENCODER_A_PIN)
			valid_state_cpt++;
		else
			valid_state_cpt--;
	} while( (valid_state_cpt!=0)&&(valid_state_cpt<VALID_STATE_CPT));

	if(valid_state_cpt)
	{
		if(ENCODER_B_PORT_IN & ENCODER_B_PIN)
			Z8_STATUS_6 |= encoder_rising_state_z6;
	    else
			Z8_STATUS_6 &= ~encoder_rising_state_z6;
	}
	else
	// falling edge
	{
		// is B = 1 on A falling edge
		if(!(ENCODER_B_PORT_IN & ENCODER_B_PIN))
		{
			if(Z8_STATUS_6 & encoder_rising_state_z6)
			{
				// mackie mode ?
				//V1.67 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS == MACKIE_MODE)
				// if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) == MACKIE_MODE_TEST_POS)
				if( (OPERATING_MODE_NVM == MACKIE_MODE) || 
					(OPERATING_MODE_NVM == LOGIC_MODE))
				{
					// if encoder sw is active sends midi else use normal encoder function
					if(ENCODER_SW_PORT_IN & ENCODER_SW_PIN)
					{
						Z8_STATUS_10 |=	must_send_mackie_encoder_z10;
						Z8_STATUS_10 &=	~mackie_encoder_left_zright_z10;
					} // end mackie mode and encoder sw active
					else
						UpdateFunctionsToIncrease();
				} // end mackie mode
				// V1.5 : encoder up in autotest mode
				else
				if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
				{
					encoder_autotest_value++;
					if(encoder_autotest_value == 128)
						encoder_autotest_value = 127;
					non_assign_sw_test_mode |= TEST_MODE_ENCODER_UP_BIT_MASK;	
				}
				else
					UpdateFunctionsToIncrease();
			}
		}
	    else
		{
			if(!(Z8_STATUS_6 & encoder_rising_state_z6))
			{
				// mackie mode ?
				//V1.67 if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) >> OPERATING_MODE_MSK_POS == MACKIE_MODE)
				// if((Z8_STATUS_2_NVM & OPERATING_MODE_MSK_Z2_NVM) == MACKIE_MODE_TEST_POS)
				if( (OPERATING_MODE_NVM == MACKIE_MODE) || 
					(OPERATING_MODE_NVM == LOGIC_MODE))
				{
					// if encoder sw is active sends midi else use normal encoder function
					if(ENCODER_SW_PORT_IN & ENCODER_SW_PIN)
					{
						Z8_STATUS_10 |=	must_send_mackie_encoder_z10;
						Z8_STATUS_10 |=	mackie_encoder_left_zright_z10;
					} // end mackie mode and encoder sw active
					else
						UpdateFunctionsToDecrease();
				} // end mackie mode
				// V1.5 : encoder in autotest mode
				else
				if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1)
				{
					encoder_autotest_value--;
					if(encoder_autotest_value == 255)
						encoder_autotest_value = 0;
					non_assign_sw_test_mode |= TEST_MODE_ENCODER_UP_BIT_MASK;	
				}

				else
					UpdateFunctionsToDecrease();
			}
		}
	}

	// Finish by acknoledging itr bits
	IRQ2 &= 0xFB;
}

/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_ok_sw()
// Inputs	: None
// Outputs	: None
// Process  : OK BUTTON ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_ok_sw(void) 
{
	// Check if sw is locked for debounce
	if(ok_sw_lock_cpt == 0)
	{
	    // Ignore falling edge interrupts
		if(OK_PORT_IN & OK_PIN)
		{
			Z8_STATUS_6 |= ok_sw_changed_z6;

			// lock UP switch for 4 x 10 ms
			ok_sw_lock_cpt = SWITCH_LOCK_CPT;
		}
	 }

	// Finish by acknoledging itr bits
	IRQ1  &= 0xFB;
}
*/
/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_cancel_sw()
// Inputs	: None
// Outputs	: None
// Process  : CANCEL BUTTON ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_cancel_sw(void) 
{
	// Check if sw is locked for debounce
	if(cancel_sw_lock_cpt == 0)
	{
	    // Ignore falling edge interrupts
		if(CANCEL_PORT_IN & CANCEL_PIN)
		{
			Z8_STATUS_6 |= cancel_sw_changed_z6;

			// lock UP switch for 4 x 10 ms
			cancel_sw_lock_cpt = SWITCH_LOCK_CPT;
		}
	 }
	// Finish by acknoledging itr bits
	IRQ1  &= 0xF7;
}
*/
/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_infrared_sw()
// Inputs	: None
// Outputs	: None
// Process  : INFRARED BUTTON ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_infrared_sw(void) 
{
	// Finish by acknoledging itr bits
	IRQ1  &= 0xEF;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_joystick_sw()
// Inputs	: None
// Outputs	: None
// Process  : JOYSTICK BUTTON ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_joystick_sw(void) 
{
	// Finish by acknoledging itr bits
	IRQ1  &= 0xBF;
}
*/

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_down_sw()
// Inputs	: None
// Outputs	: None
// Process  : DOWN BUTTON ISR - PD7 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_down_sw(void) 
{
	// treat only 1/8 time statistically this swicth = debounce  
//	if(!index_analog_itr)
//	{
		if(DOWN_SW_PORT_IN & DOWN_SW_PIN)
		{
			if(Z8_STATUS_7 & down_passed_0_z7)
			{
				UpdateFunctionsToDecrease();
				Z8_STATUS_7 &= ~down_passed_0_z7;	

				// if we are in test mode then update the dedicated register
				if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
					non_assign_sw_test_mode |= TEST_MODE_DOWN_SW_BIT_MASK;
			}
		}
		// else Z8_STATUS_7 |= down_passed_0_z7;
//	}

	// Finish by acknoledging itr bits
	IRQ1  &= 0x7F;
}
/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_foot_sw()
// Inputs	: None
// Outputs	: None
// Process  : FOOTSWITCH ISR [!!! called on both edges !!!]
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_foot_sw(void) 
{
	// Finish by acknoledging itr bits
	IRQ2  &= 0xFE;
}
*/
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	isr_encoder_sw()
// Inputs	: 	None
// Outputs	:	 None
// Process  : 	ENCODER SWITCH ISR  - PD2 ISR
//				the encoder sw used in normal mode (assignable) is pooled by the bg
//				Only the menu mode is treated here
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_encoder_sw(void) 
{
	// encoder sw pressed and menu mode active = OK action
	if(menu_state != NO_MENU)
	{
		if(ENCODER_SW_PORT_IN & ENCODER_SW_PIN)
		{
			if(Z8_STATUS_7 & encoder_sw_passed_0_z7)
			{
				Z8_STATUS_6 |= ok_sw_changed_z6;
				Z8_STATUS_7 &= ~encoder_sw_passed_0_z7;	
			}
		}
		// else Z8_STATUS_7 |= encoder_sw_passed_0_z7;
	}

	// Finish by acknoledging itr bits
	// IRQ2  &= 0xFD;
    IRQ1 &= 0xFB;
}
/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_edit_sw()
// Inputs	: None
// Outputs	: None
// Process  : EDIT BUTTON ISR [!!! called on both edges !!!] - PC0 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_edit_sw(void) 
{
		if(EDIT_PORT_IN & EDIT_PIN)
		{
			if(Z8_STATUS_7 & edit_passed_0_z7)
			{
				Z8_STATUS_6 |= edit_sw_changed_z6;
				Z8_STATUS_7 &= ~edit_passed_0_z7;	
			}
		}
//		else Z8_STATUS_7 |= edit_passed_0_z7;

	// Finish by acknoledging itr bits
	//IRQ2  &= 0xFB;
    IRQ2 &= 0xFE;
}
*/
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_up_sw()
// Inputs	: None
// Outputs	: None
// Process  : UP BUTTON ISR [!!! called on both edges !!!] - PC3_ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_up_sw(void) 
{
	// treat only 1/8 time statistically this swicth = debounce  
//	if(!index_analog_itr)
//	{
		if(UP_SW_PORT_IN & UP_SW_PIN)
		{
			if(Z8_STATUS_7 & up_passed_0_z7)
			{
				UpdateFunctionsToIncrease();
				Z8_STATUS_7 &= ~up_passed_0_z7;	

				// if we are in test mode then update the dedicated register
				if(CTRL_REG_1_7BITS_TO_EZ & autotest_mode_z_ez1) 
					non_assign_sw_test_mode |= TEST_MODE_UP_SW_BIT_MASK;
			}
		}
//		else Z8_STATUS_7 |= up_passed_0_z7;
//	}
	// Finish by acknoledging itr bits
	IRQ2  &= 0xF7;
}

// V1.8 timer 3 is used for midi clock sync internal
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer3()
// Inputs	: None
// Outputs	: None
// Process  : Timer 3 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer3(void) 
{
	// midi sync internal ?
	if(CTRL_REG_1_7BITS_TO_EZ & realtime_source_int_z_ez1)
	// if midi sync is configured as midi clock
//	if(!(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm))
	{
		// V1.9 send MIDI clock immediately
		// generate internal sync, send to out only if smpte mode is not active
		//if(!(Z8_STATUS_3_NVM & int_sync_mode_smpte_zclock_z3nvm))
		//	SendToTx0(MIDI_CLOCK);	

		realtime_generated_buffer[realtime_generated_end_index] = MIDI_CLOCK;
		realtime_generated_end_index = (realtime_generated_end_index + 1) & (REALTIME_BUFFER_LENGHT - 1);

		while( !(U0STAT0 & TDRE) );
		U0TXD = MIDI_CLOCK;
		TX_STATUS |= tx0_busy_txstat;
	}

	// Finish by acknoledging itr bits
	IRQ2  &= 0x7F;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_tx0()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 TX ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_tx0(void) 
{
	// tx0 ready ?
	if(U0STAT0 & TDRE)
	{
		if ((tx0_start_index != tx0_end_index))// || (TX_STATUS & tx0_busy_txstat))
		{
			U0TXD = TX0_Buff[tx0_start_index++ & (TX_BUFF_LENGTH-1)];
			TX_STATUS &= ~tx0_full_txstat;
		}
		else
			TX_STATUS &= ~tx0_busy_txstat;
	}

	// Finish by acknoledging itr bits
	IRQ0  &= 0xF7;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_tx1()
// Inputs	: None
// Outputs	: None
// Process  : UART 1 TX ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_tx1(void) 
{
	// tx1 ready ?
	if(U1STAT0 & TDRE)
	{
		if((tx1_start_index != tx1_end_index))// || (TX_STATUS & tx1_busy_txstat))
		{
			U1TXD = TX1_Buff[tx1_start_index++ & (TX_BUFF_LENGTH-1)];
			TX_STATUS &= ~tx1_full_txstat;
		}
		else
			TX_STATUS &= ~tx1_busy_txstat;
	}

	// Finish by acknoledging itr bits
	IRQ2  &= 0xDF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_adc()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 TX ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_adc(void) 
{
	static uchar adc_tmp;

	// Disable ADC conversion
	ADCCTL &= 0x7F;								
	// A new adc result is available, store it and start next conv
	adc_tmp  = ADCD_H >> 1;
	// enlarge conversion ad to avoid DC offset
	adc_tmp += ADCD_H >> 6 ;

	// limit adc from 0 to 127
	if(adc_tmp > 127) 
		adc_tmp = 127;	

	// Store current result to current index
	Analog[current_analog_index] = adc_tmp;

	// evaluate next ad channel and mux channel
	if(row_analog_itr <= ROW4)
	{
 		index_analog_itr = (index_analog_itr + 1) & (CTRL_PER_ROW - 1);
		// Change external mux value
		MUX_PORT_OUT &= 0xF8;
		MUX_PORT_OUT |= index_analog_itr;
	}
	
	// Change ad channel if required
	if(!index_analog_itr) 
		row_analog_itr++;

	if(row_analog_itr >= ADC_NB)
	   row_analog_itr = ROW0;

	// IMMEDIATELY start new conversion on next channel
	ADCCTL  = (ADCCTL & 0x70) | row_analog_itr ;
	ADCCTL |= 0x80;
	// ADCCTL = 0xA0 | row_analog_itr ;

	// increase current analog index
	current_analog_index++;
	if(current_analog_index >= ANALOG_SOURCES_NB)
	   current_analog_index  = 0; 	
/*
	// check digital pins state
	if(PARAM_SW_0_IN & PARAM_SW_0_PIN)
		pin_state_param_sw0_itr = 0x01;
	else
		pin_state_param_sw0_itr = 0x00;

	if(PARAM_SW_1_IN & PARAM_SW_1_PIN)
		pin_state_param_sw1_itr = 0x01;
	else
		pin_state_param_sw1_itr = 0x00;
*/
	// Finish by acknoledging itr bits
	IRQ0  &= 0xFE;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitItrs()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 ITRS
///////////////////////////////////////////////////////////////////////////////////////
void InitItrs(void)
{
	// Timers 0,1,2, uart0, adc, i2c and spi interrupt enables
	// Note that timers interrupts are not enabled here
	IRQ0ENH	= 0xF9;		// Enable ITRs TX0,RX0,T0,T1,T2,ADC + select nominal priority
	//V1.9 IRQ0ENL	= 0x00;		// Enable ITRs TX0,RX0,T0,T1,T2,ADC + select nominal priority
	IRQ0ENL	= 0x08;		// Enable ITRs TX0,RX0,T0,T1,T2,ADC + select nominal priority, TX0=high priority

	// PORTS A AND D IRQs 
	// IRQ1ENH	= 0x85;		// Enable ITRs on port A and D + select nominal priority (pd1,3,4,5,6 itrs are off)
	IRQ1ENH	= 0x84;		// Enable ITRs on port A and D + select nominal priority (pd0,1,3,4,5,6 itrs are off)
	IRQ1ENL	= 0x00;		// Enable ITRs on port A and D + select nominal priority
	IRQES 	= 0xFF;		// Define falling/rising edges itrs of both port A and D
	IRQPS 	= 0xFF;		// Select between port A and D interrupt source (All on port D)

	// Timers 3, uart1, dma, portC interrupt enables
	// Note that dma interrupts are not enabled here
//V1.8	IRQ2ENH	= 0x2C;		// Enable ITRs Port C (c0,1 itr off), TX1 select nominal priority
	IRQ2ENH	= 0xAC;		// Enable ITRs Port C (c0,1 itr off), TX1 + T3 select nominal priority
	IRQ2ENL	= 0x00;		// Enable ITRs Port C, TX1 + select nominal priority

	// Clear interrupts pending from boot
	IRQ0	= 0x00;
	IRQ1	= 0x00;
	IRQ2	= 0x00;

	// Create interrupts vectors
	SET_VECTOR(TIMER0, isr_timer0);
	SET_VECTOR(TIMER1, isr_timer1);
	SET_VECTOR(TIMER2, isr_timer2);
	SET_VECTOR(TIMER3, isr_timer3); // V1.8
	SET_VECTOR(UART0_RX, isr_rx0);
//	SET_VECTOR(P0AD, isr_mode_sw);
//	SET_VECTOR(P1AD, isr_foot_sw);
//	SET_VECTOR(P2AD, isr_ok_sw);
//	SET_VECTOR(C1, isr_ok_sw);

//	SET_VECTOR(P3AD, isr_cancel_sw);
//	SET_VECTOR(P4AD, isr_infrared_sw);
//	SET_VECTOR(P6AD, isr_joystick_sw);
	SET_VECTOR(P7AD, isr_down_sw);
	
	//SET_VECTOR(C0, isr_foot_sw);
//	SET_VECTOR(C0, isr_encoder);
//	SET_VECTOR(C0, isr_edit_sw);
//	SET_VECTOR(C1, isr_encoder_sw);
	SET_VECTOR(P2AD, isr_encoder_sw);
	SET_VECTOR(C2, isr_encoder);
	SET_VECTOR(C3, isr_up_sw);
	SET_VECTOR(UART0_TX, isr_tx0);
	SET_VECTOR(UART1_TX, isr_tx1);
	SET_VECTOR(ADC, isr_adc);

}


