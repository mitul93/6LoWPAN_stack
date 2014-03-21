/***************************************************************
Authors
Mitul Vekariya <vekariya93@gmail.com>
Mohit Agarwal	<10bec063@nirmauni.ac.in>
Parav Nagarsheth	<10bec066@nirmauni.ac.in>

HAL(Hardware Abstraction Library) for MRF24J40MC transceiver.
This file contains API to access MRF24J40MC's functionality.
*****************************************************************/

#include "beaglebone.h"
#include "hw_MRF.h"
#include "gpio_v2.h"
#include "uartStdio.h"

#ifdef __cplusplus
extern "C" {
#endif

volatile unsigned int flag = 1;
unsigned char rxBuffer[260];
unsigned int length = 64;
unsigned int chNum = 0;
unsigned char *p_rx;
unsigned char txBuffer[260];
unsigned char *p_tx;
unsigned int temp;
#define GPIO_INSTANCE_ADDRESS           (SOC_GPIO_1_REGS)
#define GPIO_INSTANCE_PIN_NUMBER        (23)
unsigned int i;
void MRF_Init(void)
{
	GPIO1ModuleClkConfig();							//	Enabling functional clocks for GPIO1 instance.
    GPIO1Pin23PinMuxSetup();						//	Selecting GPIO1[23] pin for use.
    GPIOModuleEnable(GPIO_INSTANCE_ADDRESS);		//  Enabling the GPIO module.
    GPIOModuleReset(GPIO_INSTANCE_ADDRESS);			//	Resetting the GPIO module.
    GPIODirModeSet(GPIO_INSTANCE_ADDRESS,			//Setting the GPIO pin as an output pin.
                   GPIO_INSTANCE_PIN_NUMBER,
                   GPIO_DIR_OUTPUT);
//    GPIOPinWrite(GPIO_INSTANCE_ADDRESS,				//	Driving a logic HIGH on the GPIO pin.
//                     GPIO_INSTANCE_PIN_NUMBER,
//                    GPIO_PIN_HIGH);



	SPIInit();										// Init SPI0 for MRF24J60MC

		MRF_WriteShort(RFCTL, 0x04);
	    MRF_WriteShort(RFCTL, 0x00);

	    MRF_WriteShort(RFCTL, 0x00);

	    MRF_WriteShort(PANIDL, 0xAA);
	    MRF_WriteShort(PANIDH, 0xAA);
	    MRF_WriteShort(SADRL, 0xAA);
	    MRF_WriteShort(SADRH, 0xAA);

	    // Flush RX fifo.
	    MRF_WriteShort(RXFLUSH, 0x01);

	    // Write MAC addresses here. We don't care.

	    MRF_WriteLong(RFCTRL2, 0x80);  // Enable RF PLL.

	    MRF_WriteLong(RFCTRL3, 0x00);  // Full power.
	    MRF_WriteLong(RFCTRL6, 0x80);  // Enable TX filter (recommended)
	    MRF_WriteLong(RFCTRL8, 0x10);  // Enhanced VCO (recommended)

	    MRF_WriteShort(BBREG2,0x78);   // Clear Channel Assesment use carrier sense.
	    MRF_WriteShort(BBREG6,0x40);   // Calculate RSSI for Rx packet.
	    MRF_WriteShort(RSSITHCCA,0x00);// RSSI threshold for CCA.

	    MRF_WriteLong(RFCTRL0, 0x00);  // Channel 11.
	    MRF_WriteLong(RFCTRL3, 0x00);
	    //Set GPIO3
	    MRF_WriteShort(0x34,0x08);
	    MRF_WriteShort(0x33, 0x08);
	    //Test Mode, Enable PA/LNA
	    MRF_WriteLong(0x22F, 0x0F);

	    MRF_WriteShort(RXMCR, 0x01); // Don't check address upon reception.
	//  MrfMRF_WriteShort(RXMCR, 0x00); // Check address upon reception.

	    // Reset RF module with new settings.
	    MRF_WriteShort(RFCTL, 0x04);
	    MRF_WriteShort(RFCTL, 0x00);

}



static void McSPI0AintcConfigure(void)
{
    /* Initialze ARM interrupt controller */
    IntAINTCInit();

    /* Register McSPIIsr interrupt handler */
    IntRegister(SYS_INT_SPI0INT, McSPIIsr);

    /* Set Interrupt Priority */
    IntPrioritySet(SYS_INT_SPI0INT, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable system interrupt in AINTC */
    IntSystemEnable(SYS_INT_SPI0INT);
}


void McSPITransfer(void)
{
    p_tx = txBuffer;
    p_rx = rxBuffer;

    /* SPIEN line is forced to low state.*/
    McSPICSAssert(SOC_SPI_0_REGS, chNum);

    /* Enable the Tx/Rx interrupts of McSPI.*/
    McSPIIntEnable(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum) |
                   MCSPI_INT_RX_FULL(chNum));

    /* Enable the McSPI channel for communication.*/
    McSPIChannelEnable(SOC_SPI_0_REGS, chNum);

    /* Wait until control returns back from McSPI ISR.*/
    while(flag);

    flag = 1;

    /* Force SPIEN line to the inactive state.*/
    McSPICSDeAssert(SOC_SPI_0_REGS, chNum);

    /* Disable the McSPI channel.*/
    McSPIChannelDisable(SOC_SPI_0_REGS, chNum);
}

/*
** McSPI Interrupt Service Routine. This function will clear the status of the
** Tx/Rx interrupts when generated. Will write the Tx data on transmit data
** register and also will put the received data from receive data register to
** a location in memory.
*/
static void McSPIIsr(void)
{
    unsigned int intCode = 0;

    intCode = McSPIIntStatusGet(SOC_SPI_0_REGS);

    while(intCode)
    {
        if(MCSPI_INT_TX_EMPTY(chNum) == (intCode & MCSPI_INT_TX_EMPTY(chNum)))
        {
            McSPIIntStatusClear(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));

            length--;

            McSPITransmitData(SOC_SPI_0_REGS,(unsigned int)(*p_tx++), chNum);

            if(!length)
            {
                McSPIIntDisable(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));

                McSPIIntStatusClear(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));
            }
        }

        if(MCSPI_INT_RX_FULL(chNum) == (intCode & MCSPI_INT_RX_FULL(chNum)))
        {
            McSPIIntStatusClear(SOC_SPI_0_REGS, MCSPI_INT_RX_FULL(chNum));

            *p_rx++ = (unsigned char) McSPIReceiveData(SOC_SPI_0_REGS, chNum);

            if(!(length))
            {
                McSPIIntDisable(SOC_SPI_0_REGS, MCSPI_INT_RX_FULL(chNum));

                flag = 0;
            }
        }

        intCode = McSPIIntStatusGet(SOC_SPI_0_REGS);
    }
}

