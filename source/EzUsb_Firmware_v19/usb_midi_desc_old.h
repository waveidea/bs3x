///////////////////////////////////////////////////////////////////////////
///                          usb_desc.h                                ////
////                                                                   ////
//// An example set of device / configuration descriptors.             ////
////                                                                   ////
//// Descriptors are application dependent (especially HID) so you may ////
//// have to edit them.                                                ////
////                                                                   ////
//// This file is part of CCS's PIC USB driver code, which includes:   ////
////   usb_desc.h - an example set of config and device descriptors    ////
////   usb.c - USB token and request handler code                      ////
////   usb.h - definitions, prototypes and global variables            ////
////   And the hardware peripheral level.  At the time of writing,     ////
////    CCS provides two hardware peripheral drivers.  National        ////
////    USBN960x (usbn960x.cc) and PIC16C7x5 (usb_pic.c).              ////
////                                                                   ////
//// Two examples are given using CCS's PIC USB driver.                ////
//// ex_usb_scope.c is a bulk device that sends 512 bytes to the       ////
//// host and uses the USBN960x.  ex_usb_hid.c is a HID device and     ////
//// uses the PIC16C7x5.                                               ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// Version History:                                                  ////
////                                                                   ////
//// August 2nd, 2002: Initial Public Release                          ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2002 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#IFNDEF __USB_PROTOTYPES__
   #include "usb.h"
#ENDIF

