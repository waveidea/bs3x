; USB MIDI DESCRIPTORS
#include "usb_defs.h"

; ==========================================================================
; Copyright (C) BITSTREAM 3X - Wave Idea 2004
; ==========================================================================

DSCR_DEVICE	equ	1	;; Descriptor type: Device
DSCR_CONFIG	equ	2	;; Descriptor type: Configuration
DSCR_STRING	equ	3	;; Descriptor type: String
DSCR_INTRFC	equ	4	;; Descriptor type: Interface
DSCR_ENDPNT	equ	5	;; Descriptor type: Endpoint

ET_CONTROL	equ	0	;; Endpoint type: Control
ET_ISO		equ	1	;; Endpoint type: Isochronous
ET_BULK		equ	2	;; Endpoint type: Bulk
ET_INT		equ	3	;; Endpoint type: Interrupt

public		DeviceDscr, ConfigDscr, StringDscr, UserDscr

DSCR	SEGMENT	CODE

;;-----------------------------------------------------------------------------
;; Global Variables
;;-----------------------------------------------------------------------------
;; Note: This segment must be located in on-part memory.
		rseg DSCR		;; locate the descriptor table anywhere below 8K
DeviceDscr:	db	deviceDscrEnd-DeviceDscr		;; Descriptor length
		db	DSCR_DEVICE	;; Decriptor type
		db	0x00, 0x01	;; Specification Version (BCD)
		db	00H  		;; Device class
		db	00H		;; Device sub-class
		db	00H		;; Device sub-sub-class
		db	64		;; Maximum packet size
		db	0x4D, 0x0A	;; Vendor ID
		db	0xF5, 0x00	;; Product ID
		db	0x11, 0x01	;; Product version ID
		db	1		;; Manufacturer string index
		db	2		;; Product string index
		db	0		;; Serial number string index
		db	1		;; Number of configurations
deviceDscrEnd:

ConfigDscr:	db	ConfigDscrEnd-ConfigDscr		;; Descriptor length
		db	DSCR_CONFIG	;; Descriptor type
		db	LOW(StringDscr-ConfigDscr)   ;; Config + End Points length (LSB)
		db	HIGH(StringDscr-ConfigDscr)  ;; Config + End Points length (MSB)
		db	2		;; Number of interfaces 
		db	1		;; configuration value, Interface number 
		db	3		;; Configuration string
		db	11000000b	;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
		db	0x96		;; Power requirement (100ma div 2)
ConfigDscrEnd:


; B3.1
;; removed otherwise win Me crashes
StandardAcIntrfcDscr:
		db	StandardAcIntrfcDscrEnd-StandardAcIntrfcDscr	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	0		;; Zero-based index of this interface
		db	0		;; index of this setting
		db	0		;; Number of end points
		db	1		;; Interface class  (AUDIO)
		db	1		;; Interface sub class  (AC:audio control)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index
StandardAcIntrfcDscrEnd:

; B3.2
ClassSpecificAcIntrfcDscr:
		db	ClassSpecificAcIntrfcDscrEnd-ClassSpecificAcIntrfcDscr		;; Descriptor length
		db	0x24		;; Descriptor type
		db	1		;; descriptor sub-type
		db	0x00, 0x01	;; revision of this class specification
		db	0x09, 0x00	;; Total size of class-specific descriptors
		db	1		;; number of streaming interface
		db	1		;; midi streaming interf 1 belongs to this audiocontrol interf
ClassSpecificAcIntrfcDscrEnd:

;B4.1
MidiStreamIntrfcDscr:
		db	MidiStreamIntrfcDscrEnd-MidiStreamIntrfcDscr	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0		;; index of this Alternate setting
		db	2		;; Number of end points 
		db	1		;; Interface class  (AUDIO)
		db	3		;; Interface sub class  (MIDISTREAMING)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index
MidiStreamIntrfcDscrEnd:

;B4.2 1.0
MSCIntrfcDscr:
		db	MSCIntrfcDscrEnd-MSCIntrfcDscr		;; Descriptor length
		db	0x24		;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0x00, 0x01	;; revision of this class specification
		db	LOW(MSCClassSize);; Total size of class-specific descriptors
		db	HIGH(MSCClassSize)
MSCIntrfcDscrEnd:

;B4.3 (1.1) //////////////////// MIDI IN 1 /////////////////////////////
MIDIInEmDscr:
		db	MIDIInEmDscrEnd-MIDIInEmDscr		;; Descriptor length
		db	0x24		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x01		;; ID of this jack
		db	0		;; unused
MIDIInEmDscrEnd:

