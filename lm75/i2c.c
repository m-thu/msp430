#include "i2c.h"

void rx_interrupt(void) __attribute__((interrupt(USCIAB0RX_VECTOR)));
void tx_interrupt(void) __attribute__((interrupt(USCIAB0TX_VECTOR)));

static int busy_flag;
static uint8_t *tx_data, tx_byte_count;
static uint8_t *rx_data, rx_byte_count;
static int tx_stop_flag, nack_flag;

void init_i2c(void)
{
	/* disable USCI B */
	UCB0CTL1 |= UCSWRST;

	/* enable master mode with 7 bit slave addresses */
	UCB0CTL0 = UCSYNC | UCMODE_3 | UCMST;
	/* select SMCLK as clock source */
	UCB0CTL1 |= UCSSEL_2;
	/* UCBR = f_SMCLK / f_BitClock = 1 MHz / 100 kHz = 10 */
	UCB0BR0 = 10;
	UCB0BR1 = 0;

	/* configure P1.6 as SCL */
	P1SEL |= BIT6;
	P1SEL2 |= BIT6;
	/* configure P1.7 as SDA */
	P1SEL |= BIT7;
	P1SEL2 |= BIT7;

	/* reenable USCI B */
	UCB0CTL1 &= ~UCSWRST;

	/* enable receive and transmit interrupts */
	IE2 |= UCB0TXIE | UCB0RXIE;
	/* disable STOP, START, arbitration lost interrupts */
	UCB0I2CIE &= ~(UCSTPIE | UCSTTIE | UCALIE);
	/* enable NACK interrupt */
	UCB0I2CIE |= UCNACKIE;

	busy_flag = 0;
	nack_flag = 0;
}

int i2c_busy(void)
{
	return busy_flag;
}

int i2c_nack_received(void)
{
	return nack_flag;
}

void i2c_write(uint8_t slave_addr, uint8_t *data, int count, int stop)
{
	tx_data = data;
	tx_byte_count = count;
	tx_stop_flag = stop;

	busy_flag = 1;
	nack_flag = 0;

	/* set slave address */
	UCB0I2CSA = slave_addr;
	/* select transmitter mode */
	UCB0CTL1 |= UCTR;
	/* generate START condition and transmit slave address */
	UCB0CTL1 |= UCTXSTT;
}

void i2c_read(uint8_t slave_addr, uint8_t *data, int count)
{
	rx_data = data;

	busy_flag = 1;
	nack_flag = 0;

	/* set slave address */
	UCB0I2CSA = slave_addr;
	/* select receiver mode */
	UCB0CTL1 &= ~UCTR;

	if (count == 1) {
		rx_byte_count = 0;

		/* generate START condition and transmit slave address */
		UCB0CTL1 |= UCTXSTT;
		/* wait for slave to acknowledge address */
		while (UCB0CTL1 & UCTXSTT)
			;
		/* generate STOP condition */
		UCB0CTL1 |= UCTXSTP;
	} else {
		rx_byte_count = count - 1;

		/* generate START condition and transmit slave address */
		UCB0CTL1 |= UCTXSTT;
	}
}

void rx_interrupt(void)
{
	/* check for NACK sent by slave */
	if (UCB0STAT & UCNACKIFG) {
		nack_flag = 1;
		/* reset NACK interrupt flag */
		UCB0STAT &= ~UCNACKIFG;
		/* generate STOP condition */
		UCB0CTL1 |= UCTXSTP;
		/* wait until STOP has been sent */
		while (UCB0CTL1 & UCTXSTP)
			;
		busy_flag = 0;
	}
}

void tx_interrupt(void)
{
	/* transmit buffer empty? */
	if (IFG2 & UCB0TXIFG) {
		if (tx_byte_count--) {
			/* load data into TX buffer */
			UCB0TXBUF = *tx_data++;
		} else {
			/* reset TX interrupt flag */
			IFG2 &= ~UCB0TXIFG;
			if (tx_stop_flag) {
				/* generate STOP condition */
				UCB0CTL1 |= UCTXSTP;
				/* wait until STOP has been sent */
				while (UCB0CTL1 & UCTXSTP)
					;
			}
			busy_flag = 0;
		}
	}

	/* check if byte was received */
	if (IFG2 & UCB0RXIFG) {
		/* last byte? */
		if (rx_byte_count == 0) {
			/* get data from receive buffer */
			*rx_data = UCB0RXBUF;
			busy_flag = 0;
		} else {
			/* second to last byte? */
			if (rx_byte_count == 1) {
				/* generate STOP after reception of last byte */
				UCB0CTL1 |= UCTXSTP;
			}
			/* get data */
			*rx_data++ = UCB0RXBUF;
			--rx_byte_count;
		}
	}
}
