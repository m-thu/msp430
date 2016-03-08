#include <msp430.h>
#include <stdint.h>
#include "uart.h"
#include "i2c.h"
#include "tsl2561.h"

void timer_interrupt(void) __attribute__((interrupt(TIMER0_A0_VECTOR)));

static volatile uint32_t ticks;

int main()
{
	uint32_t start;
	uint8_t sensor;

	/* disable watchdog */
	WDTCTL = WDTPW | WDTHOLD;

	/* hang if calibration constants have been erased */
	if (CALBC1_1MHZ == 0xff || CALDCO_1MHZ == 0xff) {
		for (;;)
			;
	}
	/* load calibrated settings for 1 MHz DCO clock */
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	/* 9600, N, 8, 1 */
	init_uart();

	/* select SMCLK as clock source for timer A0, reset timer */
	TA0CTL = TASSEL_2 | TACLR;
	/* divide input clock by 8: 125 kHz */
	TA0CTL |= ID_3;
	/* set compare value for 12.5 Hz tick frequency */
	TA0CCR0 = 10000;
	/* enable compare interrupt */
	TA0CCTL0 = CCIE;
	/* start timer (up mode) */
	TA0CTL |= MC_1;

	/* enable interrupts */
	__bis_SR_register(GIE);

	/* I2C master mode, 100 kHz, 7 bit slave addresses */
	init_i2c();

	/* look for TSL2561 sensor on I2C bus */
	sensor = TSL2561_ADDR_GND;
	if (tsl2561_power_up(sensor) == 0) {
		if (sensor = TSL2561_ADDR_VDD, tsl2561_power_up(sensor) == 0) {
			if (sensor = TSL2561_ADDR_FLOAT,
			    tsl2561_power_up(sensor) == 0) {
				print_uart("no sensor found!\r\n");
				for (;;)
					;
			}
		}
	}
	printf_uart("sensor found at address 0x%x\r\n", sensor);
	printf_uart("id register: 0x%x\r\n\r\n", tsl2561_get_id(sensor));

	tsl2561_set_gain_int_time(sensor, TSL2561_GAIN_1X, TSL2561_INT_101_MS);

	for(;;) {
		printf_uart("%i lx\r\n", tsl2561_get_lux(sensor));

		/* wait for ~5 s */
		start = ticks;
		while ((ticks - start) < 12*5)
			;
	}

	return 0;
}

void timer_interrupt(void)
{
	++ticks;
}
