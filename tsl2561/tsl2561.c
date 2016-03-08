#include "tsl2561.h"

static inline void write_byte(uint8_t, uint8_t, uint8_t);
static inline void write_word(uint8_t, uint8_t, uint16_t);
static inline uint8_t read_byte(uint8_t, uint8_t);
static inline uint16_t read_word(uint8_t, uint8_t);

static enum TSL2561_GAIN gain_setting;
static enum TSL2561_INT_TIME time_setting;

int tsl2561_power_up(uint8_t slave_addr)
{
	/* power up default settings */
	gain_setting = TSL2561_GAIN_1X;
	time_setting = TSL2561_INT_402_MS;
	/* try to power up device */
	write_byte(slave_addr, TSL2561_REG_CONTROL, TSL2561_POWER_UP);
	/* fail if no slave present */
	if (i2c_nack_received())
		return 0;
	/* check if device was powered up successfully */
	return (read_byte(slave_addr, TSL2561_REG_CONTROL) & 0x3)
	       == TSL2561_POWER_UP;

}

void tsl2561_power_down(uint8_t slave_addr)
{
	write_byte(slave_addr, TSL2561_REG_CONTROL, TSL2561_POWER_DOWN);
}

uint8_t tsl2561_get_id(uint8_t slave_addr)
{
	return read_byte(slave_addr, TSL2561_REG_ID);
}

void tsl2561_set_gain_int_time(uint8_t slave_addr, enum TSL2561_GAIN gain,
                               enum TSL2561_INT_TIME time)
{
	uint8_t timing_reg = 0x00;

	gain_setting = gain;
	time_setting = time;

	if (gain == TSL2561_GAIN_16X)
		timing_reg |= TSL2561_GAIN_BIT;
	timing_reg |= time & 0x3;
	write_byte(slave_addr, TSL2561_REG_TIMING, timing_reg);
}

/* only valid for T, FN and CL package! */
uint32_t tsl2561_get_lux(uint8_t slave_addr)
{
	uint16_t adc0, adc1;
	uint32_t adc_scale, ch0, ch1;
	uint32_t ratio = 0;
	uint32_t a, b, lux;

	/* get raw ADC values */
	/* channel 0: visible and IR, channel 1: IR */
	adc0 = read_word(slave_addr, TSL2561_REG_DATA0LOW);
	adc1 = read_word(slave_addr, TSL2561_REG_DATA1LOW);

	/* scale ADC values depending on integration time */
	switch (time_setting) {
	case TSL2561_INT_13_7_MS:
		adc_scale = TSL2561_ADC_13_7_MS;
		break;

	case TSL2561_INT_101_MS:
		adc_scale = TSL2561_ADC_101_MS;
		break;

	case TSL2561_INT_402_MS:
		adc_scale = TSL2561_ADC_402_MS;
		break;
	/* suppress compiler warning for unhandled case */
	default:
		adc_scale = TSL2561_ADC_402_MS;
		break;
	}

	/* scale ADC values if gain isn't set to 16X */
	if (gain_setting == TSL2561_GAIN_1X)
		adc_scale <<= 4;

	/* apply scaling factor */
	ch0 = adc0 * adc_scale;
	ch1 = adc1 * adc_scale;
	/* convert back to integer value */
	ch0 >>= TSL2561_ADC_SCALE;
	ch1 >>= TSL2561_ADC_SCALE;

	/* scale ratio ch1 / ch0, prevent division by zero */
	if (ch0 != 0)
		ratio = (ch1 << TSL2561_RATIO_SCALE) / ch0;

	/* select coefficients for piecewise linear approx. depending on
	   ratio ch1/ch0 */
	if (ratio <= TSL2561_RATIO_1) {
		a = TSL2561_A_1;
		b = TSL2561_B_1;
	} else if (ratio <= TSL2561_RATIO_2) {
		a = TSL2561_A_2;
		b = TSL2561_B_2;
	} else if (ratio <= TSL2561_RATIO_3) {
		a = TSL2561_A_3;
		b = TSL2561_B_3;
	} else if (ratio <= TSL2561_RATIO_4) {
		a = TSL2561_A_4;
		b = TSL2561_B_4;
	} else if (ratio <= TSL2561_RATIO_5) {
		a = TSL2561_A_5;
		b = TSL2561_B_5;
	} else if (ratio <= TSL2561_RATIO_6) {
		a = TSL2561_A_6;
		b = TSL2561_B_6;
	} else if (ratio <= TSL2561_RATIO_7) {
		a = TSL2561_A_7;
		b = TSL2561_B_7;
	} else if (ratio > TSL2561_RATIO_7) {
		a = 0;
		b = 0;
	}

	/* calculate lux value and convert back to integer value */
	lux = a * ch0 - b * ch1;
	lux >>= TSL2561_APPROX_SCALE;

	return lux;
}

static inline void write_byte(uint8_t slave_addr, uint8_t reg, uint8_t byte)
{
	uint8_t buf[2];

	buf[0] = TSL2561_CMD | (reg & 0xf);
	buf[1] = byte;
	i2c_write(slave_addr, buf, sizeof buf, STOP);
	while (i2c_busy())
		;
}

static inline void write_word(uint8_t slave_addr, uint8_t reg, uint16_t word)
{
	uint8_t buf[3];

	buf[0] = TSL2561_CMD | TSL2561_WORD | (reg & 0xf);
	buf[1] = word & 0xff;
	buf[2] = word >> 8;
	i2c_write(slave_addr, buf, sizeof buf, STOP);
	while (i2c_busy())
		;
}

static inline uint8_t read_byte(uint8_t slave_addr, uint8_t reg)
{
	uint8_t cmd = TSL2561_CMD | (reg & 0xf), buf;

	i2c_write(slave_addr, &cmd, sizeof cmd, RSTART);
	while (i2c_busy())
		;
	i2c_read(slave_addr, &buf, sizeof buf);
	while (i2c_busy())
		;

	return buf;
}

static inline uint16_t read_word(uint8_t slave_addr, uint8_t reg)
{
	uint8_t cmd = TSL2561_CMD | TSL2561_WORD | (reg & 0xf), buf[2];

	i2c_write(slave_addr, &cmd, sizeof cmd, RSTART);
	while (i2c_busy())
		;
	i2c_read(slave_addr, buf, sizeof buf);
	while (i2c_busy())
		;

	return (buf[1] << 8) | buf[0];
}
