#include <stdio.h>
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "cross_functions.h"

//---------------------------------------------------------------------------
// COMMON BITSTREAM 3X CONFIGURATION SOFTWARE CROSS-PLATFORM FUNCTIONS
//---------------------------------------------------------------------------
// jerome dumas copyright Wave Idea 2004
// jdumas@waveidea.com
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// check if the required control index is a DIGITAL or ANALOG control
//---------------------------------------------------------------------------
uchar GetControlType(uchar index)
{
	if(index > CONTROLS_NB) return INVALID_REQUEST;

	if( (index >= INFRARED_CTRL_INDEX) && (index <= CROSSFADER_R_CTRL_INDEX))
		return ANALOG_CTRL;

	if (index == LFO_CTRL_INDEX )
		return ANALOG_CTRL;
	
	return DIGITAL_CTRL;
}

//---------------------------------------------------------------------------
// Check_invalid_request :
//---------------------------------------------------------------------------
// check if the required action is authorized
//---------------------------------------------------------------------------
uchar Check_invalid_request(uchar SYSEX_ID0, uchar SYSEX_ID2, uchar SYSEX_ID3)
{
    uchar result = 0;

    ////////////////////////////////////////////////////////////////
    // Invalid request list for MIDI AND LCD CONTROL ASSIGN UPL/DOWN
    ////////////////////////////////////////////////////////////////
    if(  ((SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_LCD_CTRL_ASSIGN  )    ||
         ((SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN ) )
    {
        // 1) Invalid request if group is diffferent than 0 for transport indexes
        if( (SYSEX_ID2 <= FORWARD_CTRL_INDEX) && (SYSEX_ID3) )
             result = INVALID_REQUEST;

        // 2) Invalid request if index number is out of range
        if( SYSEX_ID2 >= CONTROLS_NB)
             result = INVALID_REQUEST;

        // 3) Invalid request if group number is out of range
        if( SYSEX_ID3 >= GROUP_NB)
             result = INVALID_REQUEST;
    }
    else
    ////////////////////////////////////////////////////////////////
    // Invalid request list for COMMON PARAMETERS UPL/DOWN
    ////////////////////////////////////////////////////////////////
    if( (SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_COMMON_PARAM)
    {
        // 1) Invalid request if program request on the following common params
        if( (!(SYSEX_ID0 & prog_upload_sysxid0))&&(SYSEX_ID2 == IDENTITY_REQUEST_INDEX))
             result = INVALID_REQUEST;

        // 2) Invalid request if common params address is out of range
        if( SYSEX_ID2 > LAST_EXTERNAL_PARAM_INDEX)
			// only identity address (0x7F) can be out of range
			if( SYSEX_ID2 != IDENTITY_REQUEST_INDEX)
				result = INVALID_REQUEST;

        // 3) Invalid request if group number is out of range
        if( SYSEX_ID3 >= GROUP_NB)
             result = INVALID_REQUEST;

    }

    return(result);
}

//---------------------------------------------------------------------------
// Fill_Sysex_Header : fulfill firsts common bytes of sysex header
//---------------------------------------------------------------------------
// Create a sysex for control assign lcd string
//---------------------------------------------------------------------------
void Fill_Sysex_Header(uchar *sysex, uchar SYSEX_ID0, uchar SYSEX_ID1,
                                     uchar SYSEX_ID2, uchar SYSEX_ID3)
{
        sysex[0] = MIDI_SOX;
        sysex[1] = WID_SYX_ID0;
        sysex[2] = WID_SYX_ID1;
        sysex[3] = WID_SYX_ID2;
        sysex[4] = WID_SYX_ID3;
        sysex[5] = WID_SYX_ID4;
        sysex[6] = SYSEX_ID0;
        sysex[7] = SYSEX_ID1;
        sysex[8] = SYSEX_ID2;
        sysex[9] = SYSEX_ID3;
}

//---------------------------------------------------------------------------
// Single_Byte_To_Nibbles : nibble a single byte
//---------------------------------------------------------------------------
// destination buffer is filled with nibbles
// return checksum on all 2 nibbles
//---------------------------------------------------------------------------
uchar Single_Byte_To_Nibbles(uchar data, uchar *destination)
{
   uchar checksum = 0;

   // create MSB nibble
   destination[0]    = (uchar)(data >> 4);
   // calculate checksum modulo 128
   checksum = (uchar)((checksum + destination[0]) & 0x7F);
   // create LSB nibble
   destination[1]  = (uchar)(data & 0x0F);
   // calculate checksum modulo 128
   checksum = (uchar)((checksum + destination[1]) & 0x7F);
   return(checksum);
}

//---------------------------------------------------------------------------
// Nibbles_To_Single_Byte :
//---------------------------------------------------------------------------
// data is filled with nibbles
// return checksum on all 2 nibbles
//---------------------------------------------------------------------------
uchar Nibbles_To_Single_Byte(uchar *data, uchar *nibbles_src)
{
   uchar checksum = 0;

   if(nibbles_src[0] & 0xF0) return(INVALID_SYSEX);
   if(nibbles_src[1] & 0xF0) return(INVALID_SYSEX);

   // create data from MSB nibble
   *data = (uchar)(((uchar)(nibbles_src[0] << 4)) & 0xF0);
   checksum = (uchar)((checksum + nibbles_src[0]) & 0x7F);

   // create data from LSB nibble
   *data |= (uchar)(nibbles_src[1] & 0x0F);
   checksum = (uchar)((checksum + nibbles_src[1]) & 0x7F);

   return(checksum);
}

//---------------------------------------------------------------------------
// Check_Valid_Sysex_Header :
//---------------------------------------------------------------------------
// check sysex header dedicated to Wave Idea
// return sysex data length (not sysex lenght) or INVALID_SYSEX if the sysex is not a valid wave idea sysex
//---------------------------------------------------------------------------
uchar Check_Valid_Sysex_Header(uchar *sysex, uchar index, uchar group)
{
        // Check is the sysex buffer contains a Wave Idea sysex
        if(sysex[0] != MIDI_SOX)        return(INVALID_SYSEX);
        if(sysex[1] != WID_SYX_ID0)     return(INVALID_SYSEX);
        if(sysex[2] != WID_SYX_ID1)     return(INVALID_SYSEX);
        if(sysex[3] != WID_SYX_ID2)     return(INVALID_SYSEX);
        if(sysex[4] != WID_SYX_ID3)     return(INVALID_SYSEX);
        if(sysex[5] != WID_SYX_ID4)     return(INVALID_SYSEX);
        // Check it is an incoming sysex
        if(!(sysex[6] & sysex_direction_sysxid0)) return(INVALID_SYSEX);
        // Here we have an incoming wave idea sysex
        // check that group and index are the ones expected
        if(sysex[8] != index)           return(INVALID_SYSEX);
        if(sysex[9] != group)           return(INVALID_SYSEX);
        // return sysex data lenght
        return(sysex[7]);
}

//---------------------------------------------------------------------------
// Get_Common_Param_Data_Lenght :
//---------------------------------------------------------------------------
// return common param data length (not sysex lenght)
//---------------------------------------------------------------------------
uchar Get_Common_Param_Data_Lenght(uchar address)
{
   uchar data_lenght;

   switch(address)
   {
           case USER_CURVE_0_LOW_INDEX    :
           case USER_CURVE_0_HIGH_INDEX   :
           case USER_CURVE_1_LOW_INDEX    :
           case USER_CURVE_1_HIGH_INDEX   :     data_lenght = CURVE_TABLES_LENGHT >> 1;
                                                break;
           case GROUP_NAME_INDEX          :     data_lenght = GROUP_NAME_LENGHT << 1;
                                                break;
           case IDENTITY_REQUEST_INDEX    :     data_lenght = IDENTITY_LENGHT;
                                                break;
           case INT_TEMPO_INDEX			  :     data_lenght = 2;
                                                break;
           default                        :     data_lenght = 1;
                                                break;
   }
   return(data_lenght);
}


//---------------------------------------------------------------------------
// LCD CONTROL ASSIGN TO SYSEX
//---------------------------------------------------------------------------
// Create a sysex for control assign lcd string
// action     = DOWNLOAD_WITH_ACK_REQUEST, DOWNLOAD, UPLOAD
// LCD string = must terminates with \0, if < 16 bytes : space padding done inside this function
// sysex      = pointer on sysex buffer to be filled
// index      = control index to upload/download
// group      = group to upload/download
// returns created sysex lenght, INVALID_REQUEST
//---------------------------------------------------------------------------
uchar Bs3x_Lcd_Assign_To_Sysex(uchar action, uchar *lcd_string, uchar *sysex, uchar index, uchar group)
{
		uchar local_lcd_str[/*LCD_STRING_LENGHT*/LCD_STR_MAX_LENGHT+1];
        uchar SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3;
        uchar sysex_lenght, i, j, k, checksum;
		uchar m;
		uchar result;

        SYSEX_ID0 = FRAME_IS_LCD_CTRL_ASSIGN;
        SYSEX_ID2 = index;
        SYSEX_ID3 = group;

        // Check if the requested action is coherent
        if(Check_invalid_request(SYSEX_ID0, SYSEX_ID2, SYSEX_ID3) == INVALID_REQUEST)
          return (INVALID_REQUEST);

		for(m=0; m<SYSEX_BUFFER_MAX; m++) 
			sysex[m] = 0;

        // need to program or upload LCD string ?
        if(action==UPLOAD)
        {
          // upload sysex has a fixed length
          sysex_lenght = UPLOAD_SYSEX_LENGHT;
          // flag bit program/upload
          SYSEX_ID0 |= prog_upload_sysxid0;
          // the upload answer must contains a checksum
          //SYSEX_ID0 |= checksum_return_sysxid0;
          // upload action requires a returned sysex
          SYSEX_ID0 |= acknoledge_sysxid0;
          // lenght is 0 for lcd upload
          SYSEX_ID1    = 0;
          // fill eox
          sysex[UPLOAD_SYSEX_LENGHT-1] = MIDI_EOX;
        } // end upload
        else
        {
          // donwload lcd assign sysex has a fixed length
          sysex_lenght = DOWNLOAD_LCD_STR_SYSEX_LENGHT;
          // the 3X must check received checksum
          SYSEX_ID0 |= checksum_check_sysxid0;
          // 32 nibbles to download
          SYSEX_ID1 = DOWNLOAD_LCD_STR_NIBBLES_LENGHT;

          if(action==DOWNLOAD_WITH_ACK_REQUEST)
             SYSEX_ID0 |= acknoledge_sysxid0;

          // Perform space paddiing on lcd str if lower than 16 chars
          k = 0;
          while((lcd_string[k] != '\0') && (k</*LCD_STRING_LENGHT*/LCD_STR_MAX_LENGHT)) 
		  {
			  local_lcd_str[k] = lcd_string[k];
			  k++;
		  }
          for(j=k; j</*LCD_STRING_LENGHT*/LCD_STR_MAX_LENGHT; j++)
             local_lcd_str[j] = ' ';
          local_lcd_str[/*LCD_STRING_LENGHT*/LCD_STR_MAX_LENGHT] = '\0';

		  // check the lcd string is valid
		  result = CheckLcdString(&local_lcd_str[0]);
		  if(result != LCD_STRING_NO_ERROR)
			  return(INVALID_REQUEST);

          // Now nibbles all 16 bytes of lcd string and caclulate checksum
          i=0;
          checksum = 0;
          for(j=0; j < DOWNLOAD_LCD_STR_NIBBLES_LENGHT; j=(uchar)(j+2))
          {
              if((local_lcd_str[i] < ' ') || (local_lcd_str[i] > '~')) return (INVALID_REQUEST);
              checksum = (uchar)((checksum + Single_Byte_To_Nibbles(local_lcd_str[i++], &sysex[SYSEX_DATA_START+j])) & 0x7F);
          }

          // insert calculated checksum into sysex
          sysex[SYSEX_DATA_START+DOWNLOAD_LCD_STR_NIBBLES_LENGHT]   = checksum;
          // insert EOX
          sysex[SYSEX_DATA_START+DOWNLOAD_LCD_STR_NIBBLES_LENGHT+1] = MIDI_EOX;
        } // end download

        // Fill sysex header
        Fill_Sysex_Header(sysex, SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3);

        return(sysex_lenght);
}

//---------------------------------------------------------------------------
// MIDI CONTROL ASSIGN
//---------------------------------------------------------------------------
// Create a sysex for control assign Midi structure and string
// action     = DOWNLOAD_WITH_ACK_REQUEST, DOWNLOAD, UPLOAD
// Ctrl_assign_Midi_Struct = pointer on filled structure
// sysex      = pointer on sysex buffer to be filled
// index      = control index to upload/download
// group      = group to upload/download
// returns created sysex lenght, 0 if error
//---------------------------------------------------------------------------
uchar Bs3x_Midi_Assign_To_Sysex(uchar action, Ctrl_Midi_Struct *MidiStruct, uchar *sysex, uchar index, uchar group)
{
        uchar SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3;
        uchar sysex_lenght, j, k, m, checksum;

        SYSEX_ID0 = FRAME_IS_MIDI_CTRL_ASSIGN;
        SYSEX_ID2 = index;
        SYSEX_ID3 = group;

        // Check if the requested action is coherent
        if(Check_invalid_request(SYSEX_ID0, SYSEX_ID2, SYSEX_ID3) == INVALID_REQUEST)
          return (INVALID_REQUEST);

		for(m=0; m<SYSEX_BUFFER_MAX; m++) 
			sysex[m] = 0;

        // need to program or upload LCD string ?
        if(action==UPLOAD)
        {
          // upload sysex has a fixed length
          sysex_lenght = UPLOAD_SYSEX_LENGHT;
          // flag bit program/upload
          SYSEX_ID0 |= prog_upload_sysxid0;
          // the upload answer must contains a checksum
//          SYSEX_ID0 |= checksum_return_sysxid0;
          // upload action requires a returned sysex
          SYSEX_ID0 |= acknoledge_sysxid0;
          // lenght is 0 for lcd upload
          SYSEX_ID1    = 0;
          // fill eox
          sysex[UPLOAD_SYSEX_LENGHT-1] = MIDI_EOX;
        } // end upload
        else
        {
		  // Check the midi struct to programm
		  if(CheckMidiStruct(MidiStruct, index) > MIDI_CHECKS_END_WARNINGS)
			 return (INVALID_REQUEST);

          // the 3X must check received checksum
          SYSEX_ID0 |= checksum_check_sysxid0;
          // data lenght in nibbles
          SYSEX_ID1 = (uchar)((CTRL_STRUCT_HEADER_SIZE + MidiStruct->midi_str_lenght) << 1);

          // donwload midi stuct lenght is F0 00 20 4F 00 01 + all nibbles + cs +f7
          sysex_lenght = (uchar)(SYSEX_DATA_START + SYSEX_ID1 + 1 + 1);

          if(action==DOWNLOAD_WITH_ACK_REQUEST)
             SYSEX_ID0 |= acknoledge_sysxid0;

          // Nibble all midi struct bytes
          checksum = (uchar)(            Single_Byte_To_Nibbles(MidiStruct->ctrl_status_0,           &sysex[SYSEX_DATA_START]));
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->ctrl_status_1,           &sysex[SYSEX_DATA_START+2]))  & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->ctrl_status_2,           &sysex[SYSEX_DATA_START+4]))  & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->midi_str_lenght,         &sysex[SYSEX_DATA_START+6]))  & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->midi_channel_position,   &sysex[SYSEX_DATA_START+8]))  & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->ctrl_value_position_0,   &sysex[SYSEX_DATA_START+10])) & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->ctrl_value_position_1,   &sysex[SYSEX_DATA_START+12])) & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->ctrl_value_min,          &sysex[SYSEX_DATA_START+14])) & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->ctrl_value_max,          &sysex[SYSEX_DATA_START+16])) & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->checksum_start_position, &sysex[SYSEX_DATA_START+18])) & 0x7F);
          checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->ctrl_chain,              &sysex[SYSEX_DATA_START+20])) & 0x7F);
          // Now nibble all midi string bytes
          k=0;
          for(j=0; j < MidiStruct->midi_str_lenght; j++)
          {
              checksum = (uchar)((checksum + Single_Byte_To_Nibbles(MidiStruct->midi_string[j], &sysex[SYSEX_DATA_START+22+k])) & 0x7F);
              k += 2;
          }

          // insert calculated checksum into sysex
          sysex[sysex_lenght-2] = checksum;
          // insert EOX
          sysex[sysex_lenght-1] = MIDI_EOX;
        } // end download

        // Fill sysex header
        Fill_Sysex_Header(sysex, SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3);

        return(sysex_lenght);
}