; 1.2
MIDIInExDscr:
		db	MIDIInExDscrEnd-MIDIInExDscr		;; Descriptor length
		db	0x24		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x02		;; ID of this jack
		db	0		;; unused
MIDIInExDscrEnd:

; 1.3 MIDI IN 2
MIDIIn2ExDscr:
		db	MIDIIn2ExDscrEnd-MIDIIn2ExDscr		;; Descriptor length
		db	0x24		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x05		;; ID of this jack
		db	0		;; unused
MIDIIn2ExDscrEnd:

;B4.4 1.4 //////////////////// MIDI OUT 1 /////////////////////////////
MIDIOutEmDscr:
		db	MIDIOutEmDscrEnd-MIDIOutEmDscr		;; Descriptor length
		db	0x24		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x03		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x02		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused
MIDIOutEmDscrEnd:

;B4.4 1.5 //////////////////// MIDI OUT 2 /////////////////////////////
MIDIOutEm2Dscr:
		db	MIDIOutEm2DscrEnd-MIDIOutEm2Dscr	;; Descriptor length
		db	0x24		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x06		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x05		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused
MIDIOutEm2DscrEnd:

;B4.4 1.6 //////////////////// MIDI OUT 1 /////////////////////////////
MIDIOutExDscr:
		db	MIDIOutExDscrEnd-MIDIOutExDscr		;; Descriptor length
		db	0x24		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x04		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x01		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused
MIDIOutExDscrEnd:


;B5.2 1.7 //////////////////// ENDPOINTS /////////////////////////////
MSBulkOutEPDscr:
		db	MSBulkOutEPDscrEnd-MSBulkOutEPDscr	;; Descriptor length
		db	0x25		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x02		;; number of embedded MIDI IN Jacks (???)
		db	0x03		;; ID of embedded MIDI In Jack
		db	0x06		;; ID of embedded MIDI In Jack
MSBulkOutEPDscrEnd:

;B6.2 1.8
MSBulkInEPDscr:
		db	MSBulkInEPDscrEnd-MSBulkInEPDscr	;; Descriptor length
		db	0x25		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x01		;; number of embedded MIDI Out Jacks
		db	0x01		;; ID of embedded MIDI Out Jack
MSBulkInEPDscrEnd:

;B6.1 1.9
StdBulkInEPDscr:
		db	StdBulkInEPDscrEnd-StdBulkInEPDscr	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	USB_EP_IN_DESC		;; In Endpoint 1
		db	0x02		;; Bulk, not shared
		;;db	0x40, 0x00	;; 64 bytes per packet
		db	0x40, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused
StdBulkInEPDscrEnd:

;B5.1
StdBulkOutEPDscr:
		db	StdBulkOutEPDscrEnd-StdBulkOutEPDscr	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	0x02		;; Out Endpoint 2 
		db	0x02		;; Bulk, not shared
		;;db	0x40, 0x00	;; 64 bytes per packet
		db	0x40, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused
StdBulkOutEPDscrEnd:


MSCClassSize	EQU	$-MSCIntrfcDscr

;; string language descriptor (english)
StringDscr:
StringDscr0:
		db	StringDscr0End-StringDscr0		;; String descriptor length
		db	DSCR_STRING
		db	09H,04H
StringDscr0End:

StringDscr1:	
		db	StringDscr1End-StringDscr1		;; String descriptor length
		db	DSCR_STRING
		db	'E',00
		db	'v',00
		db	'o',00
		db	'l',00
		db	'u',00
		db	't',00
		db	'i',00
		db	'o',00
		db	'n',00
StringDscr1End:

StringDscr2:	
		db	StringDscr2End-StringDscr2		;; Descriptor length
		db	DSCR_STRING
		db	'E',00
		db	'v',00
		db	'o',00
		db	'l',00
		db	'u',00
		db	't',00
		db	'i',00
		db	'o',00
		db	'n',00
		db	' ',00
		db	'U',00
		db	'C',00
		db	'-',00
		db	'3',00
		db	'3',00
		db	' ',00
		db	'U',00
		db	'S',00
		db	'B',00
		db	' ',00
		db	'D',00
		db	'r',00
		db	'i',00
		db	'v',00
		db	'e',00
		db	'r',00
StringDscr2End:

StringDscr3:	
		db	StringDscr3End-StringDscr3		;; Descriptor length
		db	DSCR_STRING
		db	'W',00
		db	'.',00
		db	'I',00
		db	'd',00
		db	'e',00
		db	'a',00

		db	' ',00

		db	'B',00
		db	'S',00
		db	'-',00
		db	'3',00
		db	'X',00
StringDscr3End:

UserDscr:		
		dw	0000H
		end
		
