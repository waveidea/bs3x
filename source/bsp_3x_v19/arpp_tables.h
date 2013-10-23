#ifndef ARPP_TABLES
#define ARPP_TABLES

rom uchar table_clock_resolution[ARP_CLK_RESO_NB] = {
arp_clradio_4th,     // nb clock ticks in one quarter
arp_clradio_8th,     // nb clock ticks in one 8th
arp_clradio_16th,    // nb clock ticks in one 16th
arp_clradio_32th,    //  nb clock ticks in one 32th
arp_clradio_64th,    // NOT REALLY nb clock ticks in one 64th
arp_clradio_trip1,   //  nb clock ticks in 8th triplets
arp_clradio_trip2,   //  nb clock ticks in 16th triplets
};

rom uchar melody[MAX_MELODIES+1][MAX_RANGES] =
{
	{0, 12, 24, 36, 48, 60, 72, 80,              0, 12, 24, 36, 48, 60, 72, 80},
    {0, 7,  12,  19, 24, 31, 43, 48,             0, 7,  12,  19, 24, 31, 43, 48},
	{0, 3,  7, 12, 15, 19, 24, 27,               0, 3,  7, 12, 15, 19, 24, 27},
	{0, 4,  7, 12, 16, 19, 24, 28,               0, 4,  7, 12, 16, 19, 24, 28},
	{0, 1,  4, 5, 8, 11, 12, 13,                 0, 1,  4, 5, 8, 11, 12, 13},
	{0, 2, 4, 5, 7, 9, 11, 12,                   0, 2, 4, 5, 7, 9, 11, 12},

 /* track 1 */ { 0,	7,	11,	12,	19,	23,	24,	31,	35,	36,	43,	47,	48,	55,	59,	60,	},
 /* track 2 */ { 0,	2,	7,	12,	14,	19,	24,	26,	31,	36,	38,	43,	48,	50,	55,	60,	},
 /* track 3 */ { 0,	7,	14,	16,	19,	24,	31,	38,	40,	43,	48,	55,	62,	64,	67,	67,	},
 /* track 4 */ { 0,	3,	5,	7,	10,	12,	15,	17,	19,	22,	24,	27,	29,	31,	34,	36,	},
 /* track 5 */ { 0,	2,	4,	7,	9,	12,	14,	16,	19,	21,	24,	26,	28,	31,	33,	36,	},
 /* track 6 */ { 0,	2,	3,	7,	12,	14,	15,	19,	24,	26,	27,	31,	36,	38,	39,	43,	},
 /* track 7 */ { 0,	3,	5,	7,	10,	12,	15,	17,	19,	22,	24,	27,	29,	31,	34,	36,	},
 /* track 8 */ { 0,	5,	7,	12,	17,	19,	24,	29,	31,	36,	41,	43,	48,	53,	55,	60,	},
 /* track 9 */ { 0,	5,	7,	10,	12,	17,	19,	22,	24,	29,	31,	34,	36,	41,	43,	46,	},
 /* track 10 */ { 0,	7,	14,	15,	19,	26,	27,	31,	36,	43,	50,	51,	55,	62,	63,	67,	},
 /* track 11 */ { 0,	7,	14,	15,	22,	29,	31,	34,	36,	43,	50,	51,	58,	65,	67,	58,	},
 /* track 12 */ { 0,	2,	5,	7,	12,	14,	17,	19,	24,	26,	29,	31,	36,	38,	41,	43,	},
 /* track 13 */ { 0,	4,	7,	11,	12,	16,	19,	23,	24,	28,	31,	35,	36,	40,	43,	47,	},
 /* track 14 */ { 0,	3,	0,	5,	0,	7,	0,	10,	0,	12,	0,	15,	0,	17,	0,	19,	},
 /* track 15 */ { 0,	3,	0,	7,	0,	12,	0,	15,	0,	19,	0,	24,	0,	27,	0,	31,	},
 /* track 16 */ { 0,	0,	3,	0,	0,	7,	0,	10,	0,	0,	12,	0,	0,	15,	0,	19,	},
  /* track 17 */ { 25,	26,	27,	28,	30,	31,	33,	35,	25,	26,	27,	28,	30,	31,	33,	35,	},
 /* track 18 */ { 37,	38,	39,	40,	41,	42,	44,	38,	37,	38,	39,	40,	41,	42,	44,	38,	},

 };
    /*
#define MAX_RHYTHMS     8

const long rhythm[MAX_RHYTHMS] =
{
  0xFFFF,   // ----
  0x7777,   // ---
  0xBBBB,   // -- -
  0x6D6D,   // - --
  0xF7F7,
  0xB7F7,
  0x76B7,
  0xFBF6
};





#define MAX_RHYTHMS	16

const long rhythm[MAX_RHYTHMS] = 
{ 
	0xFFFF,
	0xAEAD,
	0xBAEB,
	0xFBAF,
	0xC3C3,
	0xA7A7,
	0xBBBB,
	0xD34D,
	0xBD3D,
	0xDDDD,
	0xDEBD,
	0xB7EF,
	0xDEED,
	0xF7AF,
	0xE673,
	0x1919,

};


#define MAX_ACCENTS	16

const long accent[MAX_ACCENTS] =
{ 
	0x0000,
	0x5555,
	0xAAAA,
    0x0505,
	0x0A0A, 
	0xC3C3,
	0xA5A5,
	0xBBBB,
	0xD34D,
	0xBD3D,
	0xDEBD,
	0xB7EF,
	0xDEED,
	0xF7AF,
	0xE673,
	0x1919,
};

*/

