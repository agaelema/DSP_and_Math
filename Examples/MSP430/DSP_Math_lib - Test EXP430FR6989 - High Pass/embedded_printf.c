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

#include    "embedded_printf.h"
#include    "serial_conf.h"

#include    <stdarg.h>
#include    <stdint.h>


/******************************************************************************
 * Call the specific function to send data
 * Input:	unsigned char	byte (byte to be transmited)
 * Output:	none
 ******************************************************************************/
void putChar(uint8_t byte)
{
	serial_sendbyte(byte);
}


void linesUp(unsigned int lines)
{
	unsigned int i;
	for (i = 0; i < lines; ++i)
	{
		putChar(0x1b);
		putChar(0x5b);
		putChar(0x41);
	}
}


/******************************************************************************
 * Print a String in the serial port
 * input:	char *string		(string to be printed)
 * 			unsigned int width	(limit the pad size)
 * 			unsigned int pad	(control the padding)
 * return:	number of characters
 ******************************************************************************/
//int embedded_prints(char *string, unsigned int width, unsigned int pad)
unsigned int embedded_prints(char *string, unsigned int width, unsigned int pad)
{
	unsigned int return_value = 0;
	unsigned char padchar = ' ';

	if (width > 0)
	{
		unsigned int len = 0;						// string length
		char *ptr;

		for (ptr = string; *ptr; ++ptr) {			// Calculate string length
			++len;
		}

		if (len >= width) width = 0;    			// If string is longer than width
		else width -= len;

#ifdef	ENABLE_PAD_
		if (pad & PAD_ZERO) padchar = '0';
#endif
	}

#ifdef	ENABLE_PAD_
	if (!(pad & PAD_RIGHT))  	                   	// If not right padding - left justification
	{
		for (; width > 0; --width)					// if padding is possible - put the char
		{
			putChar(padchar);
			++return_value;
		}
	}
#endif

	while (*string)
	{
		putChar(*string);
		++return_value;
//		*++string;
		++string;
	}

#ifdef	ENABLE_PAD_
	for (; width > 0; --width) {
		putChar(padchar);
		++return_value;
	}
#endif

	return return_value;                        	// Return the number of characters printed
}


/******************************************************************************
 *	Print an integer number (signed or unsigned) with max size of 32bits (long)
 *	input:	char *print_buf			(pointer to the buffer when the string will be saved)
 *			signed long input		(number to be printed - use the casting "(long)" before the number
 *			unsigned int base		(base of number - decimal, hexa, binary)
 *			unsigned int sg			(1 = signed, 0 = unsigned)
 *			unsigned int width		(mas size of padding)
 *			unsigned int pad		(control the padding)
 *			unsigned char letbase	(base to select the character in the ASCII table)
 *	return:	int return_value		(number of characters printed - like standard printf)
 ******************************************************************************/