#DEFINE __USB_DESCRIPTORS__

   //usb descriptor reports
   #DEFINE USB_NUM_CONFIGURATIONS 1
   #DEFINE USB_NUM_INTERFACES     2
   #DEFINE USB_NUM_ENDPOINTS      2          //endpoint 1 IN, endpoint 2 OUT.  endpoint 0 doesnt count here
   #DEFINE USB_NUM_CLASSES        1          //hid

   //you cant use pointers when storing constants to program memory.
   BYTE CONST USB_CONFIG_DESCRIPTORS[USB_NUM_CONFIGURATIONS]={0};    //0
   BYTE CONST USB_INTERFACE_DESCRIPTORS[USB_NUM_INTERFACES]={9};     //9
   BYTE CONST USB_CLASS_DESCRIPTORS[USB_NUM_CLASSES]={18};           //18
   BYTE CONST USB_ENDPOINT_DESCRIPTORS[USB_NUM_ENDPOINTS]={27,34};   //27

   //config+interface+class+endpoint+endpoint (2 endpoints)
   #DEFINE USB_TOTAL_CONFIG_LEN      (USB_CONFIG_DESC_LEN + USB_STANDARD_AC_INTERFACE_DESC_LEN + USB_CLASS_SPECIFIC_AC_INTERFACE_DESC_LEN + USB_CLASS_SPECIFIC_AC_INTERFACE_DESC_LEN + USB_STANDARD_MS_INTERFACE_DESC_LEN + USB_CLASS_SPECIFIC_MS_INTERFACE_DESC_LEN + USB_STANDARD_MS_INTERFACE_DESC_LEN + USB_CLASS_SPECIFIC_MS_INTERFACE_DESC_LEN + USB_MIDI_IN_EMBBEDED_DESC_LEN  + USB_MIDI_IN_EXTERNAL_DESC_LEN + USB_MIDI_OUT_EMBEDDED_DESC_LEN + USB_MIDI_OUT_EXTERNAL_DESC_LEN + (USB_STANDARD_ENDPOINT_DESC_LEN + USB_CLASS_SPECIFIC_ENDPOINT_DESC_LEN)*USB_NUM_ENDPOINTS)

   // *********************************** USB DEVICE DESCRIPTORS ****************************************

   // MIDI Adapter Device Descriptor (table B1)

   BYTE CONST USB_DEVICE_DESC[USB_DEVICE_DESC_LEN] ={
   //starts of with device configuration. only one possible (table B1)
         USB_DEVICE_DESC_LEN, //the length of this report   ==0
         0x01, //the constant DEVICE (DEVICE 0x01)  ==1
         0x10,0x01, //usb version in bcd (pic167xx is 1.1) ==2,3
         0x00, //class code ==4
         0x00, //subclass code ==5
         0x00, //protocol code ==6
         USB_MAX_EP0_PACKET_LENGTH, //max packet size for endpoint 0. (SLOW SPEED SPECIFIES 8) ==7
         0xD8,0x04, //vendor id (0x04D8 is Microchip) =8,9
         0x10,0x00, //product id   ==10,11  //don't use ffff says usb-by-example guy.  oops
         0x00,0x01, //device release number  ==12,13
         0x01, //index of string description of manufacturer. therefore we point to string_1 array (see below)  ==14
         0x02, //index of string descriptor of the product  ==15
         0x00, //index of string descriptor of serial number  ==16
         USB_NUM_CONFIGURATIONS  //number of possible configurations  ==17
   };

   // *********************************** CONFIGURATION DESCRIPTORS ****************************************

   // MIDI Adapter Configuration Descriptor (table B2)
   // BYTE CONST USB_CONFIG_DESC[USB_TOTAL_CONFIG_LEN] = {
   BYTE CONST USB_CONFIG_DESC[USB_TOTAL_CONFIG_LEN] = {
   //IN ORDER TO COMPLY WITH WINDOWS HOSTS, THE ORDER OF THIS ARRAY MUST BE:
   //    config(s)
   //    interface(s)
   //    class(es)
   //    endpoint(s)

         //MIDI Adapter Configuration Descriptor (table B2)
         USB_CONFIG_DESC_LEN, //length of descriptor size          ==0
         USB_CONFIG_DESC_KEY, //constant CONFIGURATION (CONFIGURATION 0x02)     ==1
         USB_TOTAL_CONFIG_LEN,0, //size of all data returned for this config      ==2,3
         USB_NUM_INTERFACES, //number of interfaces this device supports       ==4
         0x01, //identifier for this configuration.  (IF we had more than one configurations)      ==5
         0x00, //index of string descriptor for this configuration      ==6
         0x80, //bit 6=1 if self powered, bit 5=1 if supports remote wakeup (we don't), bits 0-4 unused and bit7=1         ==8 // JDS
         150, //maximum bus power required (maximum milliamperes/2)  (0x32 = 100mA) /// JDS

  // *********************************** AUDIO CONTROL INTERFACE DESCRIPTORS ****************************************

         //MIDI Adapter Standard Audio Control Interface Descriptor (table B3)
         USB_STANDARD_AC_INTERFACE_DESC_LEN, //length of descriptor      =0
         USB_INTERFACE_DESC_KEY, //constant INTERFACE (INTERFACE 0x04)       =1
         0x00, //number defining this interface (IF we had more than one interface)    ==2
         0x00, //alternate setting     ==3
         0x00, //number of endpoins, except 0 (pic167xx has 3, but we dont have to use all).       ==4
         0x01, //class code, 01 = AUDIO     ==5
         0x01, //subclass code 01=AUDIO_CONTROL     ==6
         0x00, //protocol code      ==7
         0x00, //index of string descriptor for interface      ==8

               // The following was not defined by dbtech

         //MIDI Adapter Class-specific Audio Control Interface Descriptor (table B4)
         USB_CLASS_SPECIFIC_AC_INTERFACE_DESC_LEN, //length of descriptor      =0
         USB_CS_INTERFACE_DESC_KEY, //constant INTERFACE DESCRIPTOR (INTERFACE 0x24)       =1
         MS_HEADER, //number defining this interface (IF we had more than one interface)    =2
         0x00, //rev 1.0 =3
         0x01, //rev 1.0 =4
         0x09,0x00, //total size of specific descriptors VLR as shown in midi10.pdf
         0x01, //number of streaming interfaces      ==17
         0x01, //MIDIstreaming interface belong this audio interface      ==18

  // *********************************** MIDI STREAMING INTERFACE DESCRIPTORS ****************************************

         // MIDI Adapter Standard MS Interface Descriptor (table B5)
         USB_STANDARD_MS_INTERFACE_DESC_LEN, //length of descriptor      =10
         USB_INTERFACE_DESC_KEY, //constant INTERFACE (INTERFACE 0x04)       =11
         0x01, //number defining this interface (IF we had more than one interface)    ==12
         0x00, //alternate setting     ==13
         USB_NUM_ENDPOINTS, //number of endpoins, except 0 (pic167xx has 3, but we dont have to use all).       ==14
         0x01, //class code, 01 = AUDIO     ==15  // JDS
         0x03, //subclass code 03=MIDISTREAMING     ==16 // JDS
         0x00, //protocol code     ==17
         0x00, //index of string descriptor for interface      ==18


        //MIDI Adapter Class-specific MS Interface Descriptor (table B6)
        USB_CLASS_SPECIFIC_MS_INTERFACE_DESC_LEN, //length of descriptor      =10
        USB_CS_INTERFACE_DESC_KEY, //constant INTERFACE DESCRIPTOR (INTERFACE 0x24)       =11
        MS_HEADER, //number defining this interface (IF we had more than one interface)    ==12
        0x00, //rev 1.0
        0x01, //rev 1.0
        0x41,0x00, //total size of specific descriptors VLR as shown in midi10.pdf

        //MIDI Adapter MIDI IN Jack Descriptor (Embedded) (table B7)
        USB_MIDI_IN_EMBBEDED_DESC_LEN, //length of descriptor      =10
        USB_CS_INTERFACE_DESC_KEY, //constant INTERFACE DESCRIPTOR (INTERFACE 0x24)       =11
        0x02, //MIDI_IN_JACK subtype    ==12
        0x01, //EMBEDDED
        0x01, //ID of this jack
        0x00, //Unused

        //MIDI Adapter MIDI IN Jack Descriptor (External) (table B8)
        USB_MIDI_IN_EXTERNAL_DESC_LEN, //length of descriptor      =10
        USB_CS_INTERFACE_DESC_KEY, //constant INTERFACE DESCRIPTOR (INTERFACE 0x24)       =11
        0x02, //MIDI_IN_JACK subtype
        0x02, //EXTERNAL
        0x02, //ID of this jack
        0x00, //Unused

        //MIDI Adapter MIDI OUT Jack Descriptor (Embedded) (table B9)
        USB_MIDI_OUT_EMBEDDED_DESC_LEN, //length of descriptor      =10
        USB_CS_INTERFACE_DESC_KEY, //constant INTERFACE DESCRIPTOR (INTERFACE 0x24)       =11
        0x03, //MIDI_OUT_JACK subtype
        0x01, //EMBEDDED
        0x03, //ID of this Jack
        0x01, //Number of input pins of this jack
        0x02, //ID of the entity to which this Pin is connected
        0x01, //Output pin number of the Entity to which this Input pin is connected
        0x00, // unused

        //MIDI Adapter MIDI OUT Jack Descriptor (External) (table B10)
        USB_MIDI_OUT_EXTERNAL_DESC_LEN, //length of descriptor      =10
        USB_CS_INTERFACE_DESC_KEY, //constant INTERFACE DESCRIPTOR (INTERFACE 0x24)       =11
        0x03, //MIDI_OUT_JACK subtype
        0x02, //external
        0x04, //ID of this jack
        0x01, //number of input pins of this jack
        0x01, //ID of the Entity to which this pin is conneced
        0x01, //Output pin number of the Entity to which this Input pin is connected
        0x00, //unused


  // *********************************** ENDPOINTS DESCRIPTORS ****************************************

        //MIDI Adapter Standard Bulk OUT Endpoint Descriptor (table B11)
        USB_STANDARD_ENDPOINT_DESC_LEN, //length of descriptor                   ==28
        USB_ENDPOINT_DESC_KEY, //constant ENDPOINT (ENDPOINT 0x05)          ==29
        0x01, //endpoint number and direction (0x81 = EP1 IN)       ==30
        0x02, //transfer type supported (0x02 is bulk, 0x03 is interrupt)         ==31
        0x40,0x00, //maximum packet size supported (as in midi10.pdf)        ==32,33
        0x00, //polling interval, in ms.  (ignored for bulk - cant be smaller than 10 ?)      ==34
        0x00, //unused
        0x00, //unused

        //MIDI Adapter Class-specific Bulk OUT Endpoint Descriptor (table B12)
        USB_CLASS_SPECIFIC_ENDPOINT_DESC_LEN, //length of descriptor                   ==28
        USB_CS_ENDPOINT_DESC_KEY, //constant ENDPOINT (ENDPOINT 0x25)          ==29
        0x01, //MS General subtype
        0x01, //number of embedded MIDI IN Jacks
        0x01, //ID of the embedded MIDI Jack

        //MIDI Adapter Standard Bulk IN Endpoint Descriptor (table B13)
        USB_STANDARD_ENDPOINT_DESC_LEN, //length of descriptor                   ==28
        USB_ENDPOINT_DESC_KEY, //constant ENDPOINT (ENDPOINT 0x05)          ==29
        0x81, //endpoint number and direction (0x81 = EP1 IN)       ==30
        0x02, //transfer type supported (0x02 is bulk, 0x03 is interrupt)         ==31
        0x40,0x00, //maximum packet size supported (as in midi10.pdf)        ==32,33
        0x00, //polling interval, in ms.  (ignored for bulk - cant be smaller than 10 ?)      ==34
        0x00, //unused
        0x00, //unused

        //MIDI Adapter Class-specific Bulk IN Endpoint Descriptor (table B14)
        USB_CLASS_SPECIFIC_ENDPOINT_DESC_LEN, //length of descriptor                   ==28
        USB_CS_ENDPOINT_DESC_KEY, //constant ENDPOINT (ENDPOINT 0x25)          ==29
        0x01, //MS General subtype
        0x01, //number of embedded MIDI IN Jacks
        0x03, //ID of the embedded MIDI Jack

   };

