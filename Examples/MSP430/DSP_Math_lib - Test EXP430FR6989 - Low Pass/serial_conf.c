/******************************************************************************
 *  "serial_conf.c" used in the embedded version of "printf" function
 *
 *  author: Haroldo Amaral - agaelema@globo.com
 *  v0.4 - 2017/09/04
 ******************************************************************************
 *  log:  . Modify initial version
 *        + more defines to facilitate the configuration between devices
 *        + change function "sendbyte" to "serial_sendbyte"
 *        + add function to configure UART
 *        + add exp430fr6989 uart back-channel (USCI_UCA1) - P3.4/P3.5
 *        . change some "#ifdef" to "#if defined ( )"
 *        + add 9600bps and 115200 at 8MHZ
 *
 ******************************************************************************/

#include    "serial_conf.h"


/******************************************************************************
 * specific "include" to each device
 ******************************************************************************/
#if     defined (MSP430)					// msp430 header
#include	<msp430.h>
#endif


/******************************************************************************
 * handle serial configuration for each device
 * Input:	none
 * Output:	none
 ******************************************************************************/
void serial_configure(void)
{
#if     defined (MSP430) && defined(G2553)              // MSP430G2553 - Launchpad
#if             defined (REGISTER_VERSION)
#if                     defined (USCI_UCA0) && defined(SMCLK_DCO_1MHZ) && defined (BAUD_9600)
    P1SEL = BIT1 + BIT2;            // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;           // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;           // UART clock - SMCLK
    UCA0BR0 = 104;                  // UCA0BR0 = clock/baud = 1MHz/9600
    UCA0BR1 = 0;                    // UCA0BR0 = clock/baud = 1MHz/9600

    UCA0MCTL = UCBRS0;              // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;           // software reset - Initialize USCI state machine
#else
#error                  "Invalid USCI, Clock/Source or Baudrate - Check the serial_conf.h"
#endif
#else
#error          "Invalid configuration version (REGISTER or DRIVERLIB) - Check the serial_conf.h"
#endif          //  end of REGISTER_VERSION

#elif     defined (MSP430) && defined(FR6989)              // MSP430G2553 - Launchpad
#if             defined (REGISTER_VERSION)
#if                     defined (USCI_UCA0) && defined(LFXT32K) && defined (BAUD_9600)
    // Configure GPIO
    P2SEL0 |= BIT0 | BIT1;                    // USCI_A0 UART operation
    P2SEL1 &= ~(BIT0 | BIT1);

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__ACLK;                // CLK = ACLK
    UCA0BR0 = 3;                              // 9600 baud
    UCA0MCTLW |= 0x5300;                      // 32768/9600 - INT(32768/9600)=0.41
    // UCBRSx value = 0x53 (See UG)
    UCA0BR1 = 0;
    UCA0CTL1 &= ~UCSWRST;                     // Initialize eUSCI
    //  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
#elif                   defined (USCI_UCA1) && defined(LFXT32K) && defined (BAUD_9600)
    // Configure GPIO
    P3SEL0 |= BIT4 | BIT5;                      // USCI_A0 UART operation
    P3SEL1 &= ~(BIT4 | BIT5);

    // Configure USCI_A0 for UART mode
    UCA1CTLW0 = UCSWRST;                        // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__ACLK;                  // CLK = ACLK
    UCA1BRW = 3;                                // 9600 baud  INT(32768/9600)=3
    UCA1MCTLW |= 0x9200;                        // (32768/9600 - INT(32768/9600))=0.4133 (see User guide table 30-4)
    UCA1CTL1 &= ~UCSWRST;                       // Initialize eUSCI
    //  UCA0IE |= UCRXIE;                           // Enable USCI_A0 RX interrupt
#elif                   defined (USCI_UCA1) && defined(SMCLK_DCO_8MHZ) && defined (BAUD_9600)
    /* Configure GPIO */
    P3SEL0 |= BIT4 | BIT5;                      // USCI_A0 UART operation
    P3SEL1 &= ~(BIT4 | BIT5);

    /* Configure USCI_A1 for UART mode */
    UCA1CTLW0 = UCSWRST;                        // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK;                 // CLK = SMCLK
    // Baud Rate calculation
    // UCBRW = INT (8000000/(16*9600)) = INT (52.0833) = 52
    // UCBRFx = INT ((52.083-52)*16) = INT (1.3333) = 1
    // fractional part = 0.3333
    // User's Guide Table 30-4: UCBRSx = 0x49
    UCA1BRW = 52;                               // 8000000/16/9600
    UCA1MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    //      UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
#elif                   defined (USCI_UCA1) && defined(SMCLK_DCO_8MHZ) && defined (BAUD_115200)
    // Configure GPIO
    P3SEL0 |= BIT4 | BIT5;                    // USCI_A0 UART operation
    P3SEL1 &= ~(BIT4 | BIT5);

    /* Configure USCI_A1 for UART mode */
    UCA1CTLW0 = UCSWRST;                        // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK;                 // CLK = SMCLK
    // Baud Rate calculation
    // UCBRW = INT (8000000/(16*115200)) = INT (4.3402) = 4
    // UCBRFx = INT ((4.4302-4)*16) = INT (5.4444) = 5
    // fractional part = 0.4444
    // User's Guide Table 30-4: UCBRSx = 0x55
    UCA1BRW = 4;                               // 8000000/16/9600
    UCA1MCTLW |= UCOS16 | UCBRF_5 | 0x5500;
    UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    //      UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
#else
#error                  "Invalid USCI or baudrate - Check the serial_conf.h"
#endif

#elif           defined (DRIVERLIB_VERSION)
#error          "no driverlib prepared"             // TODO: configure USCI using driverlib

#else
#error          "Invalid configuration version (REGISTER or DRIVERLIB) - Check the serial_conf.h"
#endif          //  end of REGISTER_VERSION

#else
#error  "No serial configuration selected or valid"
#endif
}


/******************************************************************************
 * Handle the serial code to transmit one byte
 * Each microcontroller has a specific code
 * Input:	char	byte (byte to be transmitted)
 * Output:	none
 ******************************************************************************/
void serial_sendbyte(char byte)
{
#if     defined(MSP430) && defined (G2553) && defined (USCI_UCA0)
    while (!(IFG2 & UCA0TXIFG));    // USCI_A0 TX buffer ready? - not using interruption
    UCA0TXBUF = byte;               // Load Tx register that clear UCA0TXIFG

#elif   defined(MSP430) && defined (FR6989) && defined (USCI_UCA0)
    while (!(UCA0IFG & UCTXIFG));    // USCI_A0 TX buffer ready? - not using interruption
    UCA0TXBUF = byte;               // Load Tx register that clear UCA0TXIFG

#elif   defined(MSP430) && defined (FR6989) && defined (USCI_UCA1)
    while (!(UCA1IFG & UCTXIFG));    // USCI_A0 TX buffer ready? - not using interruption
    UCA1TXBUF = byte;               // Load Tx register that clear UCA0TXIFG
#endif
}