//int embedded_ltoa(char *print_buf, signed long input, unsigned int base, unsigned int sg, unsigned int width, unsigned int pad, unsigned char letbase)
unsigned int embedded_ltoa(char *print_buf, int32_t input, unsigned int base, unsigned int sg, unsigned int width, unsigned int pad, unsigned char letbase)
{
	char *s;
	char neg = 0;
	uint32_t t;
	uint32_t u = input;
	unsigned int return_value = 0;

//	if (input == 0)
//	{
//		if (base != 2)
//		{
//			print_buf[0] = '0';
//			print_buf[1] = '\0';            // Always remenber to put string end
//			return 2;
////			print_buf[(2 * label) + 0] = '0';
////			print_buf[(2 * label) + 1] = '\0';            // Always remenber to put string end
////			if ((label) && (base == HEXADEC))
////			{
////				print_buf[0] = '0';
////				print_buf[1] = 'x';
////			}
////			return ((2 * label) + 2);
//		}
//		else
//		{
//			unsigned int xx;
//			for (xx = 0; xx < width; xx++)
//			{
//				print_buf[xx] = '0';
////				print_buf[(2 * label) + xx] = '0';
//			}
//			print_buf[width] = '\0';
////			print_buf[(2 * label) + width] = '\0';
////			if (label)
////			{
////				print_buf[0] = '0';
////				print_buf[1] = 'x';
////			}
//			return (width + 1);
//		}
//	}

	if (sg && (base == 10) && (input < 0))                 // If it is a negative number in decimal base
	{
		neg = 1;
		u = -input;
	}

	s = print_buf + PRINT_BUF_LEN - 1;				// go to the end of buffer
	*s = '\0';										// print the string terminator "\0"

	do
	{
		if (base != 2)								// if decimal or hexa
		{
			t = u % base;
			if (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
			u /= base;
		}
#ifdef	ENABLE_BINARY_
		else
		{											// if binary

			while (width)
			{
				*--s = (u & 0x01) ? '1' : '0';
				u >>= 1;
				width--;
			}
			u = 0;
		}
#endif
	}while(u);

//	if (label) {									// place the label... not implemented (some bugs)
//		if (base != DECIMAL)
//		{
//			*--s = (base == HEXADEC ? 'x' : 'b');
//			*--s = '0';
//		}
//	}

	if (neg)										// if negative, put the "-" signal
	{

////	 	old version of code
//		if (width && (pad & PAD_ZERO))
//		{     // If there is width, right justified and pad with zero, output negative sign.
//			putChar('-');
//			++return_value;
//			--width;
//		}
//		else *--s = '-';                  // Otherwise put the '-' to string buffer.

		if (!(width && (pad & PAD_ZERO)))
		{
			*--s = '-';                  // Otherwise put the '-' to string buffer.
		}
#ifdef	ENABLE_PAD_
		else							// If there is width, right just. and pad with zero
		{
			putChar('-');
			++return_value;
			--width;
		}
#endif
	}

	char *buffer_end = print_buf + PRINT_BUF_LEN - 1;
	int count;;
//	count = (int)buffer_end - (int)s;
	count = (int)(buffer_end - s);

	int xx;
	for (xx = 0; xx < count + 1; xx++) {
		print_buf[xx] = *s;
		s++;
	}

	return return_value;
}

#ifdef	ENABLE_FLOAT_
#ifdef 	PRECISION_FLOAT_
/******************************************************************************
 *	Print a float number (double)
 *	input:	char *print_buf			(pointer to the buffer when the string will be saved)
 *			double input			(number to be printed - use the casting "(double)" before the number
 *			signed int dp			(number of decimal places - between 1 and 4)
 *			unsigned int sci		(1 = scientific, 0 = non scientific notation)
 *	return:	unsigned int return_value		(number of characters printed - like standard printf)
 ******************************************************************************/
//int embedded_ftoa(char *print_buf, double input, signed int dp, unsigned int sci)
unsigned int embedded_ftoa(char *print_buf, double input, signed int dp, unsigned int sci)
{
	char *s;
	unsigned int neg = 0;
	uint32_t t;
	int32_t integer;

#ifdef	ENABLE_EXTRA_DECIMAL_PLACE_
	uint64_t decimal;
#else
	uint32_t decimal;
#endif
	int Exp = 0;
	int Exp_sg = 0;

//	int return_value = 0;
	unsigned int return_value = 0;

#ifdef	ENABLE_EXTRA_DECIMAL_PLACE_
	if (dp > 9) {dp = 9;}
#else
	if (dp > 4) {dp = 4;}
#endif
	if (dp < 1) {dp = 1;}

	double number;
	number = input;

	if (number < 0) {
		neg = 1;
		number *= -1;
	}

	if (sci)
	{
		if (number >= 10)
		{
			while (number >= 10)
			{
				number /= 10;
				Exp++;
			}
		}
		if ((number < 1) && (number != 0))
		{
			while (number < 1)
			{
				number *= 10;
				Exp--;
			}
		}
		if (Exp < 0) {
			Exp *= -1;
			Exp_sg = 1;
		}
	}

	integer = (int32_t)number;
	number = number - (double)integer;


	unsigned int ii;
#ifdef	ENABLE_EXTRA_DECIMAL_PLACE_
	uint32_t mult = 1;
#else
	uint16_t mult = 1;
#endif
	for (ii = 0; ii < dp; ++ii) {		// calculate the multiplier to convert decimal in integer value with desired dp
		mult *= 10;
	}

	decimal = (uint32_t)(number * (double)mult);

	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	if (sci)
	{
		do
		{
			t = Exp % 10;
			if (t >= 10)
				t += 'a' - '0' - 10;
			*--s = t + '0';
			Exp /= 10;
		}while(Exp);

		if (Exp_sg) {
			*--s = '-';
		}
		*--s = 'e';
	}

	while (dp)                               	// convert the portion after the dot "."
	{
		t = decimal % 10;
		if (t >= 10)
			t += 'a' - '0' - 10;
		*--s = t + '0';
		decimal /= 10;
		--dp;
	}

	*--s = '.';									// put the dot separator

	do											// convert the portion before the dot "."
	{
		t = integer % 10;
		if (t >= 10)
			t += 'a' - '0' - 10;
		*--s = t + '0';
		integer /= 10;
	}while(integer);

	if (neg)									// if negative
	{
//		if (width && (pad & PAD_ZERO))
//		{     // If there is width, right justified and pad with zero, output negative sign.
//			putChar('-');
//			++return_value;
//			--width;
//		}
//		else *--s = '-';
		*--s = '-';
	}

//	volatile int count;
	int count;
	count = s - print_buf;

	int xx;
	for (xx = 0; xx < count; xx++) {			// the string is formed in the end of buffer
		print_buf[xx] = *s;						// this code move to the beginning
		s++;
	}

	return return_value;
}
#else
/******************************************************************************
 *	Print a float number (float)
 *	input:	char *print_buf			(pointer to the buffer when the string will be saved)
 *			float input				(number to be printed - use the casting "(float)" before the number
 *			signed int dp			(number of decimal places - between 1 and 4)
 *			unsigned int sci		(1 = scientific, 0 = non scientific notation)
 *	return:	unsigned int return_value		(number of characters printed - like standard printf)
 ******************************************************************************/
//int embedded_ftoa(char *print_buf, float input, signed int dp, unsigned int sci)
unsigned int embedded_ftoa(char *print_buf, float input, signed int dp, unsigned int sci)
{
	char *s;
	unsigned int neg = 0;
	unsigned long t;
	int32_t integer;
	uint32_t decimal;
	int Exp = 0;
	int Exp_sg = 0;

	unsigned int return_value = 0;

	if (dp > 4) {dp = 4;}
	if (dp < 1) {dp = 1;}

	float number;
	number = input;

	if (number < 0) {
		neg = 1;
		number *= -1;
	}

	if (sci)
	{
		if (number >= 10)
		{
			while (number >= 10)
			{
				number /= 10;
				Exp++;
			}
		}
		if ((number < 1) && (number != 0))
		{
			while (number < 1)
			{
				number *= 10;
				Exp--;
			}
		}
		if (Exp < 0) {
			Exp *= -1;
			Exp_sg = 1;
		}
	}

	integer = (int32_t)number;
	number = number - (float)integer;

	unsigned int ii;
	unsigned int mult = 1;
	for (ii = 0; ii < dp; ++ii) {
		mult *= 10;
	}

	decimal = (uint32_t)(number * (float)mult);

	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	if (sci)
	{
		do
		{
			t = Exp % 10;
			if (t >= 10)
				t += 'a' - '0' - 10;
			*--s = t + '0';
			Exp /= 10;
		}while(Exp);

		if (Exp_sg) {
			*--s = '-';
		}
		*--s = 'e';
	}

	while (dp)                               	// convert the portion after the dot "."
	{
		t = decimal % 10;
		if (t >= 10)
			t += 'a' - '0' - 10;
		*--s = t + '0';
		decimal /= 10;
		--dp;
	}

	*--s = '.';									// put the dot separator

	do											// convert the portion before the dot "."
	{
		t = integer % 10;
		if (t >= 10)
			t += 'a' - '0' - 10;
		*--s = t + '0';
		integer /= 10;
	}while(integer);

	if (neg)									// if negative
	{
//		if (width && (pad & PAD_ZERO))
//		{     // If there is width, right justified and pad with zero, output negative sign.
//			putChar('-');
//			++return_value;
//			--width;
//		}
//		else *--s = '-';
		*--s = '-';
	}

//	volatile int count;
	int count;
	count = s - print_buf;

	int xx;
	for (xx = 0; xx < count; xx++) {			// the string is formed in the end of buffer
		print_buf[xx] = *s;						// this code move to the beginning
		s++;
	}

	return return_value;
}
#endif
#endif


/******************************************************************************
 * Embedded version of the "printf()" function - use the same parameters
 * 		"u" (unsigned long), "d" (signed long), "x/X" (hexadecimal)
 * 		"b" (binary), "f" (float), "e" (float in scientific notation)
 * Limitations:		integer numbers need be casted with (long) and float/double with (float - standard)
 * 						or (double - precision_float)
 * 					integer numbers limited by 32bits signed size (long)
 * 					float variables limited to 4 decimal places in standard (reduce size and performance)
 * 					float variables limited to 9 decimal places in precision (reduce size and performance)
 * Input:		char *format			(like standard "printf( )")
 * return:		int return_value		(number of characters printed - like standard printf)
 ******************************************************************************/
unsigned int embedded_printf(char *format, ...)
{
	char print_buf[PRINT_BUF_LEN];
	unsigned int width, pad;
	unsigned int return_value = 0;
	unsigned int dp = 0;

	va_list args;
	va_start(args, format);

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;

			if (*format == '-') {
				++format;
#ifdef	ENABLE_PAD_
				pad = PAD_RIGHT;
#endif
			}
			while (*format == '0') {
				++format;
#ifdef	ENABLE_PAD_
				pad |= PAD_ZERO;
#endif
			}
			for (; *format >= '0' && *format <= '9'; ++format) {
//				width *= 10;
				width = (width << 3) + (width << 1);	// x * 10 = x * 8 + x * 2
				width += *format - '0';
			}

			if (*format == '.') {
				++format;
				for (; *format >= '0' && *format <= '9'; ++format) {
//					dp *= 10;
					dp = (dp << 3) + (dp << 1);			// x * 10 = x * 8 + x * 2
					dp += *format - '0';
				}
			}

			if (*format == 's') {				// if string - call the respective function
				char *s = (char *)va_arg(args, int);
				return_value += embedded_prints(s ? s : "(null)", width, pad);
				continue;
			}
			if (*format == 'd') {				// if signed long - call the respective function
				return_value += embedded_ltoa(print_buf, va_arg(args, int32_t), DECIMAL, SIGNED, width, pad, LOWER_CASE);
				return_value += embedded_prints(print_buf, width, pad);
				continue;
			}
			if (*format == 'x') {				// if hexadecimal (lowercase) - call the respective function
				return_value += embedded_ltoa(print_buf, va_arg(args, int32_t), HEXADEC, NON_SIGNED, width, pad, LOWER_CASE);
				return_value += embedded_prints(print_buf, width, pad);
				continue;
			}
			if (*format == 'X') {				// if hexadecimal (uppercase) - call the respective function
				return_value += embedded_ltoa(print_buf, va_arg(args, int32_t), HEXADEC, NON_SIGNED, width, pad, UPPER_CASE);
				return_value += embedded_prints(print_buf, width, pad);
				continue;
			}
			if (*format == 'u') {				// if unsigned long - call the respective function
				return_value += embedded_ltoa(print_buf, va_arg(args, int32_t), DECIMAL, NON_SIGNED, width, pad, LOWER_CASE);
				return_value += embedded_prints(print_buf, width, pad);
				continue;
			}
			if (*format == 'c') {				// if a char - direct put in the serial
				char scr[2];
				scr[0] = (char)va_arg(args, int);
				scr[1] = '\0';
				return_value += embedded_prints(scr, width, pad);
				continue;
			}
#ifdef	ENABLE_FLOAT_
			if (*format == 'f') {				// if float/double - call the respective function
#ifdef PRECISION_FLOAT_
				return_value += embedded_ftoa(print_buf, va_arg(args, double), dp, NON_SCI);
#else
				return_value += embedded_ftoa(print_buf, (float)va_arg(args, double), dp, NON_SCI);
#endif
				return_value += embedded_prints(print_buf, 0, 0);
				continue;
			}
			if (*format == 'e') {				// if scientific notation - call the respective function
#ifdef PRECISION_FLOAT_
				return_value += embedded_ftoa(print_buf, va_arg(args, double), dp, SCI);
#else
				return_value += embedded_ftoa(print_buf, (float)va_arg(args, double), dp, SCI);
#endif
				return_value += embedded_prints(print_buf, 0, 0);
				continue;
			}
#endif
#ifdef	ENABLE_BINARY_
			if (*format == 'b') {				// if binary - call the respective function
				return_value += embedded_ltoa(print_buf, va_arg(args, int32_t), BINARY, NON_SIGNED, width, pad, LOWER_CASE);
				return_value += embedded_prints(print_buf, width, 0);
				continue;
			}
#endif
		}
		else {
		out:
			putChar(*format);
			++return_value;
		}
	}
	va_end(args);
	return return_value;
}


