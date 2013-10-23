#ifndef ITR_H		//Header sentry
#define ITR_H

// PROTOTYPES
//static 
void Check_Midi_In_From_Con_Data_Received(unsigned char data_received);
//static 
void Check_Serial_From_Z8_Data_Received(unsigned char data_received);
//static 
void Check_Realtime_MIDI_Data_Received(unsigned char data_received, Midi_In_Realtime_Struct *midi_struct) reentrant;
//static 
unsigned char Get_MIDI_Msg_Size(unsigned char status) reentrant;
//static 
void MIDI_In_Add_Buffer(unsigned char data_in, Midi_In_Struct *midi_struct) reentrant;
void generateSync24(unsigned char data_in, Midi_In_Realtime_Struct *midi_struct) reentrant; // V1.9
#endif