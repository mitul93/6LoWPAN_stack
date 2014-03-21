/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

This file contains code to bootstrap node running 6LoWPAN stack.
For more details about Bootstrapping in 6LoWPAN refer RFC 6775
*****************************************************************/
#ifndef	__NODE_CONFIG_H
#define __NODE_CONFIG_H
#include <IEEE_802_15_4_packet.h>

#ifdef __cplusplus
	extern "C"	{
#endif
		
		void Node_Init();
		void Router_Solicitation(uint16_t *);
		void Router_Ad();
		void Node_Registration();
		void Neighbor_Ad();
		
#ifdef __cplusplus
	}
#endif
	
#endif