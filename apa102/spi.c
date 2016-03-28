#include "spi.h"

void tx_isr(void) __attribute__((interrupt(USCIAB0TX_VECTOR)));

static uint8_t *tx_data;
static int tx_byte_count = 0;

void init_spi(void)
{
	/* disable USCI B */
	UCB0CTL1 |= UCSWRST;

	/* clock phase = 1: output data on rising edge,
	 * clock polarity = 0: active high,
	 * transmit MSB first,
	 * 8-bit data,
	 * master mode,
	 * 3-pin SPI mode,
	 * synchronous mode */
	UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;

	/* select SMCLK as clock source */
	UCB0CTL1 |= UCSSEL_3;
	/* UCBR = f_SMCLK / f_BitClock = 16 MHz / 1 MHz = 16 */
	UCB0BR0 = 16;
	UCB0BR1 = 0;

	/* configure P1.5 as CLK */
	P1SEL |= BIT5;
	P1SEL2 |= BIT5;
	/* configure P1.7 as MOSI */
	P1SEL |= BIT7;
	P1SEL2 |= BIT7;

	/* reenable USCI B */
	UCB0CTL1 &= ~UCSWRST;

	/* enable transmit interrupt */
	IE2 |= UCB0TXIE;
}

void spi_write(uint8_t *data, int count)
{
	tx_data = data;
	tx_byte_count = count;

	--tx_byte_count;
	UCB0TXBUF = *tx_data++;
}

int spi_busy(void)
{
	return UCB0STAT & UCBUSY;
}

void tx_isr(void)
{
	if (tx_byte_count) {
		UCB0TXBUF = *tx_data++;
		--tx_byte_count;
	} else {
		/* no more bytes to transmit, clear pending interrupt */
		IFG2 &= ~UCB0TXIFG;
	}
}