//---------------------------------------------------------------------------
// SYSEX TO LCD CONTROL ASSIGN
//---------------------------------------------------------------------------
// extract the control assign lcd string from sysex OR check ackoledge result
// action = CHECK_LCD_ACKNOLEDGE_RESULT, FILL_LCD_STR_FROM_UPLOAD
// LCD string = pointer on empty lcd string, must be already allocated
// sysex      = pointer on recievd sysex buffer
// index      = expected control index
// group      = expected group
// returns from action CHECK_LCD_ACKNOLEDGE_RESULT    : INVALID_SYSEX if not a valid sysex, ACK_ANSWER_NO_ERROR, ACK_ANSWER_ERROR
// returns from action FILL_LCD_STR_FROM_UPLOAD : INVALID_SYSEX if not a valid sysex, sysex lenght if OK
//---------------------------------------------------------------------------
uchar Bs3x_Sysex_To_Lcd_Assign(uchar action, uchar *lcd_string, uchar *sysex, uchar index, uchar group)
{
        // uchar SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3;
        uchar result, i, j, k, checksum;

        // Check the sysex contains LCD string
        if((sysex[6] & FRAME_TO_Z8_TYPE_MSK) != FRAME_IS_LCD_CTRL_ASSIGN)
           return(INVALID_SYSEX);

        // Check if the requested action is coherent
        if(Check_invalid_request(sysex[6], index, group) == INVALID_REQUEST)
          return (INVALID_REQUEST);

        // First check the sysex is a valid incoming wave idea sysex
        result = Check_Valid_Sysex_Header(sysex, index, group);

        // if we are checking an upload result, sysex data lenght must be 0
        if(action == CHECK_LCD_ACKNOLEDGE_RESULT)
        {
            // expected data lenght for a ack answer is 0
            if(result) return(INVALID_SYSEX);
            // a valid sysex is found : check acknoledge result
            if(sysex[6] & acknoledge_sysxid0) return(ACK_ANSWER_NO_ERROR);
            return(ACK_ANSWER_ERROR);
        }
        else
        {
            // Here the received sysex is valid, result contains sysex data lenght in nibbles
            if(result != DOWNLOAD_LCD_STR_NIBBLES_LENGHT) return(INVALID_SYSEX);

            // Fill LCD string from sysex content, also extract checksum
            checksum = 0;
            j=0;
            for(i=0; i < (result >> 1); i++)
            {
               k = Nibbles_To_Single_Byte((uchar *)&lcd_string[i], (uchar *)&sysex[SYSEX_DATA_START+j]);
               if(k == INVALID_SYSEX) return(INVALID_SYSEX);
               checksum = (uchar)((checksum + k) & 0x7F);
               j = (uchar)(j + 2);
            }

            // Add null terminated string
            lcd_string[/*LCD_STRING_LENGHT*/LCD_STR_MAX_LENGHT] = '\0';

            // Check the extracted checksum is ok
            if(checksum != sysex[SYSEX_DATA_START+j]) return(INVALID_SYSEX);

            // return the whole sysex lenght
            return((uchar)(SYSEX_DATA_START + result + 1 + 1));
        }
}

