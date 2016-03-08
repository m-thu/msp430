#ifndef TSL2561_H
#define TSL2561_H

#include <stdint.h>
#include "i2c.h"

enum TSL2561_GAIN {
	TSL2561_GAIN_1X = 0,
	TSL2561_GAIN_16X = 1
};

enum TSL2561_INT_TIME {
	TSL2561_INT_13_7_MS = 0,
	TSL2561_INT_101_MS  = 1,
	TSL2561_INT_402_MS  = 2,
	TSL2561_INT_MANUAL  = 3
};

int tsl2561_power_up(uint8_t);
void tsl2561_power_down(uint8_t);
uint8_t tsl2561_get_id(uint8_t);
void tsl2561_set_gain_int_time(uint8_t, enum TSL2561_GAIN,
                               enum TSL2561_INT_TIME);
uint32_t tsl2561_get_lux(uint8_t);

/* possible I2C slave addresses */
#define TSL2561_ADDR_GND   0x29
#define TSL2561_ADDR_FLOAT 0x39
#define TSL2561_ADDR_VDD   0x49

/* register set */
#define TSL2561_REG_CONTROL        0x0
#define TSL2561_REG_TIMING         0x1
#define TSL2561_REG_THRESHLOWLOW   0x2
#define TSL2561_REG_THRESHLOWHIGH  0x3
#define TSL2561_REG_THRESHHIGHLOW  0x4
#define TSL2561_REG_THRESHHIGHHIGH 0x5
#define TSL2561_REG_INTERRUPT      0x6
#define TSL2561_REG_RESERVED1      0x7
#define TSL2561_REG_CRC            0x8
#define TSL2561_REG_RESERVED2      0x9
#define TSL2561_REG_ID             0xa
#define TSL2561_REG_RESERVED3      0xb
#define TSL2561_REG_DATA0LOW       0xc
#define TSL2561_REG_DATA0HIGH      0xd
#define TSL2561_REG_DATA1LOW       0xe
#define TSL2561_REG_DATA1HIGH      0xf

/********************/
/* COMMAND register */
/********************/

/* bit 7  : select command register */
#define TSL2561_CMD   (1 << 7)
/* bit 6  : interrupt clear */
#define TSL2561_CLEAR (1 << 6)
/* bit 5  : word read/write */
#define TSL2561_WORD  (1 << 5)
/* bit 4  : block read/write */
/* !! ONLY FOR SMBUS, DOESN'T WORK WITH I2C !! */
#define TSL2561_BLOCK (1 << 4)
/* bit 3-0: register address */

/**************************/
/* CONTROL register (0x0) */
/**************************/

/* bit 7-2: reserved */
/* bit 1-0: power up/down */
#define TSL2561_POWER_DOWN 0x0
#define TSL2561_POWER_UP   0x3

/*************************/
/* TIMING register (0x1) */
/*************************/

/* bit 7-5: reserved */
/* bit 4  : low (1x)/high (16x) gain */
#define TSL2561_GAIN_BIT (1 << 4)
/* bit 3  : manual timing control */
#define TSL2561_MANUAL   (1 << 3)
/* bit 2  : reserved */
/* bit 1-0: integration time */

/****************************/
/* INTERRUPT register (0x6) */
/****************************/

/* bit 7-6: reserved */
/* bit 5-4: interrupt control select */
#define TSL2561_INTR_0 (0x0 << 4)
#define TSL2561_INTR_1 (0x1 << 4)
#define TSL2561_INTR_2 (0x2 << 4)
#define TSL2561_INTR_3 (0x3 << 4)
/* bit 3-0: interrupt persistence */

/********************************/
/* ID register (0xa), read only */
/********************************/

/* bit 7-4: part number identification */
#define TSL2561_PARTNO_TSL2560 (0x0 << 4)
#define TSL2561_PARTNO_TSL2561 (0x1 << 4)
/* bit 3-0: revision number */
#define TSL2561_REVNO_MASK      0xf


/********************************************/
/* Definitions for tsl2561_get_lux function */
/* only valid for T, FN and CL package!     */
/********************************************/

