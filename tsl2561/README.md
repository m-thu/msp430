TSL2561 digital light sensor
============================

## Wiring (MSP-EXP430G2 LaunchPad)
```
P1.2 (TX)  -----> RX (USB<->UART)

P1.6 (SCL) <----> SCL (TSL2561)
P1.7 (SDA) <----> SDA (TSL2561)
VCC        ------ VCC (TSL2561)
GND        ------ GND (TSL2561)
           <----- INT (TSL2561)
```

## References
[TSL2561 datasheet](https://www.adafruit.com/datasheets/TSL2561.pdf)

[I2C-bus specification and user manual](http://www.nxp.com/documents/user_manual/UM10204.pdf)
