#ifndef PINS_DEF_H		//Header sentry
#define PINS_DEF_H

// PINS FUNCTIONS DEFINITIONS

// PORT BIT DEFINITIONS : OUTPUTS
#define TX1_TO_CON1_PORT_OUT	OUTC	// tx1 to connector MIDI Out 1 output pin
#define TX1_TO_CON1_PIN  		bmBIT1	// tx1 to connector MIDI Out 1 output pin
#define USB_LED_PORT_OUT		OUTB	// USB Led output pin
#define USB_LED_PIN				bmBIT6	// USB Led
#define Z8_RESET_PORT_OUT		OUTB	// Reset control of Z8 board
#define Z8_RESET_PIN			bmBIT7	// Reset control of Z8 board
#define OCD_EN_PORT_OUT			OUTC	// OCD Enable output port
#define OCD_EN_PIN				bmBIT7	// OCD Enable output pin

#define TX1_TO_Z8_DBG_PORT_OUT	OUTB	// 
#define TX1_TO_Z8_DBG_PIN  		bmBIT3	// 


#define SYNC24_CLOCK_PORT_OUT	OUTA	// Sync24 clock output pin
#define SYNC24_CLOCK_PIN 		bmBIT5	// Sync24 clock output pin
#define SYNC24_CONT_PORT_OUT	OUTC	// Sync24 continue output pin
#define SYNC24_CONT_PIN			bmBIT5	// Sync24 continue output pin
#define SYNC24_RUN_PORT_OUT		OUTC	// Sync24 run output pin
#define SYNC24_RUN_PIN			bmBIT6	// Sync24 run output pin

// PORT BIT DEFINITIONS : INPUTS
#define EDIT_PORT_IN			PINSB	// edit input port
#define EDIT_PIN				bmBIT4	// edit input pin
#define OK_PORT_IN				PINSB	// ok input port
#define OK_PIN					bmBIT5	// ok input pin

#ifdef HW_V10
#define CANCEL_PORT_IN			PINSA	// cancel input port
#define CANCEL_PIN				bmBIT4	// cancel input pin
#define VUSB_PORT_IN			PINSC	// usb power supply monitor input port
#define VUSB_PIN				bmBIT4	// usb power supply monitor input pin
#else
#define CANCEL_PORT_IN			PINSC	// cancel input port
#define CANCEL_PIN				bmBIT4	// cancel input pin
#define VUSB_PORT_IN			PINSA	// usb power supply monitor input port
#define VUSB_PIN				bmBIT4	// usb power supply monitor input pin
#endif

#define FOOTSWITCH_PORT_IN		PINSB	// footswitch input pin
#define FOOTSWITCH_PIN  		bmBIT1	// footswitch input pin

// OPTIONAL PINS ACTUALLY NOT USED
#define EXP_1_PORT_IN			PINSC	// expansion 1 input port
#define EXP_1_PIN				bmBIT3	// expansion 1 input pin
#define EXP_2_PORT_IN			PINSC	// expansion 2 input port
#define EXP_2_PIN				bmBIT2	// expansion 2 input pin
#define TX2_TO_CON2_PORT_OUT	OUTB	// tx2 to connector MIDI Out 2 output pin (NOT USED)
#define TX2_TO_CON2_PIN  		bmBIT0	// tx2 to connector MIDI Out 2 output pin (NOT USED)

#endif





