/******************************************************************************
 *  "serial_conf.h" used in the embedded version of "printf" function
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

#ifndef		_SERIAL_CONF_
#define		_SERIAL_CONF_

/******************************************************************************
 * list of supported Microcontrollers and their serial USCI
 * - uncomment desired device/serial
 ******************************************************************************/
//#define         MSP430G2553_USCI_A0             // P1.1 and P1.2
//#define         MSP430FR6989_USCI_A0			// P2.0 and P2.1
#define         MSP430FR6989_USCI_A1            // onboard serial - P3.4 and P3.5

/******************************************************************************
 * UART source of clock
 * - uncomment desired device/serial
 ******************************************************************************/
//#define     LFXT32K
//#define     SMCLK_DCO_1MHZ
#define     SMCLK_DCO_8MHZ

/******************************************************************************
 * list of supported baudrates
 * - uncomment desired baudrate
 ******************************************************************************/
//#define		BAUD_9600
#define		BAUD_115200

/******************************************************************************
 * Register version or Driverlib version
 * - select just one
 ******************************************************************************/
#define     REGISTER_VERSION
//#define     DRIVERLIB_VERSION


/******************************************************************************
 * Supported microcontrollers
 * If you add a new device, do a pull request and help other users
 ******************************************************************************/
#if   defined (MSP430G2553_USCI_A0)
#define     MSP430
#define     G2553
#define     USCI_UCA0

#elif     defined (MSP430FR6989_USCI_A0)
#define     MSP430
#define     FR6989
#define     USCI_UCA0

#elif   defined (MSP430FR6989_USCI_A1)
#define     MSP430
#define     FR6989
#define     USCI_UCA1

#else
#error  "Incorrect or invalid combination"
#endif




/******************************************************************************
 * Prototype of functions
 ******************************************************************************/
void serial_configure(void);
void serial_sendbyte(char byte);

#endif      // end of _SERIAL_CONF_
