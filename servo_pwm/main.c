#include <msp430.h>
#include <stdint.h>

int main()
{
	uint16_t servo_position;

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

	/* configure P1.6 as TA0.1 */
	P1DIR |= BIT6;
	P1SEL |= BIT6;
	P1SEL2 &= ~BIT6;

	/* select SMCLK as clock source for timer A0, reset timer */
	TA0CTL = TASSEL_2 | TACLR;
	/* divide input clock by 8: 125 kHz */
	TA0CTL |= ID_3;
	/* set compare value for a PWM frequency of 50 Hz */
	TA0CCR0 = 2500;
	/* output mode for compare reg. 1: reset/set */
	TA0CCTL1 |= OUTMOD_7;
	/* servo position: -90° = 1 ms */
	TA0CCR1 = 125;
	/* servo position:  90° = 2 ms */
	/*TA0CCR1 = 250;*/
	/* start timer (up mode) */
	TA0CTL |= MC_1;

	/* disable ADC10 */
	ADC10CTL0 &= ~ENC;

	/* Vref+ = Vcc, Vref- = Vss,
	 * sample+hold time: 64 x ADC10_clk,
	 * ADC10 on */
	ADC10CTL0 |= SREF_0 | ADC10SHT_3 | ADC10ON;
	/* input channel: A0 (P1.0),
	 * sample+hold source: ADC10SC bit,
	 * clock divider: 1,
	 * clock source: ADC10OSC,
	 * single channel, single conversion */
	ADC10CTL1 |= INCH_0 | SHS_0 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_0;
	/* enable analog input A0 */
	ADC10AE0 |= BIT0;

	/* enable ADC10 */
	ADC10CTL0 |= ENC;

	for (;;) {
		/* start sample and conversion */
		ADC10CTL0 |= ADC10SC;
		/* wait for conversion to finish */
		while (ADC10CTL1 & ADC10BUSY)
			;
		/* set servo position:
		 * (TA0CCR1 (90°) - TA0CCR1 (-90°)) / ADC_MAX =
		 * (250 - 125) / 1023 = 0.1222 */
		servo_position = 12 * ADC10MEM;
		servo_position /= 100;
		TA0CCR1 = 125 + servo_position;
		__delay_cycles(1000);
	}

	return 0;
}
