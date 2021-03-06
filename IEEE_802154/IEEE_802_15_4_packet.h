/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

*****************************************************************/

#ifndef __IEEE_802_15_4_H
#define __IEEE_802_15_4_H
#include <stm32w108xx.h>
#include "phy-library.h"


#ifdef __cplusplus
 extern "C" {
#endif
	 

/****************************IPHC Frame Format*******************************
			 0                                       1
       0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     | 0 | 1 | 1 |TC |FL |NH | HLIM  |CID|SAC|  SAM  | M |DAC|  DAM  |
     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*****************************************************************************/

/* IPHC |= field;
* Values of fields within the IPHC encoding first byte
* (C stands for compressed and I for inline)
*/
#define LOWPANv6_IPHC_FL_C                        0x10
#define LOWPANv6_IPHC_TC_C                        0x08
#define LOWPANv6_IPHC_NH_C                        0x04
#define LOWPANv6_IPHC_HLIM_1                      0x01
#define LOWPANv6_IPHC_HLIM_64                     0x02
#define LOWPANv6_IPHC_HLIM_255                    0x03
#define LOWPANv6_IPHC_HLIM_I                      0x00

/* Values of fields within the IPHC encoding second byte */
#define LOWPANv6_IPHC_CID                         0x80

#define LOWPANv6_IPHC_SAC                         0x40
#define LOWPANv6_IPHC_SAM_00                      0x00
#define LOWPANv6_IPHC_SAM_01                      0x10
#define LOWPANv6_IPHC_SAM_10                      0x20
#define LOWPANv6_IPHC_SAM_11                      0x30

#define LOWPANv6_IPHC_SAM_BIT                     4

#define LOWPANv6_IPHC_M                           0x08
#define LOWPANv6_IPHC_DAC                         0x04
#define LOWPANv6_IPHC_DAM_00                      0x00
#define LOWPANv6_IPHC_DAM_01                      0x01
#define LOWPANv6_IPHC_DAM_10                      0x02
#define LOWPANv6_IPHC_DAM_11                      0x03

#define LOWPANv6_IPHC_DAM_BIT                     0

/**
* \name LOWPAN_UDP encoding (works together with IPHC)
* @{
*/
#define LOWPANv6_NHC_UDP_MASK                     0xF8
#define LOWPANv6_NHC_UDP_ID                       0xF0
#define LOWPANv6_NHC_UDP_CHECKSUMC                0x04
#define LOWPANv6_NHC_UDP_CHECKSUMI                0x00
/* values for port compression, _with checksum_ ie bit 5 set to 0 */
#define LOWPANv6_NHC_UDP_CS_P_00									0xF0 /* all inline */
#define LOWPANv6_NHC_UDP_CS_P_01 								  0xF1 /* source 16bit inline, dest = 0xF0 + 8 bit inline */
#define LOWPANv6_NHC_UDP_CS_P_10 								  0xF2 /* source = 0xF0 + 8bit inline, dest = 16 bit inline */
#define LOWPANv6_NHC_UDP_CS_P_11								  0xF3 /* source & dest = 0xF0B + 4bit inline */
/** @} */

#define ICMP_type_ns 135
#define ICMP_type_na 136
#define ICMP_type_rs 133
#define ICMP_type_ra 134



typedef struct{
	uint16_t IPHC;    // = TF(11)|NH()|HLIM()|CID()|SAC()|SAM()|M()|DAC()|DAM();
}IP_hdr;

/*
typedef struct{
	uint8_t frametype;
	uint8_t sec_en;
	uint8_t frame_pending;
	uint8_t ack_req;
	uint8_t intra_PAN;
	uint8_t Dest_Add_Mode;
	uint8_t Src_Add_Mode;
}Frame_Control_Field;
*/

typedef struct{
	uint8_t PacketLength;
	uint16_t FCF;    	// = {0x61,0x08};  Data frame, Ack Req, Intra-PAN, 16-bit Destination Address enabled; rest disabled.
	uint8_t SeqNo;			// = 0x00;
	uint16_t DestPANID;
	/* 2 CRC bytes are generated by hardware. These have to be included in the PacketLength */
}IEEE_hdr;


typedef struct	{
	uint32_t addr[2];
}Addr_64;

typedef struct	{
	Addr_64 addr[2];
}Addr_128;

typedef struct	{
	uint16_t addr;
}Addr_16;

typedef struct	{
	uint8_t addr;
}Addr_8;

typedef struct	{
	uint8_t Type;
	uint8_t Length;
}option_SLLAO;

typedef struct	{
	uint8_t Type;
	uint8_t Length;
	uint32_t Status_reserved;
	uint16_t Reg_Lifetime;
	Addr_64 EUI64;
}option_ARO;

typedef struct	{
	uint8_t type,code;
	uint16_t checksum;
}ICMP_hdr;

typedef struct	{
	ICMP_hdr TCC;
	uint32_t	reserved;
	option_SLLAO Src_LLA;
}ICMP_RS;

typedef struct	{
	ICMP_hdr TCC;
	uint8_t Cur_Hop_Limit;
	uint8_t M_O_reserved;
	uint16_t Rtr_Lifetime;
	uint32_t Reach_Time;
	uint32_t Retrans_Time;
	option_SLLAO Src_LLA;
}ICMP_RA;

typedef struct	{
	ICMP_hdr TCC;
	uint32_t reserved;
	Addr_128 Tgt_addr;
	option_ARO Reg_Request;
	option_SLLAO Src_LLA;
}ICMP_NS;

typedef struct	{
	ICMP_hdr TCC;
	uint32_t R_S_O_reserved;
	Addr_128 Tgt_addr;
	option_ARO Request_Status;
}ICMP_NA;

	

/*
typedef struct{
	uint8_t TF[2];
	uint8_t NH;
	uint8_t HLIM;
	uint8_t CID;
	uint8_t
}
*/

/*
typedef struct{
	uint8_t DestPANID;
	uint8_t DestAddress;
	uint8_t SrcPANID;
	uint8_t SrcAddress;
}field_length;
*/

uint16_t cal_checksum(uint8_t *,uint8_t);
void MakePacket_IEEE(uint16_t);
void MakePacket_IPHC(uint8_t,uint8_t);

#ifdef __cplusplus
}
#endif

#endif 