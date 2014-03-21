/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

This file contains 6LoWPAN IPHC frame format
For more details refer RFC 6282
*****************************************************************/

#define LOWPANv6_NHC_Extension_Prefix					0x07
#define LOWPANv6_NHC_Extension_0						0x00
#define LOWPANv6_NHC_Extension_1						0x10
#define LOWPANv6_NHC_Extension_2						0x20
#define LOWPANv6_NHC_Extension_3						0x30
#define LOWPANv6_NHC_Extension_4						0x40
#define LOWPANv6_NHC_Extension_7						0x70
#define LOWPANv6_NHC_Extension_N_elide					0x80

/*	Values of UDP Compressed Header  */
#define LOWPANv6_NHC_UDP_Prefix							0x0F
#define LOWPANv6_NHC_UDP_CheckSum_disable				0x20
#define LOWPANv6_NHC_UDP_Port_00						0x00
#define LOWPANv6_NHC_UDP_Port_01						0x40
#define LOWPANv6_NHC_UDP_Port_10						0x80
#define LOWPANv6_NHC_UDP_Port_11						0xC0


/*	Values of IPHC Header	Lower Byte	*/
#define LOWPANv6_IPHC_dispatch				0x06
#define LOWPANv6_IPHC_TF_00					0x00
#define LOWPANv6_IPHC_TF_01					0x08
#define LOWPANv6_IPHC_TF_10					0x10
#define LOWPANv6_IPHC_TF_11					0x18
#define LOWPANv6_IPHC_N_en					0x20
#define LOWPANv6_IPHC_HLIM_Inline			0x00
#define LOWPANv6_IPHC_HLIM_1				0x40
#define LOWPANv6_IPHC_HLIM_64				0x80
#define LOWPANv6_IPHC_HLIM_255				0xC0

/*	Values of IPHC header Higher Byte		*/
#define LOWPANv6_IPHC_CID_en				0x01
#define LOWPANv6_IPHC_SAC_en				0x02
#define LOWPANv6_IPHC_SAM_00				0x00
#define LOWPANv6_IPHC_SAM_01				0x04
#define LOWPANv6_IPHC_SAM_10				0x08
#define LOWPANv6_IPHC_SAM_11				0x0C
#define LOWPANv6_IPHC_Multicast				0x10
#define LOWPANv6_IPHC_DAC_en				0x20
#define LOWPANv6_IPHC_DAM_00				0x00
#define LOWPANv6_IPHC_DAM_01				0x40
#define LOWPANv6_IPHC_DAM_10				0x80
#define LOWPANv6_IPHC_DAM_11				0xC0

typedef struct {
	uint8_t IPHC_Header[2];
	uint8_t Context_ID;
	uint8_t *IPHC_Inline_Fields;
	uint8_t NHC_UDP_Header;
	uint8_t *UDP_Inline_Fields;
	uint8_t NHC_Extension_Header;
}LOWPANv6_Header;
	
/*	Values of FCF lower byte */
#define IEEE_frame_FCF_type_data		0x01		
#define IEEE_frame_FCF_type_ack			0x02
#define IEEE_frame_FCF_type_cmd			0x03
#define IEEE_frame_FCF_sec_en			0x08
#define IEEE_frame_FCF_pending			0x10			//Pending frame
#define IEEE_frame_FCF_ack_req			0x20
#define IEEE_frame_FCF_intraPAN			0x40

/* 	Values of FCF higher byte  */
#define IEEE_frame_FCF_SAM_elide		0x00					
#define IEEE_frame_FCF_SAM_16bSA		0x08
#define IEEE_frame_FCF_SAM_64bEA		0x0C
#define IEEE_frame_FCF_DAM_elide		0x00
#define IEEE_frame_FCF_DAM_16bSA		0x80
#define IEEE_frame_FCF_DAM_64bEA		0xC0

	

typedef struct {
	uint8_t length;
	uint8_t FCF[2];
	uint8_t SeqNo;
	uint8_t *Dest_PAN_ID;
	uint8_t *Dest_Address;
	uint8_t *Src_PAN_ID;
	uint8_t *Src_Address;
	uint8_t *Inline_Fields;
	uint8_t Frame_CheckSeq[2];
}IEEE_frame;