//---------------------------------------------------------------------------
// SYSEX TO MIDI CONTROL ASSIGN
//---------------------------------------------------------------------------
// extract the control assign midi struct & string (without \0) from sysex OR check ackoledge result
// action = CHECK_MIDI_ACKNOLEDGE_RESULT, FILL_STRUCTURE_FROM_UPLOAD
// MidiStruct = pointer on empty midi struct, must be already allocated
// sysex      = pointer on recievd sysex buffer
// index      = expected control index
// group      = expected group
// returns from action CHECK_MIDI_ACKNOLEDGE_RESULT    : INVALID_SYSEX if not a valid sysex, ACK_ANSWER_NO_ERROR, ACK_ANSWER_ERROR
// returns from action FILL_STRUCTURE_FROM_UPLOAD : INVALID_SYSEX if not a valid sysex, sysex lenght if OK
//---------------------------------------------------------------------------
uchar Bs3x_Sysex_To_Midi_Assign(uchar action, Ctrl_Midi_Struct *MidiStruct, uchar *sysex, uchar index, uchar group)
{
        //uchar SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3;
        uchar result, i, j, k, checksum;

        // Check the sysex contains MIDI struct
        if((sysex[6] & FRAME_TO_Z8_TYPE_MSK) != FRAME_IS_MIDI_CTRL_ASSIGN)
           return(INVALID_SYSEX);

        // Check if the requested action is coherent
        if(Check_invalid_request(sysex[6], index, group) == INVALID_REQUEST)
          return (INVALID_REQUEST);

        // First check the sysex is a valid incoming wave idea sysex
        result = Check_Valid_Sysex_Header(sysex, index, group);

        // if we are checking an upload result, sysex data lenght must be 0
        if(action == CHECK_MIDI_ACKNOLEDGE_RESULT)
        {
            // expected data lenght for a ack answer is 0
            if(result) return(INVALID_SYSEX);
            // a valid sysex is found : check acknoledge result
            if(sysex[6] & acknoledge_sysxid0) return(ACK_ANSWER_NO_ERROR);
            return(ACK_ANSWER_ERROR);
        }
        else
        {
            // Here the received sysex is valid, result contains sysex data lenght in nibbles
            if((result == 0)||(result == INVALID_SYSEX)) return(INVALID_SYSEX);

            // Fill Midi struct header from sysex content, also extract checksum
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_status_0,           &sysex[SYSEX_DATA_START]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = k;
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_status_1,           &sysex[SYSEX_DATA_START+2]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_status_2,           &sysex[SYSEX_DATA_START+4]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->midi_str_lenght,         &sysex[SYSEX_DATA_START+6]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->midi_channel_position,   &sysex[SYSEX_DATA_START+8]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_value_position_0,   &sysex[SYSEX_DATA_START+10]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_value_position_1,   &sysex[SYSEX_DATA_START+12]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_value_min,          &sysex[SYSEX_DATA_START+14]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_value_max,          &sysex[SYSEX_DATA_START+16]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->checksum_start_position, &sysex[SYSEX_DATA_START+18]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);
            k = Nibbles_To_Single_Byte(&MidiStruct->ctrl_chain,              &sysex[SYSEX_DATA_START+20]);
            if(k == INVALID_SYSEX) return(INVALID_SYSEX);
            checksum = (uchar)((checksum + k) & 0x7F);

            // Fill Midi string from sysex content, also extract checksum
            j=0;
            for(i=0; i < MidiStruct->midi_str_lenght; i++)
            {
               k = Nibbles_To_Single_Byte(&MidiStruct->midi_string[i], &sysex[SYSEX_DATA_START+(CTRL_STRUCT_HEADER_SIZE<<1)+j]);
               if(k == INVALID_SYSEX) return(INVALID_SYSEX);
               checksum = (uchar)((checksum + k) & 0x7F);
               j += 2;
            }

            // Check the extracted checksum is ok
            if(checksum != sysex[SYSEX_DATA_START+(CTRL_STRUCT_HEADER_SIZE<<1)+j]) return(INVALID_SYSEX);

            // return the whole sysex lenght
            return((uchar)(SYSEX_DATA_START + result + 1 + 1));
        }
}

