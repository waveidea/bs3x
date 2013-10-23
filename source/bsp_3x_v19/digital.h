#ifndef DIGITAL
#define DIGITAL

#define TRANSPORT_MUX_OFFSET	3U 

enum {OTHER_SW_ID, ALL_SW_ID}; // used as the id of switch register

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckTransport(void);
void CheckSwitches(void);
void CheckOtherSwitches(void);
//void CheckSendDisplayCurrentSw(unsigned char volatile far *port, uchar pin, uchar sw_id, uchar index);
void CheckSendDisplayCurrentSw(uchar pin_state, uchar sw_id, uchar index);
#endif