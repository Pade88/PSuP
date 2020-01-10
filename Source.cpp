#include "pic18f8722.h" 
#define DELAY for(int i = 0; i < 10000; i++)
#define XTAL_FREQ 4000000  

static int moduleCounter = 0;
/*
@Author: OPA
@Date: 2.0.2020
@Parameters: Integer conf_EUSART setting UART that follow to be configured <<Range set for only 1 or 2 values>>
@Inernal pin configuration: 
			CSRC - Mode selector <<1 for Master>> | <<0 for Slave>>
			BRGH - Baud Rate selector <<1 for HS> | <<0 for LS>>
			BRG16 - 16 bit Baud Rate Register Enable <<1 for 16 bit>> || <<0 for 8 bit>>
			SYNC - EUSART Mode selector <<1 for Sync>> | <<0 for aSync>>
			SPEN - Serial Port Enable <<1 for enable>> | <<0 for holding reset over this pin>>
			TXEN - Transmit Enable bit <<1 for enable>> | <<0 for disbale>>
@Variables:
			badModuleNo - if parameter is inconsistent (different than 1 || 2 ) function set active 
						EUSART1 or EUSART2 in a random order. (Influenced by number of function calls)

			moduleCounter - it's used as trigger or enable flag, since function does not transmit a flag
							as parameter, moduleCounter increment at every function call and keeps value
							since it's a static integer. If current state of moduleCounter is even, program
							consider it as an ENABLE condition and configure <<conf_EUSART>> pins, otherwise 
							it is considered as a DISABLE statement and RCSTAxbits & TXSTAxbits are disabled.
*/
void EUSART_set(int conf_EUSART)
{
	bool badModuleNo = conf_EUSART != 1 && conf_EUSART != 2 ? true : false;
	if (badModuleNo)
		conf_EUSART = (moduleCounter % 2) + 1;

	moduleCounter = moduleCounter + 1;
	if (moduleCounter & 1)
	{
		switch (conf_EUSART)
		{
		case 1:
			TXSTA1bits.CSRC = 1;
			SPBRG1 = 129;
			TXSTA1bits.BRGH = 1;
			BAUDCON1bits.BRG16 = 0;
			TXSTA1bits.SYNC = 0;
			RCSTA1bits.SPEN = 1;
			TXSTA1bits.TXEN = 1;
			break;
		case 2:
			TXSTA2bits.CSRC = 1;
			SPBRG2 = 129;
			TXSTA2bits.BRGH = 1;
			BAUDCON2bits.BRG16 = 0;
			TXSTA2bits.SYNC = 0;
			RCSTA2bits.SPEN = 1;
			TXSTA2bits.TXEN = 1;
		}
	}
	else
	{
		switch (conf_EUSART)
		{
		case 1:
			RCSTA1bits.SPEN = 0;
			TXSTA1bits.TXEN = 0;
			break;
		case 2:
			RCSTA2bits.SPEN = 0;
			TXSTA2bits.TXEN = 0;
		}
	}
	
}


void main(void)  
{  
	// SET 1
	// Testing correct selection of <<selectedUSARTPort>> 1 or 2 and transmitting ASCII characters.
	unsigned int ASCIIcode = 0x0; 
	int selectedUSARTPort = 1;
	EUSART_set(selectedUSARTPort); // ENABLE
	while(1) 
	{  
		if (selectedUSARTPort == 1)
		{
			TXREG1 = ASCIIcode;
			DELAY;
		}
		else if(selectedUSARTPort == 2)
		{
			TXREG2 = ASCIIcode;
			DELAY;
		}

		ASCIIcode = ASCIIcode + 1;

		if (ASCIIcode > 255)
			break;
	}  
	EUSART_set(selectedUSARTPort); // DISABLE

	// SET 2 
	// Transmit ASCII characters on EUSART com, disable communication when code is beetween 150 and 200.
	// Testing ENABLE-DISABLE System and latency.
	ASCIIcode = 0x0;
	selectedUSARTPort = 2;
	EUSART_set(selectedUSARTPort); // ENABLE
	while (1)
	{
		if (selectedUSARTPort == 1)
		{
			TXREG1 = ASCIIcode;
			DELAY;
		}
		else if (selectedUSARTPort == 2)
		{
			TXREG2 = ASCIIcode;
			DELAY;
		}

		ASCIIcode = ASCIIcode + 1;

		if (ASCIIcode == 150)
		{
			EUSART_set(selectedUSARTPort); // DISABLE
		}
		if (ASCIIcode == 200)
		{
			EUSART_set(selectedUSARTPort); // ENABLE
		}

	}
}