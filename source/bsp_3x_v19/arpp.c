#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "arpp.h"
#include "arpp_tables.h"
#include "serial.h"
#include "eeprom.h"
#include "lcd.h"	// V1.7
#include "lcd_low_level.h" // V1.7

// input parameters
unsigned char      	 arp_ref_note; // midi input note value  (ref_note = 0 is used to disable arp)
unsigned char      	 arp_ref_vel;  // midi input note velocity
unsigned char        arp_clock_step;        // nb clock ticks to next arp note
unsigned char        arp_clock_counter;
unsigned char        arp_index;       // index of current note played in melody[][] table
unsigned char        arp_pulse_index;       // index of current pulse (eg bit) in rhythm[] table
unsigned char        arp_nb_keyboard_notes;
unsigned char 		 arp_last_note_off;
unsigned char 		 arp_swing_global;
uchar				 arp_offset_counter;

far unsigned char    arp_keyboard_refnotes[MAX_KEYBOARD_NOTES];

far uchar arp_buff_midi[3];	

// external variables

extern far   uchar serial_in_buffer[SERIAL_IN_BUFF_LENGHT];	// serial in buffer
extern uchar serial_in_start_index;
extern uint  current_random;
extern uchar Z8_STATUS_9;
//extern uchar Z8_STATUS_11; // V1.9
extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];		// table of control reg for eerprom saving and menus 
extern uchar current_value;	// V1.7

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitArpeggiator()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void InitArpeggiator(void)
{
   // Global variables initialization
   // NOTE: don't forget to check out the parameters initialized and reset in all the set_xxxxxx() procedures
   // cf. set_direction(), set_melody(), etc....

//   arp_clock_resolution = arp_clradio_16th;
//   arp_range = 3; //DBG MAX_RANGES-1;
//   arp_melody = 1; //0
//   arp_dir = DIR_UP;
//   arp_rhythm = 0;
//   arp_accent = 0;   
//   arp_transpose = DEFAULT_TRANSPOSE;
//   arp_gate = (unsigned char) (arp_clock_step >> 1);  //tmp  default value

   	arp_clock_step 			= table_clock_resolution[arp_clock_resolution];
   	arp_clock_counter 		= 0;
   	arp_ref_note 			= 0; // means arp is disabled
   	arp_index 				= 0; // will be re-init according to direction
	arp_last_note_off		= NOTE_UNDEF;

	Z8_STATUS_9 &= ~arp_updown_dir_z9; // FALSE;
 	Z8_STATUS_9 |= arp_first_note_z9;  // true
//   	arp_hold_toggle 		= TRUE; //DBG FALSE;
//   	arp_autotrig_toggle 	= FALSE;

//   	arp_rhythm_bk_nb 		= RHYTHMBK0;
//  	arp_accent_bk_nb 		= ACCENTBK0;
//   	arp_velocity 			= 64;
//   	arp_swing 				= 0;

   	arp_pulse_index 		= 0;
   	arp_nb_keyboard_notes 	= 0;
	arp_offset_counter		= 0;
   	set_swing();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckArpeggiator()
// Inputs	: None
// Outputs	: None
// Process  : called once midi data are present in serial in buffer
///////////////////////////////////////////////////////////////////////////////////////
void CheckArpeggiator(uchar status_received)
{
	uchar  /*b1,*/ b2, b3;
//DGB
//	uchar i;

	// parse all midi buffer
// V1.5	while(serial_in_start_index != end_index_local)
// V1.5	{
		// skip frame header FF 02
// V1.5		if(serial_in_buffer[serial_in_start_index] == SOF_TO_EZ)
// V1.5			serial_in_start_index = (serial_in_start_index + 2)&(SERIAL_IN_BUFF_LENGHT-1);	

		// copy to local received midi
// V1.5		b1 = serial_in_buffer[serial_in_start_index]; 	
		// exit if b1 is not a status (corrupted frame)
// V1.5		if(!(b1 & MIDI_STATUS_MASK))
// V1.5		return;

		//Z8_STATUS_11 |= sending_realtime_to_midi_z11; // V1.9		

		b2 = serial_in_buffer[(serial_in_start_index + 1U)&(SERIAL_IN_BUFF_LENGHT-1)];
		b3 = serial_in_buffer[(serial_in_start_index + 2U)&(SERIAL_IN_BUFF_LENGHT-1)];

		// check for NOTE ON message on selected channel
		if ((status_received == (MIDI_NOTE_ON | arp_midi_ch)) &&  b3)
		{
//		    if(arp_keyboard_notes_toggle) 
			if(arp_mode == ARP_MODE_KEYBOARD)			 	 
				add_keyboard_refnote(b2);  // add snapped note in melody buffer
		
		     else // start an arp sequence
		     {
		          if((arp_ref_note) && (arp_clock_counter <= arp_gate))
				 	  mute_note(); //tbd should I wait for arp_gate time before muting ???
		          set_direction(); //reset arp_index
		          arp_pulse_index = 0; //reset index for rhythm and accent
		     }

		     arp_ref_note = b2;    // enable arp
		     arp_ref_vel  = b3;
		     //NB:  we don't reset clock_counter here in order to apply clock resolution on input (ref_note) too
		}

         // check for NOTE OFF message on selected channel
//         else if  (!arp_hold_toggle && 	(b1 == (MIDI_NOTE_OFF		| arp_midi_ch) ||                      // note off
         else if  ((arp_mode != ARP_MODE_HOLD) && (status_received == (MIDI_NOTE_OFF	| arp_midi_ch) ||                      // note off
											  	  (status_received == (MIDI_NOTE_ON  	| arp_midi_ch) && !b3)  ))             // note with velocity null
                            			    // || (b1 == (MIDI_CONTROL_CHANGE | arp_midi_ch) &&  b2 == 0x7B))      // all notes off controler
         {

                 // if(arp_keyboard_notes_toggle)    // remove snapped note from melody buffer
				 if(arp_mode == ARP_MODE_KEYBOARD)
                 {
                      remove_keyboard_refnote(b2);
                      if (!arp_nb_keyboard_notes)  
					  	  arp_ref_note = 0;
                 }
                 else if (b2 == arp_ref_note) // disable arp only if we got NOTE OFF message for arp_ref_note
                 {
                    if(arp_clock_counter <= arp_gate)
					 	mute_note(); //tbd should I wait for arp_gate time before muting ???
                    arp_ref_note = 0 ;  // disable arp
                    set_direction(); // reset arp_index
                    arp_pulse_index = 0;  // reset index for rhythm and accent
                 }
         }

		 //Z8_STATUS_11 &= ~sending_realtime_to_midi_z11; // V1.9		

		// pass to next message
// V1.5		serial_in_start_index = (serial_in_start_index + Get_MIDI_Msg_Size(b1) + 1) & (SERIAL_IN_BUFF_LENGHT-1);
// V1.5		if(serial_in_buffer[serial_in_start_index] == EOF_TO_EZ)
// V1.5			serial_in_start_index = (serial_in_start_index + 1)&(SERIAL_IN_BUFF_LENGHT-1);	

// V1.5	} // end while
}




// *****************************************************************************
void set_direction(void)
{
   // Init direction boundarie flag
   // arp_first_note = TRUE;
	Z8_STATUS_9 |= arp_first_note_z9;		// true

   //Init arp index
   switch ((uchar)arp_dir)
   {
        case DIR_UP:
        case DIR_UPDOWN:
        case DIR_CUSTOM1:
             arp_index = 0;
             Z8_STATUS_9 &= ~arp_updown_dir_z9; // = FALSE;
             break;
        case DIR_DOWN: 
        case DIR_CUSTOM2:
             arp_index = arp_range;
             break;
        case DIR_RANDOM:
        default:
			 arp_index = GetRandomRanged(arp_range);
             break;
   }
}

////////////////////////////////////////////////////////////////////////////////
//  MIDI DATA GENERATION FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// Mute current played note in midi flow
void mute_note(void)
{
	uchar param0 = arp_transpose;	

	// calculate note off number to send
	if(arp_mode == ARP_MODE_KEYBOARD)
       //arp_buff_midi[1] = Truncate_Subst_1_127(arp_keyboard_refnotes[arp_index] + arp_transpose, DEFAULT_TRANSPOSE);
	   param0 += arp_keyboard_refnotes[arp_index];
    else
       //arp_buff_midi[1] = Truncate_Subst_1_127(arp_ref_note + melody[arp_melody][arp_index] + arp_transpose, DEFAULT_TRANSPOSE);
	   param0 += (arp_ref_note + melody[arp_melody][arp_index]);

	arp_buff_midi[1] = Truncate_Subst_1_127(param0, DEFAULT_TRANSPOSE);	   

	// is it a new note off or already sent ?
	if(arp_last_note_off != arp_buff_midi[1])
	{
		arp_last_note_off 	= arp_buff_midi[1];
		arp_buff_midi[0] 	= (MIDI_NOTE_ON | arp_midi_ch);
		arp_buff_midi[2] 	= 0;

	    // Send data to MIDI OUT port
		SendDispatchToMidi(arp_buff_midi, 3, Z8_STATUS_0_NVM);
	}

	// V1.7 : display arp activity
//	current_value = 0;
	DisplaySmallBargraphValue(LCD_ZONE_ARPP_STATE_X+10, 0);

}

// *****************************************************************************
// Identify note to play in arpeggio
void play_note(unsigned char pulse_idx)
{
	unsigned char  vel, note;

    /* set velocity according to accent */
    vel = arp_ref_vel;

    if (arp_accent)  // check if accent is enabled
    {
       if  (!((long)(1L << pulse_idx) & rhythm[arp_accent+(BANK_SIZE*arp_accent_bk_nb)]) )  //check if note doesn't fall on accent
       {
           // here we emphaze the non-accent on odd pulses
           if (pulse_idx & 1)
                // un-accented note falls on pulse 1, 3, 5, 7, 9, 11, 13, 15 (eg: 16ths)
                // vel -= (arp_ref_vel * arp_velocity) / 255 ;
				vel = Truncate_Subst_1_127(vel, (uchar)((((uint)arp_ref_vel * (uint)(arp_velocity)) >> (int)8)));
           else
                // un-accented note falls on pulse 2, 6, 10, 14 (eg: 8ths)... (tmp: or anything else)
//                vel -= (arp_ref_vel * arp_velocity) / 127 ;
				vel = Truncate_Subst_1_127(vel, (uchar)((((uint)arp_ref_vel * (uint)(arp_velocity)) >> (int)7)));

       }
    }

    // reset arp_range in SNAP KEYS mode
//  if(arp_keyboard_notes_toggle)
	if(arp_mode == ARP_MODE_KEYBOARD)
    {
         if (arp_nb_keyboard_notes)
             arp_range = arp_nb_keyboard_notes - 1;
         else
         {
             arp_range = 0;
             arp_index = 0;
             // arp_first_note = TRUE;
			 Z8_STATUS_9 |= arp_first_note_z9;		// true
         }
    }

    /* set note value */
    if (!arp_range)
    {
        arp_index = 0;
        // arp_updown_dir = 0;    // avoid negative note in DIR_UPDOWN descending direction
		Z8_STATUS_9 &= ~arp_updown_dir_z9; // = FALSE;
    }
    else 
	// if (!arp_first_note)
	if(!(Z8_STATUS_9 & arp_first_note_z9))
    {
      // update arp index for current note according to direction and boundaries
      switch (arp_dir)
      {
           case DIR_UP:
                if (++arp_index > arp_range) arp_index = 0;
                break;
           case DIR_DOWN:
                if (!arp_index)
                   arp_index = arp_range;
                else
                    arp_index--;
                break;
           case DIR_UPDOWN:
                // if (!arp_updown_dir)  //ascending
				if(!(Z8_STATUS_9 & arp_updown_dir_z9))
                {
                   if (++arp_index >=  arp_range)  
				   		// arp_updown_dir = TRUE;
						Z8_STATUS_9 |= arp_updown_dir_z9; // true
                   break;
                }
                else //descending
                {
                     if (--arp_index == 0)  
					 	// arp_updown_dir = 0;
						Z8_STATUS_9 &= ~arp_updown_dir_z9; // FALSE
                }
                break;
           case DIR_CUSTOM1:
                // if (arp_updown_dir)
				if(Z8_STATUS_9 & arp_updown_dir_z9)
                {
                   arp_index = 0;
                   // arp_updown_dir = FALSE;
				   Z8_STATUS_9 &= ~arp_updown_dir_z9; // FALSE
                }
                else
                {
                   arp_index = GetRandomRanged(arp_range);
                   // arp_updown_dir = TRUE;
				   Z8_STATUS_9 |= arp_updown_dir_z9; // true
                }
                break;
           case DIR_CUSTOM2:
                if  ((long)(1L << pulse_idx) & rhythm[arp_accent + (BANK_SIZE*arp_accent_bk_nb)])
                   arp_index = 0;
                else
                       arp_index = GetRandomRanged(arp_range);
                break;
           case DIR_RANDOM:
           default:
                arp_index = GetRandomRanged(arp_range);
                break;
           }
    }
    else
        // arp_first_note = FALSE;
		Z8_STATUS_9 &= ~arp_first_note_z9;		// false

    
	// Prepare midi buffer
	arp_buff_midi[0] = (MIDI_NOTE_ON | arp_midi_ch);
	
	// if(arp_keyboard_notes_toggle)
	if(arp_mode == ARP_MODE_KEYBOARD)
       arp_buff_midi[1] = Truncate_Subst_1_127(arp_keyboard_refnotes[arp_index] + arp_transpose, DEFAULT_TRANSPOSE);
 	else
       arp_buff_midi[1] = Truncate_Subst_1_127(arp_ref_note + melody[arp_melody][arp_index] + arp_transpose, DEFAULT_TRANSPOSE);
	
    arp_buff_midi[2] = vel & 0x7F;

    // Send data to MIDI OUT port
	SendDispatchToMidi(arp_buff_midi, 3, Z8_STATUS_0_NVM);

	// V1.7 : display arp activity
//	current_value = vel & 0x7F;
	DisplaySmallBargraphValue(LCD_ZONE_ARPP_STATE_X+10, vel & 0x7F);

	// a new note on has been played, undefined last note off
	arp_last_note_off = NOTE_UNDEF;
}


////////////////////////////////////////////////////////////////////////////////
//  KEYBOARD MODE BUFFER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
void add_keyboard_refnote(unsigned char note)
{
     unsigned char j,i = 0;

     // search for note index
     while ((i < arp_nb_keyboard_notes) && (arp_keyboard_refnotes[i] < note))
	 		i++;

     // insert note in table
     for (j = arp_nb_keyboard_notes + 1; j > i; j--)
          arp_keyboard_refnotes[j] = arp_keyboard_refnotes[j-1];
           
     arp_keyboard_refnotes[i] = note;

     // update number of notes only if we don't overflow the buffer
     if (arp_nb_keyboard_notes < MAX_KEYBOARD_NOTES) 
	 	 arp_nb_keyboard_notes++;

}


//------------------------------------------------------------------------------
void remove_keyboard_refnote(unsigned char note)
{
     unsigned char i = 0;

     // search for note index
     while ((i < arp_nb_keyboard_notes) && (arp_keyboard_refnotes[i] != note))
	 		i++;

     // check for boundaries
     if (i >= arp_nb_keyboard_notes) return;

     // reshifft table
     while (i < arp_nb_keyboard_notes)
     {
           arp_keyboard_refnotes[i] = arp_keyboard_refnotes[i+1];
           i++;
     }

     arp_nb_keyboard_notes--;
}


//------------------------------------------------------------------------------
uchar GetRandomRanged(uchar range)
{
   	uchar i;
	uchar range_mask = 1;
	uchar rnd_range = 0;

	for(i=0; i < range; i++)
	{
		if(current_random & range_mask)
			rnd_range++;						
		range_mask = range_mask << 1;
	}

	return(rnd_range);
}


//------------------------------------------------------------------------------
void CheckRealtimeArpeggio(uchar realtime_value)
{
/*
	far uchar *realtime_reg_local;

	// the realtime register is intitialized to internal (Z8_STATUS_Z10)
//	realtime_reg_local = &Z8_STATUS_TX_REALTIME;
	realtime_reg_local = &realtime_generated_buffer;
	// is yhe arpp externaly synched
	if(Z8_STATUS_0_NVM & arpp_sync_ext_z0nvm)
		// the realtime register is external (Z8_STATUS_Z9)
		// realtime_reg_local = &Z8_STATUS_RX_REALTIME;
		realtime_reg_local = &realtime_received_buffer;
*/

	//Z8_STATUS_11 |= sending_realtime_to_midi_z11; // V1.9

	// a midi start to arpp has been received ?
	// if(*realtime_reg_local & realtime_start_received_z9)
	if(realtime_value == MIDI_START)
	{
 		if (arp_mode == ARP_MODE_AUTOTRIG)
		{
			if(arp_ref_note) 
			   mute_note(); 			//tbd should I wait for arp_gate time before muting ???
			set_direction(); 			// reset arp_index
			arp_pulse_index 	= 0;   	// reset index for rhythm and accent
			arp_ref_note 		= 48;   // Truncate_Subst_1_127(48 + arp_transpose, DEFAULT_TRANSPOSE);      // enable arp
		 	arp_ref_vel  		= 120;  // tbd (arbitrary)
		 	arp_clock_counter 	= 0; 	// tbd re-intit may not be needed in case of MIDI CONTINUE message
			arp_offset_counter  = 0;
		}
	}
	else
	// a midi continue to arpp has been received ?
	//if(*realtime_reg_local & realtime_continue_received_z9)
	if(realtime_value == MIDI_CONTINUE)
	{
 		if (arp_mode == ARP_MODE_AUTOTRIG)
		{
			arp_ref_note =  48; // Truncate_Subst_1_127(48 + arp_transpose, DEFAULT_TRANSPOSE);      // enable arp
		}
	}
	else
	// a clock to arpp has been received ?
	// if(*realtime_reg_local & realtime_clock_received_z9)
	if(realtime_value == MIDI_CLOCK)
	{
		if(arp_offset_counter == arp_offset)
		{
	 		// mute note if clock matches gate
	        // NB: 1. arp_gate could = clock_step
	        if (++arp_clock_counter == arp_gate)
			{
	             if(arp_ref_note) 
				 	mute_note(); // send note off message ... must also check rhythm, etc..
			}
	
	        // play a note if clock matches resolution and if Arp is enabled (ie: ref_note != 0)
	        if (arp_clock_counter == arp_clock_step)
	        {
	            arp_clock_counter = 0;
	
	            if (arp_ref_note && (( (long)(1L << arp_pulse_index)) & rhythm[arp_rhythm+(BANK_SIZE*arp_rhythm_bk_nb)])) 
					play_note(arp_pulse_index);
				arp_pulse_index = (arp_pulse_index + 1) & 31;
	
			    // update clock_step according to arp_swing on matching pulses
	            if (!(arp_pulse_index & 1))   // means we are on pulse 0, 2, 4, 6, 8, ..etc...
	               arp_clock_step = table_clock_resolution[arp_clock_resolution] + arp_swing_global;
	            else
	               arp_clock_step = table_clock_resolution[arp_clock_resolution] - arp_swing_global;
				// recheck arp gate in case of arp_gate = 0 ticks
	            if(!arp_gate && arp_ref_note) 
					mute_note(); // send note off message
	        }				
		} // end arp offset reached
		else arp_offset_counter++;
	} // end clock received
	else
	// a midi stop to arpp has been received ?
	// if(*realtime_reg_local & realtime_stop_received_z9)
	if(realtime_value == MIDI_STOP)
	{
//		if (arp_autotrig_toggle == TRUE)
		if (arp_mode == ARP_MODE_AUTOTRIG)
        {
			if (arp_ref_note) 
			{
				mute_note(); //tbd should I wait for arp_gate time before muting ???
				arp_ref_note = 0 ;  // disable arp
			}
//			set_direction(); // reset arp_index
//			arp_pulse_index = 0;      // reset index for rhythm and accent
		}
	}

	// Z8_STATUS_11 &= ~sending_realtime_to_midi_z11; // V1.9
}

//------------------------------------------------------------------------------
uchar Truncate_Subst_1_127(uchar a, uchar b)
{
	// perform a - b, limit result to 1-127
	if(a <= b) 			return(1);
	if( (a - b) > 127)	return(127);
	return(a-b);
}

//------------------------------------------------------------------------------
void set_mode(void)
{
	if (arp_ref_note) 
	{
		mute_note(); 		//tbd should I wait for arp_gate time before muting ???
		arp_ref_note = 0 ;  	// disable arp
	}
	set_direction(); 		//reset arp_index
	arp_pulse_index = 0; 	// reset index for rhythm and accent

     if(arp_mode == ARP_MODE_KEYBOARD)   // reset snaped notes buffer
        arp_nb_keyboard_notes = 0;
}

// *****************************************************************************
void set_swing(void)
{
  if (arp_ref_note) 
  	mute_note(); //tbd should I wait for arp_gate time before muting ???

 //  arp_swing = (arp_swing * (arp_clock_resolution - 1)) / 64;
 //  arp_swing_global = (uchar)((((uint)arp_swing * (uint)((arp_clock_resolution - 1))) >> (int)6));
  arp_swing_global = (uchar)((((uint)arp_swing * (uint)((table_clock_resolution[arp_clock_resolution] - 1))) >> (int)7));
}

// *****************************************************************************
void set_resolution(void)
{
  //arp_clock_resolution = GPOINTER_TO_INT(nticks);
  arp_clock_counter = 0; //tbd: may that dequantize the whole arp ?
  arp_clock_step = table_clock_resolution[arp_clock_resolution]; // arp_clock_resolution; // 

  // reset swing value  (this parameter is hard to update dynamically)
  arp_swing = DEFAULT_SWING;
  set_swing();
}

