#ifndef AUTOMATION
#define AUTOMATION

enum {AUTOM_SW_TYPE, AUTOM_SW_SEL, AUTOM_SW_ON};
// enum {AUTOMATION_MOTION, AUTOMATION_ARPP, AUTOMATION_LFO, AUTOMATION_ARPP_SHIFT}
enum {AUTOMATION_MOTION, AUTOMATION_LFO, AUTOMATION_ARPP, AUTOMATION_ARPP_SHIFT};

enum { PHYSICAL_MOTION_PART, PHYSICAL_MOTION_LENGTH, DUMMY, 
	   PHYSICAL_LFO_AMPL, PHYSICAL_LFO_FREQ, PHYSICAL_LFO_OFFSET,
	   PHYSICAL_ARP_GATE, PHYSICAL_ARP_ACCENT, PHYSICAL_ARP_RYTHM,
 //	   PHYSICAL_LFO_AMPL, PHYSICAL_LFO_FREQ, PHYSICAL_LFO_OFFSET,
	   PHYSICAL_ARP_RANGE, PHYSICAL_ARP_TRANSPOSE, PHYSICAL_ARP_MELODY
	   }; 

#define AUTOMATION_NB				   3U	// lfo, motion, arpp
#define AUTOMATION_POTS_NB			   3U	// 3 pots dedicated to automations

// Automation switches behaviour
// current automation 	|	automation sel sw	|	automation on sw	| automation type sw
//		MOTION			|	updatecurrent autom	|		turn off/on		| start play/record
//		 ARPP			|	updatecurrent autom	|		turn off/on		|  change arp dir						
//		 LFO			|	updatecurrent autom	|		turn off/on		|  change waveform

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckAutomationAnalog(void);
void CheckAutomationSw(void);
void UpdateAutomationLeds(void);
void AutomationSwTypePressed(void);
void AutomationSwSelPressed(void);
void AutomationSwOnOffPressed(void);
void UnhookAutomationPots(uchar old_automation, uchar new_automation);

#endif
