/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

HAL(Hardware Abstraction Library) for MRF24J40MC transceiver.
This file contains API to access MRF24J40MC's functionality.
*****************************************************************/

#include "hw_types.h"
#include "mcspi.h"
#include "interrupt.h"
#include "soc_AM335x.h"
#include "clock.h"


#ifndef hw_MRF_H_
#define hw_MRF_H_


#ifdef __cplusplus
extern "C" {
#endif
#define	TXNSTAT	  0x24
#define RXMCR     0x00
#define PANIDL    0x01
#define PANIDH    0x02
#define SADRL     0x03
#define SADRH     0x04
#define EADR0     0x05
#define EADR1     0x06
#define EADR2     0x07
#define EADR3     0x08
#define EADR4     0x09
#define EADR5     0x0a
#define EADR6     0x0b
#define EADR7     0x0c
#define RXFLUSH   0x0d

#define TXNMTRIG  0x1b
#define TXSR      0x24

#define ISRSTS    0x31
#define INTMSK    0x32
#define GPIO      0x33
#define TRISGPIO  0x34

#define RFCTL     0x36

#define BBREG2    0x3A

#define BBREG6    0x3E
#define RSSITHCCA 0x3F

// MRF20J40 Long address control register mapping.
#define RFCTRL0   0x200

#define RFCTRL2   0x202
#define RFCTRL3   0x203

#define RFCTRL6   0x206
#define RFCTRL7   0x207
#define RFCTRL8   0x208

#define CLKINTCR  0x211
#define CLCCTRL   0x220


#define MCSPI_OUT_FREQ                   (8000000u)
#define MCSPI_IN_CLK                     (48000000u)

void MRF_Init(void);
void MRF_WriteShort(unsigned int address, unsigned int value);
void MRF_WriteLong(unsigned int address, unsigned int value);
unsigned int MRF_ReadShort(unsigned int address);
unsigned int MRF_ReadLong(unsigned int address);
void McSPITransfer(void);
void MRF_TransmitData(unsigned int *data, unsigned int len);
static void McSPIIsr(void);
static void McSPI0AintcConfigure(void);
void SPIInit(void);
void SPIClkConfig(void);
void McSPISetUp(void);


#ifdef __cplusplus
}
#endif

#endif
