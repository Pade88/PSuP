#include "pic18f8722.h" 
#define DELAY for(int i = 0; i < 10000; i++)
#define XTAL_FREQ 4000000  
#define BAUD 9600
#include <stdio.h>
#include <string.h>

/*
@Author: OPA
@Revision: 1.0.0.1 (Non functional changes since 1.0.0.0)
@Date: 2.0.2020
@Parameters: Integer conf_EUSART setting UART that follow to be configured <<Range set for only 1 or 2 values>>
@Inernal pin configuration: 
			CSRC - Mode selector <<1 for Master>> | <<0 for Slave>>
			BRGH - Baud Rate selector <<1 for HS> | <<0 for LS>>
			BRG16 - 16 bit Baud Rate Register Enable <<1 for 16 bit>> || <<0 for 8 bit>>
			SYNC - EUSART Mode selector <<1 for Sync>> | <<0 for aSync>>
			SPEN - Serial Port Enable <<1 for enable>> | <<0 for holding reset over this pin>>
			TXEN - Transmit Enable bit <<1 for enable>> | <<0 for disbale>>
 @Description: Function configure bits for  EUSART COM port 1 or 2  if given parameter is consistent (1 || 2 ).
 *             If parameters are inconsistent, ASA disable EUSART Communication.
*/
void EUSART_set(int conf_EUSART)
{
    if (conf_EUSART == 1)
    {
        TXSTA1bits.CSRC = 1;
        SPBRG1 = (( XTAL_FREQ/16 ) / BAUD) - 1;  
        TXSTA1bits.BRGH = 1;
        BAUDCON1bits.BRG16 = 0;
        TXSTA1bits.SYNC = 0;   
        RCSTA1bits.SPEN = 1;   
        TXSTA1bits.TXEN = 1; 
    }
    else if(conf_EUSART == 2)
    {
        TXSTA2bits.CSRC = 1;
        SPBRG2 = (( XTAL_FREQ/16 ) / BAUD) - 1;  
        TXSTA2bits.BRGH = 1;
        BAUDCON2bits.BRG16 = 0;
        TXSTA2bits.SYNC = 0;   
        RCSTA2bits.SPEN = 1;   
        TXSTA2bits.TXEN = 1;   
    }
    else
    {  
        TXSTA1bits.CSRC = 0;
        TXSTA1bits.BRGH = 0;
        BAUDCON1bits.BRG16 = 0;
        TXSTA1bits.SYNC = 0;   
        RCSTA1bits.SPEN = 0;   
        TXSTA1bits.TXEN = 0; 
        TXSTA2bits.CSRC = 0;
        TXSTA2bits.BRGH = 0;
        BAUDCON2bits.BRG16 = 0;
        TXSTA2bits.SYNC = 0;   
        RCSTA2bits.SPEN = 0;   
        TXSTA2bits.TXEN = 0;
    }
}


/*
 @Author: OPA
 @Revision 1.0.0.0 (New Entry)
 @Date 11.01.2020
 @Parameters: char that follows to be transmitted via EUSART.
 @Description: ASA use and external integer representing EUSART port number that should transmit.
 *             ASA send character and apply a delay for consistency.
 */
void EUSART_send_char(char c)
{
    extern int selectedEUSARTPort;
    if(selectedEUSARTPort == 1)
    {
        TXREG1 = c;
        DELAY;
    }
    else if(selectedEUSARTPort == 2)
    {
        TXREG2 = c;
        DELAY;
    }
    else if(selectedEUSARTPort == 0)
    {
        ;
    }
}

/*
 @Author: OPA
 @Revision 1.0.0.0 (New Entry)
 @Date 11.01.2020
 @Parameters: A pointer to a char (String) that follows to be transmitted via EUSART.
 @Description: ASA use EUSART_send_char function to transmit every bit of parameter pointer.
               When ASA send all the characters from the string, 0x20 is transmitted (0x20 = ASCII code for SPACE)
 */
void EUSART_send_String(char *c)
{
    for(int i = 0; i<strlen(c); i++)
    {
        EUSART_send_char(c[i]);
    }
    EUSART_send_char(0x20);
}

extern int selectedEUSARTPort = 1;

void main(void)  
{  
    int loopCounter = 0;
	while(1) 
	{  
        EUSART_set(selectedEUSARTPort);
        EUSART_send_String("PROIECT SUP");
        
        loopCounter = loopCounter + 1;
        if(loopCounter & 1)
            selectedEUSARTPort = 2;
        else
            selectedEUSARTPort = 1;
        
        if(loopCounter > 5)
        {
            selectedEUSARTPort = 0;
        }
    }
}