//void set_label(unsigned int x)
//{
//	label = x;
//}


/******************************************************************************
 * simple function to print a String
 ******************************************************************************/
unsigned int print_string(char *string)
{
	return embedded_prints(string, 0, NON_PAD);
}


/******************************************************************************
 * simple function to print a signed long
 ******************************************************************************/
unsigned int print_long(long number)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ltoa(buffer, number, DECIMAL, SIGNED, 0, NON_PAD, LOWER_CASE);
	return embedded_prints(buffer, 0, NON_PAD);
}


/******************************************************************************
 * simple function to print a unsigned long
 ******************************************************************************/
unsigned int print_ulong(unsigned long number)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ltoa(buffer, number, DECIMAL, NON_SIGNED, 0, NON_PAD, LOWER_CASE);
	return embedded_prints(buffer, 0, NON_PAD);
}


/******************************************************************************
 * simple function to print a hexadecimal
 ******************************************************************************/
unsigned int print_hexa(long number)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ltoa(buffer, number, HEXADEC, NON_SIGNED, 0, NON_PAD, LOWER_CASE);
	return embedded_prints(buffer, 0, NON_PAD);
}

#ifdef	ENABLE_BINARY_
/******************************************************************************
 * simple function to print a binary
 ******************************************************************************/
unsigned int print_binary(long number, unsigned int bits)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ltoa(buffer, number, BINARY, NON_SIGNED, bits, NON_PAD, LOWER_CASE);
	return embedded_prints(buffer, 0, NON_PAD);
}
#endif