//---------------------------------------------------------------------------
// COMMON PARAM DOWNLOAD/UPLOAD TO SYSEX
//---------------------------------------------------------------------------
// Create a sysex for common param
// action     = DOWNLOAD, UPLOAD
// address    = address of commmon param
// values     = pointer on common param values (may also be a single value)
//              if common param is group name (address 0x0C) values is a pointer on group
//              name string of 14 bytes or less (terminated by \0)
// sysex      = pointer on sysex buffer to be filled
// group      = group to upload/download (only used for group names)
// returns created sysex lenght, 0 if error
//---------------------------------------------------------------------------
uchar Bs3x_Common_Param_To_Sysex(uchar action, uchar address, uchar *values, uchar *sysex, uchar group)
{
        uchar SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3;
        uchar sysex_lenght, i, k, m;

        SYSEX_ID0 = FRAME_IS_COMMON_PARAM;
        SYSEX_ID2 = address;
        SYSEX_ID3 = group;

        // Get common param data lenght
        SYSEX_ID1  = Get_Common_Param_Data_Lenght(address);

		for(m=0; m<SYSEX_BUFFER_MAX; m++) 
			sysex[m] = 0;

        // need to program or upload common param ?
        if(action==UPLOAD)
        {
          // upload sysex has a fixed length
          sysex_lenght = UPLOAD_SYSEX_LENGHT;
          // flag bit program/upload
          SYSEX_ID0 |= prog_upload_sysxid0;
          // upload action requires a returned sysex
          SYSEX_ID0 |= acknoledge_sysxid0;
          // fill eox
          sysex[UPLOAD_SYSEX_LENGHT-1] = MIDI_EOX;

          // Check if the requested action is coherent
          if(Check_invalid_request(SYSEX_ID0, SYSEX_ID2, SYSEX_ID3) == INVALID_REQUEST)
                return (INVALID_REQUEST);

        } // end upload
        else
        {
          // Check if the requested action is coherent
          if(Check_invalid_request(SYSEX_ID0, SYSEX_ID2, SYSEX_ID3) == INVALID_REQUEST)
                return (INVALID_REQUEST);

          // group name (address 0x0C) is treated separately since pointer on string is passed
          if(address == GROUP_NAME_INDEX)
          {
                // Perform space paddiing on group name if lower than 14 chars
                k = 0;
                while( (values[k] != '\0') && (k<GROUP_NAME_LENGHT)) k++;
                for(i=k; i<GROUP_NAME_LENGHT; i++)
                        values[i] = ' ';
                values[GROUP_NAME_LENGHT] = '\0';

                // // Insert group names nibbles values into sysex
                k=0;
                for(i=0; i<GROUP_NAME_LENGHT; i++)
                {
                    if((values[i] < ' ') || (values[i] > '~')) return (INVALID_REQUEST);
                    Single_Byte_To_Nibbles(values[i], &sysex[SYSEX_DATA_START+k]);
                    k += 2;
                }
          }
		  else
          // tempo is sent via 2 nibbles 
          if(address == INT_TEMPO_INDEX)
          {
			Single_Byte_To_Nibbles(values[0], &sysex[SYSEX_DATA_START]);
		  }
          else
          {
                // Insert common param values into sysex
                for(i=0; i<SYSEX_ID1; i++)
                {
                     // Values must be 7 bits
                     if(values[i] & 0x80) return(INVALID_REQUEST);
                     sysex[SYSEX_DATA_START+i] = values[i];
                }
          }

          // donwload common param lenght sysex has a fixed length
          sysex_lenght = (uchar)(SYSEX_DATA_START + SYSEX_ID1 + 1);

          // insert EOX
          sysex[sysex_lenght-1] = MIDI_EOX;
        } // end download

        // Fill sysex header
        Fill_Sysex_Header(sysex, SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3);

        return(sysex_lenght);
}

