/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

This file contains method to make IEEE 802.15.4 frames
*****************************************************************/

#include <IEEE_802_15_4_packet.h>

uint8_t txpacket[];

#define offset			6
#define maclen		offset
#define BIT_16			1
#define BIT_64			2

#define Src_EUI64		ST_RadioGetEui64()
#define Src_LL64		(Src_EUI64^0x0200)

#define IEEE_buf 		((IEEE_hdr*)&txpacket[0])
#define IP_buf			((IP_hdr*)&txpacket[maclen])
#define RS_buf			((ICMP_RS*)&txpacket[iplen])
#define RA_buf			((ICMP_RA*)&txpacket[iplen])
#define NS_buf			((ICMP_NS*)&txpacket[iplen])
#define NA_buf			((ICMP_NA*)&txpacket[iplen])
#define Dest_Add_16 ((Addr_16*)&txpacket[6])
#define Dest_Add_64 ((Addr_64*)&txpacket[6])
#define Dest_Add_8	((Addr_8*)&txpacket[maclen+2])
#define Src_Add_16  ((Addr_16*)&txpacket[offset])
#define Src_Add_64  ((Addr_64*)&txpacket[offset])
#define Src_LLAO64	((Addr_64*)&txpacket[iplen + 10])
#define Src_LLAO16	((Addr_16*)&txpacket[iplen + 10])

#define RS_option_SLLA					1
#define RS_option_DLLA					2
#define RS_option_Prefix_info		3
#define RS_option_redirect			4
#define RS_option_MTU						5


void MakePacket_IEEE(uint16_t FCF)
{
	IEEE_buf->FCF = FCF;
	IEEE_buf->SeqNo = 0;
	IEEE_buf->DestPANID = ST_RadioGetPanId();
}

void MakePacket_IPHC(uint8_t iphcl,uint8_t iphch)
{
	IP_buf->IPHC[0] = 0x60 | iphcl;
	IP_buf->IPHC[1] = iphch;
}
	
/*	if(Dest_Addr_Mode == BIT_16)
	{ 
		Dest_Add_16->addr = GetDestAddr();
		offset+=2;
	}	 
	else		
	{
		Dest_Add_64->addr[0] = GetDestAddr();
		Dest_Add_64->addr[1] = (GetDestAddr() >> 32);
		offset += 8;
	}
	
	if(Src_Addr_Mode == BIT_16)
	{ 
		Src_Add_16->addr = ST_RadioGetNodeId();
		offset+=2;
	}	 
	else		
	{
		Src_Add_64->addr[0] = ST_RadioGetEui64();
		Src_Add_64->addr[1] = (ST_RadioGetEui64() >> 32);
		offset += 8;
	}
	*/


uint16_t cal_checksum(uint8_t *address,uint8_t length)
{
	uint32_t sum;
	while(length>0)
	{
		sum += *(address)+(*(address+1));
		address += 2;
		length -= 2;
	}
	if(length==1)
		sum += *(address);
	sum = (sum&0x0000FFFF)+(sum>>16);	
	return (~sum);
}
