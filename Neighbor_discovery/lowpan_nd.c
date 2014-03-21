/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

This file contains Neighbor Discovery (ND) Frame format.
For more details refer RFC 6775
*****************************************************************/
/*Neighbor Solicitation Message
	ICMP type=135 code=0
	Options: SLLAO
*/

typedef struct
{
	uint8_t reserved[4];
	uint8_t target_address[4];
}LoWPANv6_nd_ns;

/* Neighbour Advertisement
	ICMP type=136 code=0
	Options: DLLAO
*/


#define nd_na_flags_R	0x00
#define nd_na_flags_S	0x01
#define nd_na_flags_O	0x02
typedef struct
{
	uint8_t flags_reserved;
	uint8_t reserved[3];
	uint8_t target_address[4];

}LoWPANv6_nd_na;

/* Router Solicitation
	ICMP type=133 code=0
	Options: SLLAO
*/
typedef struct
{
	uint8_t reserved[4];
}LoWPANv6_nd_rs;

/*	Router Advertisement
		ICMP type=134 code=0
		Possible options: SLLAO, MTU, Prefix Information
*/

#define ra_flag_M	0x00
#define ra_flag_O	0x01
typedef struct
{
	uint8_t hop_limit;
	uint8_t flags_reserved;
	uint8_t router_lifetime[2];
	uint8_t reachable_time[4];
	uint8_t retrans_time[4];
}LoWPANv6_nd_ra;

/*
	Neighbour Discovery header
*/

#define ICMP_type_ns 135
#define ICMP_type_na 136
#define ICMP_type_rs 133
#define ICMP_type_ra 134
typedef struct
{
	uint8_t type;
	uint8_t code;
}LoWPANv6_nd_header;


/*
	Neighbour Discovery Optins
*/
#define ICMP_option_type_prefix_info 		3
#define ICMP_option_length_prefix_info	4
typedef struct
{
	uint8_t type;
	uint8_t length;
}LoWPANv6_nd_options;


/* Prefix Discovery
	 Prefix field is set to zero by sender
	 type=3
	 length=4
*/

#define nd_prefix_info_flag_L	0x00
#define nd_prifix_info_flag_A	0x01

typedef struct
{
	uint8_t prefix_length;
	uint8_t flags_reserved;
	uint8_t valid_lifetime[4];
	uint8_t preffered_lifetime[4];
	uint8_t prefix[8];
	
}LoWPANv6_nd_prefix_info;

/*this function is used to send RS message*/