//////////////////////////////////////////////////////////////////
///
///   start string descriptors
///   String 0 is a special language string, and must be defined.  People in U.S.A. can leave this alone.
///
///   You must define the length else get_next_string_character() will not see the string
///   Current code only supports 10 strings (0 thru 9)
///
//////////////////////////////////////////////////////////////////

#DEFINE USB_STRING_0_LEN 4
BYTE CONST USB_STRING_0[4] = {  //string index 0 is special
         USB_STRING_0_LEN, //length of string index
         0x03, //descriptor type 0x03 (STRING)
         0x09,0x04   //Microsoft Defined for US-English
};

#DEFINE USB_STRING_1_LEN 20
BYTE CONST USB_STRING_1[20] = { //we defined string index 1 earlier as manuf string
         USB_STRING_1_LEN, //length of string index
         0x03, //descriptor type 0x03 (STRING)
         'W',0,
         'a',0,
         'v',0,
         'e',0,
         ' ',0,
         'I',0,
         'd',0,
         'e',0,
         'a',0
};

#DEFINE USB_STRING_2_LEN 18
BYTE CONST USB_STRING_2[18] = {  //we defined string index 2 as description of prodcut
         USB_STRING_2_LEN, //length of string index
         0x03, //descriptor type 0x03 (STRING)
         'M',0,
         'I',0,
         'D',0,
         'I',0,
         ' ',0,
         'U',0,
         'S',0,
         'B',0
};