void SPIClkConfig(void)
{
		HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) =
	                             CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
	     CM_PER_L3S_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) =
	                             CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
	     CM_PER_L3_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	    HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) =
	                             CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) &
	                               CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) !=
	                                   CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

	    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) =
	                             CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) &
	        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

	    HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) =
	                             CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
	                              CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL) !=
	                                CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) =
	                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
	                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL) !=
	                               CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) =
	                             CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) &
	      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

	    HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) &= ~CM_PER_SPI0_CLKCTRL_MODULEMODE;

	    HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) |=
	                             CM_PER_SPI0_CLKCTRL_MODULEMODE_ENABLE;

	    while((HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) &
	      CM_PER_SPI0_CLKCTRL_MODULEMODE) != CM_PER_SPI0_CLKCTRL_MODULEMODE_ENABLE);


	    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
	            CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));

	    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
	            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));

	    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
	           (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK |
	            CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK)));

	    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
	           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK |
	            CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_SPI_GCLK)));


}

/*
 * This function will configure the SPI communication as per the given arguments
 */


void McSPISetUp(void)
{

    /* Reset the McSPI instance.*/
    McSPIReset(SOC_SPI_0_REGS);

    /* Enable chip select pin.*/
    McSPICSEnable(SOC_SPI_0_REGS);

    /* Enable master mode of operation.*/
    McSPIMasterModeEnable(SOC_SPI_0_REGS);

    /* Perform the necessary configuration for master mode.*/
    McSPIMasterModeConfig(SOC_SPI_0_REGS, MCSPI_SINGLE_CH,
                          MCSPI_TX_RX_MODE, MCSPI_DATA_LINE_COMM_MODE_6,
                          chNum);

    /* Configure the McSPI bus clock depending on clock mode. */
    McSPIClkConfig(SOC_SPI_0_REGS, MCSPI_IN_CLK, MCSPI_OUT_FREQ, chNum,
                   MCSPI_CLK_MODE_0);

    /* Configure the word length.*/
    McSPIWordLengthSet(SOC_SPI_0_REGS, MCSPI_WORD_LENGTH(8), chNum);

    /* Set polarity of SPIEN to low.*/
    McSPICSPolarityConfig(SOC_SPI_0_REGS, MCSPI_CS_POL_LOW, chNum);

    /* Enable the transmitter FIFO of McSPI peripheral.*/
    McSPITxFIFOConfig(SOC_SPI_0_REGS, MCSPI_TX_FIFO_ENABLE, chNum);

    /* Enable the receiver FIFO of McSPI peripheral.*/
    McSPIRxFIFOConfig(SOC_SPI_0_REGS, MCSPI_RX_FIFO_ENABLE, chNum);

    McSPICSTimeControlSet(SOC_SPI_0_REGS,MCSPI_CS_TCS_1PNT5_CLK,chNum);
}

