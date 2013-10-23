#ifndef MAIN_H		//Header sentry
#define MAIN_H

// PROTOTYPES
//void TD_Poll(void);
void Check_Midi_In_Buffer(Midi_In_Struct *midi_struct, bit merger_src_enable);
void Check_Serial_In_From_Z8_Buffer(void);
void Check_Realtime(void);
void Check_Realtime_Buffers(Midi_In_Realtime_Struct *midi_struct, bit merger_src_enable);
void Check_Overflow_Flags(void);
void Update_Sync24(unsigned char data_received);
//void Check_Realtime_From_Z8(void);

#endif