#define MAX_RHYTHMS 512

rom long rhythm[MAX_RHYTHMS] = 
{

    // MASTER   missing   0xFFFFFFFF,		0x55555555,		0xAAAAAAAA,
 	0xFFFFFFFF,	 /* 32 bits */ 	0x3FDF7F73,	 /* 25 bits */ 	0xF1FCF1FC,	 /* 22 bits */ 	0x1DDDDDD9,	 /* 21 bits */ 	0x5B5B5B5B,	 /* 20 bits */ 	0x77763B33,	 /* 20 bits */ 	0xDAD6DAD6,	 /* 20 bits */ 	0xDAD6DAD6,	 /* 20 bits */ 
	0xF454CFF8,	 /* 19 bits */ 	0x5B5B59B9,	 /* 19 bits */ 	0x5AD6DAD6,	 /* 19 bits */ 	0xB333B333,	 /* 18 bits */ 	0x12DADB6D,	 /* 18 bits */ 	0xCD99CD99,	 /* 18 bits */ 	0x1AD6B6D9,	 /* 18 bits */ 	0xAB59AB59,	 /* 18 bits */ 
	0xAB59AB59,	 /* 18 bits */ 	0xCCD9CCD9,	 /* 18 bits */ 	0xEA9554B5,	 /* 17 bits */ 	0x19E9E91B,	 /* 17 bits */ 	0xD345D345,	 /* 16 bits */ 	0xD545D545,	 /* 16 bits */ 	0xD351D351,	 /* 16 bits */ 	0x99999999,	 /* 16 bits */ 
	0x96599659,	 /* 16 bits */ 	0x36526DA5,	 /* 16 bits */ 	0x75297529,	 /* 16 bits */ 	0x897B1B13,	 /* 16 bits */ 	0x2FDD8848,	 /* 15 bits */ 	0x59650D56,	 /* 15 bits */ 	0x19F8B152,	 /* 15 bits */ 	0x64D4E564,	 /* 15 bits */ 
	0xD256D245,	 /* 15 bits */ 	0xFB892129,	 /* 15 bits */ 	0xAA54AA54,	 /* 14 bits */ 	0x34C34574,	 /* 14 bits */ 	0xF4253451,	 /* 14 bits */ 	0x75287528,	 /* 14 bits */ 	0x35293529,	 /* 14 bits */ 	0x2AA525A5,	 /* 14 bits */ 
	0x18608AF7,	 /* 14 bits */ 	0x4A5294A5,	 /* 13 bits */ 	0x112599B1,	 /* 13 bits */ 	0x94A514A5,	 /* 13 bits */ 	0x01C9C9C9,	 /* 13 bits */ 	0x25A524A5,	 /* 13 bits */ 	0xC585B111,	 /* 13 bits */ 	0xA52950C3,	 /* 13 bits */ 
	0x83330333,	 /* 13 bits */ 	0x70C34925,	 /* 13 bits */ 	0x113054DB,	 /* 13 bits */ 	0xA9494949,	 /* 13 bits */ 	0x7DC90011,	 /* 12 bits */ 	0x0D38A885,	 /* 12 bits */ 	0x63CE2408,	 /* 12 bits */ 	0x49494949,	 /* 12 bits */ 
	0x49494929,	 /* 12 bits */ 	0x24A513A4,	 /* 12 bits */ 	0x25A52425,	 /* 12 bits */ 	0x13131313,	 /* 12 bits */ 	0x17454445,	 /* 12 bits */ 	0x4A953412,	 /* 12 bits */ 	0x3D2C8086,	 /* 12 bits */ 	0x1AAB1111,	 /* 12 bits */ 
	0x54705294,	 /* 12 bits */ 	0x24003FC4,	 /* 11 bits */ 	0x28895622,	 /* 11 bits */ 	0x38142839,	 /* 11 bits */ 	0x07041589,	 /* 10 bits */ 	0x1AAB0009,	 /* 10 bits */ 	0x43443051,	 /* 10 bits */ 	0xA2608268,	 /* 10 bits */ 
	0x18044CCC,	 /* 10 bits */ 	0x414B1051,	 /* 10 bits */ 	0x05130513,	 /* 10 bits */ 	0x7040D094,	 /* 10 bits */ 	0x13131310,	 /* 10 bits */ 	0x45294105,	 /* 10 bits */ 	0x033304C1,	 /* 10 bits */ 	0x54514411,	 /* 10 bits */ 
	0x003B003B,	 /* 10 bits */ 	0x8D208282,	 /* 09 bits */ 	0x42424243,	 /* 09 bits */ 	0x41041127,	 /* 09 bits */ 	0x00C33051,	 /* 09 bits */ 	0x4240C0C3,	 /* 09 bits */ 	0x31250501,	 /* 09 bits */ 	0x51441051,	 /* 09 bits */ 
	0x20982511,	 /* 09 bits */ 	0x45145041,	 /* 09 bits */ 	0x42424254,	 /* 09 bits */ 	0x11451441,	 /* 09 bits */ 	0x00494949,	 /* 09 bits */ 	0x49491011,	 /* 09 bits */ 	0x19041029,	 /* 08 bits */ 	0x14890809,	 /* 08 bits */ 
	0x34124041,	 /* 08 bits */ 	0x14A08209,	 /* 08 bits */ 	0x61241024,	 /* 08 bits */ 	0x34530001,	 /* 08 bits */ 	0xB1003104,	 /* 08 bits */ 	0x14C12401,	 /* 08 bits */ 	0x15041848,	 /* 08 bits */ 	0x040B040B,	 /* 08 bits */ 
	0x14451041,	 /* 08 bits */ 	0x44114411,	 /* 08 bits */ 	0x4808C848,	 /* 08 bits */ 	0x12081488,	 /* 07 bits */ 	0x50091009,	 /* 07 bits */ 	0x10510105,	 /* 07 bits */ 	0x50C50001,	 /* 07 bits */ 	0x40490490,	 /* 07 bits */ 
	0x4041C104,	 /* 07 bits */ 	0x22224404,	 /* 07 bits */ 	0x02DB0000,	 /* 07 bits */ 	0x01120112,	 /* 06 bits */ 	0x82060202,	 /* 06 bits */ 	0x50401104,	 /* 06 bits */ 	0x040A040A,	 /* 06 bits */ 	0x14880808,	 /* 06 bits */ 
	0x08480848,	 /* 06 bits */ 	0x14404040,	 /* 05 bits */ 	0x10241020,	 /* 05 bits */ 	0x08080809,	 /* 05 bits */ 	0x040C0404,	 /* 05 bits */ 	0x20108404,	 /* 05 bits */ 	0x10081008,	 /* 04 bits */ 	0x50410000,	 /* 04 bits */ 

    // HOUSE
 	0xFFFFFFFF,	 /* 32 bits */ 	0xDFDFDFDF,	 /* 28 bits */ 	0xDFDFDDD7,	 /* 26 bits */ 	0xDFDF9DDF,	 /* 26 bits */ 	0xDFE5FBB7,	 /* 25 bits */ 	0xCFDFCBDE,	 /* 24 bits */ 	0xFD55FD3F,	 /* 24 bits */ 	0xFBEDB5F5,	 /* 24 bits */ 
	0xFBFA3F55,	 /* 23 bits */ 	0x7E767E77,	 /* 23 bits */ 	0x7E7E7E3D,	 /* 23 bits */ 	0xFCFCECFC,	 /* 23 bits */ 	0xDCECFCFD,	 /* 23 bits */ 	0x7776FCDC,	 /* 22 bits */ 	0xDD5F93D7,	 /* 22 bits */ 	0xF2FACCDB,	 /* 21 bits */ 
	0x7CB6DF6C,	 /* 21 bits */ 	0x8BF9CBF3,	 /* 21 bits */ 	0x0FFC0DFF,	 /* 21 bits */ 	0x7E647E7D,	 /* 21 bits */ 	0x1E7E1EFE,	 /* 21 bits */ 	0x0FDA0FDF,	 /* 20 bits */ 	0x0DFC0DFF,	 /* 20 bits */ 	0x0BF98BFB,	 /* 20 bits */ 
	0xE55F1FAA,	 /* 20 bits */ 	0x36E7DA6D,	 /* 20 bits */ 	0xA7DA1FAB,	 /* 20 bits */ 	0x7276577E,	 /* 20 bits */ 	0xFCCCACFC,	 /* 20 bits */ 	0x0FFC05FF,	 /* 20 bits */ 	0xAABFA0F7,	 /* 20 bits */ 	0x9995F95F,	 /* 20 bits */ 
	0xDAFC9AAB,	 /* 20 bits */ 	0xB6DA0CFB,	 /* 19 bits */ 	0x1EDE5676,	 /* 19 bits */ 	0xA1FA3FA3,	 /* 19 bits */ 	0x1D9E2DDD,	 /* 19 bits */ 	0x1AFD1AF9,	 /* 19 bits */ 	0x18FD1AFD,	 /* 19 bits */ 	0x1AFC1AFD,	 /* 19 bits */ 
	0x1EF41EF4,	 /* 18 bits */ 	0x4ED2EB33,	 /* 18 bits */ 	0x15B58EDB,	 /* 18 bits */ 	0x5E17176B,	 /* 18 bits */ 	0xF7031B6D,	 /* 18 bits */ 	0x12BD1AFD,	 /* 18 bits */ 	0xEF031B6D,	 /* 18 bits */ 	0xED68AD71,	 /* 18 bits */ 
	0xDA51DED1,	 /* 18 bits */ 	0x16B6B6B6,	 /* 18 bits */ 	0xB6B2CB2D,	 /* 18 bits */ 	0x5CECACCC,	 /* 17 bits */ 	0xCCCD3999,	 /* 17 bits */ 	0xDF00FE0E,	 /* 17 bits */ 	0xF2ABA465,	 /* 17 bits */ 	0x4AF98AAD,	 /* 17 bits */ 
	0x13FB039B,	 /* 17 bits */ 	0x4B16B6B6,	 /* 17 bits */ 	0x4D356CB5,	 /* 17 bits */ 	0x83BA3E83,	 /* 16 bits */ 	0xD9C3C05B,	 /* 16 bits */ 	0xAAD1CD70,	 /* 16 bits */ 	0x534FC899,	 /* 16 bits */ 	0x9149FC69,	 /* 16 bits */ 
	0x0568FD70,	 /* 15 bits */ 	0xB0ED036C,	 /* 15 bits */ 	0x00160FFF,	 /* 15 bits */ 	0x15DD01CB,	 /* 15 bits */ 	0x5C495D49,	 /* 15 bits */ 	0x116B6B49,	 /* 15 bits */ 	0x152DB24B,	 /* 15 bits */ 	0x50575553,	 /* 15 bits */ 
	0x01EF0FA1,	 /* 15 bits */ 	0xE011DEA1,	 /* 14 bits */ 	0x1492F849,	 /* 14 bits */ 	0x01B416D7,	 /* 14 bits */ 	0x5116B568,	 /* 14 bits */ 	0x10D315D3,	 /* 14 bits */ 	0x66662664,	 /* 14 bits */ 	0x4CC94CC9,	 /* 14 bits */ 
	0xCCCCAC88,	 /* 14 bits */ 	0x36D13A08,	 /* 13 bits */ 	0x1C1C1C1D,	 /* 13 bits */ 	0xCC8D3888,	 /* 13 bits */ 	0x4C495C49,	 /* 13 bits */ 	0x15244B69,	 /* 13 bits */ 	0xC069A869,	 /* 13 bits */ 	0x30290337,	 /* 12 bits */ 
	0x482C1696,	 /* 12 bits */ 	0x1124B24B,	 /* 12 bits */ 	0x64964912,	 /* 12 bits */ 	0x96492449,	 /* 12 bits */ 	0x88CCC007,	 /* 11 bits */ 	0x1A1A1A22,	 /* 11 bits */ 	0x0000CAFB,	 /* 11 bits */ 	0x1C1C181C,	 /* 11 bits */ 
	0x0FDA8080,	 /* 11 bits */ 	0x30250317,	 /* 11 bits */ 	0x28530C52,	 /* 11 bits */ 	0x33102315,	 /* 11 bits */ 	0x1B0B0B10,	 /* 11 bits */ 	0x14C91053,	 /* 11 bits */ 	0xB4253048,	 /* 11 bits */ 	0x040218FD,	 /* 11 bits */ 
	0xE2A0022D,	 /* 11 bits */ 	0x00155515,	 /* 10 bits */ 	0x0012D714,	 /* 10 bits */ 	0xA5003690,	 /* 10 bits */ 	0x10961096,	 /* 10 bits */ 	0x0012D714,	 /* 10 bits */ 	0x88490469,	 /* 10 bits */ 	0x0010B714,	 /* 09 bits */ 
	0x000B0B0B,	 /* 09 bits */ 	0x12131212,	 /* 09 bits */ 	0x30253041,	 /* 09 bits */ 	0x0002D714,	 /* 09 bits */ 	0x22142214,	 /* 08 bits */ 	0x30420C42,	 /* 08 bits */ 	0x00236448,	 /* 08 bits */ 	0x11111111,	 /* 08 bits */ 
	0x00149514,	 /* 08 bits */ 	0xC00002EA,	 /* 08 bits */ 	0x001B014C,	 /* 08 bits */ 	0x15140041,	 /* 07 bits */ 	0x22102211,	 /* 07 bits */ 	0x15140041,	 /* 07 bits */ 	0x00941404,	 /* 06 bits */ 	0x00140041,	 /* 04 bits */ 

    // BREAKS
   	0xFFFFFFFF,	 /* 32 bits */ 	0xFEEBEBE9,	 /* 24 bits */ 	0xB47EB6FE,	 /* 22 bits */ 	0x7F3A359F,	 /* 21 bits */ 	0x7D757575,	 /* 21 bits */ 	0xDC4DEAEF,	 /* 21 bits */ 	0xDE799E79,	 /* 21 bits */ 	0xEAEEE8EE,	 /* 21 bits */ 
	0xEF6DD279,	 /* 21 bits */ 	0xDF579E39,	 /* 21 bits */ 	0xFE799E59,	 /* 21 bits */ 	0x2F3F2D3F,	 /* 21 bits */ 	0x57DA3F75,	 /* 21 bits */ 	0x7D757D51,	 /* 20 bits */ 	0x6E787E6E,	 /* 20 bits */ 	0x767E52DB,	 /* 20 bits */ 
	0x5E7D8E79,	 /* 20 bits */ 	0xCBAE2E3F,	 /* 20 bits */ 	0x7E4EFC46,	 /* 19 bits */ 	0x9E798E79,	 /* 19 bits */ 	0x9AAAB577,	 /* 19 bits */ 	0x999B9E78,	 /* 18 bits */ 	0x96799679,	 /* 18 bits */ 	0xCACBF82D,	 /* 18 bits */ 
	0xACAFF82C,	 /* 18 bits */ 	0xCAFEF828,	 /* 18 bits */ 	0x8787E0EF,	 /* 18 bits */ 	0xF571F02D,	 /* 18 bits */ 	0x5455EAEE,	 /* 18 bits */ 	0x97959595,	 /* 17 bits */ 	0xBE287E68,	 /* 17 bits */ 	0x95399679,	 /* 17 bits */ 
	0xA89FFC28,	 /* 17 bits */ 	0xCABBF828,	 /* 17 bits */ 	0xA8AFF80D,	 /* 17 bits */ 	0x75557554,	 /* 17 bits */ 	0x6A8BC16F,	 /* 17 bits */ 	0x9A2AAAF2,	 /* 16 bits */ 	0xE5E1B02B,	 /* 16 bits */ 	0x5B4B6097,	 /* 16 bits */ 
	0x8ABDF828,	 /* 16 bits */ 	0xCA379E28,	 /* 16 bits */ 	0xB561F02D,	 /* 16 bits */ 	0x059515DF,	 /* 16 bits */ 	0x4555F545,	 /* 16 bits */ 	0xDD0DF029,	 /* 16 bits */ 	0xA66ACAAA,	 /* 16 bits */ 	0x6EE1A05B,	 /* 16 bits */ 
	0x178780BF,	 /* 16 bits */ 	0x1AEA9A2E,	 /* 16 bits */ 	0x6CB28A2D,	 /* 15 bits */ 	0x2A2E3E16,	 /* 15 bits */ 	0x9585C0B7,	 /* 15 bits */ 	0x6A8D816D,	 /* 15 bits */ 	0x2B8DC0A7,	 /* 15 bits */ 	0x0B7D20D3,	 /* 15 bits */ 
	0xE550F825,	 /* 15 bits */ 	0x92959695,	 /* 15 bits */ 	0x96919691,	 /* 15 bits */ 	0x35647155,	 /* 15 bits */ 	0x6AA86AE8,	 /* 15 bits */ 	0xA82DF828,	 /* 14 bits */ 	0x9183C0B7,	 /* 14 bits */ 	0x8A8BF828,	 /* 14 bits */ 
	0x14CAF829,	 /* 14 bits */ 	0x0AE86AE8,	 /* 14 bits */ 	0x94159695,	 /* 14 bits */ 	0x6AAA8A8A,	 /* 14 bits */ 	0xCBC3E080,	 /* 13 bits */ 	0x7C04FC04,	 /* 13 bits */ 	0x2E2A3514,	 /* 13 bits */ 	0x4A15782C,	 /* 13 bits */ 
	0xA4909695,	 /* 13 bits */ 	0x94519651,	 /* 13 bits */ 	0x92119695,	 /* 13 bits */ 	0x96596221,	 /* 13 bits */ 	0x96119691,	 /* 13 bits */ 	0x001ACF6A,	 /* 13 bits */ 	0x8B112965,	 /* 13 bits */ 	0x11C19195,	 /* 12 bits */ 
	0x74506491,	 /* 12 bits */ 	0x96119491,	 /* 12 bits */ 	0x94511551,	 /* 12 bits */ 	0x92D092D0,	 /* 12 bits */ 	0x96119611,	 /* 12 bits */ 	0x96919411,	 /* 12 bits */ 	0x92989298,	 /* 12 bits */ 	0x96119251,	 /* 12 bits */ 
	0x92989099,	 /* 12 bits */ 	0x94419451,	 /* 12 bits */ 	0x92989298,	 /* 12 bits */ 	0x92C0D2D0,	 /* 12 bits */ 	0x9492A851,	 /* 12 bits */ 	0x0690A691,	 /* 11 bits */ 	0x94119491,	 /* 11 bits */ 	0x94509298,	 /* 11 bits */ 
	0x9290A690,	 /* 11 bits */ 	0xA451A450,	 /* 11 bits */ 	0xE401B013,	 /* 11 bits */ 	0xD5140051,	 /* 10 bits */ 	0x95940051,	 /* 10 bits */ 	0x92909290,	 /* 10 bits */ 	0x928092B0,	 /* 10 bits */ 	0x92909290,	 /* 10 bits */ 
	0xE8086888,	 /* 10 bits */ 	0x92901291,	 /* 10 bits */ 	0x92909290,	 /* 10 bits */ 	0x011515D0,	 /* 10 bits */ 	0x0000ED3C,	 /* 10 bits */ 	0x00158455,	 /* 09 bits */ 	0x15940051,	 /* 09 bits */ 	0x92909210,	 /* 09 bits */ 
	0x92900690,	 /* 09 bits */ 	0x95940041,	 /* 09 bits */ 	0x15900051,	 /* 08 bits */ 	0x80011515,	 /* 08 bits */ 	0x92900091,	 /* 08 bits */ 	0x0018A690,	 /* 08 bits */ 	0x14940051,	 /* 08 bits */ 	0x15900051,	 /* 08 bits */ 
	0x15140051,	 /* 08 bits */ 	0x15140051,	 /* 08 bits */ 	0x8A920009,	 /* 08 bits */ 	0x001B0045,	 /* 07 bits */ 	0x88088888,	 /* 07 bits */ 	0x001B0124,	 /* 07 bits */ 	0x000B00A4,	 /* 06 bits */ 	0xC000001A,	 /* 05 bits */ 

    // LATIN
 	0xFFFFFFFF,	 /* 32 bits */ 	0xBEBDFEBD,	 /* 25 bits */ 	0xF6F5FEF5,	 /* 25 bits */ 	0xFEF1FEF5,	 /* 25 bits */ 	0xFCF5FEF5,	 /* 25 bits */ 	0xFEFBDC7B,	 /* 25 bits */ 	0xFEB5BCF7,	 /* 24 bits */ 	0x7C7EFEF9,	 /* 24 bits */ 
	0xB7FBDDD9,	 /* 24 bits */ 	0xFCBDBEBD,	 /* 24 bits */ 	0xBEBDBEBD,	 /* 24 bits */ 	0xEEDDBE9D,	 /* 23 bits */ 	0xBAF3FE9E,	 /* 23 bits */ 	0xFEB7ECF4,	 /* 23 bits */ 	0xFCF5FE95,	 /* 23 bits */ 	0xBEBD3EBD,	 /* 23 bits */ 
	0xD2FFA5DB,	 /* 22 bits */ 	0xEDB2FCF5,	 /* 22 bits */ 	0xFDD5ED35,	 /* 22 bits */ 	0x7BEDD26F,	 /* 22 bits */ 	0x7EBAD6F9,	 /* 22 bits */ 	0x8CF3BF77,	 /* 22 bits */ 	0xCDDDBAF5,	 /* 22 bits */ 	0x6DB4EDF5,	 /* 21 bits */ 
	0x9F6EEE99,	 /* 21 bits */ 	0xCDB5E5B5,	 /* 20 bits */ 	0xE9EAED35,	 /* 20 bits */ 	0x69DC9A5F,	 /* 19 bits */ 	0x5FB59C3A,	 /* 19 bits */ 	0x2D35D6DD,	 /* 19 bits */ 	0x0DB5EDB5,	 /* 19 bits */ 	0xBA5D6EC5,	 /* 19 bits */ 
	0xADB5ADA5,	 /* 19 bits */ 	0xADB5AD35,	 /* 19 bits */ 	0xADA5ADB5,	 /* 19 bits */ 	0xADB5A9B5,	 /* 19 bits */ 	0xADB5ADB1,	 /* 19 bits */ 	0xADB5ADB1,	 /* 19 bits */ 	0xADB5AD95,	 /* 19 bits */ 	0xA9B5ADB5,	 /* 19 bits */ 
	0x95336B7D,	 /* 19 bits */ 	0x7366675D,	 /* 19 bits */ 	0xF974F651,	 /* 19 bits */ 	0x5B355B35,	 /* 18 bits */ 	0x156D975B,	 /* 18 bits */ 	0x6935DB4D,	 /* 18 bits */ 	0xADB529B5,	 /* 18 bits */ 	0x6D4BE6D2,	 /* 18 bits */ 
	0xEB8B8BCC,	 /* 18 bits */ 	0xDA8CDE9C,	 /* 18 bits */ 	0x5F345D35,	 /* 18 bits */ 	0x9CD49C9E,	 /* 17 bits */ 	0x92F36AAA,	 /* 17 bits */ 	0xD935368D,	 /* 17 bits */ 	0x001DFEBD,	 /* 17 bits */ 	0xAD358D95,	 /* 17 bits */ 
	0x6D35AD25,	 /* 17 bits */ 	0x71B57925,	 /* 17 bits */ 	0xA935A9B5,	 /* 17 bits */ 	0xAD34AD35,	 /* 17 bits */ 	0xADB1AD94,	 /* 17 bits */ 	0xADB4A995,	 /* 17 bits */ 	0x8AA5DCD5,	 /* 17 bits */ 	0x00EB6D37,	 /* 16 bits */ 
	0x53355135,	 /* 16 bits */ 	0xC8D85CD9,	 /* 16 bits */ 	0x9A6355AA,	 /* 16 bits */ 	0x1AB65E8C,	 /* 16 bits */ 	0xB871B869,	 /* 16 bits */ 	0x01B5ADB5,	 /* 16 bits */ 	0x52355B35,	 /* 16 bits */ 	0xA9B5A915,	 /* 16 bits */ 
	0xA915AD95,	 /* 16 bits */ 	0x0007FCFD,	 /* 16 bits */ 	0x5F3A944A,	 /* 16 bits */ 	0x36317366,	 /* 16 bits */ 	0xD85C7874,	 /* 16 bits */ 	0x0D66B535,	 /* 16 bits */ 	0xAA8D62A3,	 /* 15 bits */ 	0xD58AA562,	 /* 15 bits */ 
	0xAD952925,	 /* 15 bits */ 	0x668EA88E,	 /* 15 bits */ 	0x51355336,	 /* 15 bits */ 	0x9E98D498,	 /* 15 bits */ 	0xD49CD249,	 /* 15 bits */ 	0x44CEC2CD,	 /* 15 bits */ 	0x36DCA642,	 /* 15 bits */ 	0xCCCCC8CC,	 /* 15 bits */ 
	0xAD145C95,	 /* 15 bits */ 	0x66651C4D,	 /* 15 bits */ 	0x2935A9A5,	 /* 15 bits */ 	0xA99AA89C,	 /* 15 bits */ 	0x00B5ADB5,	 /* 15 bits */ 	0x098933B5,	 /* 14 bits */ 	0x75321631,	 /* 14 bits */ 	0x8DD8A930,	 /* 14 bits */ 
	0x469C449D,	 /* 14 bits */ 	0x01FD0175,	 /* 14 bits */ 	0xEA4C524C,	 /* 14 bits */ 	0x188E669C,	 /* 14 bits */ 	0x168CA49C,	 /* 13 bits */ 	0xA934A905,	 /* 13 bits */ 	0x0002EEED,	 /* 13 bits */ 	0x44A35239,	 /* 13 bits */ 
	0x94D22925,	 /* 13 bits */ 	0x3C791061,	 /* 13 bits */ 	0xA934A924,	 /* 13 bits */ 	0xC2192C99,	 /* 13 bits */ 	0xC94B8898,	 /* 13 bits */ 	0x29A521A5,	 /* 13 bits */ 	0xB2499249,	 /* 13 bits */ 	0x0001FE75,	 /* 13 bits */ 
	0x09A548E5,	 /* 13 bits */ 	0x7246609A,	 /* 13 bits */ 	0x18653C70,	 /* 13 bits */ 	0x4C98C899,	 /* 13 bits */ 	0x30711D61,	 /* 13 bits */ 	0x44C1C8C9,	 /* 12 bits */ 	0x1C613870,	 /* 12 bits */ 	0x52499249,	 /* 12 bits */ 
	0x2D252924,	 /* 12 bits */ 	0x6F358001,	 /* 12 bits */ 	0x209D8291,	 /* 11 bits */ 	0x675D0001,	 /* 11 bits */ 	0x89014C4C,	 /* 10 bits */ 	0x0910998C,	 /* 10 bits */ 	0x1A411248,	 /* 09 bits */ 	0x000C4916,	 /* 08 bits */ 
};

#endif
