#ifndef LM75_H
#define LM75_H

#include <stdint.h>
#include "i2c.h"

int lm75_power_up(uint8_t);
void lm75_power_down(uint8_t);
int16_t lm75_get_temp(uint8_t);

/* I2C slave address (bits 6:3) */
#define LM75_ADDR (0x9 << 3)

/* registers */

/* temperature (read only) */
#define LM75_REG_TEMP   0x0
/* configuration (read/write) */
#define LM75_REG_CONFIG 0x1
/* T_HYST (r/w) */
#define LM75_REG_THYST  0x2
/* T_OS (r/w) */
#define LM75_REG_TOS    0x3

/* command register */

/* bit 7-5: always zero */
/* bit 4-3: fault queue */
#define LM75_FAULTS_1    (0x0 << 3)
#define LM75_FAULTS_2    (0x1 << 3)
#define LM75_FAULTS_4    (0x2 << 3)
#define LM75_FAULTS_6    (0x3 << 3)
/* bit   2: o.s. polarity (0: active low, 1: active high) */
#define LM75_OS_POLARITY (1   << 2)
/* bit   1: comparator/interrupt mode (0: comparator, 1: interrupt) */
#define LM75_CMP_INT     (1   << 1)
/* bit   0: shutdown (1: low power shutdown mode) */
#define LM75_SHUTDOWN    (1   << 0)

#endif
