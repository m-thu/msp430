#include <msp430.h>
#include <stdint.h>
#include "uart.h"
#include "i2c.h"
#include "lm75.h"

void timer_interrupt(void) __attribute__((interrupt(TIMER0_A0_VECTOR)));

static volatile uint32_t ticks;

enum {NO_SENSOR = 0, SENSOR = 1};

int main()
{
	uint32_t start;
	uint8_t i, sensors[8];
	int16_t temp;

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

	/* valid I2C slave addresses (bits 2:0): 0x0-0x7 */
	for (i = 0; i < 0x8; ++i) {
		if (lm75_power_up(LM75_ADDR | i)) {
			printf_uart("sensor found at address 0x%x\r\n",
			            LM75_ADDR | i);
			sensors[i] = SENSOR;
		} else {
			sensors[i] = NO_SENSOR;
		}
	}

	for (;;) {
		for (i = 0; i < 0x8; ++i) {
			if (sensors[i] == SENSOR) {
				/* get temperature in 0.5 °C steps */
				temp = lm75_get_temp(LM75_ADDR | i);
				printf_uart("sensor 0x%x: ", LM75_ADDR | i);
				if (temp != -1) {
					printf_uart("%i.%c °C\r\n", temp/2,
					            (temp%2)?'5':'0');
				} else {
					printf_uart("-0.5 °C\r\n");
				}
			}
		}

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
