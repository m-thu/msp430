#include "uart.h"

static void print_dec(unsigned int);
static void print_hex(unsigned int);
static void print_bin(unsigned int);

void init_uart(void)
{
	/* disable USCI A */
	UCA0CTL1 = UCSWRST;

	/* no parity, 8 data bits, 1 stop bit, UART mode */
	UCA0CTL0 = 0x00;
	/* select SMCLK as USCI clock source */
	UCA0CTL1 |= UCSSEL_2;
	/* disable oversampling, baud rate: 9600 */
	UCA0MCTL = UCBRS_1;
	UCA0BR0 = 104;
	UCA0BR1 = 0;

	/* configure P1.2 as TX */
	P1SEL |= BIT2;
	P1SEL2 |= BIT2;

	/* reenable USCI A */
	UCA0CTL1 &= ~UCSWRST;

	/* disable receive and transmit interrupts */
	IE2 &= ~(UCA0TXIE | UCA0RXIE);
}

void putchar_uart(char c)
{
	/* wait for transmission to finish */
	while ((IFG2 & UCA0TXIFG) != UCA0TXIFG)
		;
	UCA0TXBUF = c;
}

void print_uart(char *s)
{
	do
		putchar_uart(*s);
	while (*++s);
}

/* format string:
   %i,%d: decimal, %x: hex, %b binary
   %c: char, %s: string */
void printf_uart(char *fmt, ...)
{
	/* list of arguments */
	va_list arg;

	/* va_start(argument list, last argument) */
	va_start(arg, fmt);

	while (*fmt) {
		if (*fmt == '%') {
			switch (fmt[1]) {
			case 'i':
			case 'd':
				print_dec(va_arg(arg, int));
				fmt += 2;
				continue;
			case 'x':
				print_hex(va_arg(arg, int));
				fmt += 2;
				continue;
			case 'b':
				print_bin(va_arg(arg, int));
				fmt += 2;
				continue;
			case 'c':
				/* char is passed as int by va_arg */
				putchar_uart((char)va_arg(arg, int));
				fmt += 2;
				continue;
			case 's':
				print_uart(va_arg(arg, char *));
				fmt += 2;
				continue;
			default:
				break;
			}
		}
		putchar_uart(*fmt);
		++fmt;
	}

	va_end(arg);
}

static void print_dec(unsigned int i)
{
	if (i > 9) {
		print_dec(i / 10);
		print_dec(i % 10);
	} else {
		putchar_uart(i + '0');
	}
}

static void print_hex(unsigned int i)
{
	if (i > 0xf) {
		print_hex(i / 16);
		print_hex(i % 16);
	} else {
		putchar_uart(i<0xa ? i + '0' : i - 0xa + 'a');
	}
}

static void print_bin(unsigned int i)
{
	if (i > 1) {
		print_bin(i / 2);
		print_bin(i % 2);
	} else {
		putchar_uart(i ? '1' : '0');
	}
}