//---------------------------------------------------------------------------
// SYSEX TO COMMON PARAM
//---------------------------------------------------------------------------
// extract the common param values from sysex
// address    = address of expected commmon param
//              if common param is group name (address 0x0C) values is a pointer on group
//              name string of 14 bytes (terminated by \0)
// values     = pointer on common param values to be filled (may also be a single value)
// sysex      = pointer on already filled sysex buffer
// group      = group to upload/download (only used for group names)
// returns    = INVALID_SYSEX if not a valid sysex, sysex lenght if OK
//---------------------------------------------------------------------------
uchar Bs3x_Sysex_To_Common_Param(uchar address, uchar *values, uchar *sysex, uchar group)
{
        //uchar SYSEX_ID0, SYSEX_ID1, SYSEX_ID2, SYSEX_ID3;
        uchar result, i, j, k; //, checksum;

        // Check the sysex contains a common param
        if((sysex[6] & FRAME_TO_Z8_TYPE_MSK) != FRAME_IS_COMMON_PARAM)
           return(INVALID_SYSEX);

        // Check if the requested action is coherent
        if(Check_invalid_request(sysex[6], address, group) == INVALID_REQUEST)
          return (INVALID_REQUEST);

        // First check the sysex is a valid incoming wave idea sysex
        result = Check_Valid_Sysex_Header(sysex, address, group);

        // Here the received sysex is valid, result contains sysex data lenght in nibbles
        if(result != Get_Common_Param_Data_Lenght(address)) return(INVALID_SYSEX);

        // group name (address 0x0C) is treated separately since pointer on string is passed
        // un-nibble group names and add terminated string
        if(address == GROUP_NAME_INDEX)
        {
            // Fill group name from sysex content
            j=0;
            for(i=0; i < (result >> 1); i++)
            {
               k = Nibbles_To_Single_Byte(&values[i], &sysex[SYSEX_DATA_START+j]);
               if(k == INVALID_SYSEX) return(INVALID_SYSEX);
               j += 2;
            }

            // Add null terminated string
            values[i] = '\0';
        }
        else
        if(address == INT_TEMPO_INDEX)
        {
            // Fill tempo from sysex content
            k = Nibbles_To_Single_Byte(&values[0], &sysex[SYSEX_DATA_START]);
			if(k == INVALID_SYSEX) return(INVALID_SYSEX);
		}
		else
		{
            // Fill common param from sysex content
            for(i=0; i < result; i++)
                values[i] = sysex[SYSEX_DATA_START + i];
        }

        // return the whole sysex lenght
        return((uchar)(SYSEX_DATA_START + result + 1 + 1));
}

//---------------------------------------------------------------------------
// FIRMWARE TO SYSEX
//---------------------------------------------------------------------------
// Create a sysex for firmware
// action     = START_FW_UPGRADE, DOWNLOAD_EZ, DOWNLOAD_Z8, PREPARE_Z8_UPGRADE, FINISH_FW_UPGRADE
// address    base address of firmware to download (not used if action is INITIATE)
// values     = if action = DOWNLOAD : pointer on firmware int datas
//            = if action = INITIATE_FW_UPGRADE : ez firmware size, z8 firmware size
// lenght     = datas nb to download, must be <= 16 ; if action is INITIATE : must be 6
//              if action = FINISH_FW_UPGRADE length must be 0
// sysex      = pointer on sysex buffer to be filled
// returns created sysex lenght, INVALID_REQUEST
// address are 16bits splitted into : 2 msb ; 0 + 7middes ; 0 +7 lsb to be sent on 7bits sysexs
// values are 16bits containing 0000 , high nibble, 0000, low nibble
//---------------------------------------------------------------------------
uchar Bs3x_Firmware_To_Sysex(uchar action, uint address, uchar lenght, uint *values, uchar *sysex)
{
        uchar SYSEX_ID0, SYSEX_ID1; //, SYSEX_ID2, SYSEX_ID3;
        uchar sysex_lenght, i, k, m, checksum;
		
        SYSEX_ID0 = FRAME_IS_FIRMWARE_OR_PURE_MIDI | acknoledge_sysxid0;

		sysex_lenght = INVALID_REQUEST;

		for(m=0; m<SYSEX_BUFFER_MAX; m++) 
			sysex[m] = 0;


        // ACTION IS A FIRMWARE DOWNLOAD
        if((action == DOWNLOAD_EZ)||(action == DOWNLOAD_Z8))
        {
           // request a checksum check if downloading firmware datas
           SYSEX_ID0 |= checksum_check_sysxid0;
           // when upgrading fw datas, lenght must not be 0
           if(lenght == 0) return (INVALID_REQUEST);

           // firmware data lenght checks
           if(action == DOWNLOAD_EZ)
		   {
			    //if(lenght > EE_24C64_PAGE_LENGHT) return (INVALID_REQUEST);
				if(lenght > FIRMWARE_DATA_MAX_EZ) return (INVALID_REQUEST);
		   }
		   else
           if(action == DOWNLOAD_Z8)
		   {
				if(lenght > FIRMWARE_DATA_MAX_Z8) return (INVALID_REQUEST);
		   }

           // Get lenght of firmware datas in bytes in sysex (2 bytes per int)
           SYSEX_ID1  = (uchar)(lenght * 2);

           // whole sysex lenght = f0 00 20 4f 00 01 UU LL AA AA AA - 2 x data lenght + CS + F7
           sysex_lenght = (uchar)(SYSEX_HEADER_SIZE + 1 + 3 + SYSEX_ID1 + 1 + 1);

		   // check the sysex lenght is < 128
		   if(sysex_lenght > SYSEX_BUFFER_MAX) return (INVALID_REQUEST);

           if(action == DOWNLOAD_Z8)
              SYSEX_ID0 |= prog_ez_z8_sysxid0;

           // Fill sysex header
           Fill_Sysex_Header(sysex, SYSEX_ID0, SYSEX_ID1, 0, 0);

           // assign adress into sysex
           sysex[8]  = (uchar) ((address >> 14) & 0x03);
           sysex[9]  = (uchar) ((address >> 7 ) & 0x7F);
           sysex[10] = (uchar) ((address      ) & 0x7F);

           // assign firmware datas into sysex
           k=0;
           checksum=0;
           for(i=0; i<SYSEX_ID1; i=(uchar)(i+2))
           {
              if((values[k] & 0x5050)!=0x5050) return(INVALID_REQUEST);
              sysex[i+11] = (uchar) ((values[k] >> 8) & 0x00FF);
              checksum = (uchar)((checksum + sysex[i+11]) & 0x7F);
              sysex[i+12] = (uchar) ((values[k] & 0x00FF));
              checksum = (uchar)((checksum + sysex[i+12]) & 0x7F);
              k=(uchar)(k+1);
           }

           // insert calculated checksum into sysex
           sysex[sysex_lenght-2] = checksum;
        }
        else
        if(action == START_FW_UPGRADE)
        {
          // here action is START_FW_UPGRADE
          // simply send frame is firmware with lenght = 127
          if(lenght != DATA_LENGHT_START_BOOTLOADER) return (INVALID_REQUEST);

		  SYSEX_ID0 |= checksum_check_sysxid0;

          // Fill sysex header
          Fill_Sysex_Header(sysex, SYSEX_ID0, lenght, 0, 0);

          // whole sysex lenght = f0 00 20 4f 00 01 UU LL F7
          sysex_lenght = SYSEX_HEADER_SIZE + 1 + 1;

        }
        else
        if( (action == FINISH_FW_UPGRADE) || (action == PREPARE_Z8_UPGRADE))
        {
          // here action is FINISH_FW_UPGRADE
          // lenght must be 0
          if(lenght != 0) return (INVALID_REQUEST);

          // signal to finish upgrade
          SYSEX_ID0 |= fw_znext_finish_sysxid0;

		  // prepare z8 is done setting finish + prog z8 bits
		  if(action == PREPARE_Z8_UPGRADE)
			 SYSEX_ID0 |= prog_ez_z8_sysxid0;

          // Fill sysex header
          Fill_Sysex_Header(sysex, SYSEX_ID0, lenght, 0, 0);

          // whole sysex lenght = f0 00 20 4f 00 01 UU F7
          sysex_lenght = SYSEX_HEADER_SIZE + 1 + 1;

        }
         // insert EOX
        sysex[sysex_lenght-1] = MIDI_EOX;

        return(sysex_lenght);
}

