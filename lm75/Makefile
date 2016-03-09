# microcontroller
DEVICE=msp430g2553
# project name
PROJECT=lm75
# C source files
SRC=main.c uart.c i2c.c lm75.c

include ../Makefile.common

main.o: uart.h i2c.h lm75.h
uart.o: uart.h
i2c.o: i2c.h
lm75.o: lm75.h i2c.h
