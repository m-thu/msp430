#ifndef I2C_H
#define I2C_H

#include <msp430.h>
#include <stdint.h>

void init_i2c(void);
void i2c_write(uint8_t, uint8_t *, int, int);
void i2c_read(uint8_t, uint8_t *, int);
int i2c_busy(void);
int i2c_nack_received(void);

enum {STOP = 1, RSTART = 0};

#endif