//---------------------------------------------------------------------------
// BFW FILE TO FIRMWARES
//---------------------------------------------------------------------------
// ez_fw_size  = pointer on ez firmware size to get
// z8_fw_size  = pointer on z8 firmware size to get
// ez_firmware = pointer on firmware int datas to fill
// z8_firmware = pointer on firmware int datas to fill
// returns 0 if no error, INVALID_BFW
// both firmwares are read encoded, then stored and sent to midi encoded as well
//---------------------------------------------------------------------------
uchar Bs3x_BfwFile_To_Firmwares(char *filename, uchar *identity, uint *ez_fw_size, uint *z8_fw_size, uint *ez_firmware, uint *z8_firmware)
{
  uint i, checksum, checksum_file;
  uint tmp_read;

  FILE * fic;
  fic = fopen(filename, "rt");
  if (fic == NULL)
    return(INVALID_BFW);
  // place pointer at the begining of file
  fseek(fic, 0, 0);

  // get identity
  for(i=0; i< IDENTITY_LENGHT ; i++)
  {
	  fscanf(fic, "%2x", &tmp_read);
	  identity[i] = (uchar)(tmp_read); 	
  }

  // now should get a ;
  if(fgetc(fic) != ';') return(INVALID_BFW);
  // read fw sizes
  fscanf(fic, "%4x;%4x;", (uint *) ez_fw_size, (uint *) z8_fw_size);
  // get ez firmware datas
  checksum = 0;
  for(i=0; i< *ez_fw_size ; i++)
  {
      fscanf(fic, "%4x", (uint *) &tmp_read);
	  ez_firmware[i] = tmp_read;	
	  // firmware datas MSB & LSB nibbles must be 0
      if((ez_firmware[i] & 0x5050)!=0x5050) return(INVALID_BFW);
      // calculate checksum
      checksum = (checksum + ez_firmware[i]) & 0x3FFF;
  }
  // compare checksums
  fscanf(fic, "%4x", (uint *) &checksum_file);
  if(checksum != checksum_file) return(INVALID_BFW);
  // now should get a ;
  if(fgetc(fic) != ';') return(INVALID_BFW);

  // get z8 firmware datas
  checksum = 0;
  for(i=0; i< *z8_fw_size ; i++)
  {
      fscanf(fic, "%4x", (uint *) &tmp_read);
	  z8_firmware[i] = tmp_read;
      // firmware datas MSB & LSB nibbles must be 0
      if((z8_firmware[i] & 0x5050)!=0x5050) return(INVALID_BFW);
      // calculate checksum
      checksum = (checksum + z8_firmware[i]) & 0x3FFF;
  }
  // compare checksums
  fscanf(fic, "%4x", (uint *) &checksum_file);
  if(checksum != checksum_file) return(INVALID_BFW);
  // now should get a ;
  if(fgetc(fic) != ';') return(INVALID_BFW);

  return (0);
}

//---------------------------------------------------------------------------
// SYSEX FIRMWARE ACKNOLEDGE RESULT
//---------------------------------------------------------------------------
// check acknoledge result after firmware upgrade request
// action     = CHECK_FW_ACKNOLEDGE_RESULT, WAIT_FIRMWARE_ACK
// address    = base address of firmware dowloaded, 0 if action is WAIT_FIRMWARE_ACK
// lenght     = datas nb downloaded, must be < 16
// sysex      = pointer on sysex buffer received
// returns INVALID_SYSEX if not a valid sysex, or :
//         ACK_ANSWER_NO_ERROR, ACK_ANSWER_ERROR if action = CHECK_FW_ACKNOLEDGE_RESULT
//         0 if action is WAIT_FIRMWARE_ACK
// address is 16bits : it must be unsplitted from : 2 msb ; 0 + 7middes ; 0 +7 lsb
//---------------------------------------------------------------------------
uchar Bs3x_Sysex_To_Firmware(uchar action, uchar *sysex)
{
        // Check the sysex contains firmware result
        if((sysex[6] & FRAME_TO_Z8_TYPE_MSK) != FRAME_IS_FIRMWARE_OR_PURE_MIDI)
           return(INVALID_SYSEX);

        // Check is the sysex buffer contains a Wave Idea sysex
        if(sysex[0] != MIDI_SOX)        return(INVALID_SYSEX);
        if(sysex[1] != WID_SYX_ID0)     return(INVALID_SYSEX);
        if(sysex[2] != WID_SYX_ID1)     return(INVALID_SYSEX);
        if(sysex[3] != WID_SYX_ID2)     return(INVALID_SYSEX);
        if(sysex[4] != WID_SYX_ID3)     return(INVALID_SYSEX);
        if(sysex[5] != WID_SYX_ID4)     return(INVALID_SYSEX);
        // Check it is an incoming sysex
        if(!(sysex[6] & sysex_direction_sysxid0)) return(INVALID_SYSEX);

        if(action == WAIT_FIRMWARE_ACK)
        {
          if(sysex[7] != MIDI_EOX)  return(INVALID_SYSEX);
          if(sysex[6] & acknoledge_sysxid0)
             return (ACK_ANSWER_NO_ERROR);
          return(ACK_ANSWER_ERROR);
        }
/*
        if(action == CHECK_FW_ACKNOLEDGE_RESULT)
        {
           // Check if the lenght matches the expected lenght
           if(sysex[7] != (lenght*2))  return (INVALID_SYSEX);

           // Check if the address matches the expected address
           if(sysex[8]  != (uchar) ((address >> 14) & 0x03))  return(INVALID_SYSEX);
           if(sysex[9]  != (uchar) ((address >> 7 ) & 0x7F))  return(INVALID_SYSEX);
           if(sysex[10] != (uchar) ((address      ) & 0x7F))  return(INVALID_SYSEX);

           if(sysex[11] != MIDI_EOX)  return(INVALID_SYSEX);

           // a valid sysex is found : check acknoledge result
           if(sysex[6] & acknoledge_sysxid0) return(ACK_ANSWER_NO_ERROR);
           return(ACK_ANSWER_ERROR);
        }
*/
    return(INVALID_SYSEX);
}

