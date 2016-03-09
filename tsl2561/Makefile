# microcontroller
DEVICE=msp430g2553
# project name
PROJECT=tsl2561
# C source files
SRC=main.c uart.c i2c.c tsl2561.c

include ../Makefile.common

main.o: uart.h i2c.h tsl2561.h
uart.o: uart.h
i2c.o: i2c.h
tsl2561.o: tsl2561.h i2c.h