/* scale all raw ADC values by 2^TSL2561_ADC_SCALE */
#define TSL2561_ADC_SCALE 10

/* integration time 402 ms: no scaling */
/* multiply by 2^TSL2561_ADC_SCALE = 2^10 */
#define TSL2561_ADC_402_MS (1 << TSL2561_ADC_SCALE)

/* integration time 13.7 ms: 0.034 */
/* multiply by 2^10 * 322/11 = 29975 */
#define TSL2561_ADC_13_7_MS 29975

/* integration time 101 ms: 0.252 */
/* multipy by 2^10 * 322/81 = 4071 */
#define TSL2561_ADC_101_MS 4071

/* scale ratio ADC1 / ADC0 by 2^TSL2561_RATIO_SCALE */
#define TSL2561_RATIO_SCALE 9

/* scale coefficients for piecewise linear approx. by 2^TSL2561_APPROX_SCALE */
#define TSL2561_APPROX_SCALE 14

/*
 * 0 <= ch1/ch0 <= 0.125:
 * lux = 0.0304 * ch0 - 0.0272 * ch1 =
 *     =      a * ch0 -      b * ch1
 *
 * ratio1 = 0.125 * 2^9 = 64
 * a1 = 0.0304 * 2^14 = 498
 * b1 = 0.0272 * 2^14 = 446
 */
#define TSL2561_RATIO_1 64
#define TSL2561_A_1 498
#define TSL2561_B_1 446

/*
 * 0.125 < ch1/ch0 <= 0.250:
 * lux = 0.0325 * ch0 - 0.0440 * ch1
 *
 * ratio2 = 0.250 * 2^9 = 128
 * a2 = 0.0325 * 2^14 = 532
 * b2 = 0.0440 * 2^14 = 721
 */
#define TSL2561_RATIO_2 128
#define TSL2561_A_2 532
#define TSL2561_B_2 721

/*
 * 0.250 < ch1/ch0 <= 0.375
 * lux = 0.0351 * ch0 - 0.0544 * ch1
 *
 * ratio3 = 0.375 * 2^9 = 192
 * a3 = 0.0351 * 2^14 = 575
 * b3 = 0.0544 * 2^14 = 891
 */
#define TSL2561_RATIO_3 192
#define TSL2561_A_3 575
#define TSL2561_B_3 891

/*
 * 0.375 < ch1/ch0 <= 0.50
 * lux = 0.0381 * ch0 - 0.0624 * ch1
 *
 * ratio4 = 0.50 * 2^9 = 256
 * a4 = 0.0381 * 2^14 = 624
 * b4 = 0.0624 * 2^14 = 1022
 */
#define TSL2561_RATIO_4 256
#define TSL2561_A_4 624
#define TSL2561_B_4 1022

/*
 * 0.50 < ch1/ch0 <= 0.61
 * lux = 0.0224 * ch0 - 0.031 * ch1
 *
 * ratio5 = 0.61 * 2^9 = 312
 * a5 = 0.0224 * 2^14 = 367
 * b5 = 0.031 * 2^14 = 508
 */
#define TSL2561_RATIO_5 312
#define TSL2561_A_5 367
#define TSL2561_B_5 508

/*
 * 0.61 < ch1/ch0 <= 0.80
 * lux = 0.0128 * ch0 - 0.0153 * ch1
 *
 * ratio6 = 0.80 * 2^9 = 410
 * a6 = 0.0128 * 2^14 = 210
 * b6 = 0.0153 * 2^14 = 251
 */
#define TSL2561_RATIO_6 410
#define TSL2561_A_6 210
#define TSL2561_B_6 251

/*
 * 0.80 < ch1/ch0 <= 1.30
 * lux = 0.00146 * ch0 - 0.00112 * ch1
 *
 * ratio7 = 1.30 * 2^9 = 666
 * a7 = 0.00146 * 2^14 = 24
 * b7 = 0.00112 * 2^14 = 18
 */
#define TSL2561_RATIO_7 666
#define TSL2561_A_7 24
#define TSL2561_B_7 18

/*
 * 1.30 < ch1/ch0
 * lux = 0
 */

#endif