//---------------------------------------------------------------------------
// LCD STRING CHECKS
//---------------------------------------------------------------------------
// check that all data into the lcd string are coherent
// returns error message if not a valid LCD structure, or LCD_STRUCT_NO_ERROR if OK
//---------------------------------------------------------------------------
uchar CheckLcdString(uchar *lcd_str)
{
	uchar i = 0;

	while( (i < LCD_STR_MAX_LENGHT) && (lcd_str[i] != '\0') )
	{
		if( (lcd_str[i] < ' ') || (lcd_str[i] > '~') )
			return(LCD_STR_CAR_ERROR);
		i++;
	}

	// LCD strings must be 16 car always
//	if(i != LCD_STR_MAX_LENGHT) return(LCD_STR_LENGHT_ERROR);

	return(LCD_STRING_NO_ERROR);
}


//---------------------------------------------------------------------------
// MIDI STRUCT CHECKS
//---------------------------------------------------------------------------
// check that all data into the midi struct are coherent
// returns error message if not a valid MIDI structure, or MIDI_STRUCT_NO_ERROR if OK
//---------------------------------------------------------------------------
uchar CheckMidiStruct(Ctrl_Midi_Struct *MidiStruct, uchar index)
{
	int i;

	// the parameter could be undefined - it is NOT an error only if all params are set to 0
	if((MidiStruct->midi_str_lenght == 0) && 
		(MidiStruct->ctrl_value_min == 0) &&	
		(MidiStruct->ctrl_value_max == 0) &&
		(MidiStruct->midi_channel_position == 0) &&	
		(MidiStruct->ctrl_value_position_0 == 0) &&	
		(MidiStruct->ctrl_value_position_1 == 0) &&	
		(MidiStruct->checksum_start_position == 0) &&	
		(MidiStruct->ctrl_status_0 == 0) &&	
		(MidiStruct->ctrl_status_1 == 0) &&	
		(MidiStruct->ctrl_status_2 == 0))
		return(MIDI_STRUCT_UNDEF_WARNING);
	
	// if the control value must be splitted in 2 nibbles, then all 2 control value positions must be defined
	if(MidiStruct->ctrl_status_2 & split_ctrl_value)
	{
		if((MidiStruct->ctrl_value_position_0 == 0) || (MidiStruct->ctrl_value_position_1 == 0))	
			return (CTRL_NIBBLE_ERROR);
	}

	// propose to insert MIDI delay if the MIDI string is too long
	if(MidiStruct->midi_str_lenght > MIDI_STR_LENGHT_REQUIRING_DELAY)
		if((MidiStruct->ctrl_status_1 & midi_delay_msk) == 0)
			return(MIDI_STR_DELAY_REQUIRED_WARNING);

	// Check that min value & max value are 0-127
	if(MidiStruct->ctrl_value_min & 0x80) return(MIN_SUP_127_ERROR);
	if(MidiStruct->ctrl_value_max & 0x80) return(MAX_SUP_127_ERROR);

	// Check that min value < max value
	if(MidiStruct->ctrl_value_min > MidiStruct->ctrl_value_max) return(MIN_SUP_MAX_ERROR);
			  
	// Check that midi str lenght is 0-21
	if(MidiStruct->midi_str_lenght > MIDI_STR_MAX_LENGHT) return(MIDI_STR_LENGHT_ERROR);
	if(MidiStruct->midi_str_lenght == 0) return(MIDI_STR_LENGHT_ERROR);
			  
	// checks that positions are not identical
	if((MidiStruct->midi_channel_position == MidiStruct->ctrl_value_position_0) &&
	   (MidiStruct->midi_channel_position != 0)) return(DUPLICATE_POS_ERROR);
	if((MidiStruct->midi_channel_position == MidiStruct->ctrl_value_position_1) &&
	   (MidiStruct->midi_channel_position != 0)) return(DUPLICATE_POS_ERROR);
	if((MidiStruct->ctrl_value_position_0 == MidiStruct->ctrl_value_position_1) &&
  	   (MidiStruct->ctrl_value_position_0 != 0)) return(DUPLICATE_POS_ERROR);

	// Check that channel position is 0-21
	if(MidiStruct->midi_channel_position > MidiStruct->midi_str_lenght) return(MIDI_STR_CHAN_POS_ERROR);

	// Check that control value 0 position is 0-21
	if(MidiStruct->ctrl_value_position_0 > MidiStruct->midi_str_lenght) return(MIDI_STR_VAL0_POS_ERROR);
			  
	// Check that control value 1 position is 0-21
	if(MidiStruct->ctrl_value_position_1 > MidiStruct->midi_str_lenght) return(MIDI_STR_VAL1_POS_ERROR);

	// Check that checksum position is 0-21
	if(MidiStruct->checksum_start_position > MidiStruct->midi_str_lenght) return(MIDI_STR_CHKSUM_POS_ERROR);

	// Check the first byte of Midi string is a Status byte
	if(!(MidiStruct->midi_string[0] & 0x80)) return(MIDI_STR_STATUS_MISSING_ERROR);
	
	// Check the Midi string do not contains FE or FF bytes since they are used as protocol markers between EZ & Z8
	for(i=0; i<MidiStruct->midi_str_lenght; i++)
		if( (MidiStruct->midi_string[i] == 0xFE) || (MidiStruct->midi_string[i] == 0xFF)) return(MIDI_STR_INVALID_BYTE_ERROR);

	// Check the control is configured for at least 1 output
	//if(!(MidiStruct->ctrl_status_1 & MIDI_OUT_SEL_MSK))
	if(!(MidiStruct->ctrl_status_1 & midi_out_1_en))
		if(!(MidiStruct->ctrl_status_1 & midi_out_2_en))
			return(NO_MIDI_OUT_ERROR);

	// check the chained control is not chained on itself
	if(index != 0)
	if((MidiStruct->ctrl_chain & CTRL_CHAIN_NUMERO_MSK) == index)
		return(CTRL_CHAIN_LOOP_ERROR);

	// check the chained control is not chained on itself
	if((MidiStruct->ctrl_chain & CTRL_CHAIN_NUMERO_MSK) > CONTROLS_NB)
		return(CTRL_CHAIN_RANGE_ERROR);

	return(MIDI_STRUCT_NO_ERROR);
}