void SPIInit()
{
	SPIClkConfig();	// configure clock for SPI


				HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_SCLK) =
	                 (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL |
	                      CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);	// control register setting
	            HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D0) =
	                 (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL |
	                      CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);	// control register setting
	            HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D1) =
	                 (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL |
	                      CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);	// control register setting
	            HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_CS0) =
	                  (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL |
	                       CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);	// control register setting


	McSPI0AintcConfigure();	//	  Map McSPI Interrupts to AINTC

	McSPISetUp();	//	 Do the necessary set up configurations for McSPI.

}
/*
 * This function will write value to register of MRF24J40MC
 * @arg: address- 6 bit short address of register
 * 		 value  - value to be written
 */

void MRF_WriteShort(unsigned int address, unsigned int value)
{
	length=2;
	txBuffer[0]=(address<<1)|0x01;
	txBuffer[1]=value;
	McSPITransfer();

}

/*
 * This function will write value to register of MRF24J40MC
 * @arg: address- 12 bit short address of register
 * 		 value  - value to be written
 */

void MRF_WriteLong(unsigned int address, unsigned int value)
{
	length=3;
	temp=(address<<5)|(0x801F);
	txBuffer[0]=(temp&0xFF00)>>8;
	txBuffer[1]=temp&0x00FF;
	txBuffer[2]=value;
	McSPITransfer();
}

/*
 * This function will read value to register of MRF24J40MC
 * @arg: address- 6 bit short address of register
 */


unsigned int MRF_ReadShort(unsigned int address)
{
	length=2;
	txBuffer[0]=(address<<1);
	txBuffer[1]=0xFF;
	McSPITransfer();
	return rxBuffer[1];
}

/*
 * This function will read value to register of MRF24J40MC
 * @arg: address- 12 bit short address of register
 */

unsigned int MRF_ReadLong(unsigned int address)
{
	length=3;
	temp=(address<<5)|(0x800F);
	txBuffer[0]=(temp&0xFF00)>>8;
	txBuffer[1]=temp&0x00FF;
	txBuffer[2]=0xFF;
	McSPITransfer();
	return rxBuffer[2];
}

void MRF_TransmitData(unsigned int *data,unsigned int len)
{
	for(i=0;i<len;i++)
		MRF_WriteLong(i,*(data++));

    GPIOPinWrite(GPIO_INSTANCE_ADDRESS,				//	Driving a logic HIGH on the GPIO pin.
                     GPIO_INSTANCE_PIN_NUMBER,
                     	 GPIO_PIN_HIGH);

	UARTPuts("Transmitting data",-1);

	MRF_WriteShort(TXNMTRIG,0x01);

	GPIOPinWrite(GPIO_INSTANCE_ADDRESS,				//	Driving a logic LOW on the GPIO pin.
	                     GPIO_INSTANCE_PIN_NUMBER,
	                     	 GPIO_PIN_LOW);

}

#ifdef __cplusplus
}
#endif
