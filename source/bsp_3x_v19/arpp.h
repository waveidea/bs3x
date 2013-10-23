#ifndef ARPP
#define ARPP

#define MAX_KEYBOARD_NOTES 16

/*******************************************************************************
*   Clock resolutions
*******************************************************************************/
#define arp_clradio_4th    24 // nb clock ticks in one quarter
#define arp_clradio_8th    12 // nb clock ticks in one 8th
#define arp_clradio_16th   6  // nb clock ticks in one 16th
#define arp_clradio_32th   3 //  nb clock ticks in one 32th
#define arp_clradio_64th   2 // NOT REALLY nb clock ticks in one 64th
#define arp_clradio_trip1  8 //  nb clock ticks in 8th triplets
#define arp_clradio_trip2  4 //  nb clock ticks in 16th triplets

#define ARP_CLK_RESO_NB				7
#define ARP_DEFAULT_CLK_RESO_INDEX	2

/*******************************************************************************
*   Bank offsets
*******************************************************************************/
#define BANK_SIZE     128

enum { MELODYBK0, MELODYBK1, MELODYBK2, MELODYBK3 };
enum { RHYTHMBK0, RHYTHMBK1, RHYTHMBK2, RHYTHMBK3 };
enum { ACCENTBK0, ACCENTBK1, ACCENTBK2, ACCENTBK3 };

#define ARP_MELODY_BNK_NB	1
#define ARP_RHYTHM_BNK_NB	4
#define ARP_ACCENT_BNK_NB	4

/*******************************************************************************
*   Directions
*******************************************************************************/
#define DIR_UP         0U
#define DIR_DOWN       1U
#define DIR_UPDOWN     2U
#define DIR_RANDOM     3U
#define DIR_CUSTOM1    4U
#define DIR_CUSTOM2    5U

#define ARP_DIR_MAX	   6U

/*******************************************************************************
*   Arp melodies
*******************************************************************************/
#define MAX_MELODIES    	24U
#define MAX_RANGES       	16U
#define DEFAULT_TRANSPOSE	63U
#define DEFAULT_SWING		0 // 63U
#define DEFAULT_VELOCITY	64U
#define NOTE_UNDEF			0xFF
#define DEFAULT_RANGE		3U // V1.8

enum {ARP_MODE_NORMAL, ARP_MODE_HOLD , ARP_MODE_AUTOTRIG, ARP_MODE_KEYBOARD };
#define ARPP_MODE_NB		4

// PROTOTYPES
void InitArpeggiator(void);
void CheckArpeggiator(uchar status_received);
void set_resolution(void);
//void set_melody_bk(void);
//void set_rhythm_bk(void);
//void set_accent_bk(void);
//void set_melody(void);
void set_rasnapkeysnge(void);
//void set_gate(void);
//void set_accent(void);
//void set_velocity(void);
//void set_transpose(void);
void set_swing(void);
void set_direction(void);
void set_midich(void);
//void set_hold(void);
//void set_autotrig(void);  
//void set_keyboard_notes(void);
//void set_rhythm(void);
void play_note(unsigned char rhythm_idx);
void mute_note(void);

void remove_keyboard_refnote(unsigned char note);
void add_keyboard_refnote(unsigned char note);

uchar GetRandomRanged(uchar range);
void CheckRealtimeArpeggio(uchar realtime_value);
uchar Truncate_Subst_1_127(uchar a, uchar b);
void set_mode(void);
void set_swing(void);

#endif
