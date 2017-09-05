/******************************************************************************
 *  Embedded version of "printf( )" function
 *  - with similar functions but more efficient
 *
 *  author: Haroldo Amaral - agaelema@globo.com
 *  v0.5 - 2017/08/20
 *
 *  Based in this link:
 *  http://e2e.ti.com/support/development_tools/code_composer_studio/f/81/p/30479/107146
 ******************************************************************************
 *  log:  . change some "#ifdef" to "#if defined ( )"
 ******************************************************************************/

#ifndef		_EMBEDDED_PRINTF_
#define		_EMBEDDED_PRINTF_

#include    <stdint.h>
#include    "serial_conf.h"

/******************************************************************************
 * CONFIGURATIONS OF EMBEDDED_PRINTF
 * - select/uncomment desired options
 ******************************************************************************/
#define		ENABLE_PAD								// enable padding in functions
#define		ENABLE_FLOAT							// enable print float/double
//#define		PRECISION_FLOAT							// select precision version - use double variable
//#define		ENABLE_EXTRA_DECIMAL_PLACE				// enabled = 9 dp, disabled = 4 dp - depends of PRECISION_FLOAT
#define		ENABLE_BINARY							// enable binary notation


/******************************************************************************
 * PROTOTYPE OF FUNCTIONS
 ******************************************************************************/
void putChar(uint8_t byte);
void linesUp(unsigned int lines);

unsigned int embedded_prints(char *string, unsigned int width, unsigned int pad);
unsigned int embedded_ltoa(char *print_buf, int32_t input, unsigned int base, unsigned int sg, unsigned int width, unsigned int pad, unsigned char letbase);

//#ifdef	ENABLE_PAD
//#define	ENABLE_PAD_
//#endif

#if defined (ENABLE_PAD)
#define ENABLE_PAD_
#endif


//#ifdef	ENABLE_EXTRA_DECIMAL_PLACE
//#define	ENABLE_EXTRA_DECIMAL_PLACE_
//#endif

#if defined (ENABLE_EXTRA_DECIMAL_PLACE)
#define ENABLE_EXTRA_DECIMAL_PLACE_
#endif

//#ifdef ENABLE_FLOAT

#if defined (ENABLE_FLOAT)

//#ifdef PRECISION_FLOAT
#if defined (PRECISION_FLOAT)
#define PRECISION_FLOAT_
unsigned int embedded_ftoa(char *print_buf, double input, signed int dp, unsigned int sci);
#else
unsigned int embedded_ftoa(char *print_buf, float input, signed int dp, unsigned int sci);
#endif
#endif

unsigned int embedded_printf(char *format, ...);

unsigned int print_string(char *string);
unsigned int print_long(long number);
unsigned int print_ulong(unsigned long number);
unsigned int print_hexa(long number);

//#ifdef	ENABLE_BINARY
#if defined (ENABLE_BINARY)
#define ENABLE_BINARY_
unsigned int print_binary(long number, unsigned int bits);
#endif

//#ifdef ENABLE_FLOAT
#if defined (ENABLE_FLOAT)
#define ENABLE_FLOAT_

//#ifdef	PRECISION_FLOAT
#if defined (PRECISION_FLOAT)
#define PRECISION_FLOAT_
unsigned int print_float(double number, int dp);
unsigned int print_scientific(double number, int dp);
#else
unsigned int print_float(float number, int dp);
unsigned int print_scientific(float number, int dp);
#endif
#endif


#ifdef	PRECISION_FLOAT_
void embedded_string2number(unsigned char *string, double *number);
#else
void embedded_string2number(unsigned char *string, float *number);
#endif


/******************************************************************************
 * IMPORTANT DEFINITION
 ******************************************************************************/
#define		BINARY			2						// number identifier
#define		DECIMAL			10
#define		HEXADEC			16

#define		SIGNED 			1						// if signed (+/-)
#define		NON_SIGNED		0						// just (+)
#define		NON_PAD			0						// without padding
#define		UPPER_CASE		'A'
#define		LOWER_CASE		'a'

#define		SCI				1						// scientific notation "xx.yyezz"
#define		NON_SCI			0						// standard notation "xx.yy"

#define 	PAD_RIGHT		0x01					// right justified
#define 	PAD_ZERO		0x02					// padding with zero "0"

#define		PRINT_BUF_LEN 	36						// size of internal buffer to the conversion

//#define		WITH_LABEL		1
//#define		WITHOUT_LABEL	0

#endif      // end of _EMBEDDED_PRINTF_
