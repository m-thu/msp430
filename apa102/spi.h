#ifndef SPI_H
#define SPI_H

#include <msp430.h>
#include <stdint.h>

void init_spi(void);
void spi_write(uint8_t *, int);
int spi_busy(void);

#endif
