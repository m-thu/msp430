#include "lm75.h"

static inline void write_byte(uint8_t, uint8_t, uint8_t);
static inline void write_word(uint8_t, uint8_t, uint16_t);
static inline uint8_t read_byte(uint8_t, uint8_t);
static inline uint16_t read_word(uint8_t, uint8_t);

int lm75_power_up(uint8_t slave_addr)
{
	write_byte(slave_addr, LM75_REG_CONFIG, 0x00);
	/* fail if slave didn't respond */
	return !i2c_nack_received();
}

void lm75_power_down(uint8_t slave_addr)
{
	write_byte(slave_addr, LM75_REG_CONFIG, LM75_SHUTDOWN);
}

/* returns temperature in 0.5 °C steps */
int16_t lm75_get_temp(uint8_t slave_addr)
{
	uint16_t tmp;

	tmp = read_word(slave_addr, LM75_REG_TEMP) >> 7;
	/* if temperature is negative (bit 8 is set), perform sign extension */
	if (tmp & 0x100)
		tmp |= 0xfe00;
	return (int16_t)tmp;
}

static inline void write_byte(uint8_t slave_addr, uint8_t reg, uint8_t byte)
{
	uint8_t buf[2];

	buf[0] = reg & 0x7;
	buf[1] = byte;
	i2c_write(slave_addr, buf, sizeof buf, STOP);
	while (i2c_busy())
		;
}

static inline void write_word(uint8_t slave_addr, uint8_t reg, uint16_t word)
{
	uint8_t buf[3];

	buf[0] = reg & 0x7;
	buf[1] = word >> 8;
	buf[2] = word & 0xff;
	i2c_write(slave_addr, buf, sizeof buf, STOP);
	while (i2c_busy())
		;
}

static inline uint8_t read_byte(uint8_t slave_addr, uint8_t reg)
{
	uint8_t cmd = reg & 0x7, buf;

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
	uint8_t cmd = reg & 0x7, buf[2];

	i2c_write(slave_addr, &cmd, sizeof cmd, RSTART);
	while (i2c_busy())
		;
	i2c_read(slave_addr, buf, sizeof buf);
	while (i2c_busy())
		;

	return (buf[0] << 8) | buf[1];
}