#ifdef	ENABLE_FLOAT_
/******************************************************************************
 * simple function to print a float/double
 ******************************************************************************/
#ifdef	PRECISION_FLOAT_
unsigned int print_float(double number, int dp)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ftoa(buffer, number, dp, NON_SCI);
	return embedded_prints(buffer, 0, NON_PAD);
}
#else
unsigned int print_float(float number, int dp)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ftoa(buffer, number, dp, NON_SCI);
	return embedded_prints(buffer, 0, NON_PAD);
}
#endif


/******************************************************************************
 * simple function to print in scientific notation - cast to (double)
 ******************************************************************************/
#ifdef	PRECISION_FLOAT_
unsigned int print_scientific(double number, int dp)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ftoa(buffer, number, dp, SCI);
	return embedded_prints(buffer, 0, NON_PAD);
}
#else
unsigned int print_scientific(float number, int dp)
{
	char buffer[PRINT_BUF_LEN];
	embedded_ftoa(buffer, number, dp, SCI);
	return embedded_prints(buffer, 0, NON_PAD);
}
#endif
#endif


#ifdef	PRECISION_FLOAT_
//void embedded_string2number(char *string, double *number)
void embedded_string2number(unsigned char *string, double *number)
{
	unsigned int neg = 0;			// indicate a negative number
	uint32_t integer = 0;		// save the integer part
	uint32_t decimal = 0;		// save the decimal part
	volatile uint32_t decimal_count = 1;		// count the number of digits
	double number_float = 0;			// used to calculate the number

	if (*string == '-')			// check if negative
	{
		++neg;
//		*string++;
		string++;
	}
	while ((*string != '.') && (*string))// convert the integer part before the '.'
	{
//			if (*string >= '0' && *string <= '9')
//			{
		integer *= 10;
		integer += *string - '0';
//			}
//		*string++;
		string++;
	}

	if (*string == '.')		// verify the '.'
			{
//		*string++;
		string++;
		decimal_count = 1;
		while (*string)		// convert decimal part
		{
			decimal *= 10;
			decimal += *string - '0';
			decimal_count *= 10;
//			*string++;
			string++;
		}
	}
	number_float = (double) integer + (double) decimal / (double) decimal_count;
	if (neg) {
		number_float = number_float * (-1);
	}
	*number = number_float;
}
#else
//void embedded_string2number(char *string, float *number)
void embedded_string2number(unsigned char *string, float *number)
{
	unsigned int neg = 0;			// indicate a negative number
	uint32_t integer = 0;		// save the integer part
	uint32_t decimal = 0;		// save the decimal part
	volatile uint32_t decimal_count = 1;		// count the number of digits
	float number_float = 0;			// used to calculate the number

	if (*string == '-')			// check if negative
	{
		++neg;
//		*string++;
		string++;
	}
	while ((*string != '.') && (*string))// convert the integer part before the '.'
	{
//			if (*string >= '0' && *string <= '9')
//			{
		integer *= 10;
		integer += *string - '0';
//			}
//		*string++;
		string++;
	}

	if (*string == '.')		// verify the '.'
	{
//		*string++;
		string++;
		decimal_count = 1;
		while (*string)		// convert decimal part
		{
			decimal *= 10;
			decimal += *string - '0';
			decimal_count *= 10;
//			*string++;
			string++;
		}
	}
	number_float = (float) integer + (float) decimal / (float) decimal_count;
	if (neg)
	{
		number_float = number_float * (-1);
	}
	*number = number_float;
}
#endif
