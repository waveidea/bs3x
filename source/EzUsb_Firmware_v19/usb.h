#ifndef USB_H		//Header sentry
#define USB_H



// PROTOTYPES
void Check_If_Usb_Attached(void);
void Check_Usb(void);
void Create_Send_USB_Frame_From_MIDI_Buffer( BYTE CN, Midi_In_Struct *midi_struct, BYTE end_index_local);
void Create_Send_Realtime_USB_Frame_From_MIDI_Buffer( BYTE CN, BYTE realtime_data);
void Ep2out_Function(void);
#endif
