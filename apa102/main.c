#include <msp430.h>
#include <stdint.h>
#include "spi.h"

void timer_isr(void) __attribute__((interrupt(TIMER0_A0_VECTOR)));

/* number of LEDs in the string */
#define NUM_LEDS 30

static volatile uint32_t ticks;

/* start frame: 32 x 0 */
static uint8_t start_frame[] = {0x00, 0x00, 0x00, 0x00};
/* end frame: 32 x 1 */
static uint8_t end_frame[] = {0xff, 0xff, 0xff, 0xff};
/* global brightness + RGB values of all LEDs in the string */
static uint8_t frame_buffer[4*NUM_LEDS];

int main()
{
	uint32_t start;
	int i, j;
	/* byte 1: 3 x 1, 5 bits global brightness,
	 * byte 2: blue,
	 * byte 3: green,
	 * byte 4: red */
	uint8_t led_frame_red[] = {0xff, 0x00, 0x00, 0x05};
	uint8_t led_frame_off[] = {0xff, 0x00, 0x00, 0x00};

	uint8_t r, g, b;

	/* disable watchdog */
	WDTCTL = WDTPW | WDTHOLD;

	/* hang if calibration constants have been erased */
	if (CALBC1_16MHZ == 0xff || CALDCO_16MHZ == 0xff) {
		for (;;)
			;
	}
	/* load calibrated settings for 16 MHz DCO clock */
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	/* select SMCLK as clock source for timer A0, reset timer */
	TA0CTL = TASSEL_2 | TACLR;
	/* divide input clock by 8: 2 MHz */
	TA0CTL |= ID_3;
	/* set compare value for 1 kHz tick frequency */
	TA0CCR0 = 2000;
	/* enable compare interrupt */
	TA0CCTL0 = CCIE;
	/* start timer (up mode) */
	TA0CTL |= MC_1;

	/* enable interrupts */
	__bis_SR_register(GIE);

	/* 1 MHz clock frequency */
	init_spi();

	/******************************************************/
	/* light up all LEDs in the string, one after another */
	/******************************************************/

	for (i = 0; i < NUM_LEDS; ++i) {
		spi_write(start_frame, sizeof start_frame);
		while (spi_busy())
			;
		for (j = 0; j < NUM_LEDS; ++j) {
			if (j == i) {
				spi_write(led_frame_red, sizeof led_frame_red);
				while (spi_busy())
					;
			} else {
				spi_write(led_frame_off, sizeof led_frame_off);
				while (spi_busy())
					;
			}
		}
		spi_write(end_frame, sizeof end_frame);
		while (spi_busy())
			;

		/* wait for 0.5 s */
		start = ticks;
		while ((ticks - start) < 250)
			;
	}

	/************************************/
	/* cycle through color combinations */
	/************************************/

	/* set global brightness of all LEDs to maximum */
	for (i = 0; i < NUM_LEDS; ++i)
		frame_buffer[i*4] = 0xff;

	for (;;) {
		for (r = 0; r < 100; r+=5) {
			for (g = 0; g < 100; g+=5) {
				for (b = 0; b < 100; b+=5) {
					for (i = 0; i < NUM_LEDS; ++i) {
						/* blue */
						frame_buffer[i*4+1] = (b+i*2) % 100;
						/* green */
						frame_buffer[i*4+2] = (g+i*2) % 100;
						/* red */
						frame_buffer[i*4+3] = (r+i*2) % 100;
					}

					spi_write(start_frame, sizeof start_frame);
					while (spi_busy())
						;
					spi_write(frame_buffer, sizeof frame_buffer);
					while (spi_busy())
						;
					spi_write(end_frame, sizeof end_frame);
					while (spi_busy())
						;

					start = ticks;
					while ((ticks - start) < 50)
						;
				}
			}
		}
	}

	return 0;
}

void timer_isr(void)
{
	++ticks;
}

