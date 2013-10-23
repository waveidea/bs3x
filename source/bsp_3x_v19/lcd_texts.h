#ifndef LCD_TEXTS
#define LCD_TEXTS

///////////////////////////////////////////////////////////
// LCD TEXTS
///////////////////////////////////////////////////////////
rom uchar sending_txt[] 			= "Sending";
rom uchar saving_txt[] 				= "Saving";
rom uchar scene_txt[] 				= "scene";
rom uchar snapshot_txt[] 			= "snapshot";
rom uchar fader_txt[] 				= "x-fader";
rom uchar infrared_txt[] 			= "infrared";
rom uchar ribbon_txt[] 				= "ribbon"; // V1.5 HW2.0
rom uchar hold_to_txt[] 			= "Hold to";
rom uchar info_txt[]				= "info";
rom uchar expansion_txt[]			= "expansion";
rom uchar connected_txt[]			= "Connected";
rom uchar remove_txt[] 				= "Remove";
rom uchar standard_mode_txt[]		= " Standard  mode ";
rom uchar reason_mode_txt[]			= "  Reason  mode  "; // v1.1
rom uchar user_defined_txt[]		= "  User defined  ";
rom uchar mackie_mode_txt[]			= " Mackie Control "; // v1.5
rom uchar logic_mode_txt[]			= " Logic Control "; // v1.69
rom uchar initializing_txt[]		= "INITIALIZING ...";
rom uchar default_lcd_str_txt[]		= "    CC # ___    ";
rom uchar default_grp_name_txt[]	= "  group  __   ";
rom uchar lfo_txt[]					= "lfo";
rom uchar amplitude_txt[]			= "Amplitude:";
rom uchar frequency_txt[]			= "Frequency:";
rom uchar offset_txt[]				= "Offset   :";
rom uchar arpp_txt[]				= "arppeggio";
rom uchar midi_in_txt[]				= "MIDI IN";
rom uchar serial_in_txt[]			= "SERIAL IN";
rom uchar usb_in_txt[]				= "USB  IN";
rom uchar realtime_txt[]			= "REALTIME";
rom uchar overflow_txt[]			= "OVERFLOW !";
rom uchar prog_txt[]				= "prog";
rom uchar upload_txt[]				= "upload";
rom uchar corrupted_sysex_txt[]		= "Corrupted SYSEX";
rom uchar corrupted_struct_txt[]	= "Assign. undef";
rom uchar received_txt[]			= "received !";
rom uchar common_param_txt[]		= "Common param.";
rom uchar programmed_txt[]			= "programmed";
rom uchar uploaded_txt[]			= "uploaded";
rom uchar control_txt[]				= "Control ?";
//rom uchar upgrading_txt[]			= "UPGRADING FW ..."; // V1.4 commented since not used
rom uchar motion_txt[]				= "motion";
//rom uchar bs3x_info_txt[]			= " BS3X INFO "; // V1.4
rom uchar firmware_txt[]			= "firmware :"; // V1.4
rom uchar hardware_txt[]			= "hardware :"; // V1.4
rom uchar serial_txt[]				= "serial   :"; // V1.4
rom uchar date_txt[]				= "date     :"; // V1.4
// rom uchar mackie_mtc_init_txt[] 	= "00 000.00.00.000"; // V1.5
//rom uchar mackie_mtc_init_txt[] 	= "      .  .  .   "; // V1.5
rom uchar tempo_txt[] 				= "tempo"; // V1.69

// V1.5 dedicated test firmware
#ifdef TEST
rom uchar test_txt[]				= " ***  test  *** "; // V1.5
#endif


/*
rom uchar adc_calibrate_txt[]		= "ANALOG SETUP";
rom uchar please_set_txt[]			= "Please set";
rom uchar left_txt[]				= "LEFT";
rom uchar right_txt[]				= "RIGHT";
rom uchar push_ok_txt[]				= "Then push OK";
*/

#endif