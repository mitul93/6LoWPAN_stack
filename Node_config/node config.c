/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

This file contains code to bootstrap node running 6LoWPAN stack.
For more details about Bootstrapping in 6LoWPAN refer RFC 6775
*****************************************************************/
#include <node_config.h>

extern uint8_t txpacket[];

void Node_Init(uint16_t *dest_addr)
{
	Router_Solicitation(dest_addr);
}


void Router_Solicitation(uint16_t *dest_addr)
{
	uint8_t length;
	uint8_t flag = 0;
	uint16_t FCF;
	uint8_t iphcl = 0x1B;	// TF elide, NH is ICMP, HLMT is 255
	uint8_t iphch = 0;
	MakePacket_IEEE(FCF);
	if(!(*(dest_addr+3)&&*(dest_addr+2)&&*(dest_addr+1)&&*(dest_addr)))
	{		
		Dest_Add_16->addr = 0xFFFF;
		offset += 2;
		flag = 1;
		iphch |= 0x0B;
	}
	else if(!((*dest_addr+3)&&(*dest_addr+2)&&(*dest_addr+1)))
	{
		Dest_Add_16->addr = *dest_addr;
		offset += 2;
		iphch |= 0x03;
	}
	else
	{
		Dest_Add_64->addr[0] = *dest_addr;
		Dest_Add_64->addr[1] = (*dest_addr>>32);
		offset += 8;
		iphch |= 0x03;
	}
	
	if(!ST_RadioGetNodeId())
	{
		Src_Add_64->addr[0] = Src_LL64;
		Src_Add_64->addr[1] = (Src_LL64 >> 32);
		maclen = offset + 8;
		iphch |= 0x30;
	}
	else
	{
		Src_Add_16->addr = ST_RadioGetNodeId();
		maclen = offset + 2;
		iphch |= 0x30;
  }
	MakePacket_IPHC(iphcl,iphch);
	
	if(flag)
	{	
		Dest_Add_8->addr = 0x02;
		iplen = maclen + 3;
	}
	else	iplen = maclen + 2;
	
	RS_buf->TCC.Type = ICMP_type_rs;
	RS_buf->TCC.Code = 0;
	RS_buf->reserved = 0;
	RS_buf->Src_LLA.Type = 1;
	
	if(!ST_RadioGetNodeId())
	{
		Src_LLAO64->addr[0] = ST_RadioGetEui64();
		Src_LLAO64->addr[1] = (ST_RadioGetEui64()>>32);
		IEEE_buf->length = iplen + 18 + 2;									//2 bytes for hardware generated CRC,
	}
	else
	{
		Src_LLAO16->addr = ST_RadioGetNodeId();
		IEEE_buf->length = iplen + 12 + 2;
	}
	RS_buf->TCC.checksum = 0;
	RS_buf->TCC.checksum = cal_checksum(&(txpacket[iplen]), (length - iplen));
}