//---------------------------------------------------------------------------
// MIDI STRUCT INIT
//---------------------------------------------------------------------------
// Init all data into the midi struct so they are coherent
// INIT_EMPTY or INIT_DEFAULT
//---------------------------------------------------------------------------
void InitMidiStruct(Ctrl_Midi_Struct *MidiStruct, uchar group, uchar index, uchar init_type)
{
	uchar tmp, offset_index_from_group = 0;

	if(init_type==INIT_EMPTY)
	{
		MidiStruct->ctrl_status_0			= 0; 
		MidiStruct->ctrl_status_1			= 0; 
		MidiStruct->ctrl_status_2			= 0; 
		MidiStruct->midi_str_lenght			= 0;
		MidiStruct->midi_channel_position	= 0;
		MidiStruct->ctrl_value_position_0	= 0;
		MidiStruct->ctrl_value_position_1	= 0;
		MidiStruct->ctrl_value_min			= 0;
		MidiStruct->ctrl_value_max			= 0;
		MidiStruct->checksum_start_position	= 0;
		MidiStruct->ctrl_chain				= 0;
		MidiStruct->midi_string[0]			= '\0';
		return;
	}

	for(tmp=0; tmp < group; tmp++)
	{
		offset_index_from_group += GROUP_INDEX_OFFSET;
		offset_index_from_group &= 0x7F;
	}

	// standard mode generates CC # 
	MidiStruct->ctrl_status_0 = (uchar)((index + offset_index_from_group) & 0x7F); 

	// if switch :toggle mode, do not exclude from scene, do not autosend, send to all outs, no midi delay
	MidiStruct->ctrl_status_1 = switch_ctrl_hook_mode | usb_out_en | midi_out_1_en | midi_out_2_en; 

	// default curve model is linear
	MidiStruct->ctrl_status_2 = 0; 

	// default user mode is standard CC # 0
	MidiStruct->midi_str_lenght			= 3;
	MidiStruct->midi_channel_position	= 1;
	MidiStruct->ctrl_value_position_0	= 3;
	MidiStruct->ctrl_value_position_1	= 0;
	MidiStruct->ctrl_value_min			= 0;
	MidiStruct->ctrl_value_max			= 127;
	MidiStruct->checksum_start_position	= 0;
	MidiStruct->ctrl_chain				= 0;
	MidiStruct->midi_string[0]			= 0xB0;
	MidiStruct->midi_string[1]			= (uchar)((index + offset_index_from_group) & 0x7F);
	MidiStruct->midi_string[2]			= 0x00;

}

//---------------------------------------------------------------------------
// LCD STRING INIT
//---------------------------------------------------------------------------
// Init all data into the midi struct so they are coherent
//---------------------------------------------------------------------------
void InitLcdString(uchar *lcd_str, uchar group, uchar index, uchar init_type)
{
	if(init_type==INIT_EMPTY)
	{
		lcd_str[0] = '\0';
		return;
	}

	uchar tmp, offset_index_from_group = 0;
	uchar loc_lcd_char = '0';
    uchar loc_tmp_value;

	for(tmp=0; tmp < group; tmp++)
	{
		offset_index_from_group += GROUP_INDEX_OFFSET;
		offset_index_from_group &= 0x7F;
	}

	offset_index_from_group = (uchar)((index + offset_index_from_group) & 0x7F);

	loc_tmp_value = offset_index_from_group;

	if(offset_index_from_group < 100) 
	   	lcd_str[9] = '0';
	else
	if(offset_index_from_group < 200) 
	{ 	
	 	lcd_str[9] = '1';
		loc_tmp_value -= 100;
	}
	else
	{ 	
	 	lcd_str[9] = '2';
		loc_tmp_value -= 200;
	}

	// Treat now the 2 remaining digits
	while(loc_tmp_value > 9)
	{
		loc_tmp_value = (uchar)(loc_tmp_value - 10);
		loc_lcd_char++;
	}

	lcd_str[10] = loc_lcd_char;
	lcd_str[11] = (uchar)(loc_tmp_value + '0');	

	lcd_str[0] = ' ';
	lcd_str[1] = ' ';
	lcd_str[2] = ' ';
	lcd_str[3] = ' ';
	lcd_str[4] = 'C';
	lcd_str[5] = 'C';
	lcd_str[6] = ' ';
	lcd_str[7] = '#';
	lcd_str[8] = ' ';
	
	lcd_str[12] = '\0';
//	lcd_str[13] = ' ';
//	lcd_str[14] = ' ';
//	lcd_str[15] = ' ';
}

//---------------------------------------------------------------------------
// COMMONS PARAMS INIT
//---------------------------------------------------------------------------
// Init all data i
//---------------------------------------------------------------------------
void InitCommonParams(uchar *common_params, uchar init_type, float fw_version_float)
{
	int idx;
	// reset common params
	for(idx=0; idx< LAST_COMMON_PARAM_INDEX; idx++)
		common_params[idx]  = 0;

	if(init_type==INIT_EMPTY)
		return;

	// enable mergers 
	common_params[CTRL_REG_0_7BITS_TO_EZ_INDEX] = (1<<merger_src_midi_in_state_z_ez0_bit_pos)|
												  (1<<merger_src_usb_in_state_z_ez0_bit_pos) |
												  (1<<merger_dst_usb_out_state_z_ez0_bit_pos)|
												  (1<<merger_dst_out1_state_z_ez0_bit_pos);
	common_params[CTRL_REG_1_7BITS_TO_EZ_INDEX] = (1<<realtime_source_int_z_ez1_bit_pos); // V1.5 0;
	// defalut values 
	common_params[Z8_STATUS_0_NVM_INDEX] =	arpp_dst_midi_out1_z0nvm | arpp_dst_midi_out2_z0nvm | arpp_dst_usb_out_z0nvm;
	// defalut values 
	common_params[Z8_STATUS_1_NVM_INDEX] =	/* V1.5 hook_zjump_mode_z1nvm | */
											infrared_state_z1nvm |
											motion_loop_state_z1nvm;
	// V1.5 
	if(fw_version_float < (float) 1.8)
		common_params[Z8_STATUS_1_NVM_INDEX] |=	hook_zjump_mode_z1nvm_fw_v17;

	// defalut values 
	common_params[Z8_STATUS_2_NVM_INDEX] =	shift_lock_midi_z2nvm; // V1.5
	// defalut values 
	common_params[CURRENT_AUTOM_INDEX]	 =	1;
	// defalut values 
	common_params[INT_TEMPO_INDEX]		 =	100; // +30 will be added
	// defalut values 
	common_params[OPERATING_MODE_NVM_INDEX]	 =	0; // V1.4 for firmware v1.6 and higher
	// defalut values 
	common_params[Z8_STATUS_3_NVM_INDEX] =	0; // V1.5

}

//---------------------------------------------------------------------------
// USER CURVES INIT
//---------------------------------------------------------------------------
// Init all data i
//---------------------------------------------------------------------------
void InitUserCurves(uchar *user_curve, uchar init_type)
{
	unsigned int idx;
	// reset user curves
	for(idx=0; idx< CURVE_TABLES_LENGHT; idx++)
	{
		if(init_type==INIT_EMPTY)
			user_curve[idx]  = 0;
		else
			user_curve[idx]  = (unsigned char)idx;
	}
}


